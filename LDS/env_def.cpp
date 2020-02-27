//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "macro.h"
#include "env_def.h"
#include "Interpolator.h"
#include "LogFile.h"
#include "XhLicAgent.h"
#include "LdsPortalLocale.h"

//CTower Ta;	//已移至Global.cpp中，否则Ta如果初始化晚于其余参数会出问题 wjh-2019.10.13
CSysParaLibrary g_sysPara;
//#ifdef __LDS_CONTEXT_
//#include ".\StdComTempl\IStdComTempl.h"
//规范代码，移至StdComTempl.cpp中，并且已通过Global.cpp处理了全局变量初始化先后顺序的问题 wjh-2019.10.14
//CStdComTmplLibrary globalStdComTmplLibrary;
//#endif
CSysParaLibrary::WASHERTHICK_ITEMS::WASHERTHICK_ITEMS()
{
	cnMaxWasherN=2;
	memset(this,0,sizeof(WASHERTHICK_ITEMS));
}
CXhChar16 CSysParaLibrary::WASHERTHICK_ITEMS::ToThickStr(BYTE thicks[4])
{
	CXhChar16 thickstr;
	for(int i=0;i<4;i++)
	{
		if(thicks[i]>0)
			thickstr.Append(CXhChar16("%d",thicks[i]),',');
	}
	return thickstr;
}
int CSysParaLibrary::WASHERTHICK_ITEMS::FromThickStr(const char* thickstr,BYTE thicks[4])
{
	CXhChar50 xarrWords[4];
	UINT i, count=DELIMITER_STR::ParseWordsFromStr(thickstr,",;|",xarrWords,4);
	for(i=0;i<4;i++)
		thicks[i]=(i<count)?(BYTE)atoi(xarrWords[i]):0;
	return count;
}
CXhChar16 CSysParaLibrary::WASHERTHICK_ITEMS::ToThickStr(int d)
{
	if(d==12)
		return ToThickStr(this->xarrM12Thicks);
	else if(d==16)
		return ToThickStr(this->xarrM16Thicks);
	else if(d==20)
		return ToThickStr(this->xarrM20Thicks);
	else if(d==24)
		return ToThickStr(this->xarrM24Thicks);
	else
		return "";
}
int CSysParaLibrary::WASHERTHICK_ITEMS::FromThickStr(const char* thickstr,int d)
{
	if(d==12)
		return FromThickStr(thickstr,this->xarrM12Thicks);
	else if(d==16)
		return FromThickStr(thickstr,this->xarrM16Thicks);
	else if(d==20)
		return FromThickStr(thickstr,this->xarrM20Thicks);
	else if(d==24)
		return FromThickStr(thickstr,this->xarrM24Thicks);
	else
		return 0;
}
CSysParaLibrary::CSysParaLibrary()
{
	InitSysPara();
}
void CSysParaLibrary::InitSysPara()
{
	bDisplayAllHole=FALSE;
	//显示条件1
	char0_arr[0]=	"STL";
	char0_arr[1]=	"STL";
	char0_arr[2]=	"STL";
	char0_arr[3]=	"STL";
	char0_arr[4]=	"STL";
	char0_arr[5]=	"STL";
	//显示条件2
	char1_arr[0]= "PZXFH";	
	char1_arr[1]= "PZXFH";	
	char1_arr[2]= "PZXFH";	
	char1_arr[3]= "PZXFH";	
	char1_arr[4]= "PZXFH";	
	char1_arr[5]= "PZXFH";	
	//显示条件3
	char2_arr[0]=	"12Q";	//前面视图
	char2_arr[1]=	"34H";	//后面视图
	char2_arr[2]=	"Y23";	//右面视图
	char2_arr[3]=	"Z14";	//左面视图
	char2_arr[4]=	"1234QHZYT";
	char2_arr[5]=	"1234QHZYT";
	display.bDispVirtualPart = TRUE;
	fRotateVelocity = 30;	//Z轴方向的范围
	iFileSaveDocType=0;	//1.*.tma;2.*.lma;3.*.tsa;4.*.lds;5.*.tda
	uFileSaveVersion=0;
	ciLinkAngleDlgFocusCtrlMode=0;	//自动设定
	m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL2_COMMON;
	memset(bNodeArr,1,sizeof(BOOL)*6);

	dock.pageModel.bDisplay=TRUE;
	dock.pageModel.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.pageProp.bDisplay=TRUE;
	dock.pageProp.uDockBarID=AFX_IDW_DOCKBAR_LEFT;
	dock.pageCmd.bDisplay=TRUE;
	dock.pageCmd.uDockBarID=AFX_IDW_DOCKBAR_BOTTOM;
	dock.pageOutput.bDisplay=TRUE;
	dock.pageOutput.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.pageRevision.bDisplay=TRUE;
	dock.pageRevision.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.pageLogger.bDisplay=FALSE;
	dock.pageLogger.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.pagePartLabel.bDisplay=TRUE;
	dock.pagePartLabel.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.pageWeldPart.bDisplay=TRUE;
	dock.pageWeldPart.uDockBarID=AFX_IDW_DOCKBAR_RIGHT;
	dock.m_nLeftPageWidth=270;
	dock.m_nRightPageWidth=250;
	dock.m_nBottomPageHeight=100;
	bMoveDispFrame	= FALSE;	//实体移动时显示线框图
	display.bNodePnt	= TRUE;		//显示单线节点
	display.bPoleLine	= TRUE;		//显示单线杆件线
	display.bPartSketch	= FALSE;	//显示构件轮廓边以及轮廓点
	display.bLoadArrow= TRUE;		//显示荷载箭头
	display.bWirePoint= TRUE;		//显示挂点挂线方向
	display.nLoadArrowScale = 50;	//荷载箭头显示大小
	display.xPointStyle.cbPointSize	= 6;
	display.xPointStyle.ciPointStyle= 1;//0.表示方点显示;1表示圆点显示
	display.bDimNodeLabel=TRUE;			//标注节点编号
	display.nNodeLabelTextHeight=80;	//节点编号字高
	display.bDimRodDesignInfo=FALSE;		//标注杆件规格
	display.nRodDesignInfoTextHeight=80;//杆件规格字高
	//display.bDimMemberEffic=TRUE;	//标注单元设计裕度
	//display.nMemberEfficTextHeight=35;	//单元设计裕度字高
	display.bSolidJg	= TRUE;		//显示实体角钢
	display.bSolidTube	= TRUE;		//显示实体钢管
	display.bSolidJoint	= TRUE;	//显示实体接头
	display.bSolidLjb	= TRUE;		//显示实体连接板
	display.bSolidLs	= TRUE;		//显示实体螺栓
	display.bSolidSphere	= TRUE;		//显示球体
	display.bSolidSlot	= TRUE;		//显示实体槽钢
	display.bSolidFlat	= TRUE;		//显示实体扁钢
	display.bSolidArcAngle= TRUE;		//显示实体环形角钢
	display.bSolidArcFlat	= TRUE;		//显示实体环形扁钢
	display.bSolidArcSlot	= TRUE;		//显示实体环形槽钢
	display.bSolidStdPart	= TRUE;		//显示实体标准构件
	display.bSolidLine		= TRUE;		//显示实体单线
	display.bDispVirtualPart = FALSE;	//显示虚拟辅助构件
	display.bSolidFoundation = TRUE;
	display.nSmoothness		= 36;		//默认为36面
	display.bSolidTransectFace = FALSE;
	display.cHoleWallMode=1;	//0.不显示;1.仅显示虚拟螺栓孔;2.显示全部孔壁
	//格式刷功能
#if defined(__LDS_FILE_)||defined(__LDS_)
	brush.bMaterial=TRUE;
	brush.bLayer=TRUE;
	brush.bGuige=TRUE;
	brush.bSeg=TRUE;
	brush.bCfgword=TRUE;
	brush.bDisplayHole=TRUE;
	brush.bVirtualPart=TRUE;
	brush.bColour=TRUE;
#endif
	flSelectLsTlb_N = 10;
	flSelectLs_table[0].SetProPorty(4.8,12,20,30);
	flSelectLs_table[1].SetProPorty(4.8,16,25,35);
	flSelectLs_table[2].SetProPorty(6.8,20,30,40);
	flSelectLs_table[3].SetProPorty(8.8,24,40,50);
	flSelectLs_table[4].SetProPorty(8.8,27,50,60);
	flSelectLs_table[5].SetProPorty(8.8,30,55,65);
	flSelectLs_table[6].SetProPorty(8.8,36,60,70);
	flSelectLs_table[7].SetProPorty(8.8,39,70,80);
	flSelectLs_table[8].SetProPorty(8.8,42,80,90);
	flSelectLs_table[9].SetProPorty(8.8,48,80,90);
	//GetSingleWord(SELECTINDEX_NODE)=1;但由于初始化全局变量时，还未导入证书故不能调用GetSingleWord() wjh-2015.4.16
	m_dwSelectPartsType = 0XFFFFFFFF-0x00000001;
	m_fMaxScaleScr2User = 0.5;
	//力学分析设计参数
#if defined(__LDS_)||defined(__TSA_)

	bCalStdFoundationForce=FALSE;
	bUpdateRodDesignForce=FALSE;
	bCreateAnalFile=FALSE;
	bRecNodeOffset=TRUE;
	bCreateAnsysCmdFile=FALSE;
	bCreateDetailRptFile=TRUE;
	iDetailDocType=0;
	iLanguage=0;	//中文
	reactionExportType=1;
	bMergeRptFileMirObj=TRUE;
	iLegShiftType=1;	//0:不进行任何轮换;1:进行等高腿轮换;2:进行高低腿轮换
	iEfficacyCeofType = 0; //0.效用系数|1.承载系数
	m_bAuxPoleAnalysisInc=FALSE;
	//通过颜色显示单元设计效用比时的颜色条显示刻度,不含0,100%,>100%必显项
	efficColorBarScaleArr.Resize(3);
	efficColorBarScaleArr[0]=0.55;
	efficColorBarScaleArr[1]=0.85;
	efficColorBarScaleArr[2]=0.95;
	//风振系数插值表默认值
	InterpBetaZArr.Resize(10);
	InterpBetaZArr[0].pivot_key=10;	InterpBetaZArr[0].val_des=1.40;	InterpBetaZArr[0].val_std=1.20;
	InterpBetaZArr[1].pivot_key=20;	InterpBetaZArr[1].val_des=1.45;	InterpBetaZArr[1].val_std=1.22;
	InterpBetaZArr[2].pivot_key=30;	InterpBetaZArr[2].val_des=1.50;	InterpBetaZArr[2].val_std=1.25;
	InterpBetaZArr[3].pivot_key=40;	InterpBetaZArr[3].val_des=1.55;	InterpBetaZArr[3].val_std=1.27;
	InterpBetaZArr[4].pivot_key=50;	InterpBetaZArr[4].val_des=1.60;	InterpBetaZArr[4].val_std=1.30;
	InterpBetaZArr[5].pivot_key=60;	InterpBetaZArr[5].val_des=1.62;	InterpBetaZArr[5].val_std=1.31;
	InterpBetaZArr[6].pivot_key=70;	InterpBetaZArr[6].val_des=1.64;	InterpBetaZArr[6].val_std=1.32;
	InterpBetaZArr[7].pivot_key=80;	InterpBetaZArr[7].val_des=1.66;	InterpBetaZArr[7].val_std=1.33;
	InterpBetaZArr[8].pivot_key=90;	InterpBetaZArr[8].val_des=1.68;	InterpBetaZArr[8].val_std=1.34;
	InterpBetaZArr[9].pivot_key=100;	InterpBetaZArr[9].val_des=1.70;	InterpBetaZArr[9].val_std=1.35;
	//杆塔覆冰重力荷载系数
	InterpIceLoadCoefArr.Resize(3);
	InterpIceLoadCoefArr[0].pivot_key=15; InterpIceLoadCoefArr[0].val_des=1.2;
	InterpIceLoadCoefArr[1].pivot_key=20; InterpIceLoadCoefArr[1].val_des=1.5;
	InterpIceLoadCoefArr[2].pivot_key=30; InterpIceLoadCoefArr[2].val_des=2.0;
	//弯矩项
	nM = 1;
	nSigemaM = 1;
	nPrecentSigemaM = 1;
	iShowDeformedMapPlan = 0;  //0.位移变形|1.效用系数|2.承载系数|3.内力比
	AnalysisPara.m_iNonlinearStyle=0;	//否则以线性方式计算
	AnalysisPara.m_iResolverStyle=1;	//求解器方式,默认为波前法
	AnalysisPara.m_nLoadStepCounts=1;	//分几次加载
	AnalysisPara.m_nIterationCounts=25;	//每次增量最多迭代次数
	AnalysisPara.m_fConvergeLim=5;		//应力分析计算收敛阈值(N)
	AnalysisPara.m_fBeamLateralStifReductCoef=1.0;
	AnalysisPara.m_nSubSpaceIterCounts=50;	//子空间迭代法最多迭代次数
	AnalysisPara.m_fFreqsTolerance=0.001;	//频率千分之一误差率
#endif
	BoltPadThick.xarrM12Thicks[0]=2;
	BoltPadThick.xarrM16Thicks[0]=3;
	BoltPadThick.xarrM20Thicks[0]=3;
	BoltPadThick.xarrM24Thicks[0]=4;
	nAutoSaveTime=120000;			//两分钟
	m_bSaveLogVersion=FALSE;
	m_bDispPartACS=FALSE;
	m_bDispWorkUcs=FALSE;
	m_bDispBlockLCS=FALSE;
	m_bEnablePrompt=TRUE;
	m_bDragOpenWndSel=m_bLineDepthTest=m_bDblClkStartRotateOper=true;
	m_iModFileCodeType=0;
	//设计参数选项
	//火曲线间隙: 火曲线到角钢边缘距离一般为5mm~10mm
	HuoQuDist	= 10;
	m_uDisplaySlices = 12;	//圆柱形回转体的显示片数
	//角钢间隙: 角钢与角钢对接时之间的距离一般为10mm
	JgEndDist	= 10;
	JointPlankDist	=20;
	//修改对接法兰钢管正负头
	m_bModifyCoupledFLTubeOddment=TRUE;
	//法兰与钢管间隙:法兰与钢管之间的距离一般为1mm
	TubeFlDist = 1;
	m_nPlankEdgeAngleThreshold=80;
	//角钢或板的构件顶点与构件边间距
	VertexDist	= 5;
	CollideDist = 5;//碰撞间隙
	FootNailDist= 450;
	fStressDiffuseAngle=30;
	fInsertPlateEdgeSpace=0;
	fNailRepScope = 60.0;	//脚钉代孔范围
	fTubeWeldRoadMinD = 200.0;//钢管直径大于该值时该钢管存在焊道线
	nAutoAdjustTubePos = 2;	//0.不启用 1.插板偏移 2.钢管偏移
	m_fKaiHeJiaoThreshold = 2;	//角钢开合角度阈值默认为2° wht 11-05-06
	m_fKaiHeJiaoMinDeita = 20;	//一般该间隙为10~50mm
	m_fMaxKaiHeJiaoClearance=3;	//角钢开合角时制造规定允许的最小连接件间隙，一般不允许大于3mm (增立杜连喜反馈说制图规定里有规定）　wjh-2017.12.14
	m_nGroupJgMachiningGap=0;	//组合角钢加工间隙
	m_nMatCharPosType = 0;
	m_cMatCharSeparator = 0;
	m_fMinCutAngleLen = 10;     //最小切角值默认为10；
	 m_fMaxIgnoreCutAngleLen=2; //最大忽略切角值
	m_b45CutAngleNeedCoefR=TRUE;
	m_fCutAngleCoefR = 2.0/3;
	m_bAnglePreferSimplePosMode=TRUE;
	for(int i=0;i<4;i++)
	{
		arrRule[i].bActive = true;
		arrRule[i].iOrder  = i;
	}
	arrRule[0].name = MAIN_OTHER;
	arrRule[1].name = DOWN_UP;
	arrRule[2].name = LEFT_RIGHT;
	arrRule[3].name = FRONT_SIDE;
	bFristJgCodeNo = TRUE;
	m_bDelBoltsResideOnPlate=FALSE;
#ifdef __TSA_
	iColorSchema=1;	//TSA配色方案
#else
	iColorSchema=3;	//构件配色方案
#endif
	//TSA配色方案
	crTsaMode.crNode = RGB(255,128,0);
	crTsaMode.crPole = RGB(150,150,255);
	crTsaMode.crPostLine = RGB(0,0,0);
	crTsaMode.crPrimaryLine = RGB(128,65,50);
	crTsaMode.crAuxLine = RGB(0,128,255);
	crTsaMode.crDimText = RGB(0,0,0);
	crTsaMode.crDeformedPoleLine = RGB(128,0,128);
	crTsaMode.crLoadArrow = RGB(0,255,255);
	crTsaMode.crBackground = RGB(192,192,192);
	crTsaMode.bEfficColorLinearVary=false;
	crTsaMode.crZeroStressElem=0;	//零杆默认为黑色
	crTsaMode.crOverflowElem = RGB(255,0,0);
	//颜色分界方案1,四分界五分区,离散变色
	crTsaMode.crElemDivArr[0].efficPercent=60;
	crTsaMode.crElemDivArr[0].crElemDivision=RGB(0,128,255);
	crTsaMode.crElemDivArr[1].efficPercent=80;
	crTsaMode.crElemDivArr[1].crElemDivision=RGB(0,255,0);
	crTsaMode.crElemDivArr[2].efficPercent=90;
	crTsaMode.crElemDivArr[2].crElemDivision=RGB(200,250,140);
	crTsaMode.crElemDivArr[3].efficPercent=95;
	crTsaMode.crElemDivArr[3].crElemDivision=RGB(255,255,0);
	crTsaMode.crElemDivArr[4].efficPercent=100;
	crTsaMode.crElemDivArr[4].crElemDivision=RGB(255,145,0);
	crTsaMode.crElemDivArr[5].efficPercent=255;	//>100终止不显示,为无效分界
	crTsaMode.crElemDivArr[5].crElemDivision=RGB(0,255,0);
	//颜色分界方案2,四分界五分区,渐变色
	/*crTsaMode.crElemDivArr[0].efficPercent=255;	//>100终止不显示,为无效分界
	crTsaMode.crElemDivArr[0].crElemDivision=RGB(0,0,255);
	crTsaMode.crElemDivArr[1].efficPercent=60;
	crTsaMode.crElemDivArr[1].crElemDivision=RGB(0,128,255);
	crTsaMode.crElemDivArr[2].efficPercent=80;
	crTsaMode.crElemDivArr[2].crElemDivision=RGB(0,255,0);
	crTsaMode.crElemDivArr[3].efficPercent=90;
	crTsaMode.crElemDivArr[3].crElemDivision=RGB(255,255,0);
	crTsaMode.crElemDivArr[4].efficPercent=95;
	crTsaMode.crElemDivArr[4].crElemDivision=RGB(255,145,0);
	crTsaMode.crElemDivArr[5].efficPercent=100;
	crTsaMode.crElemDivArr[5].crElemDivision=RGB(255,0,0);
	//颜色分界方案3, 两分界渐变色,
	crTsaMode.crElemDivArr[0].efficPercent=0;
	crTsaMode.crElemDivArr[0].crElemDivision=RGB(0,255,0);
	crTsaMode.crElemDivArr[1].efficPercent=55;
	crTsaMode.crElemDivArr[1].crElemDivision=RGB(255,255,0);
	crTsaMode.crElemDivArr[2].efficPercent=95;
	crTsaMode.crElemDivArr[2].crElemDivision=RGB(255,205,0);
	crTsaMode.crElemDivArr[3].efficPercent=100;
	crTsaMode.crElemDivArr[3].crElemDivision=RGB(255,0,0);
	crTsaMode.crElemDivArr[4].efficPercent=255;
	crTsaMode.crElemDivArr[4].crElemDivision=RGB(255,0,0);
	crTsaMode.crElemDivArr[5].efficPercent=255;	//>100终止不显示,为无效分界
	crTsaMode.crElemDivArr[5].crElemDivision=RGB(0,255,0);*/
	//构件配色方案
	crPartMode.crNode = RGB(255,255,255);
	crPartMode.crControlNode = RGB(255,255,255);		//控制节点
	crPartMode.crDivideScaleNode = RGB(255,255,0);	//等分节点
	crPartMode.crOffsetNode = RGB(128,128,0);		//偏移节点
	crPartMode.crAxisValFixNode= RGB(255,80,0);	//指定坐标分量节点
	crPartMode.crLine = RGB(255,255,255);	//RGB(192,192,192);		//(150,150,255);
	crPartMode.crPole = RGB(192,192,192);		//(150,150,255);重新调整为银灰色更好看 wjh-2018.6.30
	crPartMode.crAngleEdge  = RGB(0,100,255);
	crPartMode.crAngleWingX = RGB(220,220,220);//RGB(150,150,255);//RGB(192,192,192);	//
	crPartMode.crAngleWingY = RGB(220,220,220);//RGB(150,150,255);//RGB(192,192,192);	////RGB(0,128,255);
	crPartMode.crSonAngle1Edge  = RGB(0,100,255);
	crPartMode.crSonAngle1WingX = RGB(0,128,128);
	crPartMode.crSonAngle1WingY = RGB(0,128,128);
	crPartMode.crBolt12  = RGB(255,0,255);
	crPartMode.crBolt16 = RGB(128,0,64);
	crPartMode.crBolt20 = RGB(128,0,255);
	crPartMode.crBolt24 = RGB(46, 0, 91);
	LOCALE_PROPITEM* pLocaleItem = gxPortalLocalizer.GetLocaleItem("M16.Color");
	if (pLocaleItem != NULL)
		crPartMode.crBolt16 = pLocaleItem->liVal;
	pLocaleItem = gxPortalLocalizer.GetLocaleItem("M20.Color");
	if (pLocaleItem != NULL)
		crPartMode.crBolt20 = pLocaleItem->liVal;
	pLocaleItem = gxPortalLocalizer.GetLocaleItem("M24.Color");
	if (pLocaleItem != NULL)
		crPartMode.crBolt24 = pLocaleItem->liVal;
	crPartMode.crBolt27 = RGB(200,160,88);
	crPartMode.crBolt30 = RGB(130,20,130);
	crPartMode.crBolt36 = RGB(233,13,133);
	crPartMode.crBolt39 = RGB(45,195,195);
	crPartMode.crBolt42 = RGB(255,160,0);
	crPartMode.crBolt48 = RGB(180,180,60);
	crPartMode.crOtherBolt = RGB(128,0,255);
	crPartMode.crAntiTheftNut=RGB(255,0,0);
	crPartMode.crAntiLoosenNut=RGB(150,150,255);
	crPartMode.crPlate	= RGB(159,120,50);//RGB(0,255,128);
	crPartMode.crCutFaceOrPolyJoint = RGB(255,128,128);
	crPartMode.crPartBackground = RGB(150,150,255);
	crPartMode.crShortAngle = RGB(90,90,90);
	crPartMode.crStdPart = RGB(100,100,100);
	crPartMode.crBackground = RGB(0,64,160);
	//材质配色方案
	crMaterialMode.crNode = RGB(255,255,255);
	crMaterialMode.crPole = RGB(150,150,255);
	crMaterialMode.crLine = RGB(150,150,255);
	crMaterialMode.crBackground = RGB(0,64,160);
	crMaterialMode.crQ235 = RGB(255,0,0);
	crMaterialMode.crQ345 = RGB(0,255,0);
	crMaterialMode.crQ390 = RGB(0,0,255);
	crMaterialMode.crQ420 = RGB(255,127,0);
	crMaterialMode.crQ460 = RGB(127,255,0);
	crMaterialMode.crOther = RGB(0,255,127);

	//智能连接设计设置参数 wht 10-01-15
	m_bDisplayConnectDlg=FALSE;				//设计过程中显示设置对话框
	intelligentConnect.m_nPlateThick=6;		//钢板厚度
	intelligentConnect.m_cMaterial='S';		//钢板材质
	intelligentConnect.m_bAutoThick=TRUE;	//自动计算钢板厚度
	intelligentConnect.m_bAutoMat=TRUE;		//自动设置钢板材质
	intelligentConnect.m_bOnlyDesignCurView=FALSE;	//仅设计当前面连接板
	intelligentConnect.m_bDesignCommonConnect=TRUE;	//智能设计普通连接
	intelligentConnect.m_bDesignBendConnect=TRUE;	//智能设计制弯板连接
	intelligentConnect.m_bDesignAngleJoint=TRUE;	//智能设计角钢接头
	intelligentConnect.m_bDesignFL=FALSE;			//智能设计法兰
	intelligentConnect.m_bDesignTubeCoverPlate=FALSE;//智能设计钢管上的环向封板
	intelligentConnect.m_iPlateProfileStyle=0;		//钢板外形计算方式
	intelligentConnect.m_iSingleAngleJointStyle=2;	//单角钢接头方式
	intelligentConnect.m_iTwoAngleJointStyle=0;		//双主材角钢接头方式
	intelligentConnect.m_iFourAngleJointStyle=0;	//四主材角钢接头方式
	intelligentConnect.m_iBoltLayoutStyle=0;		//双排螺栓布置方式
	intelligentConnect.m_nOffset = 60;				//孤立角钢端连接初始化间距
	intelligentConnect.m_nOffsetAdd = 20;			//孤立角钢端连接偏移量增长步长
	intelligentConnect.m_bPriorOffsetGradientRod=true;	//优先对斜向（三个分量均不为零）的孤立射线端连接进行基准偏移
	//螺栓外孔间隙A(不含垫板)
	intelligentConnect.m_n12LsOutterSpace = 15;
	intelligentConnect.m_n16LsOutterSpace = 20;
	intelligentConnect.m_n20LsOutterSpace = 25;
	intelligentConnect.m_n24LsOutterSpace = 30;
	//螺栓外孔间隙(含垫板)
	intelligentConnect.m_n12LsOutterSpaceHasWasher = 20;
	intelligentConnect.m_n16LsOutterSpaceHasWasher = 25;
	intelligentConnect.m_n20LsOutterSpaceHasWasher = 30;
	intelligentConnect.m_n24LsOutterSpaceHasWasher = 35;
	//螺栓肢翼边距B
	intelligentConnect.m_n12LsAngleEdgeSpace=25;
	intelligentConnect.m_n16LsAngleEdgeSpace=25;
	intelligentConnect.m_n20LsAngleEdgeSpace=30;
	intelligentConnect.m_n24LsAngleEdgeSpace=40;
	//外孔螺栓错孔间距C
	intelligentConnect.m_n12LsOutterStaggerSpace=40;
	intelligentConnect.m_n16LsOutterStaggerSpace=40;
	intelligentConnect.m_n20LsOutterStaggerSpace=50;
	intelligentConnect.m_n24LsOutterStaggerSpace=60;
	//共用螺栓错孔间距S
	intelligentConnect.m_n12LsShareStaggerSpace=35;
	intelligentConnect.m_n16LsShareStaggerSpace=35;
	intelligentConnect.m_n20LsShareStaggerSpace=45;
	intelligentConnect.m_n24LsShareStaggerSpace=55;

	//
	m_arrL1RayPole.Add("L_1LB1");
	m_arrL1RayPole.Add("L_1LB2");
	m_arrL1RayPole.Add("L_1LE1");
	m_arrL1RayPole.Add("L_1LE2");
	m_arrL1RayPole.Add("L_1LO1");
	//
	m_arrL2RayPole.Add("L_2LB1");
	m_arrL2RayPole.Add("L_2LB2");
	m_arrL2RayPole.Add("L_2LB3");
	m_arrL2RayPole.Add("L_2LB4");
	m_arrL2RayPole.Add("L_2LE1");
	m_arrL2RayPole.Add("L_2LE2");
	m_arrL2RayPole.Add("L_2LC1");
	m_arrL2RayPole.Add("L_2LC2");
	m_arrL2RayPole.Add("L_2LO1");
	//
	m_arrL3RayPole.Add("L_3LB1");
	m_arrL3RayPole.Add("L_3LB2");
	m_arrL3RayPole.Add("L_3LC1");
	m_arrL3RayPole.Add("L_3LC2");
	m_arrL3RayPole.Add("L_3LC3");
	m_arrL3RayPole.Add("L_3LO1");
	//
	m_arrInsertPlateType.Add("C_END");
	m_arrInsertPlateType.Add("U_END");
	m_arrInsertPlateType.Add("X_END");
	//
	m_arrFlType.Add("FLD");
	m_arrFlType.Add("FLP");
}
//Next function from LoadTaFile.cpp
BOOL CSysParaLibrary::Read(CString file_path)	//读配置文件
{
	CString version;
	WORD w;
	if(file_path.IsEmpty())
		return FALSE;
	CFile file;
	if(!file.Open(file_path, CFile::modeRead))
		return FALSE;
	try{
	CArchive ar(&file,CArchive::load);

	ar>>version;
	int i;
	for(i=0;i<6;i++)
	{
		//显示条件1
		ar>>g_sysPara.char0_arr[i];
		//显示条件2
		ar>>g_sysPara.char1_arr[i];
		//显示条件3
		ar>>g_sysPara.char2_arr[i];
		ar>>w;	g_sysPara.bNodeArr[i]=w;
	}
	if(compareVersion(version,"1.54")>=0)
	{
		ar>>dock.m_nLeftPageWidth;
		ar>>dock.m_nRightPageWidth;
		ar>>dock.m_nBottomPageHeight;
		ar>>w;	dock.pageProp.bDisplay=w;
		ar>>dock.pageProp.uDockBarID;
		ar>>w;	dock.pageModel.bDisplay=w;
		ar>>dock.pageModel.uDockBarID;
		ar>>w;	dock.pageCmd.bDisplay=w;
		ar>>dock.pageCmd.uDockBarID;
		ar>>w;	dock.pagePartLabel.bDisplay=w;
		ar>>dock.pagePartLabel.uDockBarID;
		ar>>w;	dock.pageWeldPart.bDisplay=w;
		ar>>dock.pageWeldPart.uDockBarID;
		ar>>w;	dock.pageOutput.bDisplay=w;
		ar>>dock.pageOutput.uDockBarID;
		ar>>w;	dock.pageRevision.bDisplay=w;
		ar>>dock.pageRevision.uDockBarID;
		if(compareVersion(version,"1.64")>=0)
		{
			ar>>w;	dock.pageLogger.bDisplay=w;
			ar>>dock.pageLogger.uDockBarID;
		}
	}
	if(compareVersion(version,"1.10")>=0)
		ar>>g_sysPara.m_dwSelectPartsType;
	else
		g_sysPara.m_dwSelectPartsType=0XFFFFFFFF;
	//初始化时不能默认实体下可选中节点,否则极易造成误操作
	if(g_sysPara.m_dwSelectPartsType&GetSingleWord(SELECTINDEX_NODE))
		g_sysPara.m_dwSelectPartsType-=GetSingleWord(SELECTINDEX_NODE);

	if(compareVersion(version,"1.03")>=0)
		ar>>g_sysPara.nAutoSaveTime;
	//读取自动保存方式 wzh 12-04-24
	if(compareVersion(version,"1.53")>=0)
		ar>>g_sysPara.iAutoSaveType;
	if(compareVersion(version,"1.67")>=0)
	{
		UINT uVersion=0;
		ar>>uVersion;	//极其容易错误的将当前版本存储为低版本从而导致丢失很多信息　wjh-2015.5.22
		g_sysPara.uFileSaveVersion=0;
	}
	if(compareVersion(version,"1.60")>=0)
		ar>>g_sysPara.m_bSaveLogVersion;
	else 
		g_sysPara.m_bSaveLogVersion=FALSE;
	if(compareVersion(version,"1.89")>=0)
		ar>>g_sysPara.m_iModFileCodeType;
	else
		g_sysPara.m_iModFileCodeType=0;
	if(compareVersion(version,"1.61")>=0)
	{
		ar>>g_sysPara.m_bDispPartACS;
		ar>>g_sysPara.m_bDispWorkUcs;
		ar>>g_sysPara.m_bDispBlockLCS;
	}
	else 
	{
		g_sysPara.m_bDispPartACS=FALSE;
		g_sysPara.m_bDispWorkUcs=FALSE;
		g_sysPara.m_bDispBlockLCS=FALSE;
	}
	if(compareVersion(version,"1.75")>=0)
	{
		ar.Read(&m_bDragOpenWndSel,sizeof(bool));
		ar.Read(&m_bLineDepthTest,sizeof(bool));
		if(compareVersion(version,"1.76")>=0)
			ar.Read(&m_bDblClkStartRotateOper,sizeof(bool));
	}
	ar>>fRotateVelocity;	//塔高,用于控制旋转速度
	if(compareVersion(version,"1.64")>=0)
		ar>>m_fMaxScaleScr2User;
	ar>>w;	g_sysPara.bMoveDispFrame;	//实体移动时显示线框图
	ar>>w;	g_sysPara.display.bNodePnt	= w;	//显示单线节点
	if(compareVersion(version,"1.63")>=0)
	{
		ar>>g_sysPara.display.xPointStyle.wPointMode;
		if(compareVersion(version,"1.86")<0)//旧版默认升级为圆点显示
			g_sysPara.display.xPointStyle.ciPointStyle=1;
		ar>>CLDSLinePart::bDisplayLineVecMark;
	}
	if(compareVersion(version,"1.72")>=0&&compareVersion(version,"1.85")<0)
	{
		bool bEnableSysWarningLog;
		ar>>bEnableSysWarningLog;
		if(bEnableSysWarningLog)
			m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL2_COMMON;
		else
			m_ciLowestWarningLevel=ILog2File::WARNING_LEVEL1_IMPORTANT;
	}
	else if(compareVersion(version,"1.85")>=0)
		ar>>m_ciLowestWarningLevel;
	if(compareVersion(version,"1.72")>=0)
	{
		ar>>CLDSLineTube::ciFlTubeOddment;
		if(compareVersion(version,"1.73")>=0)
			ar>>CLDSLineAngle::m_bDisplaySolidAxisLine;
	}
	if(compareVersion(version,"1.88")>=0)
		ar>>CLDSGeneralPlate::m_ciDisplayVertexType;
	if(compareVersion(version,"1.64")>=0)
	{
		ar>>w;	display.bDimNodeLabel=w;			//标注节点编号
		ar>>display.nNodeLabelTextHeight;	//节点编号字高
		ar>>w;	display.bDimRodDesignInfo=w;		//标注杆件规格
		ar>>display.nRodDesignInfoTextHeight;//杆件规格字高
	}
	ar>>w;	g_sysPara.display.bPoleLine	= w;	//显示单线杆件线
	if(compareVersion(version,"1.36")>=0)
	{
		ar>>w;	
		g_sysPara.display.bPartSketch	= w;	//显示构件轮廓标以及轮廓点
	}
	else 
		g_sysPara.display.bPartSketch=FALSE;
	if(compareVersion(version,"1.28")>=0)
	{
		ar>>w;	g_sysPara.display.bLoadArrow = w;	//显示荷载箭头
		ar>>g_sysPara.display.nLoadArrowScale;	//荷载箭头显示比例
		if(compareVersion(version,"1.91")>=0)
			ar>>w;	g_sysPara.display.bWirePoint=w;
	}
	if(compareVersion(version,"1.56")>=0)
	{
		ar>>w;	CLDSNode::m_bDisplayBeforeArcLift=w;//单线显示预拱前位置
	}
	ar>>w;	g_sysPara.display.bSolidJg	= w;	//显示实体角钢
	if(compareVersion(version,"1.14")>=0)
	{
		ar>>w;	g_sysPara.display.bSolidTube	= w;//显示实体钢管
	}
	ar>>w;	g_sysPara.display.bSolidJoint	= w;//显示实体接头
	ar>>w;	g_sysPara.display.bSolidLjb	= w;	//显示实体连接板
	ar>>w;	g_sysPara.display.bSolidLs	= w;	//显示实体螺栓
	if(compareVersion(version,"1.02")>=0)
	{
		if(compareVersion(version,"1.06")>=0)
		{
			ar>>w;	g_sysPara.display.bSolidTransectFace=w;	//显示实体相贯面
		}
		if(compareVersion(version,"1.12")>=0)
		{
			ar>>w;	g_sysPara.display.bSolidSphere=w;	//显示球体
			ar>>w;	g_sysPara.display.bSolidSlot=w;		//显示实体槽钢
		}
		if(compareVersion(version,"1.14")>=0)
		{
			ar>>w;	g_sysPara.display.bSolidFlat=w;		//显示实体扁钢
			ar>>w;	g_sysPara.display.bSolidArcAngle=w;	//显示实体环形角钢
			ar>>w;	g_sysPara.display.bSolidArcSlot=w;	//显示实体环形槽钢
			ar>>w;	g_sysPara.display.bSolidArcFlat=w;	//显示实体环形扁钢
		}
		if(compareVersion(version,"1.15")>=0)
		{
			ar>>w;	g_sysPara.display.bSolidStdPart=w;	//显示实体标准构件
		}
		ar>>w;
		g_sysPara.display.bSolidLine=w;	//显示实体单线
		if(compareVersion(version,"1.90")>=0)
		{	
			ar>>w;  
			g_sysPara.display.bSolidFoundation=w;
		}
		else
			g_sysPara.display.bSolidFoundation=TRUE;
		if(compareVersion(version,"1.23")>=0)
		{	
			ar>>w;  
			g_sysPara.display.nSmoothness=w;	//实体显示时圆弧显示的最小面数
		}
		else
			g_sysPara.display.nSmoothness=36;
	}
	//因为g_sysPara.display.bSolidLs已保存，如果g_sysPara.bDisplayAllHole不保存
	//关闭文件时如果为现实螺栓孔状态，则会出现既不显示螺栓也不显示螺栓孔的情况 wht 10-07-08
	if(compareVersion(version,"1.32")>=0)
	{
		ar>>w;
		g_sysPara.bDisplayAllHole=w;		//显示所有孔 1.32
	}
	else 
		g_sysPara.bDisplayAllHole=!g_sysPara.display.bSolidLs;
#if defined(__LDS_)||defined(__TSA_)||defined(__LDSGROUP_)||defined(__TSAGROUP_)
	if(compareVersion(version,"1.25")>=0)
		ar>>bCalStdFoundationForce;
	if(compareVersion(version,"1.51")>=0)
		ar>>bUpdateRodDesignForce;
	if(compareVersion(version,"1.19")>=0)
	{
		ar>>bCreateAnalFile;
		if(compareVersion(version,"1.41")>=0)
			ar>>bRecNodeOffset;
		ar>>bCreateAnsysCmdFile;
		if(compareVersion(version,"1.37")<0)
			ar>>bCreateDetailRptFile;//bCreateBriefRptFile;
		ar>>bCreateDetailRptFile;
		if(compareVersion(version,"1.21")>=0)
		{
			if(compareVersion(version,"1.31")>=0)
				ar>>iLanguage;
			if(compareVersion(version,"1.37")<0)
				ar>>iDetailDocType;//iBriefDocType;
			ar>>iDetailDocType;
			if(compareVersion(version,"1.48")>=0)
				ar>>m_bAuxPoleAnalysisInc;
		}
	}
	else
	{
		bCreateAnalFile=FALSE;
		bCreateAnsysCmdFile=FALSE;
		bCreateDetailRptFile=TRUE;
	}
	if(compareVersion(version,"1.17")>=0)
	{
		double fUnbalanceForceLimit;	//(kN)	东北院程序以前遗留问题，已停用
		if(compareVersion(version,"1.64")<0)
		{
			ar>>fUnbalanceForceLimit;
			ar>>fUnbalanceForceLimit;
		}
		ar>>AnalysisPara.m_iNonlinearStyle;	//否则以线性方式计算
		ar>>AnalysisPara.m_iResolverStyle;	//求解器方式
		ar>>AnalysisPara.m_nLoadStepCounts;	//分几次加载
		ar>>AnalysisPara.m_nIterationCounts;	//每次增量最多迭代次数
		ar>>AnalysisPara.m_fConvergeLim;		//应力分析计算收敛阈值
		if(compareVersion(version,"1.49")>=0)
		{
			ar>>AnalysisPara.m_nSubSpaceIterCounts;	//子空间迭代法最多迭代次数
			ar>>AnalysisPara.m_fFreqsTolerance;		//频率千分之一误差率
		}
		if(compareVersion(version,"1.64")<0)
		{
			ar>>Ta.m_iLoadExecuteCode;	//荷载计算执行规范
			ar>>Ta.m_iMemberSizeCheckCode;	//构件验算优化选材执行规范
			if(compareVersion(version,"1.31")>=0)
				ar>>Ta.m_iSlendernessLimCode;
			if(compareVersion(version,"1.18")>=0)
				ar>>Ta.m_fLimitBearCoef;
		}
		if(compareVersion(version,"1.63")>=0)
		{
			ar>>w;InterpBetaZArr.Resize(w);
			for(WORD i=0;i<w;i++)
				ar.Read(&InterpBetaZArr[i],sizeof(double)*3);	//风振系数插值表
		}
		else if(compareVersion(version,"1.52")>=0)
		{
			for(int i=0;i<10;i++)
				ar.Read(&InterpBetaZArr[i],sizeof(double)*3);	//风振系数插值表
		}
		else
		{
			double m_fWindBetaZForFoundation;	//为保证文件格式读取,临时设定
			ar>>m_fWindBetaZForFoundation;	//基础设计时风荷载调整系数
		}
		if(compareVersion(version,"1.68")>=0)
		{
			ar>>w;InterpIceLoadCoefArr.Resize(w);
			for(WORD i=0;i<w;i++)
				ar.Read(&InterpIceLoadCoefArr[i],sizeof(double)*2);	//杆塔覆冰重力荷载系数表
		}
		if(compareVersion(version,"1.29")<0)
		{	
			double dd;	//SelMatPara.m_fWindMagnifyCoefForIce
			ar>>dd;	//覆冰时风荷载放大系数
		}
		if(compareVersion(version,"1.64")<0)
		{
			ar>>Ta.m_fSafetyCoef;
			ar>>Ta.m_fGamaGForDesign;		//杆塔设计时重力荷载分项系数
			ar>>Ta.m_fGamaGForFoundation;			//基础设计时重力荷载分项系数
			ar>>Ta.m_fGamaQForTowerWindLoad;//杆塔本身及附加风荷载分项系数
			if(compareVersion(version,"1.24")<0)
			{
				double m_fFaiForTowerWindLoad;
				ar>>m_fFaiForTowerWindLoad;	//杆塔本身风荷载组合系数
			}
			if(compareVersion(version,"1.52")<0)
			{
				char m_csM20_50LsGrade[5];
				char m_csM50_63LsGrade[5];
				char m_csM63_160LsGrade[5];
				char m_csM160_200LsGrade[5];
				long m_nM20_50LsD;
				long m_nM50_63LsD;
				long m_nM63_160LsD;
				long m_nM160_200LsD;
				ar.Read(m_csM20_50LsGrade,5);
				ar.Read(m_csM50_63LsGrade,5);
				ar.Read(m_csM63_160LsGrade,5);
				ar.Read(m_csM160_200LsGrade,5);
				ar>>m_nM20_50LsD;
				ar>>m_nM50_63LsD;
				ar>>m_nM63_160LsD;
				ar>>m_nM160_200LsD;
			}
			ar>>Ta.SelMatPara.m_nMainPoleSlendernessLimit;
			ar>>Ta.SelMatPara.m_nXiePoleSlendernessLimit;
			ar>>Ta.SelMatPara.m_nAuxPoleSlendernessLimit;
			ar>>Ta.SelMatPara.m_nTensivePoleSlendernessLimit;
			if(compareVersion(version,"1.18")>=0)
			{
				ar>>Ta.SelMatPara.m_nMainTubeSlendernessLimit;
				ar>>Ta.SelMatPara.m_nXieTubeSlendernessLimit;
				ar>>Ta.SelMatPara.m_nAuxTubeSlendernessLimit;
				ar>>Ta.SelMatPara.m_nTensiveTubeSlendernessLimit;
			}
			else
			{
				Ta.SelMatPara.m_nMainTubeSlendernessLimit=130;
				Ta.SelMatPara.m_nXieTubeSlendernessLimit=200;
				Ta.SelMatPara.m_nAuxTubeSlendernessLimit=250;
				Ta.SelMatPara.m_nTensiveTubeSlendernessLimit=400;
			}
			if(compareVersion(version,"1.33")>=0)
			{
				ar>>Ta.SelMatPara.m_bPreferSingleAngle;
				ar>>Ta.SelMatPara.m_iSizeOrderRule;
			}
			ar>>Ta.SelMatPara.m_nMaxSelMatCount;
			ar>>Ta.SelMatPara.m_nConvergeLim;
			ar.Read(Ta.SelMatPara.m_csMinAngleSize,10);
			if(compareVersion(version,"1.37")>=0)
				ar.Read(Ta.SelMatPara.m_csMinForceAngleSize,10);
			ar.Read(Ta.SelMatPara.m_csMinMainAngleSize,10);
		}
	}
#endif
	//设计参数选项
	//火曲线间隙: 火曲线到角钢边缘距离一般为5mm~10mm
	ar>>w;	g_sysPara.HuoQuDist	= w;
	//角钢间隙: 角钢与角钢对接时之间的距离一般为8mm
	ar>>w;	g_sysPara.JgEndDist	= w;
	ar>>w;	g_sysPara.JointPlankDist	=w;
	ar>>w;	g_sysPara.VertexDist	= w;
	if(compareVersion(version,CString("1.40"))>=0)
	{
		ar>>w;	g_sysPara.CollideDist = w;
	}
	else
		g_sysPara.CollideDist = 5;
	ar>>w;	g_sysPara.FootNailDist= w;
	if(compareVersion(version,CString("1.05"))>=0)
	{
		ar>>w;	g_sysPara.fNailRepScope = w;
	}
	else
		g_sysPara.fNailRepScope=60;
	if(compareVersion(version,CString("1.11"))>=0)
	{
		ar>>w;	g_sysPara.fTubeWeldRoadMinD = w;
	}
	else
		g_sysPara.fTubeWeldRoadMinD=200.0;
	if(compareVersion(version,"1.08")>=0)
		ar>>g_sysPara.fStressDiffuseAngle;	//应力扩散角
	else
		g_sysPara.fStressDiffuseAngle=30;
	if(compareVersion(version,"1.59")>=0)
		ar>>g_sysPara.fInsertPlateEdgeSpace;
	else
		g_sysPara.fInsertPlateEdgeSpace=0;
	//修改对接法兰钢管正负头
	if(compareVersion(version,"1.77")>=0)
		ar>>w,g_sysPara.m_bModifyCoupledFLTubeOddment=w;
	if(compareVersion(version,CString("1.07"))>=0)
	{
		ar>>w;	g_sysPara.TubeFlDist = w;
	}
	else
		g_sysPara.TubeFlDist=1;
	if(compareVersion(version,CString("1.04"))>=0)
	{
		ar>>w;	g_sysPara.m_nPlankEdgeAngleThreshold=w;
	}
	if(compareVersion(version,"1.16")>=0)
		ar>>g_sysPara.nAutoAdjustTubePos;	//0.不启用 1.插板偏移 2.钢管偏移
	if(compareVersion(version,"1.22")>=0&&compareVersion(version,"1.84")<0)
	{
		UINT m_uRollProcessType;
		ar>>m_uRollProcessType; //槽型插板卷边处理方式 0.展开后倒角处为两条直线 1.展开后倒角处为一条直线
	}
	if(compareVersion(version,"1.30")>=0&&compareVersion(version,"1.50")<0)
	{
		UINT uTubeEndLjDlgType;
		ar>>uTubeEndLjDlgType; //节点板钢管端头设计框类型 0.无设计预览 1.有设计预览 wht 10-06-30
	}
	if(compareVersion(version,"1.46")>=0)
		ar>>m_fKaiHeJiaoThreshold; //角钢开合角阈值 wht 10-06-30
	else
		m_fKaiHeJiaoThreshold=2;
	if(compareVersion(version,"1.78")>=0)
		ar>>m_fKaiHeJiaoMinDeita;
	else 
		m_fKaiHeJiaoMinDeita=20;
	if(compareVersion(version,"1.57")>=0)   //最小切角值
		ar>>m_fMinCutAngleLen;
	else
		m_fMinCutAngleLen=0;
	if(compareVersion(version,"1.83")>=0)
		ar>>m_fMaxIgnoreCutAngleLen;
	else
		m_fMaxIgnoreCutAngleLen=0;
	if(compareVersion(version,"1.58")>=0)
		ar>>m_nMatCharPosType;
	else 
		m_nMatCharPosType=0;
	if(compareVersion(version,"1.82")>=0)
		ar>>m_b45CutAngleNeedCoefR;
	else
		m_b45CutAngleNeedCoefR=TRUE;
	if(compareVersion(version,"1.68")>=0)
		ar>>m_fCutAngleCoefR;
	else if(compareVersion(version,"1.66")>=0)
	{	//由V1.66时的进入内圆弧比例系数改为V1.68中的通过内圆弧占比计算的切割间隙占比系数 wjh-2014.1.6
		ar>>m_fCutAngleCoefR;
		m_fCutAngleCoefR=1-m_fCutAngleCoefR;
	}
	else 
		m_fCutAngleCoefR=2.0/3;
	if(compareVersion(version,"1.80")>=0)
	{
		BYTE flag;
		ar>>flag;
		m_bAnglePreferSimplePosMode=flag;
	}
	else
		m_bAnglePreferSimplePosMode=TRUE;
	if(compareVersion(version,"1.81")>=0)
		ar>>display.cHoleWallMode;
	else
		display.cHoleWallMode=1;
	if(compareVersion(version,"1.71")>=0)
		ar>>CLDSPart::UPDATE_PARTNO_AND_SEG;
	if(compareVersion(version,CString("1.01"))>=0)
	{
		for(i=0;i<4;i++)
		{
			ar>>g_sysPara.arrRule[i].iOrder;
			ar>>w;
			if(w==0)
				g_sysPara.arrRule[i].bActive=false;
			else
				g_sysPara.arrRule[i].bActive=true;
			ar>>w;
			switch(w)
			{
			case 0:
				g_sysPara.arrRule[i].name = DOWN_UP;
				break;
			case 1:
				g_sysPara.arrRule[i].name = LEFT_RIGHT;
				break;
			case 2:
				g_sysPara.arrRule[i].name = MAIN_OTHER;
				break;
			case 3:
				g_sysPara.arrRule[i].name = FRONT_SIDE;
				break;
			default:
				g_sysPara.arrRule[i].name = DOWN_UP;
				break;
			}
		}
	}
	//单螺栓垫圈最小厚度 wht 10-12-24
	if(compareVersion(version,"1.45")>=0)
	{
		ar.Read(BoltPadThick.xarrM12Thicks,4);
		ar.Read(BoltPadThick.xarrM16Thicks,4);
		ar.Read(BoltPadThick.xarrM20Thicks,4);
		ar.Read(BoltPadThick.xarrM24Thicks,4);
	}
	else
	{
		BoltPadThick.xarrM12Thicks[0]=2;
		BoltPadThick.xarrM16Thicks[0]=3;
		BoltPadThick.xarrM20Thicks[0]=3;
		BoltPadThick.xarrM24Thicks[0]=4;
	}
	//单螺栓设计前提示用户
	ar>>w;	g_sysPara.b1BoltDesignPrompt = w;
	ar>>crPartMode.crNode;			//节点颜色
	if(compareVersion(version,"1.86")>=0)
	{
		ar>>crPartMode.crControlNode;
		ar>>crPartMode.crDivideScaleNode;
		ar>>crPartMode.crOffsetNode;
		ar>>crPartMode.crAxisValFixNode;
	}
	ar>>crPartMode.crPole;			//角钢颜色
	ar>>crPartMode.crLine;		//角钢单线颜色
	if(compareVersion(version,"1.09")>=0)
	{
		ar>>crPartMode.crAngleWingX;		//角钢X肢显示颜色
		ar>>crPartMode.crAngleWingY;		//角钢Y肢显示颜色
	}
	else
	{
		crPartMode.crAngleWingX=crPartMode.crPole;
		crPartMode.crAngleWingY=crPartMode.crPole;
	}
	ar>>crPartMode.crAngleEdge;	//角钢肢边颜色
	if(compareVersion(version,"1.47")>=0)
	{	//读取1号子角钢颜色 wht 11-05-20
		ar>>crPartMode.crSonAngle1WingX;	//1号子角钢X肢显示颜色
		ar>>crPartMode.crSonAngle1WingY;	//1号子角钢Y肢显示颜色
		ar>>crPartMode.crSonAngle1Edge;		//1号子角钢肢边颜色
	}
	else
	{
		crPartMode.crSonAngle1WingX=RGB(0,128,128);
		crPartMode.crSonAngle1WingY=RGB(0,128,128);
		crPartMode.crSonAngle1Edge=crPartMode.crAngleEdge;
	}
	ar>>crPartMode.crOtherBolt;			//螺栓颜色
	ar>>crPartMode.crBolt12;			//螺栓颜色
	ar>>crPartMode.crBolt16;			//螺栓颜色
	ar>>crPartMode.crBolt20;			//螺栓颜色
	ar>>crPartMode.crBolt24;			//螺栓颜色
	if(compareVersion(version,"1.09")>=0)
	{
		ar>>crPartMode.crBolt27;			//螺栓颜色
		ar>>crPartMode.crBolt30;			//螺栓颜色
		ar>>crPartMode.crBolt36;			//螺栓颜色
		ar>>crPartMode.crBolt39;			//螺栓颜色
		ar>>crPartMode.crBolt42;			//螺栓颜色
		ar>>crPartMode.crBolt48;			//螺栓颜色
	}
	else
	{
		crPartMode.crBolt27 = crPartMode.crOtherBolt;
		crPartMode.crBolt30 = crPartMode.crOtherBolt;
		crPartMode.crBolt36 = crPartMode.crOtherBolt;
		crPartMode.crBolt39 = crPartMode.crOtherBolt;
		crPartMode.crBolt42 = crPartMode.crOtherBolt;
		crPartMode.crBolt48 = crPartMode.crOtherBolt;
	}
	if(compareVersion(version,"1.92")>=0)
	{
		ar>>crPartMode.crAntiTheftNut;	//防盗帽颜色
		ar>>crPartMode.crAntiLoosenNut;	//防松帽颜色
	}
	ar>>crPartMode.crPlate;		//板颜色
	if(compareVersion(version,"1.13")<0)
	{
		COLORREF cr;
		ar>>cr;//Index.crJoint;		//接头颜色
		ar>>cr;//Index.crShortJg;		//短角钢颜色
	}
	ar>>crPartMode.crCutFaceOrPolyJoint;		//火曲角钢颜色
	ar>>crPartMode.crBackground;	//主背景颜色
	ar>>crPartMode.crPartBackground;//构件编辑器背景颜色
	//1.70
	if(compareVersion(version,"1.70")>=0)
	{
		CString keyStr;
		COLORREF cr;
		int nNum;
		ar>>nNum;
		for(int i=0;i<nNum;i++)
		{
			ar>>keyStr;
			ar>>cr;
			plateColRef.colorHashList.SetValue(keyStr.GetBuffer(),cr);
		}
	}
	//1.27
	if(compareVersion(version,"1.27")>=0)
	{
		ar>>iColorSchema;
		ar>>crMaterialMode.crNode;
		ar>>crMaterialMode.crLine;
		ar>>crMaterialMode.crPole;
	}
	else
	{
		iColorSchema=3;	//构件配色方案
		crMaterialMode.crNode = RGB(255,255,255);
		crMaterialMode.crPole = RGB(150,150,255);
		crMaterialMode.crLine = RGB(150,150,255);
	}
	//1.26
	if(compareVersion(version,"1.26")>=0)
	{
		ar>>crPartMode.crShortAngle;
		ar>>crPartMode.crStdPart;
		ar>>crMaterialMode.crBackground;
		ar>>crMaterialMode.crQ235;
		ar>>crMaterialMode.crQ345;
		ar>>crMaterialMode.crQ390;
		ar>>crMaterialMode.crQ420;
		ar>>crMaterialMode.crQ460;
		ar>>crMaterialMode.crOther;
		//
		ar>>crTsaMode.crBackground;
		ar>>crTsaMode.crDeformedPoleLine;
		if(compareVersion(version,"1.87")>=0)
			ar>>crTsaMode.crPostLine;
		ar>>crTsaMode.crPrimaryLine;
		if(compareVersion(version,"1.52")>=0)
			ar>>crTsaMode.crAuxLine;
		if(compareVersion(version,"1.74")>=0)
			ar>>crTsaMode.crDimText;
		ar>>crTsaMode.crLoadArrow;
		ar>>crTsaMode.crNode;
		ar>>crTsaMode.crPole;
	}
	else
	{
		crPartMode.crShortAngle = RGB(90,90,90);
		crPartMode.crStdPart = RGB(100,100,100);
		crMaterialMode.crBackground = RGB(0,64,160);
		crMaterialMode.crQ235 = RGB(255,0,0);
		crMaterialMode.crQ345 = RGB(0,255,0);
		crMaterialMode.crQ390 = RGB(0,0,255);
		crMaterialMode.crQ420 = RGB(255,127,0);
		crMaterialMode.crQ460 = RGB(127,255,0);
		crMaterialMode.crOther = RGB(0,255,127);
		//
		crTsaMode.crNode = RGB(255,128,0);
		crTsaMode.crPole = RGB(150,150,255);
		crTsaMode.crPostLine = RGB(0,0,0);
		crTsaMode.crPrimaryLine=crTsaMode.crPostLine;
		crTsaMode.crDeformedPoleLine = RGB(128,0,128);
		crTsaMode.crLoadArrow = RGB(0,255,255);
		crTsaMode.crBackground = RGB(192,192,192);
	}
	//1.69
	if(compareVersion(version,"1.69")>=0)
	{
		ar>>crTsaMode.crZeroStressElem;
		ar>>crTsaMode.bEfficColorLinearVary;
		ar>>crTsaMode.crOverflowElem;
		for(int j=0;j<6;j++)
		{
			ar>>crTsaMode.crElemDivArr[j].efficPercent;
			ar>>crTsaMode.crElemDivArr[j].crElemDivision;
		}
	}
	else
	{
		crTsaMode.crZeroStressElem = RGB(0,0,0);
		crTsaMode.bEfficColorLinearVary =true;
		crTsaMode.crOverflowElem=RGB(255,0,0);
		crTsaMode.crElemDivArr[0].efficPercent=60;
		crTsaMode.crElemDivArr[0].crElemDivision=RGB(0,128,255);
		crTsaMode.crElemDivArr[1].efficPercent=80;
		crTsaMode.crElemDivArr[1].crElemDivision=RGB(0,255,0);
		crTsaMode.crElemDivArr[2].efficPercent=90;
		crTsaMode.crElemDivArr[2].crElemDivision=RGB(255,255,0);
		crTsaMode.crElemDivArr[3].efficPercent=95;
		crTsaMode.crElemDivArr[3].crElemDivision=RGB(255,145,0);
		crTsaMode.crElemDivArr[4].efficPercent=100;
		crTsaMode.crElemDivArr[4].crElemDivision=RGB(255,0,0);
		crTsaMode.crElemDivArr[5].efficPercent=255;	//>100终止不显示,为无效分界
		crTsaMode.crElemDivArr[5].crElemDivision=RGB(0,255,0);
	}
	//智能连接设计设置参数 wht 10-01-15
	if(compareVersion(version,"1.20")>=0)
	{
		ar>>m_bDisplayConnectDlg;				//设计过程中显示设置对话框
		ar>>intelligentConnect.m_nPlateThick;		//钢板厚度
		ar>>intelligentConnect.m_cMaterial;			//钢板材质
		ar>>intelligentConnect.m_bAutoThick;		//自动计算钢板厚度
		ar>>intelligentConnect.m_bAutoMat;			//自动设置钢板材质
		if(compareVersion(version,"1.50")>=0)
		{
			ar>>intelligentConnect.m_bDesignCommonConnect;	//智能设计普通连接
			if(compareVersion(version,"1.79")>=0)
				ar>>intelligentConnect.m_bDesignBendConnect;//智能设计制弯板连接
			ar>>intelligentConnect.m_bDesignAngleJoint;		//智能设计角钢接头
			ar>>intelligentConnect.m_bDesignFL;				//智能设计法兰
			ar>>intelligentConnect.m_bDesignTubeCoverPlate;	//智能设计钢管上的环向封板
		}
		ar>>intelligentConnect.m_iPlateProfileStyle;	//钢板外形计算方式
		ar>>intelligentConnect.m_iSingleAngleJointStyle;//单角钢接头方式
		ar>>intelligentConnect.m_iTwoAngleJointStyle;	//双主材角钢接头方式
		ar>>intelligentConnect.m_iFourAngleJointStyle;	//四主材角钢接头方式
		ar>>intelligentConnect.m_iBoltLayoutStyle;		//双排螺栓布置方式
		ar>>intelligentConnect.m_bOnlyDesignCurView;	//仅设计当前面连接板
	}
	else
	{
		m_bDisplayConnectDlg=FALSE;				//设计过程中显示设置对话框
		intelligentConnect.m_nPlateThick=6;		//钢板厚度
		intelligentConnect.m_cMaterial='S';		//钢板材质
		intelligentConnect.m_bAutoThick=TRUE;	//自动计算钢板厚度
		intelligentConnect.m_bAutoMat=TRUE;		//自动设置钢板材质
		intelligentConnect.m_iPlateProfileStyle=0;		//钢板外形计算方式
		intelligentConnect.m_iSingleAngleJointStyle=2;	//单角钢接头方式
		intelligentConnect.m_iTwoAngleJointStyle=0;		//双主材角钢接头方式
		intelligentConnect.m_iFourAngleJointStyle=0;	//四主材角钢接头方式
		intelligentConnect.m_iBoltLayoutStyle=0;		//双排螺栓布置方式
		intelligentConnect.m_bOnlyDesignCurView=FALSE;	//仅设计当前面连接板
	}
	if(compareVersion(version,"1.43")>=0)
	{
		ar>>intelligentConnect.m_nOffset;				
		ar>>intelligentConnect.m_nOffsetAdd;
		if(compareVersion(version,"1.64")>=0)
			ar>>intelligentConnect.m_bPriorOffsetGradientRod;
		ar>>intelligentConnect.m_n12LsOutterSpace;
		ar>>intelligentConnect.m_n16LsOutterSpace;
		ar>>intelligentConnect.m_n20LsOutterSpace;
		ar>>intelligentConnect.m_n24LsOutterSpace;
		ar>>intelligentConnect.m_n12LsOutterSpaceHasWasher;
		ar>>intelligentConnect.m_n16LsOutterSpaceHasWasher;
		ar>>intelligentConnect.m_n20LsOutterSpaceHasWasher;
		ar>>intelligentConnect.m_n24LsOutterSpaceHasWasher;
		
	}
	else
	{
		intelligentConnect.m_nOffset = 60;				
		intelligentConnect.m_nOffsetAdd = 10;
		intelligentConnect.m_n12LsOutterSpace = 15;
		intelligentConnect.m_n16LsOutterSpace = 20;
		intelligentConnect.m_n20LsOutterSpace = 25;
		intelligentConnect.m_n24LsOutterSpace = 30;
		intelligentConnect.m_n12LsOutterSpaceHasWasher = 20;
		intelligentConnect.m_n16LsOutterSpaceHasWasher = 25;
		intelligentConnect.m_n20LsOutterSpaceHasWasher = 30;
		intelligentConnect.m_n24LsOutterSpaceHasWasher = 35;
	}
	if (compareVersion(version,"1.62")>=0)
	{
		ar>>intelligentConnect.m_n12LsAngleEdgeSpace;
		ar>>intelligentConnect.m_n16LsAngleEdgeSpace;
		ar>>intelligentConnect.m_n20LsAngleEdgeSpace;
		ar>>intelligentConnect.m_n24LsAngleEdgeSpace;
		ar>>intelligentConnect.m_n12LsOutterStaggerSpace;
		ar>>intelligentConnect.m_n16LsOutterStaggerSpace;
		ar>>intelligentConnect.m_n20LsOutterStaggerSpace;
		ar>>intelligentConnect.m_n24LsOutterStaggerSpace;
		ar>>intelligentConnect.m_n12LsShareStaggerSpace;
		ar>>intelligentConnect.m_n16LsShareStaggerSpace;
		ar>>intelligentConnect.m_n20LsShareStaggerSpace;
		ar>>intelligentConnect.m_n24LsShareStaggerSpace;
	}
	else
	{
		intelligentConnect.m_n12LsAngleEdgeSpace=25;
		intelligentConnect.m_n16LsAngleEdgeSpace=25;
		intelligentConnect.m_n20LsAngleEdgeSpace=30;
		intelligentConnect.m_n24LsAngleEdgeSpace=40;
		intelligentConnect.m_n12LsOutterStaggerSpace=40;
		intelligentConnect.m_n16LsOutterStaggerSpace=40;
		intelligentConnect.m_n20LsOutterStaggerSpace=50;
		intelligentConnect.m_n24LsOutterStaggerSpace=60;
		intelligentConnect.m_n12LsShareStaggerSpace=35;
		intelligentConnect.m_n16LsShareStaggerSpace=35;
		intelligentConnect.m_n20LsShareStaggerSpace=45;
		intelligentConnect.m_n24LsShareStaggerSpace=55;
	}
	if(compareVersion(version,"1.34")>=0)
	{ 
		int n=0;
		ar>>n;
		m_arrL1RayPole.RemoveAll();
		m_arrL1RayPole.SetSize(n);
		for(i=0;i<n;i++)
		{
			ar>>m_arrL1RayPole[i];//名称变更
			if(m_arrL1RayPole[i].GetLength()>4&&m_arrL1RayPole[i][4]=='S')
				m_arrL1RayPole[i].SetAt(4,'B');
		}
		//
		n=0;
		ar>>n;
		m_arrL2RayPole.RemoveAll();
		m_arrL2RayPole.SetSize(n);
		for(i=0;i<n;i++)
		{
			ar>>m_arrL2RayPole[i];//名称变更
			if(m_arrL2RayPole[i].GetLength()>4&&m_arrL2RayPole[i][4]=='S')
				m_arrL2RayPole[i].SetAt(4,'B');
		}
		//
		n=0;
		ar>>n;
		m_arrL3RayPole.RemoveAll();
		m_arrL3RayPole.SetSize(n);
		for(i=0;i<n;i++)
			ar>>m_arrL3RayPole[i];
		if (compareVersion(version,"1.42")<0)//添加两种结构
		{
			m_arrL3RayPole.Add("L_3LB1");
			m_arrL3RayPole.Add("L_3LB2");
		}
		if(compareVersion(version,"1.65")<0)	//添加一种结构
			m_arrL3RayPole.Add("L_3LC3");
		//
		n=0;
		ar>>n;
		m_arrLnRayPole.RemoveAll();
		m_arrLnRayPole.SetSize(n);
		for(i=0;i<n;i++)
			ar>>m_arrLnRayPole[i];
	}
	else
	{
		m_arrL1RayPole.RemoveAll();
		m_arrL1RayPole.Add("L_1LB1");
		m_arrL1RayPole.Add("L_1LB2");
		m_arrL1RayPole.Add("L_1LO1");
		m_arrL1RayPole.Add("L_1LE1");
		m_arrL1RayPole.Add("L_1LE2");
		//
		m_arrL2RayPole.RemoveAll();
		m_arrL2RayPole.Add("L_2LB1");
		m_arrL2RayPole.Add("L_2LB2");
		m_arrL2RayPole.Add("L_2LB3");
		m_arrL2RayPole.Add("L_2LB4");
		m_arrL2RayPole.Add("L_2LO1");
		m_arrL2RayPole.Add("L_2LE1");
		m_arrL2RayPole.Add("L_2LE2");
		m_arrL2RayPole.Add("L_2LC1");
		m_arrL2RayPole.Add("L_2LC2");
		//
		m_arrL3RayPole.RemoveAll();
		m_arrL3RayPole.Add("L_3LO1");
		m_arrL3RayPole.Add("L_3LC1");
		m_arrL3RayPole.Add("L_3LC2");
		m_arrL3RayPole.Add("L_3LC3");
		//
		m_arrLnRayPole.RemoveAll();
	}
	//
	if(compareVersion(version,"1.35")>=0)
	{
		int n=0;
		ar>>n;
		m_arrInsertPlateType.RemoveAll();
		m_arrInsertPlateType.SetSize(n);
		for(i=0;i<n;i++)
			ar>>m_arrInsertPlateType[i];
		n=0;
		ar>>n;
		m_arrFlType.RemoveAll();
		m_arrFlType.SetSize(n);
		for(i=0;i<n;i++)
			ar>>m_arrFlType[i];
	}
	else
	{
		//钢管插板优先级
		m_arrInsertPlateType.RemoveAll();
		m_arrInsertPlateType.Add("C");
		m_arrInsertPlateType.Add("U");
		m_arrInsertPlateType.Add("X");
		//法兰优先级
		m_arrFlType.RemoveAll();
		m_arrFlType.Add("FLD");
		m_arrFlType.Add("FLP");
	}
	//格式刷设计参数 xc 10-11-01
	if(compareVersion(version,"1.38")>=0)
	{
		ar>>g_sysPara.brush.bCfgword;
		ar>>g_sysPara.brush.bColour;
		ar>>g_sysPara.brush.bDisplayHole;
		ar>>g_sysPara.brush.bGuige;
		ar>>g_sysPara.brush.bLayer;
		ar>>g_sysPara.brush.bMaterial;
		ar>>g_sysPara.brush.bSeg;
		ar>>g_sysPara.brush.bVirtualPart;
		//轮廓边构件选择
		ar>>g_sysPara.m_bModifyRelationPart;
		ar>>g_sysPara.bMergeRptFileMirObj;
		//
		if(compareVersion(version,"1.39")>=0)
		{
			ar>>g_sysPara.reactionExportType;
			ar>>g_sysPara.iLegShiftType;
		}
		if (compareVersion(version,"1.44")>=0)
		{
			ar>>g_sysPara.iEfficacyCeofType;
			ar>>g_sysPara.nM;
			ar>>g_sysPara.nSigemaM;
			ar>>g_sysPara.nPrecentSigemaM;
			ar>>g_sysPara.iShowDeformedMapPlan;
		}
		else
		{
			g_sysPara.iEfficacyCeofType = 0;
			g_sysPara.nM = 1;
			g_sysPara.nSigemaM = 1;
			g_sysPara.nPrecentSigemaM = 1;
			g_sysPara.iShowDeformedMapPlan = 0;
		}
	}
	else
	{
		g_sysPara.brush.bCfgword = FALSE;
		g_sysPara.brush.bColour = FALSE;
		g_sysPara.brush.bDisplayHole = FALSE;
		g_sysPara.brush.bGuige = FALSE;
		g_sysPara.brush.bLayer = FALSE;
		g_sysPara.brush.bMaterial = FALSE;
		g_sysPara.brush.bSeg = FALSE;
		g_sysPara.brush.bVirtualPart = FALSE;
		//轮廓边构件选择
		g_sysPara.m_bModifyRelationPart = FALSE;
		g_sysPara.bMergeRptFileMirObj = FALSE;
	}
	//法兰选择螺栓
	if(compareVersion(version,"1.38")>=0)
	{
		ar>>g_sysPara.flSelectLsTlb_N;
		for (int i=0;i<g_sysPara.flSelectLsTlb_N;i++)
		{
			ar>>g_sysPara.flSelectLs_table[i].d;
			ar>>g_sysPara.flSelectLs_table[i].disFL;
			ar>>g_sysPara.flSelectLs_table[i].disTube;
			ar>>g_sysPara.flSelectLs_table[i].grade;
		}
	}
	else
	{
		g_sysPara.flSelectLsTlb_N = 0;
	}
	//读取命令组参数 wht 12-04-24
	if(compareVersion(version,"1.53")>=0)
	{
		for(int i=0;i<4;i++)
		{
			ar>>cmdGroupParaArr[i].nMaxWarnings;
			if(compareVersion(version,"1.55")>=0)
				ar>>cmdGroupParaArr[i].bOutputToNotepad;
			ar>>cmdGroupParaArr[i].nCmdCount;
			for(int j=0;j<cmdGroupParaArr[i].nCmdCount;j++)
			{
				ar>>cmdGroupParaArr[i].orders[j];	//命令序号
				ar>>cmdGroupParaArr[i].actives[j];	//命令激活装填
			}
		}
	}
	}
	catch(...)
	{	//捕获读取配置文件异常，并启用系统默认配置 wht 16-05-25
		AfxMessageBox("读取配置文件失败,将启用默认配置！");
		InitSysPara();
		return FALSE;
	}
	GetPublicSysParaFromReg("Settings","KaiHeJiaoThreshold");
	GetPublicSysParaFromReg("Settings","KaiHeJiaoMinDeita");
	GetPublicSysParaFromReg("Settings","MatCharPosType");
	GetPublicSysParaFromReg("Settings","m_iPlankWeightStatType");
	GetPublicSysParaFromReg("Settings","m_iCirclePlankAreaType");
	GetPublicSysParaFromReg("Settings","m_iProfileAlgorithm");
	GetPublicSysParaFromReg("Settings","m_bEnablePrompt");
	//通过配置文件初始化文件属性中的卷边处理方式、卷边偏移方式及火曲变形处理方式
	GetPublicSysParaFromReg("Settings","RollProcessType");
	GetPublicSysParaFromReg("Settings","RollOffsetType");
	GetPublicSysParaFromReg("Settings","DeformType");
	GetPublicSysParaFromReg("Settings","DeformCoef");
	GetPublicSysParaFromReg("Settings","MatSeparator");
	GetPublicSysParaFromReg("Settings","m_bDelBoltsResideOnPlate");
	GetPublicSysParaFromReg("Settings","ciLinkAngleDlgFocusCtrlMode");
	//
	return TRUE;
}
BOOL CSysParaLibrary::Write(CString file_path)	//写配置文件
{
	CString version("1.92");
	if(file_path.IsEmpty())
		return FALSE;
	CFile file;
	if(!file.Open(file_path, CFile::modeCreate|CFile::modeWrite))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar200("System configuration files{%s} write failed!",file_path));
