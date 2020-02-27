#ifndef __SYS_PARAMETER_
#define __SYS_PARAMETER_
#include "HashTable.h"
#include "XhCharString.h"
#include "MemberProperty.h"
typedef struct
{
	int crNode;
	int crOutLine;		//可见轮廓线及一般字型
	int crDashLine;		//不可见轮廓线及非本段实体
	int crLs;			//螺栓符号及标注
	int crDimSimTri;	//相似三角形及标注
	int crSingleLine;	//单实线主用于单线图
	int crAxisLine;		//点划线（中心线）
	int crHuoQuLine;	//火曲线
	int crDragLine;		//引出线
}TOWERATOM_COLOR_INDEX;
//图框尺寸
typedef struct tagMAP_FRAME_SIZE
{
	BYTE style;				//框架类型0-9
	CXhChar100 sMapFrameName;//图框名称 A0 A0-2 A1 A1-2 A3 A3-2 A4 A4-2
	UINT nLength;			//图框宽度
	UINT nWidth;			//图框长度
	short nLeftMargin;		//左侧页边间隙
	short nRightMargin;		//右侧页边间隙
	short nTopMargin;		//上侧页边间隙
	short nBottomMargin;	//下侧页边间隙
	short nBottomRightFixLen;	//图框右下侧固定长度
	short nBottomRightFixWidth;	//图框右下侧固定宽度
	int GetInsideLen(){return nLength-nLeftMargin-nRightMargin;}
	int GetInsideWidth(){return nWidth-nTopMargin-nBottomMargin;}
	int GetTableValidWidth(){return nWidth-nTopMargin-nBottomMargin-nBottomRightFixWidth;}
	tagMAP_FRAME_SIZE()
	{
		style=0;	//A0
		sMapFrameName.Copy("A0");
		nLength=1194;
		nWidth=868;
		nLeftMargin=25;
		nRightMargin=nTopMargin=nBottomMargin=10;
		nBottomRightFixLen=280;
		nBottomRightFixWidth=160;
	}
}MAP_FRAME_SIZE;
struct DRAWING_SCALE
{
	double fRealToDraw;			//基础绘图比例尺＝实际尺寸/绘图尺寸，如1:20时，fRealToDraw=20
	//0:圆孔，实际圆孔绘制直径=螺孔直径*(1/fRealToDraw)
	//1:图符, 实际图符绘制直径=图符原直径
	//2:比例图符，实际图符绘制直径=图符原直径*(1/fRealToDraw)*fBoltSymbolFactor
	//3:比例直径图符，实际图符绘制直径=螺孔直径*图符原直径*(1/fRealToDraw)*fBoltDiaFactor
	int iBoltSymbolStyle;	//螺栓(孔)的图符处理方式0:圆孔1:图符2:比例图符;3:比例直径图符;其余不处理
	double fBoltSymbolFactor;	//比例图符时的比例因子
	double fBoltDiaFactor;		//比例直径图符时的比例因子
	DRAWING_SCALE(){
		fRealToDraw=1;		//1:1
		iBoltSymbolStyle=1;	//图符
		fBoltSymbolFactor=1;
		fBoltDiaFactor=0.5;
	}
	//根据螺栓(孔)的图符处理方式，返回相应的螺栓图符(孔)的绘图复合比例系数
	double ComplexCoef();
};
struct TEXT_PROP
{
	double fHigh;
	double fWtoH;
	char sFont[16];
	TEXT_PROP(){memset(this,0,sizeof(TEXT_PROP));}
};

