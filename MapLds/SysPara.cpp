#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "SysPara.h"
#include "RxTools.h"
#include "DrawDim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDrawSysPara sys;
//根据螺栓(孔)的图符处理方式，返回相应的螺栓图符(孔)的绘图复合比例系数
double DRAWING_SCALE::ComplexCoef()
{
	if(fabs(fRealToDraw)<EPS)
		return 1.0;
	if(iBoltSymbolStyle==0)
		return 1.0/fRealToDraw;
	else if(iBoltSymbolStyle==1)
		return 1.0;
	else if(iBoltSymbolStyle==2)
		return (1.0/fRealToDraw)*fBoltSymbolFactor;
	else if(iBoltSymbolStyle==3)
		return (1.0/fRealToDraw)*fBoltDiaFactor;
	else
		return 1.0;
}
CDrawSysPara::CDrawSysPara()
{
	//FillMemory(this,sizeof(CDrawSysPara),0);//注释此行否则清空了前面哈希表m_hashMapFrameList的值
	//初始化m_hashMapFrameList
	MAP_FRAME_SIZE mapFrm;
	mapFrm.style = 0;
	strcpy(mapFrm.sMapFrameName,"A0");
	mapFrm.nLength = 1194;
	mapFrm.nWidth = 868;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin
		=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=280;
	mapFrm.nBottomRightFixWidth=160;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 1;
	strcpy(mapFrm.sMapFrameName,"A0-2");
	mapFrm.nLength = 868;
	mapFrm.nWidth = 1194;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin
		=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=280;
	mapFrm.nBottomRightFixWidth=160;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 2;
	strcpy(mapFrm.sMapFrameName,"A1");
	mapFrm.nLength = 841;
	mapFrm.nWidth = 594;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 3;
	strcpy(mapFrm.sMapFrameName,"A1-2");
	mapFrm.nLength = 594;
	mapFrm.nWidth = 841;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 4;
	strcpy(mapFrm.sMapFrameName,"A2");
	mapFrm.nLength = 594;
	mapFrm.nWidth = 420;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 5;
	strcpy(mapFrm.sMapFrameName,"A2-2");
	mapFrm.nLength = 420;
	mapFrm.nWidth = 594;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 6;
	strcpy(mapFrm.sMapFrameName,"A3");
	mapFrm.nLength = 420;
	mapFrm.nWidth = 297;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 7;
	strcpy(mapFrm.sMapFrameName,"A3-2");
	mapFrm.nLength = 297;
	mapFrm.nWidth = 420;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 8;
	strcpy(mapFrm.sMapFrameName,"A4");
	mapFrm.nLength = 297;
	mapFrm.nWidth = 210;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=5;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	mapFrm.style = 9;
	strcpy(mapFrm.sMapFrameName,"A4-2");
	mapFrm.nLength = 210;
	mapFrm.nWidth = 297;
	mapFrm.nLeftMargin=25;
	mapFrm.nRightMargin=mapFrm.nTopMargin
		=mapFrm.nBottomMargin=10;
	mapFrm.nBottomRightFixLen=130;
	mapFrm.nBottomRightFixWidth=50;
	m_hashMapFrameList.SetValue(mapFrm.sMapFrameName,mapFrm);

	int m_iCurMapType=0;        //当前出图类型
	int m_iCurMapSegNo=1;		//当前分段图的分段号
	int m_iCurBodyNo=1;		//当前总图的本体号
	bDrawMainPoleJointLine=TRUE;
	nJointLineOffset = 100;	//分段标记线距离其基准角钢距离
	nJointLineLen = 400;	//分段标记线长度
	nBriefGuigeN=4;
	//常规设置
		//绘图比例
	general_set.LineMapScale = 100;	//单线图比例 一般为1:100 1:200 1:300
	general_set.ForceLineScale = 50;//司令图(力学简化线)  一般为1:50
	general_set.StruMapScale.fRealToDraw = 20;		//结构图比例 一般为1:10 1:20 1:30 1:15
	general_set.StruMapScale.iBoltSymbolStyle=2;	//结构图螺栓图符方式,根据刘伟调查当下电子图纸，统一改为结构图比例图符 wjh-2015.10.08
	general_set.StruMapScale.fBoltSymbolFactor=20;	//结构图螺栓图符比例因子
	general_set.StruMapScale.fBoltDiaFactor=0.5;	//结构图螺栓图符比例直径因子
	general_set.PartMapScale.fRealToDraw = 5;		//构件图比例 一般为1:5 1:10
	general_set.PartMapScale.iBoltSymbolStyle=3;	//构架图螺栓图符方式,根据刘伟调查当下电子图纸，统一改为结构图比例直径图符 wjh-2015.10.08
	general_set.PartMapScale.fBoltSymbolFactor=5;	//构件图螺栓图符比例因子
	general_set.PartMapScale.fBoltDiaFactor=0.5;	//构件图螺栓图符比例直径因子
	general_set.WeldMapScale=general_set.StruMapScale;	//组焊图默认与结构图绘图比例一致
	general_set.WeldMapScale.fRealToDraw=general_set.WeldMapScale.fBoltSymbolFactor=10;
	general_set.fWeldLineSymbolHigh	= 0.8;
	general_set.fWeldLineSymbolSpace= 0.8;
	general_set.fHoleAugmentInNC	= 0.0;	//孔径增大值
	general_set.iDimPrecision		= 0;	//0.1mm 1.0.5mm 2.0.1mm
	general_set.iPartBOMStyle		= 0;	//0.标准样式 1.加宽样式 默认为标准样式
	general_set.iSumMatBOMStyle		= 0;	//0.样式一 1.样式二
	general_set.cMatCharSeparator	= 0;
		//构件工艺卡及其排列
	general_set.nPartRowsN = 4;
	general_set.nRowHigh = 165;				//第行构件图高度
	general_set.nColumnWidth = 250;			//第列构件图宽度
	general_set.bProcessCardTbl = FALSE;	//工艺卡图框
	general_set.bCardLayoutBaseRow=FALSE;	//工艺卡以行为基准
	general_set.bHasFrame = FALSE;			//绘制装订线
	//数字化移交属性设置
	general_set.lCrossarmCr = RGB(255,255,0);  //左横担颜色
	general_set.rCrossarmCr = RGB(255,0,0);//右横担颜色
		//结构图绘制设置
	stru_map.bExportByEnglish		= FALSE;
	stru_map.fMinPartThick			= 12;
	stru_map.fMinWingWide			= 60;
	stru_map.bDimSpecialLsSpace		= 1;
	stru_map.bDimSpecialJgZhun		= 1;
	stru_map.bDimStruJgLen			= 1;
	stru_map.bDimPartGuiGe			= 1;		//标注构件规格 wht 11-03-29
	stru_map.bDrawCutAngleAndWingInfo = 1;
	stru_map.bOnlyDrawHorizontalPlate = FALSE;	//仅绘制在当前结构图中水平显示的钢板 wht 10-06-15
	stru_map.bMergeAttachPlatesToBasePlateInBOM=TRUE;
	stru_map.bUsePolyLineDrawEdge	  = TRUE;	//用多段线绘制角钢和钢板的轮廓边	 wht 10-06-15
	stru_map.fPolyLineWidth = 0.1;              //多段线宽度           xc 10-12-17
	stru_map.fPoleAxisLineStartOutSpreadLen=30;	//杆件心线始端延伸长度 wht 10-12-13
	stru_map.fPoleAxisLineEndOutSpreadLen=30;	//杆件心线终端延伸长度
	stru_map.bReadAdditionalBuffer=TRUE;		//读取附加缓存 wht 11-08-03
		//结构图字高设置
	dim_map.fPartGuigeTextSize		= 2.5;	//构件规格标注文字高
	dim_map.fJgOddmentTextSize		= 2.5;	//角钢正负头标注
	dim_map.fOtherTextSize			= 2.0;	//其余普通文字高度
	dim_map.fSimPolyTextSize		= 2.0;  //相似形标注
	dim_map.fPlateThickTextSize	= 2.0;  //板厚标注
	dim_map.fLsGuigeTextSize		= 2.0;	//螺栓规格标注文字高
	dim_map.fSimTriScale			= 0.2;	//相似形比例 相似形比例默认值修改为0.2 wht 11-05-06
	//组焊图绘图设置
	part_group_map.fMinFontSize		=1.0;			//组焊图中字体最小高度
	part_group_map.fMinDrawThick	=0.5;			//厚度最小绘制值,之前默认为2mm,通过实际验证设为0.5mm即可2mm偏大不好看 wjh-2018.7.29
	part_group_map.nKmJgGIncrement	=0;				//靠模角钢心距调整值
	part_group_map.bModulatePartGroupLen=TRUE;		//调整组焊件长度
	part_group_map.fPartGroupDistThreshold=200;		//组焊图缩放阈值
	part_group_map.fPartGroupDistZoomCoef=0.5;		//组焊图缩放系数
	part_group_map.bDrawWeldLineSymbol=TRUE;		//组焊图中绘制焊缝线标记
	part_group_map.iPlateLocateDrawStyle=1;			//组焊图中以三竖线方式绘制钢板对活点
	part_group_map.iLocateSymboleHeight=5;			//阻焊图中对活点标记高度
	part_group_map.bDimEndOuterPlaneSpace=TRUE;		//杆件外端面到外端面距离
	part_group_map.bAutoDimPartNo=TRUE;				//自动标注构件编号
	part_group_map.bAutoMatchView=TRUE;				//自动组合组焊件视图
	part_group_map.bDimAbsoluteDist=TRUE;			//标注定位点绝对尺寸
	part_group_map.bDimAfterWeldCheckLine=TRUE;		//标注焊接后检测线
	part_group_map.bDimBeforeWeldLocatSpace=TRUE;	//标注焊接前定位尺寸
	part_group_map.bDimPlatePoleRayAndAngle=FALSE;	//标注钢板射线以及射线与基准线之间的夹角
	part_group_map.bSmartFilterRedundantPart=TRUE;	//视图中自动筛选出与当前视图无关的多余构件跳过不显示
	part_group_map.bAngularDimension=TRUE;			//标注角度线
	part_group_map.bCreatePartGroupBOM=TRUE;		//构件明细表
	part_group_map.part_table.bZoomInProcessCard=FALSE;	//工艺卡内进行比例缩放
	part_group_map.part_table.bDispPartGroupNo=FALSE;	//组焊件编号
	part_group_map.part_table.bDispPartGroupNum=FALSE;	//组焊件数量
	part_group_map.part_table.bDispPartNo=TRUE;		//构件编号
	part_group_map.part_table.bDispPartNum=TRUE;	//构件数量
	part_group_map.part_table.bDispPartGuiGe=TRUE;	//构件规格
	part_group_map.part_table.bDispPartLen=TRUE;	//构件长度
	part_group_map.part_table.bDispPartWeight=TRUE;	//构件重量以及同一编号构件重量小计 总重
	part_group_map.part_table.bDispNotes=FALSE;		//备注
	part_group_map.part_table.bDispInternal=FALSE;	//焊缝级别－内在
	part_group_map.part_table.bDispAppearance=FALSE;//焊缝级别－外观
	//构件图绘制设置
	part_map.iExportDeformedProfile=TRUE;	//构件图考虑火曲变形
	part_map.iExportDeformedProfile=1;		//考虑火曲变形
	part_map.bDimDeformedProfileText=TRUE;	//构件图考虑火曲变形时是否标注火曲变形相关文字 wht 10-11-01
	part_map.bDimPlaneAngle=TRUE;			//标注火曲构件面夹角
	part_map.iDimAngleType=2;				//0.锐角;1.钝角;2.自动 wht 10-11-01
	part_map.fMinHuoquAngle=1;				//最小火曲角度阈值
	part_map.bDimHuoquType=TRUE;			//标注构件火曲类型
	part_map.bUseZhengFanQun=FALSE;			//启用正反曲 默认使用外曲内曲 wht 11-05-07
	part_map.iPartMapStyle=0;				//0表示普通;1表示浙江盛达
		//角钢构件图
	part_map.angle.iJgZoomSchema=3;			//0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放
	part_map.angle.ciJgLenDrawMode=0;		//0.向右延伸 1.中间挤压
	part_map.angle.bModulateLongJg=TRUE;	//调整角钢长度
	part_map.angle.bOneCardMultiPart=FALSE;	//角钢允许一卡多件情况
	part_map.angle.iJgGDimStyle=2;			//0.始端标注  1.中间标注 2.自动判断
	part_map.angle.bMaxExtendAngleLength=TRUE;
	part_map.angle.nMaxBoltNumStartDimG=100;//集中在始端标注准距支持的最大螺栓数
	part_map.angle.iLsSpaceDimStyle=2;		//0.X轴方向	  1.Y轴方向  2.自动判断 3.不标注
	part_map.angle.nMaxBoltNumAlongX=50;	//沿X轴方向标注支持的最大螺栓个数
	part_map.angle.bDimCutAngle=TRUE;		//标注角钢切角
	part_map.angle.bDimCutAngleMap=TRUE;	//标注角钢切角示意图
	part_map.angle.bDimPushFlatMap=TRUE;	//标注压扁示意图
	part_map.angle.bJgUseSimpleLsMap=FALSE;	//角钢使用简化螺栓图符
	part_map.angle.bDimLsAbsoluteDist=TRUE;	//标注螺栓绝对尺寸
	part_map.angle.bMergeLsAbsoluteDist=TRUE;//合并相邻等距螺栓绝对尺寸
	part_map.angle.bDimRibPlatePartNo=TRUE;	//标注角钢肋板编号
	part_map.angle.bDimRibPlateMaterial=TRUE;	//标注角钢肋板材质
	part_map.angle.bDimRibPlateSetUpPos=TRUE;//标注角钢肋板安装位置
	part_map.angle.iCutAngleDimType=0;		//角钢切角标注样式 
	//开合角标注
	part_map.angle.bDimKaiHe=TRUE;			//标注角钢开合角
	part_map.angle.bDimKaiheAngleMap=TRUE;	//标注角钢开合角示意图
	part_map.angle.fKaiHeJiaoThreshold=2;	//默认开合角标注阈值为2°
	CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=part_map.angle.fKaiHeJiaoThreshold;	//默认开合角标注阈值为2°
	part_map.angle.fKaiHeJiaoMinDeita=20;	//一般该间隙为10~50mm
	part_map.angle.bDimKaiheSumLen=TRUE;
	part_map.angle.bDimKaiheAngle=TRUE;
	part_map.angle.bDimKaiheSegLen=TRUE;
	part_map.angle.bDimKaiheScopeMap=FALSE;
		//钢板构件图
	part_map.plate.bDimPlateRayLs=TRUE;		//标注板螺栓射线距离
	part_map.plate.bDimPlateBoltAngle=FALSE;//标注板螺栓角度
	part_map.plate.bDimBoltEdgeSpace=FALSE;	//标注板螺栓边距
	part_map.plate.bDimDiffRayLsSpace=FALSE;//标注相邻射线螺栓间距
	part_map.plate.bDimLsToHuoquLine=FALSE;	//标注螺栓到火曲线间距
	part_map.plate.bDimLsToWeldEdge=FALSE;	//标注螺栓到焊接边间距
	part_map.plate.bDimPlateEdgeLen=FALSE;	//标注钢板边长
	part_map.plate.bDrawInnerEdge=FALSE;    //绘制内轮廓边
	part_map.plate.fInnerEdgeOffset=3.0;    //轮廓边偏移距
	part_map.plate.bDimArcEdgeR=FALSE;		//标注钢板圆弧边半径
	part_map.plate.bDimArcAngle=FALSE;		//标注钢板圆弧边角度
	part_map.plate.bAdjustPlateUcs=FALSE;	//调整钢板坐标系保证最长边沿水平方向
	part_map.plate.bDimCoord=FALSE;			//板构件图中是否标注坐标
	part_map.plate.bPlateUseSimpleLsMap=FALSE;		//钢板使用简化螺栓图符
	part_map.plate.bZoomDimTextSizeByWidth=FALSE;	//根据钢板长宽中较小值缩放钢板相关标注的字体高度
	part_map.plate.fTextSizeZoomCoef=0.03;			//字体高度与宽度之间的缩放系数
	part_map.plate.bDrawSlopeCutLine = FALSE;		//焊缝处的坡口线
	part_map.plate.bDrawDatumLine=FALSE;			//绘制钢板的基准线
	part_map.plate.iCutArcsType=0;					//焊烧死角处的切弧方式 0--不切 1--切角 2--切弧
	part_map.plate.iZhiWanLineType=1;				//制弯线位置处理方式
	part_map.plate.fInnerOffThreshold=0;			//制弯线向内偏移阈值
	part_map.plate.bMarkHuoquPos=FALSE;				//标记火曲位置
	part_map.plate.bDrawHuoQuPrjMap=FALSE;			//绘制火曲示意图
	part_map.plate.bDrawHuoQuMarkLine=TRUE;			//绘制火曲特征线
	part_map.plate.bDisplaypPartInfoTbl=FALSE;      //显示构件信息表
	part_map.plate.nPartInfoTblColumnWidth=20;      //构件表列宽
	part_map.plate.bUseDimStyleConfigFile=FALSE;	//启用钢板配置文件
	part_map.plate.bShowPlateCfgInfoDlg=TRUE;		//显示钢板配置对话框
	strcpy(part_map.plate.sPlateConfigFilePath,"");	//钢板配置文件
		//钢管构件图
	part_map.tube.fSlotWidthProcessError=0;	//开槽宽度加工误差值
	part_map.tube.fSlotLenProcessError=0;	//开槽长度加工误差值
	part_map.tube.fReelTubeThresholdD=300;	//钢板卷管直径阈值

	//图纸中的标注参数
		//字体 字高
	strcpy(dim_map.sFontName,"gbhzfs.shx");
		//材料表内数据及结构图内构件编号高：3.0；gbhzfs.shx
	dim_map.fHeaderTextSize		= 6;	//表格表头文字高
	dim_map.fCommonTextSize		= 4;	//普通表格汉字高
	dim_map.fNumericTextSize	= 3;	//表格中的数字高
	dim_map.PartNo.fPartNoTextSize = 3.0;	//构件编号文字高
	dim_map.PartNo.fPartNoCirD = 8.0;		//构件编号圈直径 
	dim_map.PartNo.fMainPoleNoOuterCirD = 14.0;		//主材编号外圆直径
	dim_map.PartNo.fMainPoleNoInnerCirD = 12.0;		//构件编号内圆直径
	dim_map.PartNo.fPartNoMargin   = 0.3;	//编号与编号框之间的间隙值
	dim_map.PartNo.bDrawRefLine	   = FALSE;	//绘制引出线
	dim_map.PartNo.iMirMsg		   = -1;	//无对称
		//段号标注相关参数	wht 11-06-27
	dim_map.fSegICirD		= 10;
	dim_map.fSegITextSize	= 5;

	dim_map.fDimTextSize		= 2.5;	//长度尺寸标注文本高
	dim_map.fDimAngleTextSize	= 2.5;	//角度尺寸标注文本高
	dim_map.fDimArrowSize		= 1.5;	//尺寸标注箭头长
	strcpy(dim_map.sArrowType,"_OBLIQUE");	//尺寸标注箭头类型 箭头样式默认为倾斜 wht 11-05-06
	dim_map.fDimGap				= 0.75;	//文字与标注线间隙值
		//剖视图符
	dim_map.fCutawayViewSymbolLen=8;		//长度
	dim_map.fCutawayViewSymbolTextSize=4;	//字体高度
	dim_map.fCutawayViewPolyLineWidth=0.3;	//剖视图多段线宽度
		//画笔颜色
	dim_map.crIndex.crLs	  = 1;		//螺栓--红色
	dim_map.crIndex.crOutLine = 7;		//轮廓线--黑白
	dim_map.crIndex.crSingleLine= 7;	//单线图--黑白
	dim_map.crIndex.crHuoQuLine = 3;	//火曲线--绿色
	dim_map.crIndex.crDashLine = 1;		//长短画线--红色
	dim_map.crIndex.crAxisLine = 2;		//(心)轴线--黄线
	dim_map.crIndex.crNode = 4;			//节点--青色
	dim_map.crIndex.crDimSimTri= 1;		//标注--黑白
	dim_map.crIndex.crDragLine = 3;		//引出线--绿色
	//各类图纸中字体大小的比例随动设定
	summary.bZoomAdaptiveWithScale=chief.bZoomAdaptiveWithScale=segment.bZoomAdaptiveWithScale=welding.bZoomAdaptiveWithScale=part.bZoomAdaptiveWithScale=true;
	summary.wiScaleModel2Draw=50;	//总图默认按1:50绘图比例时设定字体高度
	chief.wiScaleModel2Draw=50;		//司令图默认按1:50绘图比例时设定字体高度
	segment.wiScaleModel2Draw=20;	//结构图默认按1:20绘图比例时设定字体高度
	welding.wiScaleModel2Draw=10;	//组焊图默认按1:10绘图比例时设定字体高度
	part.wiScaleModel2Draw=5;		//零件图、大样图默认按1:5绘图比例时设定字体高度
	//初始化全局变量的同时，初始化默认典型图层 wht 10-05-07
	//结构图中可见轮廓线图层
	LayerTable::VisibleProfileLayer.layerName.Copy("1");
	LayerTable::VisibleProfileLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::VisibleProfileLayer.lineWeight=AcDb::kLnWt025;
	LayerTable::VisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//白色
	LayerTable::VisibleProfileLayer.layerId=0;
	LayerTable::VisibleProfileLayer.lineTypeId=0;
	//结构图中不可见轮廓线图层
	LayerTable::UnvisibleProfileLayer.layerName.Copy("2");
	LayerTable::UnvisibleProfileLayer.lineTypeName.Copy("HIDDEN");
	LayerTable::UnvisibleProfileLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::UnvisibleProfileLayer.colorIndex=sys.dim_map.crIndex.crDashLine;	//白色
	LayerTable::UnvisibleProfileLayer.layerId=0;
	LayerTable::UnvisibleProfileLayer.lineTypeId=0;
	//受力材单线图、总图
	LayerTable::BriefLineLayer.layerName.Copy("3");
	LayerTable::BriefLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BriefLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::BriefLineLayer.colorIndex=sys.dim_map.crIndex.crOutLine;//白色
	LayerTable::BriefLineLayer.layerId=0;
	LayerTable::BriefLineLayer.lineTypeId=0;
	//辅材单线图
	LayerTable::AuxLineLayer.layerName.Copy("4");
	LayerTable::AuxLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::AuxLineLayer.lineWeight=AcDb::kLnWt018;
	LayerTable::AuxLineLayer.colorIndex=4;	//青色
	LayerTable::AuxLineLayer.layerId=0;
	LayerTable::AuxLineLayer.lineTypeId=0;
	//螺栓图符
	LayerTable::BoltSymbolLayer.layerName.Copy("5");
	LayerTable::BoltSymbolLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BoltSymbolLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BoltSymbolLayer.colorIndex=sys.dim_map.crIndex.crLs;
	LayerTable::BoltSymbolLayer.layerId=0;
	LayerTable::BoltSymbolLayer.lineTypeId=0;
	//相似形标注
	LayerTable::SimPolyLayer.layerName.Copy("6");
	LayerTable::SimPolyLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::SimPolyLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::SimPolyLayer.colorIndex=sys.dim_map.crIndex.crDimSimTri;
	LayerTable::SimPolyLayer.layerId=0;
	LayerTable::SimPolyLayer.lineTypeId=0;
	//中心线、对称线、定位轴线
	LayerTable::AxisLineLayer.layerName.Copy("7");
	LayerTable::AxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::AxisLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::AxisLineLayer.colorIndex=sys.dim_map.crIndex.crAxisLine;
	LayerTable::AxisLineLayer.layerId=0;
	LayerTable::AxisLineLayer.lineTypeId=0;
	//子角钢中心线
	LayerTable::SonJgAxisLineLayer.layerName.Copy("7A");
	LayerTable::SonJgAxisLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::SonJgAxisLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::SonJgAxisLineLayer.colorIndex=sys.dim_map.crIndex.crAxisLine;
	LayerTable::SonJgAxisLineLayer.layerId=0;
	LayerTable::SonJgAxisLineLayer.lineTypeId=0;
	//辅助火曲线
	LayerTable::AssistHuoQuLineLayer.layerName.Copy("8");
	LayerTable::AssistHuoQuLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::AssistHuoQuLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::AssistHuoQuLineLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::AssistHuoQuLineLayer.layerId=0;
	LayerTable::AssistHuoQuLineLayer.lineTypeId=0;
	//角钢火曲、钢板火曲
	LayerTable::BendLineLayer.layerName.Copy("8");
	LayerTable::BendLineLayer.lineTypeName.Copy("DIVIDE");
	LayerTable::BendLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BendLineLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::BendLineLayer.layerId=0;
	LayerTable::BendLineLayer.lineTypeId=0;
	//断开界线
	LayerTable::BreakLineLayer.layerName.Copy("9");
	LayerTable::BreakLineLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::BreakLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BreakLineLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::BreakLineLayer.layerId=0;
	LayerTable::BreakLineLayer.lineTypeId=0;
	//螺栓线
	LayerTable::BoltLineLayer.layerName.Copy("10");
	LayerTable::BoltLineLayer.lineTypeName.Copy("DASHDOT2X");
	LayerTable::BoltLineLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::BoltLineLayer.colorIndex=sys.dim_map.crIndex.crDimSimTri;
	LayerTable::BoltLineLayer.layerId=0;
	LayerTable::BoltLineLayer.lineTypeId=0;
	//尺寸标注
	LayerTable::DimSizeLayer.layerName.Copy("11");
	LayerTable::DimSizeLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimSizeLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::DimSizeLayer.colorIndex=sys.dim_map.crIndex.crHuoQuLine;
	LayerTable::DimSizeLayer.layerId=0;
	LayerTable::DimSizeLayer.lineTypeId=0;
	//文字标注
	LayerTable::DimTextLayer.layerName.Copy("12");
	LayerTable::DimTextLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::DimTextLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::DimTextLayer.colorIndex=sys.dim_map.crIndex.crOutLine;
	LayerTable::DimTextLayer.layerId=0;
	LayerTable::DimTextLayer.lineTypeId=0;
	//除以上以外的所有图元所在图层
	LayerTable::CommonLayer.layerName.Copy("13");
	LayerTable::CommonLayer.lineTypeName.Copy("CONTINUOUS");
	LayerTable::CommonLayer.lineWeight=AcDb::kLnWt013;
	LayerTable::CommonLayer.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::CommonLayer.layerId=0;
	LayerTable::CommonLayer.lineTypeId=0;
	//板边破损标记线
	LayerTable::DamagedSymbolLine.layerName.Copy("14");
	LayerTable::DamagedSymbolLine.lineTypeName.Copy("ZIGZAG");
	LayerTable::DamagedSymbolLine.lineWeight=AcDb::kLnWt013;
	LayerTable::DamagedSymbolLine.colorIndex=sys.dim_map.crIndex.crDragLine;
	LayerTable::DamagedSymbolLine.layerId=0;
	LayerTable::DamagedSymbolLine.lineTypeId=0;
	//客户定制化开发
	strcpy(m_sCompanyName,"");
}