#else 
		AfxMessageBox(CXhChar200("系统配置文件{%s}写入失败,Win7、Win8及更高版本系统请设置以管理员身份运行软件!",file_path));
#endif
		return FALSE;
	}
	CArchive ar(&file,CArchive::store);
	ar<<version;
	int i;
	for(i=0;i<6;i++)
	{
		//显示条件1
		ar<<g_sysPara.char0_arr[i];
		//显示条件2
		ar<<g_sysPara.char1_arr[i];
		//显示条件3
		ar<<g_sysPara.char2_arr[i];
		ar<<(WORD)g_sysPara.bNodeArr[i];
	}
	ar<<dock.m_nLeftPageWidth;
	ar<<dock.m_nRightPageWidth;
	ar<<dock.m_nBottomPageHeight;
	ar<<(WORD)dock.pageProp.bDisplay;
	ar<<dock.pageProp.uDockBarID;
	ar<<(WORD)dock.pageModel.bDisplay;
	ar<<dock.pageModel.uDockBarID;
	ar<<(WORD)dock.pageCmd.bDisplay;
	ar<<dock.pageCmd.uDockBarID;
	ar<<(WORD)dock.pagePartLabel.bDisplay;
	ar<<dock.pagePartLabel.uDockBarID;
	ar<<(WORD)dock.pageWeldPart.bDisplay;
	ar<<dock.pageWeldPart.uDockBarID;
	ar<<(WORD)dock.pageOutput.bDisplay;
	ar<<dock.pageOutput.uDockBarID;
	ar<<(WORD)dock.pageRevision.bDisplay;
	ar<<dock.pageRevision.uDockBarID;
	ar<<(WORD)dock.pageLogger.bDisplay;
	ar<<dock.pageLogger.uDockBarID;

	ar<<g_sysPara.m_dwSelectPartsType;
	ar<<g_sysPara.nAutoSaveTime;
	ar<<g_sysPara.iAutoSaveType;
	ar<<g_sysPara.uFileSaveVersion;
	ar<<g_sysPara.m_bSaveLogVersion;
	ar<<g_sysPara.m_iModFileCodeType;
	ar<<g_sysPara.m_bDispPartACS;
	ar<<g_sysPara.m_bDispWorkUcs;
	ar<<g_sysPara.m_bDispBlockLCS;
	ar.Write(&m_bDragOpenWndSel,sizeof(bool));
	ar.Write(&m_bLineDepthTest,sizeof(bool));
	ar.Write(&m_bDblClkStartRotateOper,sizeof(bool));
	ar<<fRotateVelocity;	//塔高,用于控制旋转速度
	ar<<m_fMaxScaleScr2User;//局部放大定位时的最大缩放显示比例
	ar<<(WORD)g_sysPara.bMoveDispFrame;	//实体移动时显示线框图
	ar<<(WORD)g_sysPara.display.bNodePnt;	//显示单线节点
	ar<<g_sysPara.display.xPointStyle.wPointMode;
	ar<<CLDSLinePart::bDisplayLineVecMark;
	ar<<m_ciLowestWarningLevel;		//全局抑制系统警示信息
	ar<<CLDSLineTube::ciFlTubeOddment;
	ar<<CLDSLineAngle::m_bDisplaySolidAxisLine;
	ar<<CLDSGeneralPlate::m_ciDisplayVertexType;
	ar<<(WORD)display.bDimNodeLabel;			//标注节点编号
	ar<<display.nNodeLabelTextHeight;	//节点编号字高
	ar<<(WORD)display.bDimRodDesignInfo;		//标注杆件规格
	ar<<display.nRodDesignInfoTextHeight;//杆件规格字高
	ar<<(WORD)g_sysPara.display.bPoleLine;	//显示单线杆件线
	ar<<(WORD)g_sysPara.display.bPartSketch;//显示构件轮廓边以及轮廓点 wht 10-09-01
	ar<<(WORD)g_sysPara.display.bLoadArrow;	//显示荷载箭头
	ar<<g_sysPara.display.nLoadArrowScale;	//荷载箭头显示比例
	ar<<(WORD)g_sysPara.display.bWirePoint;	//显示挂点

	ar<<(WORD)CLDSNode::m_bDisplayBeforeArcLift;//单线显示预拱前位置
	ar<<(WORD)g_sysPara.display.bSolidJg;	//显示实体角钢
	ar<<(WORD)g_sysPara.display.bSolidTube;	//显示实体钢管
	ar<<(WORD)g_sysPara.display.bSolidJoint;//显示实体接头
	ar<<(WORD)g_sysPara.display.bSolidLjb;	//显示实体连接板
	ar<<(WORD)g_sysPara.display.bSolidLs;	//显示实体螺栓
	//ar<<(WORD)g_sysPara.display.bDispVirtualPart;	//显示虚拟辅助构件
	ar<<(WORD)g_sysPara.display.bSolidTransectFace;	//显示相贯面
	ar<<(WORD)g_sysPara.display.bSolidSphere;	//显示球体
	ar<<(WORD)g_sysPara.display.bSolidSlot;		//显示实体槽钢
	ar<<(WORD)g_sysPara.display.bSolidFlat;		//显示实体扁钢
	ar<<(WORD)g_sysPara.display.bSolidArcAngle;	//显示实体环形角钢
	ar<<(WORD)g_sysPara.display.bSolidArcSlot;	//显示实体环形槽钢
	ar<<(WORD)g_sysPara.display.bSolidArcFlat;	//显示实体环形扁钢
	ar<<(WORD)g_sysPara.display.bSolidStdPart;	//显示实体标准构件
	ar<<(WORD)g_sysPara.display.bSolidLine;		//显示实体单线
	ar<<(WORD)g_sysPara.display.bSolidFoundation;
	ar<<(WORD)g_sysPara.display.nSmoothness;	//实体显示时圆弧显示的最小面数
	//因为g_sysPara.display.bSolidLs已保存，如果g_sysPara.bDisplayAllHole不保存
	//关闭文件时如果为显示螺栓孔状态，则会出现既不显示螺栓也不显示螺栓孔的情况 wht 10-07-08
	ar<<(WORD)g_sysPara.bDisplayAllHole;		//显示所有孔 1.32