class CTextItemProp;
#ifndef __TEXTFRAMDE_SIZE_DEF_
#define __TEXTFRAMDE_SIZE_DEF_
struct FRAMESIZE{
	short siFrameStyle;	//-1.自动判断编号标注圈样式;0.圈圈;1.腰圆矩形;2.普通矩形框
	double fMargin;		//文本距边框的间隙
	double fFrameWidth;	//边框的宽度
	double fTextHeight;	//字体高度
	//double fFrameHeight;	//边框的高度一般由边框样式、边距及边框宽度决定
};
#endif
struct IFontStyleConfig
{
	virtual int GetDrawingTypeId()=0;
	virtual double FontScale(){return 1.0;}
	virtual CTextItemProp* Parent()=0;
	virtual FRAMESIZE GetLabelFrame();
	virtual FRAMESIZE GetLabelFlFrame();
	virtual FRAMESIZE GetLabelEndPlateFrame();
	__declspec(property(get=GetLabelFrame)) FRAMESIZE labelFrame;
	__declspec(property(get=GetLabelFlFrame)) FRAMESIZE labelFrameFL;
	__declspec(property(get=GetLabelEndPlateFrame)) FRAMESIZE labelFrameEndPlate;
	DECLARE_PROPERTY(double,fHeaderTextSize); 	//表格表头文字高
	DECLARE_PROPERTY(double,fCommonTextSize); 	//普通表格标题汉字高
	DECLARE_PROPERTY(double,fNumericTextSize);	//表格中数字文字高
	DECLARE_PROPERTY(double,fPartNoTextSize);	//构件编号文字高
	//长度标注
	DECLARE_PROPERTY(double,fDimTextSize);		//长度尺寸标注文本高
	DECLARE_PROPERTY(double,fDimAngleTextSize);	//角度尺寸标注文本高
	//剖视图符
	//DECLARE_PROPERTY(double,fCutawayViewSymbolLen);		//剖视图符长度
	DECLARE_PROPERTY(double,fCutawayViewSymbolTextSize);//剖视图符字体高度
	//结构图标注
	DECLARE_PROPERTY(double,fPartNoMargin);			//构件编号与编号框之间的间隙值
	DECLARE_PROPERTY(double,fPartGuigeTextSize);	//构件规格文字高		2.5
	DECLARE_PROPERTY(double,fJgOddmentTextSize);	//角钢正负头标注文字高	2.5
	DECLARE_PROPERTY(double,fPlateThickTextSize);	//板厚标注		 2.0
	DECLARE_PROPERTY(double,fLsGuigeTextSize);	//螺栓规格文字高 2.0
	DECLARE_PROPERTY(double,fOtherTextSize);	//其余普通文字高度
	//DECLARE_PROPERTY(double,fSimTriScale);		//相似形比例
	DECLARE_PROPERTY(double,fSimPolyTextSize);	//相似形标注	 2.0
};
class CDrawSysPara;
class CTextItemProp
{
	CDrawSysPara* _pSysPara;
	void InitTextItemPropHashtable();
	double GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,int txt_size0_wtoh1=0);
	bool GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,TEXT_PROP* pFontStyle);
public:
	CHashList<TEXT_PROP>hashListProp;
	static CStringKeyHashTable<long>propHashtable;
	static long GetPropID(char* propStr);	//得到属性ID