//从注册表中获取共用参数 wht 11-06-10
BOOL CDrawSysPara::GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"KaiHeJiaoThreshold")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.angle.fKaiHeJiaoThreshold=atof(sValue);
			else 
				return FALSE;
			CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=part_map.angle.fKaiHeJiaoThreshold;	//默认开合角标注阈值为2°
		}
		else if(stricmp(lpszEntry,"KaiHeJiaoMinDeita")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.angle.fKaiHeJiaoMinDeita=atof(sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MatCharPosType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				general_set.iMatCharPosType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MatSeparator")==0)
		{
			DWORD dwValue=0;
			if(RegQueryValueEx(hKey,lpszEntry, NULL, &dwDataType,(BYTE*)&dwValue, &dwLength)== ERROR_SUCCESS)
				general_set.cMatCharSeparator=(BYTE)dwValue;
			else
				return FALSE;
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				CLDSPlate::m_iPlankWeightStatType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"m_iCirclePlankAreaType")==0)
		{
			DWORD dwValue=0;
			dwLength=sizeof(DWORD);
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
				CLDSParamPlate::m_iCirclePlankAreaType = (UINT)dwValue;
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"MinHuoquAngle")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.fMinHuoquAngle=atof(sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"InnerOffThreshold")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				part_map.plate.fInnerOffThreshold=atof(sValue);
			else 
				return FALSE;
		}
		RegCloseKey(hKey);
	}
	else 
		return FALSE;
	return TRUE;
}
void CDrawSysPara::WritePublicSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	//保存共用参数至注册表
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwLength=0;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"KaiHeJiaoThreshold")==0)
		{
			sprintf(sValue,"%f",part_map.angle.fKaiHeJiaoThreshold);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"KaiHeJiaoMinDeita")==0)
		{
			sprintf(sValue,"%f",part_map.angle.fKaiHeJiaoMinDeita);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"MatCharPosType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&general_set.iMatCharPosType,dwLength);
		}
		else if(stricmp(lpszEntry,"MatSeparator")==0)
		{
			dwLength=sizeof(int);
			BYTE biValue=(BYTE)general_set.cMatCharSeparator;
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&biValue,dwLength);
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&CLDSPlate::m_iPlankWeightStatType,dwLength);
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			dwLength=sizeof(int);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&CLDSParamPlate::m_iCirclePlankAreaType,dwLength);
		}
		else if(stricmp(lpszEntry,"MinHuoquAngle")==0)
		{
			sprintf(sValue,"%f",part_map.fMinHuoquAngle);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"InnerOffThreshold")==0)
		{
			sprintf(sValue,"%f",part_map.plate.fInnerOffThreshold);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		RegCloseKey(hKey);
	}
}
BOOL CDrawSysPara::GetDrawingSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwDataType,dwLength=MAX_PATH;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"CompanyName")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				strcpy(m_sCompanyName,sValue);
			else 
				return FALSE;
		}
		else if(stricmp(lpszEntry,"Critic")==0)
		{
			if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
				strcpy(Ta.m_sCritic,sValue);
			else 
				return FALSE;
		}
		RegCloseKey(hKey);
	}
	else 
		return FALSE;
	return TRUE;
}
void CDrawSysPara::WriteDrawingSysParaToReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	char sValue[MAX_PATH]="";
	char sSubKey[MAX_PATH]="";
	DWORD dwLength=0;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sSubKey,"Software\\Xerofox\\TMA-EN\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#endif