#if defined(__LDS_)||defined(__TSA_)||defined(__LDSGROUP_)||defined(__TSAGROUP_)
	ar<<bCalStdFoundationForce;
	ar<<bUpdateRodDesignForce;
	ar<<bCreateAnalFile;
	ar<<bRecNodeOffset;
	ar<<bCreateAnsysCmdFile;
	ar<<bCreateDetailRptFile;
	ar<<iLanguage;
	ar<<iDetailDocType;
	ar<<m_bAuxPoleAnalysisInc;
	//应力分析数据
	ar<<AnalysisPara.m_iNonlinearStyle;	//否则以线性方式计算
	ar<<AnalysisPara.m_iResolverStyle;	//求解器方式
	ar<<AnalysisPara.m_nLoadStepCounts;	//分几次加载
	ar<<AnalysisPara.m_nIterationCounts;	//每次增量最多迭代次数
	ar<<AnalysisPara.m_fConvergeLim;		//应力分析计算收敛阈值
	ar<<AnalysisPara.m_nSubSpaceIterCounts;	//子空间迭代法最多迭代次数
	ar<<AnalysisPara.m_fFreqsTolerance;		//频率千分之一误差率

	ar<<(WORD)InterpBetaZArr.Size();
	for(i=0;i<(int)InterpBetaZArr.Size();i++)
		ar.Write(&InterpBetaZArr[i],sizeof(double)*3);	//风振系数插值表
	//double m_fWindBetaZForFoundation=1.3;	//为保证文件格式读取,临时设定
	//ar<<m_fWindBetaZForFoundation;	//基础设计时风荷载调整系数
	ar<<(WORD)InterpIceLoadCoefArr.Size();
	for(i=0;i<(int)InterpIceLoadCoefArr.Size();i++)
		ar.Write(&InterpIceLoadCoefArr[i],sizeof(double)*2);	//杆塔覆冰重力荷载系数表