public:
	CTextItemProp();
	void SetParentSysConfig(CDrawSysPara* pSysParaConfig){_pSysPara=pSysParaConfig;}
	READONLY_PROPERTY(CDrawSysPara*,pSysPara);
	GET(pSysPara){return _pSysPara;}
	//cClass:'0'基准级别'1'总图'2'司令图'3'结构图'4'构件图5'组焊图'
	void SetTextItemProp(char *sType,TEXT_PROP &textProp,BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextSize(char *sType,double high, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextWtoH(char *sType,double WtoH, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void SetTextFont(char *sType,char* font, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	double TextSize(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	double TextWtoH(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	char*  TextFont(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5=0);
	void InitSpecialProp(BYTE class_datum0_sum1_chief2_seg3_part4_weld5);
	static const int BASIC_CONFIG		= 0;	//基本字体样式配置
	static const int SUMMARY_DRAWING	= 1;	//总图字体样式
	static const int CHIEF_DRAWING		= 2;	//司令图字体样式
	static const int SEGMENT_DRAWING	= 3;	//分段结构图字体样式
	static const int ANGLE_DRAWING		= 4;	//角钢类零件图（含工艺卡)字体样式
	static const int WELD_DRAWING		= 5;	//组焊图字体样式
	static const int PLATE_DRAWING		= 6;	//样板图（含工艺卡)字体样式
	friend struct IFontStyleConfig;
	class BASIC_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, basic)));}
		virtual int GetDrawingTypeId(){return BASIC_CONFIG;}
	}basic;

	class SUMMARY_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, summary)));}
		virtual int GetDrawingTypeId(){return SUMMARY_DRAWING;}
	}summary;	//总图

	class CHIEF_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, chief)));}
		virtual int GetDrawingTypeId(){return CHIEF_DRAWING;}
	}chief;		//司令图

	class SEG_DRAW_FONT : public IFontStyleConfig
	{
	public:
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, segment)));}
		virtual int GetDrawingTypeId(){return SEGMENT_DRAWING;}
	}segment;		//分段结构图

	class ANGLE_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		ANGLE_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, angle)));}
		virtual int GetDrawingTypeId(){return ANGLE_DRAWING;}
	}angle;		//角钢零件图

	class PLATE_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		PLATE_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, plate)));}
		virtual int GetDrawingTypeId(){return PLATE_DRAWING;}
	}plate;		//样板图

	class WELD_DRAW_FONT : public IFontStyleConfig
	{
	public:
		double zoomscale;
		WELD_DRAW_FONT(){zoomscale=1.0;}
		virtual double FontScale(){return zoomscale;}
		CTextItemProp* Parent(){return ((CTextItemProp*)((BYTE*)this - offsetof(CTextItemProp, weld)));}
		virtual int GetDrawingTypeId(){return WELD_DRAWING;}
	}weld;		//组焊图
}; 
class CDrawSysPara
{
public:
	CDrawSysPara();
	CStringKeyHashList<MAP_FRAME_SIZE> m_hashMapFrameList;	//图框尺寸哈希表
	int m_iCurMapType;      //当前出图类型 0.基准级别 1.总图 2.司令图 3.结构图 4.构件图 5.组焊图
	int m_iCurMapSegNo;		//当前分段图的分段号
	int m_iCurBodyNo;		//当前总图的本体号
	double m_fCurMapScale;	//绘图塔块中的当前绘图比例
	char sFilePath[MAX_PATH];
	int nBriefGuigeN;		//简化规格数
	//主材分段标志线 wht 10-12-22
	BOOL bDrawMainPoleJointLine;//主材分段标志线
	int nJointLineOffset;		//分段标记线距离其基准角钢距离
	int nJointLineLen;			//分段标记线长度
	//常规设置---整个出图模块共用的设置
	struct GENERAL_SET{
		//绘图比例
		//用下一行代码替代iLsApertureStyle之前的代码，并进行相关更改 wjh-2014.6.19
		//DRAWING_SCALE StruMapScale,PartMapScale,WeldMapScale;
		//DrawUnit.cpp中应使用StruMapScale;PaperPatternDrawing.cpp中应使用PartMapScale
		//SegMap.cpp中应使用WeldMapScale
		double LineMapScale;		//单线图比例 一般为1:100 1:200 1:300
		double ForceLineScale;		//司令图(力学简化线)  一般为1:50
		DRAWING_SCALE StruMapScale;		//结构图比例 一般为1:10 1:20 1:30 1:15
		DRAWING_SCALE PartMapScale;		//构件图比例 一般为1:5 1:10
		DRAWING_SCALE WeldMapScale;		//组焊图比例
		int iMatCharPosType;    //构件编号的材质字符位置选择方式0.不需要 1.构件编号前 2.构件标号后
		char cMatCharSeparator;	//构件编号的材质字符分隔符: 无0  空格' '
		double fHoleAugmentInNC;//孔径增大值
		double fWeldLineSymbolHigh;	 //焊缝线高度
		double fWeldLineSymbolSpace; //焊缝线间隙
		//精确度
		int iDimPrecision;		//尺寸精确度
		int iPartBOMStyle;		//构件明细表样式 0.标准样式(TMA)，1.加宽样式(其他) wht 10-11-11
		int iSumMatBOMStyle;	//材料汇总表样式 0.样式一(材质从大到小,角钢规格从大到小)，1.样式二 wht 11-05-06
		//工艺卡路径及排列方式
		BOOL bProcessCardTbl;		//生成工艺卡图框
		int nPartRowsN;				//成批构件图每列行数
		int nRowHigh;				//每行构件图高度
		int nColumnWidth;			//每列构件图宽度
		BOOL bCardLayoutBaseRow;	//以行为基准排列工艺卡
		BOOL bHasFrame;				//绘制装订线
		char m_sJgCard[MAX_PATH];
		char m_sSlotCard[MAX_PATH];	//槽钢工艺卡 wht 10-09-14
		char m_sTubeCard[MAX_PATH];
		char m_sPlankCard[MAX_PATH];
		char m_sWeldPartGroupCard[MAX_PATH];
		char m_sProcessPartStatCard[MAX_PATH];
		char m_sSetupAidedPlate[MAX_PATH]; //工装辅助钢板库文件路径
		//出图模块路径
		char m_sTMAMapArxPath[MAX_PATH];	//TMA
		char m_sLMAMapArxPath[MAX_PATH];	//LMA
		char m_sLDSMapArxPath[MAX_PATH];	//LDS
		char m_sTSAMapArxPath[MAX_PATH];	//TSA
		//以下是数字化移交属性
		COLORREF lCrossarmCr; //左横担颜色
		COLORREF rCrossarmCr; //右横担颜色
	}general_set;
	//结构图设置
	struct{
		BOOL bExportByEnglish;
		double fMinPartThick;	//角钢、钢管、钢板等构件最小绘制厚度 wht 12-03-24
		double fMinWingWide; 	//角钢最小肢宽/钢管最小半径 	
		BOOL bDimSpecialLsSpace;//标注特殊螺栓间距
		BOOL bDimSpecialJgZhun;	//标注特殊角钢准距
		BOOL bDimStruJgLen;		//结构图中标注角钢长度
		BOOL bDimPartGuiGe;		//结构图中标注构件规格 wht 11-03-29
		BOOL bDrawCutAngleAndWingInfo;	//绘制切角、肢信息
		BOOL bOnlyDrawHorizontalPlate;	//仅绘制在当前结构图中水平显示的钢板 wht 10-06-15
		BOOL bMergeAttachPlatesToBasePlateInBOM;	//分段明细表中将附属子构件（十字插板或法兰）合并到基（底）板中，不单独输出 wjh-17.5.26
		BOOL bUsePolyLineDrawEdge;		//用多段线绘制角钢和钢板的轮廓边	 wht 10-06-15
		double fPolyLineWidth;          //多段线宽度                         xc 10-12-17
		double fPoleAxisLineStartOutSpreadLen;	//杆件心线始端延伸长度 wht 10-12-13
		double fPoleAxisLineEndOutSpreadLen;	//杆件心线终端延伸长度
		BOOL bReadAdditionalBuffer;		//读取附加缓存 wht 11-08-03
	}stru_map;
	//组焊图设置
	struct WELDDRAWING{
		double  fMinFontSize;			//字体最小高度 默认为2.5
		double  fMinDrawThick;			//厚度最小绘制值
		int		nKmJgGIncrement;		//靠模角钢心距增大值
		BOOL bModulatePartGroupLen;		//调整组焊件长度
		double fPartGroupDistThreshold;	//组焊图缩放阈值
		double fPartGroupDistZoomCoef;	//组焊图缩放系数
		BOOL bDrawWeldLineSymbol;		//组焊图中绘制焊缝线标记
		int  iPlateLocateDrawStyle;		//组焊图中绘制钢板对活点绘制方式：0.不绘制;1.三竖线;2.三角旗帜
		int  iLocateSymboleHeight;		//组焊图中对活点标记高度
		BOOL bDimEndOuterPlaneSpace;	//杆件外端面到外端面距离
		BOOL bAutoDimPartNo;			//自动标注构件编号
		BOOL bAutoMatchView;			//自动组合组焊件视图
		BOOL bDimAbsoluteDist;			//标注定位点绝对尺寸
		BOOL bDimAfterWeldCheckLine;	//标注焊接后检测线
		BOOL bDimBeforeWeldLocatSpace;	//标注焊接前定位尺寸
		BOOL bDimPlatePoleRayAndAngle;	//标注钢板射线以及射线与基准线之间的夹角
		BOOL bSmartFilterRedundantPart;	//视图中自动筛选出与当前视图无关的多余构件跳过不显示 wjh-2014.3.29
		BOOL bAngularDimension;			//标注角度线
		BOOL bCreatePartGroupBOM;		//构件明细表
		//构件明细表
		struct{
			BOOL bZoomInProcessCard;	//工艺卡内进行比例缩放
			BOOL bDispPartGroupNo;		//组焊件编号
			BOOL bDispPartGroupNum;		//组焊件数量
			BOOL bDispPartNo;			//构件编号
			BOOL bDispPartNum;			//构件数量
			BOOL bDispPartGuiGe;		//构件规格
			BOOL bDispPartLen;			//构件长度
			BOOL bDispPartWeight;		//构件重量以及同一编号构件重量小计 总重
			BOOL bDispNotes;			//备注
			BOOL bDispInternal;			//大吉ERP提出的[内在]焊缝级别
			BOOL bDispAppearance;		//大吉ERP提出的[外观]焊缝级别
		}part_table;
	}part_group_map;
	//构件图设置 包括构件图共有设置 角钢构件图设置 钢板构件图设置
	struct{
		BOOL bExportDeformedProfile;	//构件图考虑火曲变形
		int  iExportDeformedProfile;	//火曲变形考虑方案：0.不考虑1.考虑2.大样图考虑 3.工艺卡考虑  wh-17-03-27
		BOOL bDimDeformedProfileText;	//考虑火曲变形时是否标注文字 wht 10-11-01
		BOOL bDimPlaneAngle;			//标注火曲构件面夹角
		int	 iDimAngleType;				//火曲角度标注类型 0.锐角 1.钝角 2.自动 wht 10-11-01
		double fMinHuoquAngle;			//构件最小火曲角度：小于此火曲角度的制弯构件不标注火曲信息
		BOOL bDimHuoquType;				//标注构件火曲类型
		BOOL bUseZhengFanQun;			//启用正反曲 默认使用外曲内曲  wht 11-05-07
		int  iPartMapStyle;				//0表示普通;1表示浙江盛达
		struct{
			BOOL bModulateLongJg;		//调整角钢长度 暂未使用，由iJgZoomSchema代替该变量 wht 11-05-07
			int iJgZoomSchema;			//角钢绘制方案，0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放
			BYTE ciJgLenDrawMode;		//角钢进行长宽分别缩放，绘图长度超出工艺卡时的处理模式：0.向右延伸(超出) 1.中间挤压
			BOOL bMaxExtendAngleLength;	//最大限度延伸角钢绘制长度
			double fLsDistThreshold;	//角钢长度自动调整螺栓间距阈值(大于此间距时就要进行调整);
			double fLsDistZoomCoef;		//螺栓间距缩放系数
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
		}angle;
		struct{
			BOOL bMarkHuoquPos;			//标记火曲位置
			BOOL bDrawHuoQuPrjMap;		//绘制火曲示意图
			BOOL bDrawHuoQuMarkLine;	//绘制火曲特征线
			BOOL bAdjustPlateUcs;		//调整钢板坐标系保证最长边沿水平方向
			BOOL bDimPlateRayLs;		//标注板螺栓射线距离
			BOOL bDimPlateBoltAngle;	//标注板螺栓射线角度
			BOOL bDimBoltEdgeSpace;		//标注板螺栓边距
			BOOL bDimDiffRayLsSpace;	//标注不同射线螺栓间距
			BOOL bDimLsToHuoquLine;		//标注螺栓到火曲线间距
			BOOL bDimLsToWeldEdge;		//标注螺栓到焊接边间距
			BOOL bDimPlateEdgeLen;		//标注钢板边长
			BOOL bDimArcEdgeR;			//标注钢板圆弧边半径
			BOOL bDimArcAngle;			//标注钢板圆弧边角度
			BOOL bDrawInnerEdge;        //绘制内轮廓边
			double fInnerEdgeOffset;    //轮廓边偏移距
			BOOL bDimCoord;				//板构件图中是否标注坐标
			BOOL bPlateUseSimpleLsMap;	//钢板使用简化螺栓图符
			BOOL bZoomDimTextSizeByWidth;//根据钢板长宽中较小值缩放钢板相关标注的字体高度
			double fTextSizeZoomCoef;	//字体高度与宽度之间的缩放系数
			BOOL bDrawSlopeCutLine;     //焊缝处的坡口线
			BOOL bDrawDatumLine;		//绘制钢板基准线(钢管轴线),钢管上节点板一般螺栓射线应该延伸至此基准线
			int  iCutArcsType;          //焊烧死角处的切弧方式 0--不切 1--切角 2--切弧
			int	 iZhiWanLineType;		//插板制弯线位置处理方式 0--外侧(以前处理方式) 1.内侧(现在处理方式)
			double fInnerOffThreshold;	//插板制弯线向内偏移阈值(偏移一个板厚的基础上)
			BOOL bDisplaypPartInfoTbl;  //显示构件信息表
			int  nPartInfoTblColumnWidth;//构件表的列宽
			BOOL bUseDimStyleConfigFile;//启用钢板标注样式配置文件
			BOOL bShowPlateCfgInfoDlg;	//是否显示钢板配置对话框
			char sPlateConfigFilePath[MAX_PATH];//钢板配置文件路径
		}plate;
		struct{
			double fSlotWidthProcessError;	//开槽宽度加工误差值
			double fSlotLenProcessError;	//开槽长度加工误差值
			double fReelTubeThresholdD;		//钢板卷管直径阈值 大于该直径的钢管为钢板卷管
		}tube;
	}part_map;
	struct{
		TOWERATOM_COLOR_INDEX crIndex;//画笔颜色
		char sFontName[MAX_PATH];	//默认使用字体 gbhzfs.shx
		//材料表内数据及结构图内构件编号高：3.0；gbhzfs.shx
		double	fHeaderTextSize;	//表格表头文字高
		double	fCommonTextSize;	//普通表格标题汉字高
		double	fNumericTextSize;	//表格中数字文字高
		//构件编号相关参数
		struct{
			int		iPartNoFrameStyle;	//编号框类型 //0.圆圈 1.带圆弧的矩形框 2.矩形框	3.自动判断
			int		iStdInsertPlatePartNoFrameStyle;//标准插板编号框类型 wht 12-03-19	 
			int		iFlPartNoFrameStyle;			//法兰编号框类型 
			double	fPartNoTextSize;	//构件编号文字高
			double	fPartNoCirD;		//构件编号圈直径
			double	fPartNoMargin;		//构件编号与编号框之间的间隙值 
			BOOL	bDrawRefLine;		//绘制引出线
			double  fMainPoleNoOuterCirD;//主材编号外圆直径
			double  fMainPoleNoInnerCirD;//构件编号内圆直径
			int		iMirMsg;			//0:X轴对称；1:Y轴对称;其余不对称处理 wht 11-03-29
		}PartNo;
		//段号标注相关参数	wht 11-06-27
		double fSegICirD;			//段号圈直径
		double fSegITextSize;		//段号文字高
		//长度标注
		double	fDimTextSize;		//长度尺寸标注文本高
		double	fDimAngleTextSize;	//角度尺寸标注文本高
		double	fDimArrowSize;		//尺寸标注箭头长
		char	sArrowType[50];		//尺寸标注箭头类型 wht 10-07-27
		double  fDimGap;			//文字与标注线之间的间距 wht 10-08-02
		//剖视图符
		double	fCutawayViewSymbolLen;		//剖视图符长度
		double	fCutawayViewSymbolTextSize;	//剖视图符字体高度
		double	fCutawayViewPolyLineWidth;	//剖视图多短线宽度
		//相似形设置
		double	fSimTriScale;		//相似形比例
		double  fSimPolyTextSize;	//相似形标注	 2.0
		//其他字高设置
		double	fLsGuigeTextSize;	//螺栓规格文字高 2.0
		double  fOtherTextSize;		//其余普通文字高度
		double	fPartGuigeTextSize;	//构件规格文字高		2.5
		double	fJgOddmentTextSize;	//角钢正负头标注文字高	2.5
		double  fPlateThickTextSize;//板厚标注		 2.0
	}dim_map;	//图纸上的标注
	double TEXT_X_FACTOR(){return 0.7;}
	BOOL GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);	//从注册表中获取共用参数 wht 11-06-10
	void WritePublicSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);	//保存共用参数至注册表
	BOOL IsDeformed(BOOL bPatternDrawing);
	void InitDrawingEnv();
	CTextItemProp fonts;
	struct TEXTSIZECFG{
		bool bZoomAdaptiveWithScale;	//随绘图比例随动缩放
		WORD wiScaleModel2Draw;			//当前设定字体高度所对应的基准绘图比例=模型尺寸/图纸尺寸　如1:20记为20
		TEXTSIZECFG(){bZoomAdaptiveWithScale=true;wiScaleModel2Draw=20;}
	}summary,chief,segment,welding,part;
	//
	char m_sCompanyName[101];	//公司名称
	void WriteDrawingSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);
	BOOL GetDrawingSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);
};

extern CDrawSysPara sys;
//extern CTextItemProp textItemProp;
#endif