#elif defined(__LMA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#elif defined(__TDA_FILE_)
	sprintf(sSubKey,"Software\\Xerofox\\TDA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(stricmp(lpszEntry,"CompanyName")==0)
		{
			sprintf(sValue,"%s",m_sCompanyName);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		else if(stricmp(lpszEntry,"Critic")==0)
		{
			sprintf(sValue,"%s",Ta.m_sCritic);
			dwLength=strlen(sValue);
			RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		}
		RegCloseKey(hKey);
	}
}
void CDrawSysPara::InitDrawingEnv()
{
	CSizeTextDim::dimStyle.textHeight=sys.dim_map.fDimTextSize;
	//CKaiHeAngleDim::dimStyle.textHeight=sys.dim_map.fDimTextSize;
}
BOOL CDrawSysPara::IsDeformed(BOOL bPatternDrawing)
{
	if(sys.part_map.iExportDeformedProfile==1)	//都考虑火曲变形
		return TRUE;
	else if(sys.part_map.iExportDeformedProfile==2&&bPatternDrawing)	//大样图考虑火曲变形
		return TRUE;
	else if(sys.part_map.iExportDeformedProfile==3&&!bPatternDrawing)	//工艺卡考虑火曲变形
		return TRUE;
	else
		return FALSE;
}
CStringKeyHashTable<long>CTextItemProp::propHashtable;	//属性字符串哈希表
const DWORD HASHTABLESIZE = 500;
CTextItemProp textItemProp;
CTextItemProp::CTextItemProp()
{
	_pSysPara=NULL;
	InitTextItemPropHashtable();
	TEXT_PROP textProp;
	textProp.fHigh = 2.0;
	textProp.fWtoH = 0.7;
	sprintf(textProp.sFont,"%s","gbhzfs.shx");
	SetTextItemProp("DimText",textProp);
	SetTextItemProp("DimAngleText",textProp);
	SetTextItemProp("PartNo",textProp);
	textProp.fHigh = 2.0;
	SetTextItemProp("PartGuigeText",textProp);
	SetTextItemProp("JgOddmentText",textProp);
	textProp.fHigh = 2;
	SetTextItemProp("LsGuigeText",textProp);
	SetTextItemProp("PlateThickText",textProp);
	SetTextItemProp("OtherText",textProp);
	textProp.fHigh = 6;
	SetTextItemProp("HeaderText",textProp);
	textProp.fHigh = 4;
	SetTextItemProp("CommonText",textProp);
	textProp.fHigh = 3;
	SetTextItemProp("NumericText",textProp);
	textProp.fHigh = 0.3;
	textProp.fWtoH = 0.7;
	SetTextItemProp("PartNoMargin",textProp);
	textProp.fHigh = 2;
	textProp.fWtoH = 0.3;
	SetTextItemProp("SimPolyText",textProp);
	textProp.fHigh = 4;//高度
	textProp.fWtoH = 8;//宽度
	sprintf(textProp.sFont,"%s","黑粗体");
	SetTextItemProp("CutawayViewSymbolText",textProp);

	textProp.fHigh = 4;
	textProp.fWtoH = 10;//圆圈直径
	SetTextItemProp("SegCircleD",textProp);
	textProp.fHigh = 3;
	textProp.fWtoH = 14;//圆圈直径
	SetTextItemProp("MainPartCircleD",textProp);
	textProp.fWtoH = 12;//圆圈直径
	SetTextItemProp("MainPartInsertCircleD",textProp);
	textProp.fWtoH = 8;//圆圈直径
	SetTextItemProp("OtherPartCircleD",textProp);
	textProp.fHigh = 4;//斜线
	textProp.fWtoH = 2;//圆圈直径
	SetTextItemProp("LsMapM16AndM20",textProp);
	textProp.fHigh = 4.8;//斜线
	textProp.fWtoH = 2.4;//圆圈直径
	SetTextItemProp("LsMapM24",textProp);
	textProp.fHigh = 4;//斜线
	textProp.fWtoH = 10;//圆圈直径
	SetTextItemProp("Seg",textProp);
}