#endif
	//绘图参数选项
	//火曲线间隙: 火曲线到角钢边缘距离一般为5mm~10mm
	ar<<(WORD)g_sysPara.HuoQuDist;
	//角钢间隙: 角钢与角钢对接时之间的距离一般为8mm
	ar<<(WORD)g_sysPara.JgEndDist;		//接头处上下角钢之间的间隙
	ar<<(WORD)g_sysPara.JointPlankDist;	//接头与板间的间隙
	ar<<(WORD)g_sysPara.VertexDist;		//角钢顶点间隙
	ar<<(WORD)g_sysPara.CollideDist;    //碰撞间隙
	ar<<(WORD)g_sysPara.FootNailDist;	//脚钉缺省间距
	ar<<(WORD)g_sysPara.fNailRepScope;	//脚钉代孔范围
	ar<<(WORD)g_sysPara.fTubeWeldRoadMinD;//存在焊道线的钢管最小直径
	ar<<g_sysPara.fStressDiffuseAngle;	//应力扩散角
	ar<<g_sysPara.fInsertPlateEdgeSpace;  //插板边距
	//1.77版本  修改对接法兰钢管正负头
	ar<<(WORD)g_sysPara.m_bModifyCoupledFLTubeOddment;
	//法兰与钢管间隙:法兰与钢管之间的距离一般为1mm
	ar<<(WORD)g_sysPara.TubeFlDist;
	ar<<(WORD)g_sysPara.m_nPlankEdgeAngleThreshold;
	ar<<nAutoAdjustTubePos;	//0.不启用 1.插板偏移 2.钢管偏移
	ar<<m_fKaiHeJiaoThreshold;			//1.46 角钢开合角阈值  //wht 11-05-06
	ar<<m_fKaiHeJiaoMinDeita;			//1.78 开合角外侧最小间隙
	ar<<m_fMinCutAngleLen;                  //最小切角值
	ar<<m_fMaxIgnoreCutAngleLen;            //最大忽略切角值
	ar<<m_nMatCharPosType;               //构件编号的材质字符位置（0.不需要 1.构件编号前 2.构件编号后）
	ar<<m_b45CutAngleNeedCoefR;
	ar<<m_fCutAngleCoefR;
	ar<<(BYTE)m_bAnglePreferSimplePosMode;
	ar<<display.cHoleWallMode;
	ar<<CLDSPart::UPDATE_PARTNO_AND_SEG;			//1.71 构件编号随段号更新
	for(i=0;i<4;i++)
	{
		ar<<g_sysPara.arrRule[i].iOrder;
		if(g_sysPara.arrRule[i].bActive)
			ar<<(WORD)1;
		else
			ar<<(WORD)0;
		switch(g_sysPara.arrRule[i].name)
		{
		case DOWN_UP:
			ar<<(WORD)0;
			break;
		case LEFT_RIGHT:
			ar<<(WORD)1;
			break;
		case MAIN_OTHER:
			ar<<(WORD)2;
			break;
		case FRONT_SIDE:
			ar<<(WORD)3;
			break;
		default:
			ar<<(WORD)0;
			break;
		}
	}
	//单螺栓垫圈最小厚度	wht 10-12-24
	ar.Write(BoltPadThick.xarrM12Thicks,4);
	ar.Write(BoltPadThick.xarrM16Thicks,4);
	ar.Write(BoltPadThick.xarrM20Thicks,4);
	ar.Write(BoltPadThick.xarrM24Thicks,4);
	//单螺栓设计前提示用户
	ar<<(WORD)g_sysPara.b1BoltDesignPrompt;
	ar<<crPartMode.crNode;			//节点颜色
	ar<<crPartMode.crControlNode;   //控制节点
	ar<<crPartMode.crDivideScaleNode;//等分节点
	ar<<crPartMode.crOffsetNode;	//偏移节点
	ar<<crPartMode.crAxisValFixNode;//指定坐标分量节点
	ar<<crPartMode.crPole;			//角钢颜色
	ar<<crPartMode.crLine;			//角钢单线颜色
	ar<<crPartMode.crAngleWingX;	//角钢X肢显示颜色
	ar<<crPartMode.crAngleWingY;	//角钢Y肢显示颜色
	ar<<crPartMode.crAngleEdge;		//角钢肢边颜色
	ar<<crPartMode.crSonAngle1WingX;//1号子角钢X肢显示颜色 wht 11-05-20
	ar<<crPartMode.crSonAngle1WingY;//1号子角钢Y肢显示颜色
	ar<<crPartMode.crSonAngle1Edge;	//1号子角钢肢边颜色
	ar<<crPartMode.crOtherBolt;			//螺栓颜色
	ar<<crPartMode.crBolt12;			//螺栓颜色
	ar<<crPartMode.crBolt16;			//螺栓颜色
	ar<<crPartMode.crBolt20;			//螺栓颜色
	ar<<crPartMode.crBolt24;			//螺栓颜色
	ar<<crPartMode.crBolt27;			//螺栓颜色
	ar<<crPartMode.crBolt30;			//螺栓颜色
	ar<<crPartMode.crBolt36;			//螺栓颜色
	ar<<crPartMode.crBolt39;			//螺栓颜色
	ar<<crPartMode.crBolt42;			//螺栓颜色
	ar<<crPartMode.crBolt48;			//螺栓颜色
	ar<<crPartMode.crAntiTheftNut;		//防盗帽颜色
	ar<<crPartMode.crAntiLoosenNut;		//防松帽颜色
	ar<<crPartMode.crPlate;				//板颜色
	ar<<crPartMode.crCutFaceOrPolyJoint;//火曲角钢颜色
	ar<<crPartMode.crBackground;		//背景颜色
	ar<<crPartMode.crPartBackground;	//构件编辑器背景颜色
	//1.70
	CString keyStr;
	int nNum=plateColRef.colorHashList.GetNodeNum();
	ar<<nNum;
	for(COLORREF* pColRef=plateColRef.colorHashList.GetFirst();pColRef;pColRef=plateColRef.colorHashList.GetNext())
	{
		keyStr.Format("%s",plateColRef.colorHashList.GetCursorKey());
		ar<<keyStr;
		ar<<*pColRef;
	}
	//1.27
	ar<<iColorSchema;
	ar<<crMaterialMode.crNode;
	ar<<crMaterialMode.crLine;
	ar<<crMaterialMode.crPole;
	//1.26
	ar<<crPartMode.crShortAngle;
	ar<<crPartMode.crStdPart;
	ar<<crMaterialMode.crBackground;
	ar<<crMaterialMode.crQ235;
	ar<<crMaterialMode.crQ345;
	ar<<crMaterialMode.crQ390;
	ar<<crMaterialMode.crQ420;
	ar<<crMaterialMode.crQ460;
	ar<<crMaterialMode.crOther;
	ar<<crTsaMode.crBackground;
	ar<<crTsaMode.crDeformedPoleLine;
	ar<<crTsaMode.crPostLine;
	ar<<crTsaMode.crPrimaryLine;
	ar<<crTsaMode.crAuxLine;
	ar<<crTsaMode.crDimText;
	ar<<crTsaMode.crLoadArrow;
	ar<<crTsaMode.crNode;
	ar<<crTsaMode.crPole;
	//1.69
	ar<<crTsaMode.crZeroStressElem;
	ar<<crTsaMode.bEfficColorLinearVary;
	ar<<crTsaMode.crOverflowElem;
	for(int j=0;j<6;j++)
	{
		ar<<crTsaMode.crElemDivArr[j].efficPercent;
		ar<<crTsaMode.crElemDivArr[j].crElemDivision;
	}

	//智能连接设计设置参数 wht 10-01-15
	ar<<m_bDisplayConnectDlg;				//设计过程中显示设置对话框
	ar<<intelligentConnect.m_nPlateThick;		//钢板厚度
	ar<<intelligentConnect.m_cMaterial;			//钢板材质
	ar<<intelligentConnect.m_bAutoThick;		//自动计算钢板厚度
	ar<<intelligentConnect.m_bAutoMat;			//自动设置钢板材质
	ar<<intelligentConnect.m_bDesignCommonConnect;	//智能设计普通连接
	ar<<intelligentConnect.m_bDesignBendConnect;	//智能设计制弯板连接
	ar<<intelligentConnect.m_bDesignAngleJoint;		//智能设计角钢接头
	ar<<intelligentConnect.m_bDesignFL;				//智能设计法兰
	ar<<intelligentConnect.m_bDesignTubeCoverPlate;	//智能设计钢管上的环向封板
	ar<<intelligentConnect.m_iPlateProfileStyle;	//钢板外形计算方式
	ar<<intelligentConnect.m_iSingleAngleJointStyle;//单角钢接头方式
	ar<<intelligentConnect.m_iTwoAngleJointStyle;	//双主材角钢接头方式
	ar<<intelligentConnect.m_iFourAngleJointStyle;	//四主材角钢接头方式
	ar<<intelligentConnect.m_iBoltLayoutStyle;		//双排螺栓布置方式
	ar<<intelligentConnect.m_bOnlyDesignCurView;	//仅设计当前面连接板
	//新加设置参数
	ar<<intelligentConnect.m_nOffset;				//孤立角钢端连接基准偏移值
	ar<<intelligentConnect.m_nOffsetAdd;			//孤立角钢端连接基准偏步长
	ar<<intelligentConnect.m_bPriorOffsetGradientRod;
	//外孔间隙
	ar<<intelligentConnect.m_n12LsOutterSpace;
	ar<<intelligentConnect.m_n16LsOutterSpace;
	ar<<intelligentConnect.m_n20LsOutterSpace;
	ar<<intelligentConnect.m_n24LsOutterSpace;
	ar<<intelligentConnect.m_n12LsOutterSpaceHasWasher;
	ar<<intelligentConnect.m_n16LsOutterSpaceHasWasher;
	ar<<intelligentConnect.m_n20LsOutterSpaceHasWasher;
	ar<<intelligentConnect.m_n24LsOutterSpaceHasWasher;
	//螺栓肢翼边距B
	ar<<intelligentConnect.m_n12LsAngleEdgeSpace;
	ar<<intelligentConnect.m_n16LsAngleEdgeSpace;
	ar<<intelligentConnect.m_n20LsAngleEdgeSpace;
	ar<<intelligentConnect.m_n24LsAngleEdgeSpace;
	//外孔螺栓错孔间距C
	ar<<intelligentConnect.m_n12LsOutterStaggerSpace;
	ar<<intelligentConnect.m_n16LsOutterStaggerSpace;
	ar<<intelligentConnect.m_n20LsOutterStaggerSpace;
	ar<<intelligentConnect.m_n24LsOutterStaggerSpace;
	//共用螺栓错孔间距S
	ar<<intelligentConnect.m_n12LsShareStaggerSpace;
	ar<<intelligentConnect.m_n16LsShareStaggerSpace;
	ar<<intelligentConnect.m_n20LsShareStaggerSpace;
	ar<<intelligentConnect.m_n24LsShareStaggerSpace;
	//
	ar<<m_arrL1RayPole.GetSize();
	for(i=0;i<m_arrL1RayPole.GetSize();i++)
		ar<<m_arrL1RayPole[i];
	//
	ar<<m_arrL2RayPole.GetSize();
	for(i=0;i<m_arrL2RayPole.GetSize();i++)
		ar<<m_arrL2RayPole[i];
	//
	ar<<m_arrL3RayPole.GetSize();
	for(i=0;i<m_arrL3RayPole.GetSize();i++)
		ar<<m_arrL3RayPole[i];
	//
	ar<<m_arrLnRayPole.GetSize();
	for(i=0;i<m_arrLnRayPole.GetSize();i++)
		ar<<m_arrLnRayPole[i];
	//钢管插板优先级 wht 10-08-19
	ar<<m_arrInsertPlateType.GetSize();
	for(i=0;i<m_arrInsertPlateType.GetSize();i++)
		ar<<m_arrInsertPlateType[i];
	//法兰优先级
	ar<<m_arrFlType.GetSize();
	for(i=0;i<m_arrFlType.GetSize();i++)
		ar<<m_arrFlType[i];
	//格式刷设计参数 xc 10-11-01 1.38
	ar<<g_sysPara.brush.bCfgword;
	ar<<g_sysPara.brush.bColour;
	ar<<g_sysPara.brush.bDisplayHole;
	ar<<g_sysPara.brush.bGuige;
	ar<<g_sysPara.brush.bLayer;
	ar<<g_sysPara.brush.bMaterial;
	ar<<g_sysPara.brush.bSeg;
	ar<<g_sysPara.brush.bVirtualPart;
	//轮廓边构件选择
	ar<<g_sysPara.m_bModifyRelationPart;
	//合并力学分析计算书中的对称节点和杆件
	ar<<g_sysPara.bMergeRptFileMirObj;
	ar<<g_sysPara.reactionExportType;
	ar<<g_sysPara.iLegShiftType;
	ar<<g_sysPara.iEfficacyCeofType;
	ar<<g_sysPara.nM;
	ar<<g_sysPara.nSigemaM;
	ar<<g_sysPara.nPrecentSigemaM;
	ar<<g_sysPara.iShowDeformedMapPlan;
	//
	ar<<g_sysPara.flSelectLsTlb_N;
	for (i=0;i<g_sysPara.flSelectLsTlb_N;i++)
	{
		ar<<g_sysPara.flSelectLs_table[i].d;
		ar<<g_sysPara.flSelectLs_table[i].disFL;
		ar<<g_sysPara.flSelectLs_table[i].disTube;
		ar<<g_sysPara.flSelectLs_table[i].grade;
	}
	WritePublicSysParaToReg();	//保存共用参数至注册表 
	//保存命令组参数 wht 12-04-24
	for(i=0;i<4;i++)
	{
		ar<<cmdGroupParaArr[i].nMaxWarnings;
		ar<<cmdGroupParaArr[i].bOutputToNotepad;
		ar<<cmdGroupParaArr[i].nCmdCount;
		for(int j=0;j<cmdGroupParaArr[i].nCmdCount;j++)
		{
			ar<<cmdGroupParaArr[i].orders[j];	//命令序号
			ar<<cmdGroupParaArr[i].actives[j];	//命令激活装填
		}
	}
	return TRUE;
}
COLORREF CSysParaLibrary::CalEfficColor(double efficCoef)
{
	BYTE r=0,g=0,b=0;
#ifdef __LDS_CONTEXT_
	if(efficCoef<=0)
	{
		r=GetRValue(crTsaMode.crZeroStressElem);
		g=GetGValue(crTsaMode.crZeroStressElem);
		b=GetBValue(crTsaMode.crZeroStressElem);
	}
	else if(efficCoef>1.0)
	{
		r=GetRValue(crTsaMode.crOverflowElem);
		g=GetGValue(crTsaMode.crOverflowElem);
		b=GetBValue(crTsaMode.crOverflowElem);
	}
	else if(crTsaMode.bEfficColorLinearVary)	//渐变配色方案
	{
		C2dInterpolator interp;
		double effic_rows[6];
		double critem_cols[3]={0,1,2};
		double data[18];
		for(int i=0;i<6;i++)
		{
			effic_rows[i]=crTsaMode.crElemDivArr[i].efficPercent;
			data[i*3+0]=GetRValue(crTsaMode.crElemDivArr[i].crElemDivision);
			data[i*3+1]=GetGValue(crTsaMode.crElemDivArr[i].crElemDivision);
			data[i*3+2]=GetBValue(crTsaMode.crElemDivArr[i].crElemDivision);
		}
		interp.Init(6,3,data);
		interp.InitHeader(critem_cols,effic_rows);
		r=(BYTE)ftoi(interp.Interpolate(0,efficCoef*100));
		g=(BYTE)ftoi(interp.Interpolate(1,efficCoef*100));
		b=(BYTE)ftoi(interp.Interpolate(2,efficCoef*100));
	}
	else	//分段配色方案
	{
		for(int i=0;i<6;i++)
		{
			int percent=ftoi(efficCoef*100);
			if(crTsaMode.crElemDivArr[i].efficPercent>100)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("The current gradient-color-scheme's color line is short of the 100's threshold");
#else 
				logerr.Log("当前渐变配色方案的颜色分界线缺少100的最终临界值");
#endif
				break;
			}
			else if(percent<=crTsaMode.crElemDivArr[i].efficPercent)
			{
				r=GetRValue(crTsaMode.crElemDivArr[i].crElemDivision);
				g=GetGValue(crTsaMode.crElemDivArr[i].crElemDivision);
				b=GetBValue(crTsaMode.crElemDivArr[i].crElemDivision);
				break;
			}
			//else continue;
		}
	}