void CTextItemProp::SetTextSize(char *sType,double high, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	textProp.fHigh = high;
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

void CTextItemProp::SetTextWtoH(char *sType,double WtoH, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	textProp.fWtoH = WtoH;
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

void CTextItemProp::SetTextFont(char *sType,char* font, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	TEXT_PROP textProp;
	sprintf(textProp.sFont,"%s",sType);
	SetTextItemProp(sType,textProp,class_datum0_sum1_chief2_seg3_part4_weld5);
}

bool CTextItemProp::GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,TEXT_PROP* pFontStyle)
{
	long id = GetPropID(sType);
	TEXT_PROP *pQueryFontStyle=NULL;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		pQueryFontStyle=hashListProp.GetValue(combinedId);
	}
	if(pQueryFontStyle==NULL)
		pQueryFontStyle = hashListProp.GetValue(id);
	if(pQueryFontStyle!=NULL)
		*pFontStyle=*pQueryFontStyle;
	else
		return false;
	return true;
}
double CTextItemProp::GetSignProp(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5,
								  int txt_size0_wtoh1/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP *datumTextProp=NULL;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp=hashListProp.GetValue(combinedId);
		if(datumTextProp)
		{
			if(txt_size0_wtoh1==0)
				return datumTextProp->fHigh;
			else
				return datumTextProp->fWtoH;
		}
		else
		{
			datumTextProp=hashListProp.GetValue(id);
			if(datumTextProp)
			{
				if(txt_size0_wtoh1==0)
					return datumTextProp->fHigh;
				else
					return datumTextProp->fWtoH;
			}
			else
				return 0;
		}
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
		{
			if(txt_size0_wtoh1==0)
				return datumTextProp->fHigh;
			else
				return datumTextProp->fWtoH;
		}
		else
			return 0;
	}
}

double CTextItemProp::TextSize(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	return GetSignProp(sType,class_datum0_sum1_chief2_seg3_part4_weld5);
}

double CTextItemProp::TextWtoH(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	return GetSignProp(sType,class_datum0_sum1_chief2_seg3_part4_weld5,1);
}

char* CTextItemProp::TextFont(char *sType, BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP *datumTextProp;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp = hashListProp.GetValue(combinedId);
		if(datumTextProp)	
			return datumTextProp->sFont;
		else
		{
			datumTextProp = hashListProp.GetValue(id);
			if(datumTextProp)
				return datumTextProp->sFont;
			else
				return NULL;
		}
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
			return datumTextProp->sFont;
		else
			return NULL;
	}
}
void CTextItemProp::SetTextItemProp(char *sType,TEXT_PROP &textProp,
									BYTE class_datum0_sum1_chief2_seg3_part4_weld5/*=0*/)
{
	long id = GetPropID(sType);
	TEXT_PROP* datumTextProp;
	if(class_datum0_sum1_chief2_seg3_part4_weld5!=0)
	{
		long iFirstSign = class_datum0_sum1_chief2_seg3_part4_weld5;
		long combinedId = (iFirstSign<<24)+id;
		datumTextProp = hashListProp.GetValue(combinedId);
		if(!datumTextProp)
		{
			datumTextProp = hashListProp.GetValue(id);
			if(!datumTextProp)
				return;
		}
		if(textProp.fHigh == 0)
			textProp.fHigh = datumTextProp->fHigh;
		if(textProp.fWtoH == 0)
			textProp.fWtoH = datumTextProp->fWtoH;
		if(strlen(textProp.sFont)==0)
			sprintf(textProp.sFont,"%s",datumTextProp->sFont);
		hashListProp.SetValue(combinedId,textProp);
	}
	else
	{
		datumTextProp = hashListProp.GetValue(id);
		if(datumTextProp)
		{
			if(textProp.fHigh == 0)
				textProp.fHigh = datumTextProp->fHigh;
			if(textProp.fWtoH == 0)
				textProp.fWtoH = datumTextProp->fWtoH;
			if(strlen(textProp.sFont)==0)
				sprintf(textProp.sFont,"%s",datumTextProp->sFont);
		}
		hashListProp.SetValue(id,textProp);
	}
}