#endif
	return RGB(r,g,b);
}

//从注册表中获取共用参数 wht 11-06-10
void CSysParaLibrary::GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry)	
{
	char sSubKey[MAX_PATH]="";
#ifdef __TMA_
	sprintf(sSubKey,"Software\\Xerofox\\TMA\\%s",lpszSection);
#elif defined(__LMA_)
	sprintf(sSubKey,"Software\\Xerofox\\LMA\\%s",lpszSection);
#elif defined(__TSA_)
	sprintf(sSubKey,"Software\\Xerofox\\TSA\\%s",lpszSection);
#else
	sprintf(sSubKey,"Software\\Xerofox\\LDS\\%s",lpszSection);
#endif
	HKEY hSecKey=0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hSecKey)==ERROR_SUCCESS)
	{	//确保注册表中存在该项时再继续查找 wht 11-06-10
		/*m_uRollProcessType=AfxGetApp()->GetProfileInt(_T("Settings"),
		_T("RollProcessType"),m_uRollProcessType);
		CString sValue=_T("");
		sValue.Format("%f",m_fKaiHeJiaoThreshold);
		sValue=AfxGetApp()->GetProfileString(_T("Settings"),_T("KaiHeJiaoThreshold"),sValue);
		m_fKaiHeJiaoThreshold=atof(sValue);
		RegCloseKey(hSecKey);*/
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		if(stricmp(lpszEntry,"KaiHeJiaoThreshold")==0)
		{
			char sValue[MAX_PATH]="";
			if (RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,NULL, &dwCount) == ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_SZ);
				if(RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,(LPBYTE)&sValue[0],&dwCount)==ERROR_SUCCESS)
					m_fKaiHeJiaoThreshold=atof(sValue);
			}
		}
		else if(stricmp(lpszEntry,"KaiHeJiaoMinDeita")==0)
		{
			char sValue[MAX_PATH]="";
			if (RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,NULL, &dwCount)==ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_SZ);
				if(RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,(LPBYTE)&sValue[0], &dwCount)==ERROR_SUCCESS)
					m_fKaiHeJiaoMinDeita=atof(sValue);
			}
		}
		else if(stricmp(lpszEntry,"GroupJgMachiningGap")==0)
		{
			if(RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType,(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				m_nGroupJgMachiningGap=(INT)dwValue;
		}
		else if(stricmp(lpszEntry,"MatCharPosType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("MatCharPosType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				m_nMatCharPosType=(INT)dwValue;
		}
		else if(stricmp(lpszEntry,"m_iPlankWeightStatType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("m_iPlankWeightStatType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				CLDSPlate::m_iPlankWeightStatType=(INT)dwValue;
		}
		else if(stricmp(lpszEntry,"m_iCirclePlankAreaType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("m_iCirclePlankAreaType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				CLDSParamPlate::m_iCirclePlankAreaType=(INT)dwValue;
		}
		else if(stricmp(lpszEntry,"m_iProfileAlgorithm")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("m_iProfileAlgorithm"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				CLDSPlate::m_siProfileAlgor=(short)dwValue;
		}
		else if(stricmp(lpszEntry,"m_bEnablePrompt")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("m_bEnablePrompt"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				m_bEnablePrompt=(BOOL)dwValue;
		}
		else if(stricmp(lpszEntry,"RollProcessType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("RollProcessType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				Ta.m_ciRollProcessType=(BYTE)dwValue;
		}
		else if(stricmp(lpszEntry,"RollOffsetType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("RollOffsetType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				Ta.m_ciRollOffsetType=(BYTE)dwValue;
		}
		else if(stricmp(lpszEntry,"DeformType")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("DeformType"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				Ta.m_ciDeformType=(BYTE)dwValue;
		}
		else if(stricmp(lpszEntry,"DeformCoef")==0)
		{
			char sValue[MAX_PATH]="";
			if (RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,NULL, &dwCount)==ERROR_SUCCESS)
			{
				ASSERT(dwType == REG_SZ);
				if(RegQueryValueEx(hSecKey,lpszEntry,NULL,&dwType,(LPBYTE)&sValue[0], &dwCount)==ERROR_SUCCESS)
					Ta.m_fDeformCoef=atof(sValue);
			}
		}
		else if(stricmp(lpszEntry,"MatSeparator")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("MatSeparator"), NULL, &dwType,
				(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				m_cMatCharSeparator=(BYTE)dwValue;
		}
		else if(stricmp(lpszEntry,"m_bDelBoltsResideOnPlate")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("m_bDelBoltsResideOnPlate"), NULL, &dwType,(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				m_bDelBoltsResideOnPlate=(BOOL)dwValue;
		}
		else if(stricmp(lpszEntry,"ciLinkAngleDlgFocusCtrlMode")==0)
		{
			if(RegQueryValueEx(hSecKey, _T("ciLinkAngleDlgFocusCtrlMode"), NULL, &dwType,(LPBYTE)&dwValue, &dwCount)== ERROR_SUCCESS)
				ciLinkAngleDlgFocusCtrlMode=(char)dwValue;
		}
		RegCloseKey(hSecKey);
	}
}
void CSysParaLibrary::WritePublicSysParaToReg()	//保存共用参数至注册表
{
	CString sValue=_T("");
	sValue.Format("%f",m_fKaiHeJiaoThreshold);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("KaiHeJiaoThreshold"),sValue);
	sValue.Format("%f",m_fKaiHeJiaoMinDeita);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("KaiHeJiaoMinDeita"),sValue);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("GroupJgMachiningGap"),m_nGroupJgMachiningGap);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("MatCharPosType"),m_nMatCharPosType);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("m_iPlankWeightStatType"),CLDSPlate::m_iPlankWeightStatType);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("m_iCirclePlankAreaType"),CLDSParamPlate::m_iCirclePlankAreaType);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("m_iProfileAlgorithm"),CLDSPlate::m_siProfileAlgor);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("m_bEnablePrompt"),m_bEnablePrompt);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("RollProcessType"),Ta.m_ciRollProcessType);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("RollOffsetType"),Ta.m_ciRollOffsetType);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("DeformType"),Ta.m_ciDeformType);
	sValue.Format("%f",Ta.m_fDeformCoef);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("DeformCoef"),sValue);
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("MatSeparator"),BYTE(m_cMatCharSeparator));
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("m_bDelBoltsResideOnPlate"),BYTE(m_bDelBoltsResideOnPlate));
	AfxGetApp()->WriteProfileInt(_T("Settings"),_T("ciLinkAngleDlgFocusCtrlMode"),BYTE(ciLinkAngleDlgFocusCtrlMode));
}
#if defined(__TMA_)||defined(__LMA_)
#include "DataCompare.h"
#include "LicFuncDef.h"
#endif
char CSysParaLibrary::get_ciLiveFocusCtrlInLinkAngleDlg()
{	//0.动态设定;1.规格框优先;2.构件编号优先
#if defined(__TMA_)||defined(__LMA_)
	bool blSupportLabelInput=PRODUCT_FUNC::IsHasInternalTest()||(GetLicVersion()>=1000003&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM));
	if (globalDataCmpModel.GetExterDrwBomPartsCount()==0)
	{
		CAttachBuffer* pAttachBuff=Ta.AttachBuffer.GetValue(CTower::BOM_COMPARE_MODEL);
		if (pAttachBuff)
		{
			pAttachBuff->SeekToBegin();
			globalDataCmpModel.ReadFromDrawAndBomBuffer(pAttachBuff);
		}
	}
	if( blSupportLabelInput&&ciLinkAngleDlgFocusCtrlMode==0&&globalDataCmpModel.GetExterDrwBomPartsCount()>0)
		return 2;	//构件编号优先
	else
#endif
		return ciLinkAngleDlgFocusCtrlMode>0?ciLinkAngleDlgFocusCtrlMode:1;
}
////////////////////////////////////////////////////////////////////
#include "LicFuncDef.h"
bool PRODUCT_FUNC::m_bDisableInternalTest=false;
// 证书授权功能类型判断
BOOL PRODUCT_FUNC::IsHasBasicPortal()	//是否具有基本操作框架功能
{
	DWORD identity=0x01;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_BASIC);
	else
		return VerifyValidFunction(identity);
}
BOOL PRODUCT_FUNC::IsHasRoofDesign()	//是否可以设计微波平台
{
	if(PRODUCT_TYPE!=1&&PRODUCT_TYPE!=3)
		return TRUE;
	/*else if(PRODUCT_TYPE==1)
	{
		BYTE identity[16]={0};
		if(GetLicVersion()>=1000003)
			StringToMemory(FUNC_IDENTITY_ROOF_DESIGN,identity);
		else
			identity[0]=0x02;
		return VerifyValidFunction(identity);
	}
	else*/
		return FALSE;
}
BOOL PRODUCT_FUNC::IsHasStdTemplate()	//是否可以设计微波平台
{
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
	return TRUE;
#else
	if(PRODUCT_TYPE!=1&&PRODUCT_TYPE!=2)
		return TRUE;
	else
	{
		BYTE identity[16]={0};
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_STD_PANEL);
		else
			return FALSE;
	}