long CTextItemProp::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
		return 0;
}
void CTextItemProp::InitTextItemPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("DimText",id++);		    //尺寸标注长度
	propHashtable.SetValueAt("DimAngleText",id++);	//尺寸标注角度
	propHashtable.SetValueAt("PartNoMargin",id++);      //字框间距
	propHashtable.SetValueAt("PartGuigeText",id++);			//构件规格长度
	propHashtable.SetValueAt("SimPolyText",id++);       //相似形
	propHashtable.SetValueAt("JgOddmentText",id++);		//正负头
	propHashtable.SetValueAt("PartNo",id++);			//构件编号
	propHashtable.SetValueAt("LsGuigeText",id++);       //螺栓规格个数标注
	propHashtable.SetValueAt("PlateThickText",id++);    //板厚材质标注
	propHashtable.SetValueAt("HeaderText",id++);     //明细表头
	propHashtable.SetValueAt("CommonText",id++);	 //标题
	propHashtable.SetValueAt("NumericText",id++);    //明细表内容
	propHashtable.SetValueAt("CutawayViewSymbolText",id++); //剖面符号
	propHashtable.SetValueAt("OtherText",id++);	        //其余文字

	propHashtable.SetValueAt("LsSpaceDimText",id++);	//螺栓间距标注
	propHashtable.SetValueAt("HuoQuSpaceDimText",id++);	//火曲间距标注
	propHashtable.SetValueAt("SegCircleD",id++);		//段号圆圈直径
	propHashtable.SetValueAt("MainPartCircleD",id++);	//主材构件外圆圈直径
	propHashtable.SetValueAt("MainPartInsertCircleD",id++);	//主材构件内圆圈直径
	propHashtable.SetValueAt("OtherPartCircleD",id++);	//其他构件内圆圈直径
	propHashtable.SetValueAt("LsMapM16AndM20",id++);	//螺栓图符M16M20
	propHashtable.SetValueAt("LsMapM24",id++);			//螺栓图符M24
	propHashtable.SetValueAt("Seg",id++);				//段号
}

void CTextItemProp::InitSpecialProp(BYTE class_datum0_sum1_chief2_seg3_part4_weld5)
{
	sys.dim_map.fHeaderTextSize	= TextSize("HeaderText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fCommonTextSize	= TextSize("CommonText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fNumericTextSize = TextSize("NumericText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.PartNo.fPartNoTextSize = TextSize("PartNo",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fDimTextSize = TextSize("DimText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fDimAngleTextSize = TextSize("DimAngleText",class_datum0_sum1_chief2_seg3_part4_weld5);	
	sys.dim_map.fCutawayViewSymbolTextSize = TextSize("CutawayViewSymbolText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fCutawayViewSymbolLen = TextWtoH("CutawayViewSymbolText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.PartNo.fPartNoMargin = TextSize("PartNoMargin",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fPartGuigeTextSize = TextSize("PartGuigeText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fJgOddmentTextSize = TextSize("JgOddmentText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fOtherTextSize = TextSize("OtherText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fSimPolyTextSize = TextSize("SimPolyText",class_datum0_sum1_chief2_seg3_part4_weld5);
	//sys.dim_map.fSimTriScale = TextWtoH("SimPolyText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fPlateThickTextSize = TextSize("PlateThickText",class_datum0_sum1_chief2_seg3_part4_weld5);
	sys.dim_map.fLsGuigeTextSize = TextSize("LsGuigeText",class_datum0_sum1_chief2_seg3_part4_weld5);
}
/////////////////////////////////////////////////////////////////////
FRAMESIZE IFontStyleConfig::GetLabelFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iPartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
FRAMESIZE IFontStyleConfig::GetLabelFlFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iFlPartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
FRAMESIZE IFontStyleConfig::GetLabelEndPlateFrame()
{
	FRAMESIZE frame;
	CDrawSysPara* pSysPara=(Parent()->pSysPara!=NULL)?Parent()->pSysPara:&sys;
	frame.siFrameStyle=pSysPara->dim_map.PartNo.iStdInsertPlatePartNoFrameStyle;
	frame.fFrameWidth=pSysPara->dim_map.PartNo.fPartNoCirD;
	frame.fMargin=this->fPartNoMargin;
	frame.fTextHeight=this->fPartNoTextSize;
	return frame;
}
//IFontStyleConfig标注文字高度属性
//表格表头文字高
IMPLEMENT_GET(IFontStyleConfig,fHeaderTextSize)
{
	return Parent()->TextSize("HeaderText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fHeaderTextSize)
{
	Parent()->SetTextSize("HeaderText",value,GetDrawingTypeId());
}
//普通表格标题汉字高
IMPLEMENT_GET(IFontStyleConfig,fCommonTextSize)
{
	return Parent()->TextSize("CommonText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fCommonTextSize)
{
	Parent()->SetTextSize("CommonText",value,GetDrawingTypeId());
}
//表格中数字文字高
IMPLEMENT_GET(IFontStyleConfig,fNumericTextSize)
{
	return Parent()->TextSize("NumericText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fNumericTextSize)
{
	Parent()->SetTextSize("NumericText",value,GetDrawingTypeId());
}
//构件编号文字高
IMPLEMENT_GET(IFontStyleConfig,fPartNoTextSize)
{
	return Parent()->TextSize("PartNo",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartNoTextSize)
{
	Parent()->SetTextSize("PartNo",value,GetDrawingTypeId());
}
//长度尺寸标注文本高
IMPLEMENT_GET(IFontStyleConfig,fDimTextSize)
{
	return Parent()->TextSize("DimText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fDimTextSize)
{
	Parent()->SetTextSize("DimText",value,GetDrawingTypeId());
}
//角度尺寸标注文本高
IMPLEMENT_GET(IFontStyleConfig,fDimAngleTextSize)
{
	return Parent()->TextSize("DimAngleText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fDimAngleTextSize)
{
	Parent()->SetTextSize("DimAngleText",value,GetDrawingTypeId());
}
//剖视图符
//DECLARE_PROPERTY(double,fCutawayViewSymbolLen);		//剖视图符长度
//剖视图符字体高度
IMPLEMENT_GET(IFontStyleConfig,fCutawayViewSymbolTextSize)
{
	return Parent()->TextSize("CutawayViewSymbolText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fCutawayViewSymbolTextSize)
{
	Parent()->SetTextSize("CutawayViewSymbolText",value,GetDrawingTypeId());
}
//结构图标注
//构件编号与编号框之间的间隙值
IMPLEMENT_GET(IFontStyleConfig,fPartNoMargin)
{
	return Parent()->TextSize("PartNoMargin",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartNoMargin)
{
	Parent()->SetTextSize("PartNoMargin",value,GetDrawingTypeId());
}
//构件规格文字高		2.5
IMPLEMENT_GET(IFontStyleConfig,fPartGuigeTextSize)
{
	return Parent()->TextSize("PartGuigeText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPartGuigeTextSize)
{
	Parent()->SetTextSize("PartGuigeText",value,GetDrawingTypeId());
}
//角钢正负头标注文字高	2.5
IMPLEMENT_GET(IFontStyleConfig,fJgOddmentTextSize)
{
	return Parent()->TextSize("JgOddmentText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fJgOddmentTextSize)
{
	Parent()->SetTextSize("JgOddmentText",value,GetDrawingTypeId());
}
//板厚标注		 2.0
IMPLEMENT_GET(IFontStyleConfig,fPlateThickTextSize)
{
	return Parent()->TextSize("PlateThickText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fPlateThickTextSize)
{
	Parent()->SetTextSize("PlateThickText",value,GetDrawingTypeId());
}
//螺栓规格文字高 2.0
IMPLEMENT_GET(IFontStyleConfig,fLsGuigeTextSize)
{
	return Parent()->TextSize("LsGuigeText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fLsGuigeTextSize)
{
	Parent()->SetTextSize("LsGuigeText",value,GetDrawingTypeId());
}
//DECLARE_PROPERTY(double,fSimTriScale);		//相似形比例
//相似形标注	 2.0
IMPLEMENT_GET(IFontStyleConfig,fSimPolyTextSize)
{
	return Parent()->TextSize("SimPolyText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fSimPolyTextSize)
{
	Parent()->SetTextSize("SimPolyText",value,GetDrawingTypeId());
}
//其余普通文字高度
IMPLEMENT_GET(IFontStyleConfig,fOtherTextSize)
{
	return Parent()->TextSize("OtherText",GetDrawingTypeId())*FontScale();
}
IMPLEMENT_SET(IFontStyleConfig,fOtherTextSize)
{
	Parent()->SetTextSize("OtherText",value,GetDrawingTypeId());
}