#endif
}
BOOL PRODUCT_FUNC::IsHasPPE()	//是否可以设计微波平台
{
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
	return TRUE;
#else
	if(PRODUCT_TYPE!=1&&PRODUCT_TYPE!=2)
		return TRUE;
	else
	{
		BYTE identity[16]={0};
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PPE_EDITOR);
		else
			return FALSE;
	}
#endif
}
BOOL PRODUCT_FUNC::IsHasCopyLeg()		//是否具有接腿拷贝功能
{
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
	return TRUE;
#else
	if(PRODUCT_TYPE!=1&&PRODUCT_TYPE!=2)
		return TRUE;
	else
	{
		BYTE identity[16]={0};
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_COPY_LEG);
		else
			return FALSE;
	}
#endif
}
BOOL PRODUCT_FUNC::IsHasGrxModule()	//是否支持皓晨绘图模块
{
#if defined(__TMA_FILE_)||defined(__LMA_FILE_)
	DWORD identity=0x04;
	if(GetLicVersion()>=1000003)
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_GRX_MODULE);
	else
		return VerifyValidFunction(identity);
#endif
	return FALSE;
}
BOOL PRODUCT_FUNC::IsHasZrxModule()	//是否支持中望绘图模块
{
#if defined(__TMA_FILE_)||defined(__LMA_FILE_)
	DWORD identity=0x08;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_ZRX_MODULE);
	else
		return VerifyValidFunction(identity);
		//DWORD dwFuncFlag=GetProductFuncFlag();
		//return (dwFuncFlag&0x08)!=0;
#endif
	return FALSE;
}
BOOL PRODUCT_FUNC::IsHasAngleFoot()	//是否支持角钢塔脚
{
	if(PRODUCT_TYPE==2)
	{	//LMA软件
		DWORD identity=0x02;
		if(GetLicVersion()>=1000003)
			return TRUE;//StringToMemory(LICFUNC::FUNC_IDENTITY_ANGLE_FOOT,identity);
		else
			return VerifyValidFunction(identity);
	}
	else if(PRODUCT_TYPE==3)
		return FALSE;
	else
		return TRUE;
}
BOOL PRODUCT_FUNC::IsHasColumnRayRod()//是否支持柱面射线构件
{
#ifndef __COMPLEX_PART_INC_
	return FALSE;
#else
	if(PRODUCT_TYPE==1||PRODUCT_TYPE==3)
		return FALSE;
	else if(PRODUCT_TYPE==2)
	{	//LMA软件
		DWORD identity=0x10;
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_COLUMN_RAYROD);
		else
			return VerifyValidFunction(identity);
	}
	else
		return TRUE;
#endif
}
BOOL PRODUCT_FUNC::IsHasSphereDesign()//是否支持球节点
{
#ifndef __COMPLEX_PART_INC_
	return FALSE;
#else
	if(PRODUCT_TYPE==1||PRODUCT_TYPE==3)
		return FALSE;
	else if(PRODUCT_TYPE==2)
	{	//LMA软件
		DWORD identity=0x20;
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SPHERE_DESIGN);
		else
			return VerifyValidFunction(identity);
	}
	else
		return TRUE;
#endif
}
BOOL PRODUCT_FUNC::IsHasAutoArcLift()	//是否自动圆弧预拱
{
	if(PRODUCT_TYPE==3)
		return FALSE;
	else if(PRODUCT_TYPE==4||PRODUCT_TYPE==5)
		return TRUE;
	else
	{	//LMA软件
		DWORD identity=0;
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_AUTO_ARCLIFT);
		else
		{
			if(PRODUCT_TYPE==1)
				identity=0x10;
			else if(PRODUCT_TYPE==2)
				identity=0x40;
		}
		return VerifyValidFunction(identity);
	}
}
BOOL PRODUCT_FUNC::IsHasShadowObject()//是否支持影射对象
{
	if(PRODUCT_TYPE==3)
		return FALSE;
	else if(PRODUCT_TYPE==4||PRODUCT_TYPE==5)
		return TRUE;
	else
	{	//LMA软件
		DWORD identity=0;
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_SHADOW_OBJECT);
		else
		{
			if(PRODUCT_TYPE==1)
				identity=0x20;
			else if(PRODUCT_TYPE==2)
				identity=0x80;
		}
		return VerifyValidFunction(identity);
	}
}
BOOL PRODUCT_FUNC::IsHasBlockSetup()	//是否支持部件组装
{
	if(PRODUCT_TYPE==3)
		return FALSE;
	else if(PRODUCT_TYPE==4||PRODUCT_TYPE==5)
		return TRUE;
	else
	{	//T/LMA软件
		DWORD identity=0x01;
		if(GetLicVersion()>=1000003)
			return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_BLOCK_ASSEMBLY);
		else
			return VerifyValidFunction(identity);
	}
	return TRUE;
}
BOOL PRODUCT_FUNC::IsHasTidExport()	//是否支持导出Tid文件
{
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT);
	else
		return FALSE;
}
BOOL PRODUCT_FUNC::IsHasInternalTest()	//是否支持内部测试功能
{
	if(m_bDisableInternalTest)
		return FALSE;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST);
	else
		return FALSE;
}
BOOL PRODUCT_FUNC::IsHasCableAnalysis()		//是否支持非线性拉线塔分析
{
#ifndef __PART_DESIGN_INC_
	//if(PRODUCT_TYPE!=3&&PRODUCT_TYPE!=4)
	return FALSE;
#else
	DWORD identity=0x02;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_CABLE_ANLYS);
	else
		return VerifyValidFunction(identity);
#endif
}
BOOL PRODUCT_FUNC::IsHasDynamicAnalysis()		//是否支持动力特性分析
{
#ifndef __PART_DESIGN_INC_
	//if(PRODUCT_TYPE!=3&&PRODUCT_TYPE!=4)
		return FALSE;
#else
	DWORD identity=0x04;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_DYNAMIC_ANLYS);
	else
		return VerifyValidFunction(identity);
#endif
}
BOOL PRODUCT_FUNC::IsHasNonlinearAnalysis()	//是否支持非线性梁杆结构分析
{
#ifndef __PART_DESIGN_INC_
	//if(PRODUCT_TYPE!=3&&PRODUCT_TYPE!=4)
		return FALSE;
#else
	DWORD identity=0x08;
	if(GetLicVersion()>=1000003)
		return VerifyValidFunction(LICFUNC::FUNC_IDENTITY_NONLINEAR_ANLYS);
	else
		return VerifyValidFunction(identity);
#endif
}
////////////////////////////////////////////////////////////////////

#if defined(__LDS_)||defined(__TSA_)
//根据高度插值计算风振系数值
double CSysParaLibrary::Beta_z_InterpByHigh(double highpos,bool bForFoundation)
{
	for(DWORD i=0;i<g_sysPara.InterpBetaZArr.Size();i++)
	{
		if(highpos<InterpBetaZArr[i].pivot_key)
		{
			if(i==0)
			{	//小于最低插值点高度
				if(!bForFoundation)	//用于杆塔设计
					return InterpBetaZArr[0].val_des;
				else				//用于基础设计
					return InterpBetaZArr[0].val_std;
			}
			else
			{
				if(!bForFoundation)	//用于杆塔设计
				{
					return InterpBetaZArr[i-1].val_des+(InterpBetaZArr[i].val_des-InterpBetaZArr[i-1].val_des)*
						(highpos-InterpBetaZArr[i-1].pivot_key)/(InterpBetaZArr[i].pivot_key-InterpBetaZArr[i-1].pivot_key);
				}
				else				//用于基础设计
				{
					return InterpBetaZArr[i-1].val_std+(InterpBetaZArr[i].val_std-InterpBetaZArr[i-1].val_std)*
						(highpos-InterpBetaZArr[i-1].val_std)/(InterpBetaZArr[i].pivot_key-InterpBetaZArr[i-1].pivot_key);
				}
			}
		}
	}
	//if(i==10)	//大于最高插值点高度
	if(!bForFoundation)	//用于杆塔设计
		return InterpBetaZArr[9].val_des;
	else				//用于基础设计
		return InterpBetaZArr[9].val_std;
}
double CSysParaLibrary::IceLoadCoefInterpByThick(double ice_thick)
{
	for(DWORD i=0;i<InterpIceLoadCoefArr.Size();i++)
	{
		if(ice_thick<InterpIceLoadCoefArr[i].pivot_key)
		{
			if(i==0)	//小于最低插值点高度
				return InterpIceLoadCoefArr[0].val_des;
			else
				return InterpIceLoadCoefArr[i-1].val_des+(InterpIceLoadCoefArr[i].val_des-InterpIceLoadCoefArr[i-1].val_des)*
					(ice_thick-InterpIceLoadCoefArr[i-1].pivot_key)/(InterpIceLoadCoefArr[i].pivot_key-InterpIceLoadCoefArr[i-1].pivot_key);
		}
	}
	return InterpIceLoadCoefArr[2].val_des;
}

/*bool CSysParaLibrary::IsReadOnly(int coefType)
{
	if(m_iLoadExecuteCode!=3)
		return true;
	else
		return false;
}*/
#endif
LPCLASSFACTORY glClf;	//类工厂
LPCLASSFACTORY solid3dClf;	//类工厂
ISolidDraw *g_pSolidDraw;
ISolidSet *g_pSolidSet;
ISolidSnap *g_pSolidSnap;
ISolidOper *g_pSolidOper;

//返回当前显示环境下的视平面法线(屏幕朝外方向) wjh-2017.5.25
GEPOINT GetCurrViewNormal()
{
	GEPOINT vNormal,vStart,vEnd;
	if(g_pSolidOper!=NULL)
	{
		g_pSolidOper->ScreenToUser(&vStart.x,&vStart.y,&vStart.z,0,0,0.5);
		g_pSolidOper->ScreenToUser(&vEnd.x,  &vEnd.y,  &vEnd.z,  0,0,0.6);
		vNormal=vEnd-vStart;
		normalize(vNormal);
	}
	return vNormal;
}
void NewNode(CLDSNode *pNode)
{
	f3dPoint node_pos;
	if(pNode==NULL||pNode->BelongModel()!=&Ta)
		return;
	node_pos = pNode->ActivePosition();
	node_pos.ID = pNode->handle;
	node_pos.pen.crColor = g_sysPara.crPartMode.crNode;
	//得到当前活动视图
	CDisplayView *pView = console.GetActiveView();
	//如果为定制视图将新生成的结点添加到定制视图的显示集合中
	if(pView->m_iViewType==1)
		pView->AppendViewPart(pNode->handle);
	if(console.DispNodeSet.FromHandle(pNode->handle)==NULL)
		console.DispNodeSet.append(pNode);
	g_pSolidDraw->NewPoint(node_pos);
}

void NewLinePart(CLDSLinePart *pPart,bool updatesolid/*=true*/)
{
	if(pPart==NULL||pPart->BelongModel()!=&Ta)
		return;
	PEN_STRU linepen;
	linepen.style=PS_SOLID;
	linepen.width=0;
	linepen.crColor = g_sysPara.crTsaMode.crPrimaryLine;
	g_pSolidSet->SetPen(linepen);
	//得到当前活动视图
	CDisplayView *pView = console.GetActiveView();
	if(pPart->pStart==NULL||pPart->pEnd==NULL)
	{	//短角钢/脚钉底座
		if(updatesolid)
		{
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		}
		return;
	}
	try{
		if(console.DispPartSet.FromHandle(pPart->handle)==NULL)
		{
			//如果为定制视图将新生成的杆件添加到定制视图的显示集合中
			if(pView->m_iViewType==1)
				pView->AppendViewPart(pPart->handle);
			console.DispPartSet.append(pPart);
		}
		if(updatesolid)
		{
#ifdef __GGG_
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),0);
#else
			pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
#endif
		}
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

void NewPlate(CLDSGeneralPlate* pPlate)
{
	if(pPlate==NULL||pPlate->BelongModel()!=&Ta)
		return;
	try{
		CDisplayView *pView = console.GetActiveView();
		if(console.DispPartSet.FromHandle(pPlate->handle)==NULL)
		{
			//如果为定制视图将新生成的钢板添加到定制视图的显示集合中
			if(pView->m_iViewType==1)
				pView->AppendViewPart(pPlate->handle);
			console.DispPartSet.append(pPlate);
		}
		pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void DrawPartLineMap(CLDSPart *pPart,int iActiveVertex/*=-1*/)
{
	if(pPart==NULL)
		return;
	if(!pPart->is_visible)
		return;
	if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		int i=0;
		CLsRefList *pLsRefList=NULL;
		ATOM_LIST<PROFILE_VERTEX> *pVertexList=NULL;
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->face_N!=1)
				return; //目前暂时仅支持显示单面板轮廓边 wht 10-09-15
			pLsRefList=pPlate->GetLsRefList();
			pVertexList=&pPlate->vertex_list;
		}
		else //if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			pLsRefList=pParamPlate->GetLsRefList();
			pVertexList=&pParamPlate->vertex_list;
		}
		//轮廓点货轮廓线的ID不能使用等于构件的ID,否则在使用g_pSolidDraw->DelEnt(ID)时会出现删除不掉指定构件的问题  wht 10-09-08
		int sign=1;
		f3dArcLine arcLine;
		arcLine.pen.width=2;
		arcLine.pen.crColor=RGB(0,180,0);
		arcLine.ID=0;	//pPart->handle;	
		f3dLine line;
		line.pen.style=PS_SOLID;
		line.pen.width=2;
		line.pen.crColor=RGB(0,180,0);
		line.ID=0;		//pPart->handle;
		PROFILE_VERTEX *pVertex=NULL,*pPrevPnt=pVertexList->GetTail(),*pFirstVertex=pVertexList->GetFirst();
		for(pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext(),i++)
		{
			pVertex->vertex.ID=i+1;
			f3dPoint vertex=pVertex->vertex;
			//vertex.ID=pPart->handle;
			vertex.pen.crColor=RGB(0,255,0);
			coord_trans(vertex,pPart->ucs,TRUE);
			g_pSolidDraw->NewPoint(vertex);	
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//绘制顶点索引号
				PROFILE_VERTEX *pNextVertex=pVertexList->GetNext();
				if(pNextVertex)
					pVertexList->GetPrev();
				else 
					pNextVertex=pFirstVertex;
				f3dPoint next_vec=pNextVertex->vertex-pVertex->vertex;
				f3dPoint prev_vec=pPrevPnt->vertex-pVertex->vertex;
				normalize(next_vec);
				normalize(prev_vec);
				f3dPoint mid_vec=next_vec+prev_vec;
				normalize(mid_vec);
				vector_trans(mid_vec,pPart->ucs,TRUE);
				f3dPoint draw_pos=vertex-mid_vec*20+pPart->ucs.axis_z*pPart->GetThick();
				char alphabet[2]={'A'+i,0};
				PEN_STRU oldpen=g_pSolidSet->SetPen(RGB(255,0,0),PS_SOLID,2); //默认轮廓点显示红色
				if(i==iActiveVertex) //当前编辑轮廓点显示黄色
					g_pSolidSet->SetPen(RGB(255,128,0),PS_SOLID,2);
				g_pSolidDraw->NewText(alphabet,draw_pos,20,f3dPoint(0.0),f3dPoint(0,0,0),3,2);
				g_pSolidSet->SetPen(oldpen);
			}
			//轮廓线
			line.startPt.Set(pPrevPnt->vertex.x,pPrevPnt->vertex.y);
			line.endPt.Set(pVertex->vertex.x,pVertex->vertex.y);
			line.startPt.ID=pPrevPnt->vertex.ID;
			line.endPt.ID=pVertex->vertex.ID;
			coord_trans(line.startPt,pPart->ucs,TRUE);
			coord_trans(line.endPt,pPart->ucs,TRUE);
			if(pPrevPnt->type==1)
			{	//圆弧
				if(pPrevPnt->sector_angle<0)
					sign=-1;
				f3dPoint work_norm(0,0,sign);
				vector_trans(work_norm,pPart->ucs,TRUE);
				arcLine.CreateMethod2(line.startPt,line.endPt,work_norm,sign*pPrevPnt->sector_angle);
				g_pSolidDraw->NewArcLine(arcLine);
			}
			else if(pPrevPnt->type==2)	//指定圆弧R
			{
				f3dPoint center_pick=pPrevPnt->center;
				if(pPrevPnt->radius<0)
					sign=-1;
				f3dPoint work_norm(0,0,sign);
				vector_trans(work_norm,pPart->ucs,TRUE);
				//type==2时圆心拾取点记录的坐标为相对坐标此处需要转换为绝对坐标 wht 11-04-28
				coord_trans(center_pick,pPart->ucs,TRUE);
				arcLine.CreateMethod3(line.startPt,line.endPt,work_norm,sign*pPrevPnt->radius,center_pick);
				
				/*double half_len = 0.5*DISTANCE(pPrevPnt->vertex,pVertex->vertex);
				if(pPrevPnt->radius<0)
					sign=-1;
				f3dPoint work_norm(0,0,sign);
				vector_trans(work_norm,pPart->ucs,TRUE);
				if((fabs(pPrevPnt->radius)-half_len)<EPS)
					pPrevPnt->sector_angle = Pi;
				else if(fabs(half_len/pPrevPnt->radius)<1)
					pPrevPnt->sector_angle = 2*asin(half_len/fabs(pPrevPnt->radius));
				arcLine.CreateMethod2(line.startPt,line.endPt,work_norm,sign*pPrevPnt->sector_angle);*/
				g_pSolidDraw->NewArcLine(arcLine);
			}
			else if(pPrevPnt->type==3)	//椭圆弧
			{
				f3dPoint center = pPrevPnt->center;
				f3dPoint col_axis=pPrevPnt->column_norm;
				if(pPrevPnt->radius<0)
					sign=-1;
				f3dPoint work_norm(0,0,sign);
				vector_trans(work_norm,pPart->ucs,TRUE);
				arcLine.CreateEllipse(center,line.startPt,line.endPt,col_axis,work_norm,sign*pPrevPnt->radius);
				g_pSolidDraw->NewArcLine(arcLine);
			}
			else //直线
				g_pSolidDraw->NewLine(line);
			pPrevPnt = pVertex;
		}
		//螺栓孔
		f3dPoint ls_pos;
		CLsRef *pLsRef=NULL;
		for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			project_point(ls_pos,pPart->ucs.origin,pPart->ucs.axis_z);
			if(fabs(ls_pos.x)<eps)
				ls_pos.x = 0;
			if(fabs(ls_pos.y)<eps)
				ls_pos.y = 0;
			if(fabs(ls_pos.z)<eps)
				ls_pos.z = 0;
			f3dCircle circle;
			circle.norm=(*pLsRef)->get_norm();
			circle.centre = ls_pos;
			circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
			circle.ID	  = (*pLsRef)->handle;
			if((*pLsRef)->IsFootNail())
				circle.pen.width = 2;	//脚钉
			else
				circle.pen.width = 1;
			circle.pen.crColor=RGB(255,0,0);
			g_pSolidDraw->NewCircle(circle);
		}
	}
	/*else if(pPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)pPart;
		pLineTube->BuildUCS();
		f3dPoint vertex_arr[16];
		double tube_t=pLineTube->GetThick(),tube_r=pLineTube->GetDiameter()*0.5;
		f3dPoint startPt=pLineTube->Start()-pLineTube->startOdd()*pLineTube->ucs.axis_z;
		f3dPoint endPt=startPt+pLineTube->ucs.axis_z*pLineTube->GetLength();
		int i=0;
		for(i=0;i<4;i++)
		{
			vertex_arr[i*2]=startPt+pLineTube->ucs.axis_x*(tube_r-tube_t);
			vertex_arr[i*2+1]=startPt+pLineTube->ucs.axis_x*tube_r;
			rotate_point_around_axis(vertex_arr[i*2],Pi*0.5*i,startPt,startPt+pLineTube->ucs.axis_z*1000);
			rotate_point_around_axis(vertex_arr[i*2+1],Pi*0.5*i,startPt,startPt+pLineTube->ucs.axis_z*1000);
		}
		for(i=0;i<8;i++)
			vertex_arr[8+i]=vertex_arr[i]+pLineTube->ucs.axis_z*pLineTube->GetLength();
		//绘制4个圆
		f3dCircle circle;
		circle.norm=pLineTube->ucs.axis_z;
		circle.ID	  = pLineTube->handle;
		circle.pen.width = 1;
		circle.pen.crColor=RGB(0,150,0);
		//
		circle.centre = startPt;
		circle.radius = tube_r;
		g_pSolidDraw->NewCircle(circle);
		circle.radius = tube_r-tube_t;
		g_pSolidDraw->NewCircle(circle);
		//
		circle.centre = endPt;
		circle.radius = tube_r;
		g_pSolidDraw->NewCircle(circle);
		circle.radius = tube_r-tube_t;
		g_pSolidDraw->NewCircle(circle);
		//绘制侧面轮廓线
		for(i=0;i<9;i++)
		{
			f3dLine line;
			if(i<8)
				line.startPt=vertex_arr[i];
			else 
				line.startPt=startPt;
			line.startPt.pen.crColor=RGB(255,0,0);
			g_pSolidDraw->NewPoint(line.startPt);	
			if(i<8)
				line.endPt=vertex_arr[i+8];
			else 
				line.endPt=endPt;
			line.endPt.pen.crColor=RGB(255,0,0);
			g_pSolidDraw->NewPoint(line.endPt);
			//
			line.pen.crColor=RGB(0,130,0);
			line.pen.style=PS_SOLID;
			g_pSolidDraw->NewLine(line);
		}
	}*/
	else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//角钢轮廓线
		f3dPoint vertex_arr[12];
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
		pLineAngle->getUCS(pLineAngle->ucs);
		f3dPoint startPt=pLineAngle->Start()-pLineAngle->startOdd()*pLineAngle->ucs.axis_z;
		f3dPoint endPt=startPt+pLineAngle->ucs.axis_z*pLineAngle->GetLength();
		f3dPoint x_wing_vec=pLineAngle->GetWingVecX();
		f3dPoint y_wing_vec=pLineAngle->GetWingVecY();
		f3dPoint x_wing_norm=pLineAngle->get_norm_x_wing();
		f3dPoint y_wing_norm=pLineAngle->get_norm_y_wing();
		double wing_wide=pLineAngle->GetWidth(),wing_thick=pLineAngle->GetThick();
		vertex_arr[0]=startPt;
		vertex_arr[1]=startPt+x_wing_vec*wing_wide;
		vertex_arr[2]=vertex_arr[1]-x_wing_norm*wing_thick;
		vertex_arr[3]=vertex_arr[2]-x_wing_vec*(wing_wide-wing_thick);
		vertex_arr[4]=vertex_arr[3]+y_wing_vec*(wing_wide-wing_thick);
		vertex_arr[5]=vertex_arr[4]+y_wing_norm*wing_thick;
		int i=0;
		for(i=0;i<6;i++)
			vertex_arr[6+i]=vertex_arr[i]+pLineAngle->ucs.axis_z*pLineAngle->GetLength();
		//
		for(i=0;i<6;i++)
		{
			f3dLine line;
			line.pen.crColor=RGB(0,130,0);
			line.pen.style=PS_SOLID;
			line.startPt.pen.crColor=RGB(255,0,0);
			line.endPt.pen.crColor=RGB(255,0,0);
			//绘制顶面顶点以及轮廓边
			line.startPt=vertex_arr[i];
			line.endPt=vertex_arr[(i+1)%6];
			g_pSolidDraw->NewPoint(line.startPt);
			g_pSolidDraw->NewPoint(line.endPt);
			g_pSolidDraw->NewLine(line);
			//绘制底面顶点以及轮廓边
			line.startPt=vertex_arr[6+i];
			line.endPt=vertex_arr[6+(i+1)%6];
			g_pSolidDraw->NewPoint(line.startPt);
			g_pSolidDraw->NewPoint(line.endPt);
			g_pSolidDraw->NewLine(line);
			//绘制侧面轮廓边
			line.startPt=vertex_arr[i];
			line.endPt=vertex_arr[i+6];
			g_pSolidDraw->NewLine(line);
		}
		//螺栓孔
		f3dPoint ls_pos;
		CLsRef *pLsRef=NULL;
		for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
		{
			ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			f3dPoint ls_norm=pLsRef->GetLsPtr()->get_norm();
			if(fabs(ls_norm*x_wing_norm)>fabs(ls_norm*y_wing_norm))
				project_point(ls_pos,pPart->ucs.origin,x_wing_norm);
			else
				project_point(ls_pos,pPart->ucs.origin,y_wing_norm);
			if(fabs(ls_pos.x)<eps)
				ls_pos.x = 0;
			if(fabs(ls_pos.y)<eps)
				ls_pos.y = 0;
			if(fabs(ls_pos.z)<eps)
				ls_pos.z = 0;
			f3dCircle circle;
			circle.norm=ls_norm;
			circle.centre = ls_pos;
			circle.radius = ((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment)/2;
			circle.ID	  = 0;	//(*pLsRef)->handle;
			if((*pLsRef)->IsFootNail())
				circle.pen.width = 2;	//脚钉
			else
				circle.pen.width = 1;
			circle.pen.crColor=RGB(255,0,0);
			g_pSolidDraw->NewCircle(circle);
		}
	}
}

void NewArcLinePart(CLDSArcPart *pArcPart)
{
	f3dArcLine arcline;
	//得到当前活动视图
	CDisplayView *pView = console.GetActiveView();
	if(pArcPart==NULL)
		return;
	try{
		arcline.pen.crColor = g_sysPara.crPartMode.crLine;
		arcline.ID = pArcPart->handle;
		if(console.DispPartSet.FromHandle(pArcPart->handle)==NULL)
		{
			//如果为定制视图将新生成的杆件添加到定制视图的显示集合中
			if(pView->m_iViewType==1)
				pView->AppendViewPart(pArcPart->handle);
			console.DispPartSet.append(pArcPart);
		}
		pArcPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pArcPart->GetSolidPartObject());
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
}

void RefLsToPart(CLsRefList *pLsRefList,CSuperSmartPtr<CLDSPart> pDestPart,BOOL bUpdateL0/*=TRUE*/)
{
	if(pLsRefList==NULL||pDestPart==NULL)
		return;
	SCOPE_STRU scope;
	scope.ClearScope();
	PROFILE_VERTEX *pVertex=NULL;
	if(pDestPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		for(pVertex=pDestPart.ParamPlatePointer()->vertex_list.GetFirst();pVertex!=NULL;pVertex=pDestPart.ParamPlatePointer()->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
	} 
	else if(pDestPart->GetClassTypeId()==CLS_PLATE)
	{
		for(pVertex=pDestPart.PlatePointer()->vertex_list.GetFirst();pVertex!=NULL;pVertex=pDestPart.PlatePointer()->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
	}
	CLsRefList *pBoltRefList=pDestPart->GetLsRefList();
	if(pBoltRefList==NULL)
		return;
	for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
	{
		if(pBoltRefList->FromHandle((*pLsRef)->handle))
			continue;	//避免重复引用
		f3dPoint pos=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(pos,pDestPart->ucs,FALSE);
		pos.z=0;	
		if(!scope.IsIncludePoint(pos))
			continue;	//螺栓不在目的构件范围内则继续
		if(fabs(pLsRef->GetLsPtr()->ucs.axis_z*pDestPart->ucs.axis_z)<EPS_COS2)	
			continue;	//螺栓法线方向与板法线方向不共线
		if(pDestPart->GetClassTypeId()==CLS_PARAMPLATE)
			pDestPart.ParamPlatePointer()->AppendLsRef(*pLsRef);
		else if(pDestPart->GetClassTypeId()==CLS_PLATE)
			pDestPart.PlatePointer()->AppendLsRef(*pLsRef);
		else if(pDestPart->IsLinePart())
			pDestPart.LinePartPointer()->AppendMidLsRef(*pLsRef);
		else if(pDestPart->IsArcPart())
			pDestPart.ArcPartPointer()->AppendLsRef(*pLsRef);
		if(bUpdateL0)
			pLsRef->GetLsPtr()->AddL0Thick(pDestPart->handle,TRUE);
		pLsRef->GetLsPtr()->correct_pos();
		pLsRef->GetLsPtr()->CalGuigeAuto();
		(*pLsRef)->SetModified();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
}
#endif

#if defined(__LDS_)||defined(__TSA_)
BOOL CheckNodeI(const CFGWORD* pCfgwordBody,const CFGWORD* pCfgwordLeg)//, short int *MAX_NODE_I/*=NULL*/)
{
	CLDSNode *pNode;
	CFGWORD cfgwordBody,cfgwordLeg;
	if(pCfgwordBody==NULL||pCfgwordLeg==NULL)
	{
		for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
			cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
		}
	}
	if(pCfgwordBody==NULL)
		pCfgwordBody=&cfgwordBody;
	if(pCfgwordLeg==NULL)
		pCfgwordLeg=&cfgwordLeg;
	CLogErrorLife loglife(&logerr);
	loglife.SetTemporyWarningLevel();
	loglife.EnableExportLogTempory();
	int nHits=0;
	int max_node_i=0;
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(pNode->IsLegObj()&&!pCfgwordLeg->And(pNode->cfgword))
			continue;	//不在当前检查接腿范围内
		else if(!pNode->IsLegObj()&&!pCfgwordBody->And(pNode->cfgword))
			continue;	//不在当前检查塔身本体范围内
		//if(pNode->m_bVirtual)
		//	continue;
		Ta.Node.push_stack();
		for(CLDSNode *pOtherNode=Ta.Node.GetNext();pOtherNode;pOtherNode=Ta.Node.GetNext())
		{
			if(pOtherNode->IsLegObj()&&!pCfgwordLeg->And(pOtherNode->cfgword))
				continue;	//不在当前检查接腿范围内
			else if(!pOtherNode->IsLegObj()&&!pCfgwordBody->And(pOtherNode->cfgword))
				continue;	//不在当前检查塔身本体范围内
			if(pNode->point_i==pOtherNode->point_i&&pNode->point_i>=10)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("node 0X%X and node 0X%X share the same number(%d),serial number is not reasonable!",pNode->handle,pOtherNode->handle,pNode->point_i);
#else
				logerr.Log("节点0X%X与节点0X%X有相同的编号(%d)，编号不合理！",pNode->handle,pOtherNode->handle,pNode->point_i);
#endif
				nHits++;
				g_pSolidDraw->SetEntSnapStatus(pNode->handle);
				g_pSolidDraw->SetEntSnapStatus(pOtherNode->handle);
			}
		}
		Ta.Node.pop_stack();
		if(pNode->point_i/10<1||pNode->point_i%10>3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Node 0X%X not numbered legally(%d)",pNode->handle,pNode->point_i);
#else
			logerr.Log("节点0X%X还未进行合法编号(%d)",pNode->handle,pNode->point_i);
#endif
			nHits++;
			g_pSolidDraw->SetEntSnapStatus(pNode->handle);
		}
		max_node_i=__max(max_node_i,(int)pNode->point_i/10);
	}
	//if(MAX_NODE_I)
	//	*MAX_NODE_I=max_node_i;
	return !logerr.IsHasContents();
}
#endif
SOLIDMODEL_DISPLAY_PARA GetCurrSolidModelDisplayPara()
{
	SOLIDMODEL_DISPLAY_PARA displayPara;
	displayPara.bDisplayHole=g_sysPara.bDisplayAllHole;
	displayPara.smoothness=g_sysPara.display.nSmoothness;
	displayPara.sample_len=g_pSolidOper->GetScaleUserToScreen();
	displayPara.scale_of_user2scr=g_pSolidSet->GetArcSamplingLength();
	return displayPara;
}
///连接板不同颜色配置方案
CPlateColorRef::CPlateColorRef()
{
	COLORREF col_ref=RGB(0,255,128);
	colorHashList.SetHashTableGrowSize(50);
	SetPlateColor("crPLATE",col_ref);
}
void CPlateColorRef::SetPlateColor(char* keyStr,COLORREF &colRef)
{
	if(strlen(keyStr)<=0)
		return;
	colorHashList.SetValue(keyStr,colRef);
}
COLORREF CPlateColorRef::GetPlateColor(char* keyStr)
{
	COLORREF *pColRef=colorHashList.GetValue(keyStr);
	if(pColRef)
		return *pColRef;
	else
		return RGB(0,255,128);
}
COLORREF CPlateColorRef::GetPlateColor(BYTE cMaterial)
{
	CXhChar50 keyStr;
	if(cMaterial == CSteelMatLibrary::Q235BriefMark())		//'S')
		keyStr.Copy("crPLATE_S");
	else if (cMaterial == CSteelMatLibrary::Q345BriefMark())//'H')
		keyStr.Copy("crPLATE_H");
	else if (cMaterial == CSteelMatLibrary::Q390BriefMark())//'G')
		keyStr.Copy("crPLATE_G");
	else if (cMaterial == CSteelMatLibrary::Q420BriefMark())//'P')
		keyStr.Copy("crPLATE_P");
	else if (cMaterial == CSteelMatLibrary::Q460BriefMark())//'T')
		keyStr.Copy("crPLATE_T");
	else
		keyStr.Copy("crPLATE");
	return GetPlateColor(keyStr);
}