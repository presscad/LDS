#ifndef __ENV_DEF_H_
#define __ENV_DEF_H_

#define MAX_CHAR_GUIGE_16 16
#ifndef THANDLE_DEF
typedef long THANDLE;
#endif
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "Buffer.h"
#include "Tower.h"
#include "Console.h"
#ifndef __DRAWING_CONTEXT_
#include "I_DrawSolid.h"
#endif
#include ".\XhCharString.h"
#include "ArrayList.h"
//============================================================================
const int TREE_ITEM_GRADE=100000;
const int TREE_GROUP_GRADE=1000000;
enum APPLICATION_TYPE{TA_ASSEMBLE_ENV,TA_PART_EDITOR,};
struct TREEITEM_INFO{
	TREEITEM_INFO(){;}
	TREEITEM_INFO(long type,DWORD dw){itemType=type;dwRefData=dw;}
	long itemType;
	DWORD dwRefData;
};
enum PART_CODE_ORDER_RULE
{DOWN_UP,LEFT_RIGHT,MAIN_OTHER,FRONT_SIDE,};
struct RULE_STRU
{
	bool bActive;
	PART_CODE_ORDER_RULE name;
	int iOrder;
};
//命令组参数，用来保存命令组中命令顺序及激活状态
typedef struct tagCMD_GROUP_PARA{
	int nMaxWarnings;
	int nCmdCount;
	BOOL bOutputToNotepad;	//输出到记事本
	BYTE orders[192];
	BOOL actives[192];
	tagCMD_GROUP_PARA(){memset(this,0,sizeof(tagCMD_GROUP_PARA));}
}CMD_GROUP_PARA;
enum SHANK_CTRL_PT_DAO_WEI_STYLE{HORIZON_OFFSET=0,PROJECT_OFFSET};
enum VIEW_DIRECT{  
	PERSPECTIVE = 0, FRONT_VIEW = 1,
	BACK_VIEW = 2,LEFT_VIEW = 3,RIGHT_VIEW = 4, RANDOM_VIEW = 5};
//#-------------------设计依据结构--------------
typedef struct tagLS_POS_IN_JG_STRU{
	double dist_to_end;
	BOOL bInWingX;
	double dist_to_ber;
}LS_POS_IN_JG_STRU;
typedef struct tagJGGUIGE
{
	char cMaterial;		//材质
	double wing_wide,wing_thick;	//肢宽、肢厚
}JGGUIGE;
typedef struct tagTUBEGUIGE
{
	char cMaterial;		//材质
	double d,t;	//直径、厚度
}TUBEGUIGE;

//#-------------------设计依据结构-------------
//不同材质的连接板的颜色配置方案
class CPlateColorRef
{
public:
	CHashStrList<COLORREF>colorHashList;
public:
	CPlateColorRef();
	void SetPlateColor(char* keyStr,COLORREF &textProp);
	COLORREF GetPlateColor(char* keyStr);
	COLORREF GetPlateColor(BYTE cMaterial);
};
//系统参数结构
class CSysParaLibrary : public ISysPara
{
public:
	CSysParaLibrary();
	void InitSysPara();
	//显示条件选项
	CString char0_arr[6];	//显示条件1
	CString char1_arr[6];	//显示条件2
	CString char2_arr[6];	//显示条件3
	BOOL	bNodeArr[6];	//单线同时显示节点
	BOOL	bMoveDispFrame;	//实体移动时显示线框图
	long iLanguage;				//0.中文；1.英文
	long iDetailDocType;		//0.文本文档；1.EXCEL文档
	BOOL bMergeRptFileMirObj;	//合并力学分析计算书中的对称节点和杆件
	long reactionExportType;	//0.输出全部;1.合并高低腿;2.合并工况3.仅输出(呼高)极值工况
	int nM;						//M阈值
	int nSigemaM;
	int nPrecentSigemaM;
	long iEfficacyCeofType;		//功效系数类型0.设计裕度系数|1.承载系数
	long iShowDeformedMapPlan;	//变形图预览方案0.位移变形|1.效用系数|2.承载系数|3.内力比
	BOOL bCalStdFoundationForce;//计算基础作用力标准值
	BOOL bUpdateRodDesignForce;	//杆件最大作用力设计值
	char ciLinkAngleDlgFocusCtrlMode;	//0.动态设定;1.规格框优先;2.构件编号优先
	char get_ciLiveFocusCtrlInLinkAngleDlg();
	__declspec(property(get=get_ciLiveFocusCtrlInLinkAngleDlg)) char ciLiveFocusCtrlInLinkAngleDlg;
	long iLegShiftType;			//0:不进行任何接腿轮换;1:进行等高腿轮换;2:进行高低腿轮换
	BOOL bRecNodeOffset;		//力学分析时记录节点在某呼高的某工况下的位移
	BOOL bCreateAnalFile;		//生成分析过程文件
	BOOL bCreateAnsysCmdFile;	//生成分析过程ANSYS命令行文件
	BOOL bCreateDetailRptFile;	//生成详细计算书
	BOOL bDisplayAllHole;		//临时显示所有孔
	BYTE m_ciLowestWarningLevel;
	struct DOCKPAGEPOS
	{
		BOOL bDisplay;
		UINT uDockBarID;
	};
	struct DOCK_ENV{
		DOCKPAGEPOS pageModel,pageProp,pageCmd,pageOutput,pageRevision,pageLogger,pagePartLabel,pageWeldPart;
		short m_nLeftPageWidth,m_nRightPageWidth,m_nBottomPageHeight;
	}dock;
	union POINTSTYLE{
		WORD wPointMode;
		struct{
			BYTE cbPointSize;
			BYTE ciPointStyle;
		};
	};
	struct DISPLAY_TYPE{
		BOOL	bPartSketch;//显示构件轮廓边以及顶点 wht 10-09-01
		BOOL	bNodePnt;	//显示单线节点
		BOOL	bPoleLine;	//显示单线杆件线
		BOOL	bLoadArrow;	//显示荷载箭头
		BOOL	bWirePoint;	//显示挂点挂线方向
		int		nLoadArrowScale;//荷载箭头显示大小
		POINTSTYLE	xPointStyle;	//节点显示大小
		BOOL	bDimNodeLabel;	//标注节点编号
		int		nNodeLabelTextHeight;	//节点编号字高
		BOOL	bDimRodDesignInfo;			//标注杆件规格
		int		nRodDesignInfoTextHeight;//杆件设计信息（含规格、裕度）字高
		//BOOL	bDimMemberEffic;		//标注单元设计裕度
		//int		nMemberEfficTextHeight;	//单元设计裕度字高
		BOOL	bSolidJg;	//显示实体角钢
		BOOL	bSolidTube;	//显示实体钢管
		BOOL	bSolidJoint;//显示实体接头
		BOOL	bSolidLjb;	//显示实体连接板
		BOOL	bSolidLs;	//显示实体螺栓
		BOOL	bSolidSphere;//显示球体
		BOOL	bSolidSlot;	//显示实体槽钢
		BOOL	bSolidFlat;	//显示实体扁钢
		BOOL	bSolidArcAngle;	//显示实体环形角钢
		BOOL	bSolidArcFlat;	//显示实体环形扁钢
		BOOL	bSolidArcSlot;	//显示实体环形槽钢
		BOOL	bSolidStdPart;	//显示实体标准构件
		BOOL    bSolidTransectFace;	//显示实体相贯面，尤其是指钢管相贯面
		BOOL    bSolidLine;	//显示实体同时显示单线
		BOOL	bSolidFoundation;	//显示铁塔基础
		BOOL    bDispVirtualPart;	//显示虚（隐含）角钢    
		int     nSmoothness;//显示实体时的细腻程度
		BYTE    cHoleWallMode;	//0.不显示;1.仅显示虚拟螺栓孔;2.显示全部孔壁
	}display;
	struct BRUSH_PROPERTY_TYPE{
		BOOL bMaterial;
		BOOL bLayer;
		BOOL bGuige;
		BOOL bSeg;
		BOOL bCfgword;
		BOOL bDisplayHole;
		BOOL bVirtualPart;
		BOOL bColour;
	}brush;
	//
	typedef struct tagFLSELECTLSTLB
	{
		double grade,d,disTube,disFL;//级别，直径，距管壁最小距离，距法兰外侧最小距离
		tagFLSELECTLSTLB(){memset(this,0,sizeof(tagFLSELECTLSTLB));}
		void SetProPorty(double gradeTemp,double dTemp,double disTubeTemp,double disFLTemp)
		{grade = gradeTemp;d=dTemp;disTube=disTubeTemp;disFL=disFLTemp;}
	}FL_SELECTLS_TLB;
	FL_SELECTLS_TLB flSelectLs_table[50];
	int flSelectLsTlb_N;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	//int  m_iLoadExecuteCode;	//0:电力线路规范1:高耸结构规范2:建筑结构荷载规范3:用户指定;荷载计算执行规范，风振系数、构件体型系数、可变荷载组合系数及分项系数取值所遵循的规范
	//int m_iMemberSizeCheckCode;	//构件规格验算选材规范0:电力线路规范1:高耸结构规范2:美国ASCE10-97
	//int m_iSlendernessLimCode;	//杆件长细比限制规范0:电力线路规范1:高耸结构规范2:美国ASCE10-97　3:用户指定
	//int GetExecuteLoadCode(){return m_iLoadExecuteCode;}
	//enum COEF_TYPE{Bw_I,GamaQ,GamaQ_W,GamaQ_I,GamaQ_A,GamaQ_U,GamaQ_T,GamaQKesai,Kesai_W,Kesai_I,Kesai_A,Kesai_U,Kesai_T};
	//bool IsReadOnly(int coefType);
	//double gamaG();				//永久荷载（重力荷载）分项系数
	double Beta_z_InterpByHigh(double highpos,bool bForFoundation);	//根据高度插值计算风振系数值
	double IceLoadCoefInterpByThick(double ice_thick);	//根据冰厚计算杆塔覆冰重力荷载增大系数
	struct INTERP_BETAZ{
		double pivot_key;	//插值基准参数值
		double val_des;	//设计值(设计杆塔用)
		double val_std;	//标准值(设计基础用)
	};
	DYN_ARRAY<INTERP_BETAZ> InterpBetaZArr;	//10个插值点:[][0]高度;[][1]杆塔设计插值;[][2]基础设计值
	DYN_ARRAY<INTERP_BETAZ> InterpIceLoadCoefArr;	//3个插值点:[][0]覆冰厚度mm;[][1]杆塔覆冰荷载增大系数;
	DYN_ARRAY<double> efficColorBarScaleArr;//通过颜色显示单元设计效用比时的颜色条显示刻度,不含0,100%,>100%必显项
	struct{
		double m_fConvergeLim;//应力分析计算收敛阈值(N)
		double m_fBeamLateralStifReductCoef;	//梁单元横向刚度折减系数
		long m_iNonlinearStyle;		//0:线性，否则以非线性方式计算
		long m_iResolverStyle;		//求解器选择0：高斯消元法1：波前法
		long m_nLoadStepCounts;		//分几次加载
		long m_nIterationCounts;	//每次增量最多迭代次数
		long m_nSubSpaceIterCounts;	//动力特性分析中子空间迭代次数
		double m_fFreqsTolerance;	//动力特性分析中频率相对误差率
	}AnalysisPara;
#endif
	//单螺栓垫圈厚度 wht 10-12-24
	struct WASHERTHICK_ITEMS{
		BYTE cnMaxWasherN;		//垫圈最大叠加数量
		BYTE xarrM12Thicks[4];
		BYTE xarrM16Thicks[4];
		BYTE xarrM20Thicks[4];
		BYTE xarrM24Thicks[4];
		WASHERTHICK_ITEMS();
		CXhChar16 ToThickStr(BYTE thicks[4]);
		int FromThickStr(const char* thickstr,BYTE thicks[4]);
		CXhChar16 ToThickStr(int d);
		int FromThickStr(const char* thickstr,int d);
	}BoltPadThick;
	double fRotateVelocity;		//用于控制旋转速度
	BOOL m_bDispPartACS;	//是否在选中构件时显示构件的装配坐标系及特征线框(轮廓线)
	BOOL m_bDispWorkUcs;	//是否显示的输入坐标系(工作坐标系)
	BOOL m_bDispBlockLCS;	//是否显示部件的定位坐标系
	BOOL m_bEnablePrompt;	//是否启用提示框
	bool m_bDragOpenWndSel;	//拖拽开窗功能开关项
	bool m_bLineDepthTest;	//单线消隐功能，false:单线模型置最前台显示（不被实体遮挡）
	bool m_bDblClkStartRotateOper;	//双击启动旋转操作
	//设计参数选项
	//火曲线间隙: 火曲线到角钢边缘距离一般为5mm~10mm
	long HuoQuDist;
	//角钢间隙: 角钢与角钢对接时之间的距离一般为8mm
	long JgEndDist;
	long JointPlankDist;
	long VertexDist;
	long FootNailDist;
	//碰撞间隙
	long CollideDist;
	//修改对接法兰钢管正负头
	BOOL m_bModifyCoupledFLTubeOddment;
	//法兰与钢管间隙:法兰与装备法兰的钢管之间的距离一般为1mm
	long TubeFlDist;
	long m_nPlankEdgeAngleThreshold;
	double fStressDiffuseAngle;	//角度制单位，一般取30°
	double fInsertPlateEdgeSpace;  //插板边距
	double fNailRepScope;	//螺栓代孔范围
	BOOL m_bDelBoltsResideOnPlate;	//同步删除钢板螺栓
	//钢管直径大于该值时该钢管存在焊道线
	double fTubeWeldRoadMinD;
	DWORD m_dwSelectPartsType;
	double m_fMaxScaleScr2User;//局部放大定位时的最大缩放显示比例
	BOOL m_bCanSelectEdgeLine;//轮廓点构件选择是否选择边界线
	UINT m_uDisplaySlices;
	UINT nAutoSaveTime;	//单位：ms
	UINT iAutoSaveType;
	UINT uFileSaveVersion;		//当前文件格式保存版本，0表示当前程序版本
	UINT iFileSaveDocType;		//当前文件格式保存版本，0表示当前产品默认格式;1.*.tma;2.*.lma;3.*.tsa;4.*.lds;5.*.tda
	BOOL m_bSaveLogVersion;		//保存版本日志 wht 12-11-16
	UINT m_iModFileCodeType;	//mod文件的编码方式 0.UTF-8|1.ANSI
	//使用连接板命令生成钢板时自动调整钢板或槽型插板位置
	UINT nAutoAdjustTubePos;	//0.不启用 1.插板偏移 2.钢管偏移
	double m_fKaiHeJiaoThreshold;	//开合角阈值默认为2°wht 11-05-06
	double m_fKaiHeJiaoMinDeita;	//开合角外侧最小间隙 wht 16-02-01
	double m_fMaxKaiHeJiaoClearance;//开合角外侧最小间隙 wht 16-02-01
	int m_nGroupJgMachiningGap;	//组合角钢加工间隙,用于初始化组合角钢上连接板的厚度
	int m_nMatCharPosType;       //构件编号的材质字符位置类型 0.不添加   1.构件编号前    2  构件编号后
	char m_cMatCharSeparator;	//构件编号的材质字符分隔符: 无0  空格' '
	double m_fMinCutAngleLen;          //最小切角值
	double m_fMaxIgnoreCutAngleLen;	   //最大忽略切角值
	BOOL m_b45CutAngleNeedCoefR;	//角钢45°切角时，是否需要考虑内圆弧半径系数
	double m_fCutAngleCoefR;	//角钢切角时，切去的内圆弧半径系数
	BOOL m_bAnglePreferSimplePosMode;
	//单螺栓设计前提示用户
	BOOL b1BoltDesignPrompt;
	RULE_STRU arrRule[4];	//规则
	BOOL bFristJgCodeNo;
	int iColorSchema;//1:crTsaMode,2:crMaterialMode,3:crPartMode
	struct CRTSA
	{
		COLORREF crBackground;
		COLORREF crDimText;
		COLORREF crPostLine;	//主材材单线
		COLORREF crPrimaryLine;	//普通受力材
		COLORREF crAuxLine;
		COLORREF crNode;
		COLORREF crPole;
		COLORREF crDeformedPoleLine;
		COLORREF crLoadArrow;
		bool bEfficColorLinearVary;	//true:单元效率颜色预览方案为渐变色;false:分段离散变色
		COLORREF crZeroStressElem;	//零杆单元
		COLORREF crOverflowElem;	//越界应力单元 设计参数>临界设计条件
		struct ELEMCLR_DIV{
			BYTE efficPercent;		//分界线百分比数
			COLORREF crElemDivision;//默认至多支持设计参数六个分界线
		}crElemDivArr[6];
	}crTsaMode;
	struct CRMATERIAL
	{
		COLORREF crBackground;
		COLORREF crLine;
		COLORREF crNode;
		COLORREF crPole;
		COLORREF crQ235;
		COLORREF crQ345;
		COLORREF crQ390;
		COLORREF crQ420;
		COLORREF crQ460;
		COLORREF crOther;
	}crMaterialMode;		
	struct CRPARTCOMPLEX
	{
		COLORREF crBackground;
		COLORREF crPartBackground;	//构件编辑器背景色
		COLORREF crLine;			//构件编辑器内单线
		COLORREF crNode;			//设计环境节点
		COLORREF crControlNode;		//控制节点
		COLORREF crDivideScaleNode; //等分节点
		COLORREF crOffsetNode;		//偏移节点
		COLORREF crAxisValFixNode;	//指定坐标分量节点
		COLORREF crPole;			//设计环境杆件单线显示
		COLORREF crAngleEdge;
		COLORREF crAngleWingX;
		COLORREF crAngleWingY;
		COLORREF crSonAngle1Edge;	//1号子角钢肢边			
		COLORREF crSonAngle1WingX;	//1号子角钢X肢颜色	
		COLORREF crSonAngle1WingY;	//1号子角钢X肢颜色 wht 11-05-20
		COLORREF crBolt12;
		COLORREF crBolt16;
		COLORREF crBolt20;
		COLORREF crBolt24;
		COLORREF crBolt27;
		COLORREF crBolt30;
		COLORREF crBolt36;
		COLORREF crBolt39;
		COLORREF crBolt42;
		COLORREF crBolt48;
		COLORREF crOtherBolt;
		COLORREF crAntiTheftNut;
		COLORREF crAntiLoosenNut;
		COLORREF crPlate;
		COLORREF crCutFaceOrPolyJoint;
		COLORREF crStdPart;
		COLORREF crShortAngle;
	}crPartMode;
	
	//智能连接设计参数	wht 10-01-14
	BOOL m_bDisplayConnectDlg;		//设计过程中显示设置对话框
	struct{
		int	 m_nPlateThick;			//钢板厚度
		char m_cMaterial;			//钢板材质
		BOOL m_bAutoThick;			//自动计算钢板厚度
		BOOL m_bAutoMat;			//自动设置钢板材质
		BOOL m_bOnlyDesignCurView;	//仅设计当前面连接板
		BOOL m_bDesignCommonConnect;//智能设计普通连接
		BOOL m_bDesignBendConnect;	//智能设计制弯板连接
		BOOL m_bDesignAngleJoint;	//智能设计角钢接头
		BOOL m_bDesignFL;			//智能设计法兰
		BOOL m_bDesignTubeCoverPlate;//智能设计钢管上的环向封板
		int m_iPlateProfileStyle;	//钢板外形计算方式
		int m_iSingleAngleJointStyle;//单角钢接头方式
		int m_iTwoAngleJointStyle;	//双主材角钢接头方式
		int m_iFourAngleJointStyle;	//四主材角钢接头方式
		int m_iBoltLayoutStyle;		//双排螺栓布置方式
		int m_nOffset;              //孤立射线端连接基准偏移距离
		int m_nOffsetAdd;			//孤立射线端连接基准偏移步长
		bool m_bPriorOffsetGradientRod;	//优先对斜向（三个分量均不为零）的孤立射线端连接进行基准偏移
		//螺栓外孔间隙A(不含垫板)
		int m_n12LsOutterSpace;
		int m_n16LsOutterSpace;
		int m_n20LsOutterSpace;
		int m_n24LsOutterSpace;
		//螺栓外孔间隙(含垫板)
		int m_n12LsOutterSpaceHasWasher;
		int m_n16LsOutterSpaceHasWasher;
		int m_n20LsOutterSpaceHasWasher;
		int m_n24LsOutterSpaceHasWasher;
		//螺栓肢翼边距B
		int m_n12LsAngleEdgeSpace;		//
		int m_n16LsAngleEdgeSpace;
		int m_n20LsAngleEdgeSpace;
		int m_n24LsAngleEdgeSpace;
		//外孔螺栓错孔间距C
		int m_n12LsOutterStaggerSpace;		//
		int m_n16LsOutterStaggerSpace;
		int m_n20LsOutterStaggerSpace;
		int m_n24LsOutterStaggerSpace;
		//共用螺栓错孔间距S
		int m_n12LsShareStaggerSpace;		//
		int m_n16LsShareStaggerSpace;
		int m_n20LsShareStaggerSpace;
		int m_n24LsShareStaggerSpace;
	}intelligentConnect;
	BOOL m_bModifyRelationPart; //成组修改图层名时是否更改关联构件
	//连接结构优先级设定
	CStringArray m_arrL1RayPole;	//角钢主材1根射线杆件的连接
	CStringArray m_arrL2RayPole;	//角钢主材2根射线杆件的连接
	CStringArray m_arrL3RayPole;	//角钢主材3根射线杆件的连接
	CStringArray m_arrLnRayPole;	//角钢主材多根射线杆件的连接
	//插板优先级
	CStringArray m_arrInsertPlateType;//插板类型优先级
	CStringArray m_arrFlType;		//法兰类型
	//连接板颜色
	CPlateColorRef plateColRef;
	//命令组参数 分别对应cmdRoutineChk,cmdPartNoChk,cmdCollisionChk,cmdCorrectErros命令组 wht 12-04-24
	CMD_GROUP_PARA cmdGroupParaArr[4];
	
	BOOL Read(CString file_path);	//读配置文件
	BOOL Write(CString file_path);	//写配置文件
	void GetPublicSysParaFromReg(LPCTSTR lpszSection, LPCTSTR lpszEntry);//从注册表中获取共用参数 wht 11-06-10
	void WritePublicSysParaToReg();	//保存共用参数至注册表
	COLORREF CalEfficColor(double efficCoef);
	virtual int GetBendClearance(){return HuoQuDist;}
	virtual BOOL IsPreferAngleSimplePosMode(){return m_bAnglePreferSimplePosMode;}
	virtual BOOL IsDisplayAllHole(){return bDisplayAllHole;}
	virtual BYTE HoleWallDisplayMode(){return display.cHoleWallMode;}
	virtual BOOL IsDelBoltsResideOnPlate(){return m_bDelBoltsResideOnPlate;}
};
struct PRODUCT_FUNC{
	static bool m_bDisableInternalTest;	//执行时进程内阻止内部测试授权
	static BOOL IsHasBasicPortal();	//是否具有基本操作框架功能
	static BOOL IsHasRoofDesign();	//是否可以设计微波平台
	static BOOL IsHasPPE();			//是否具有工艺编辑管理功能
	static BOOL IsHasStdTemplate();	//是否具有插入标准面功能
	static BOOL IsHasCopyLeg();		//是否具有接腿拷贝功能
	static BOOL IsHasGrxModule();	//是否支持皓晨绘图模块
	static BOOL IsHasZrxModule();	//是否支持中望绘图模块
	static BOOL IsHasAngleFoot();	//是否支持角钢塔脚
	static BOOL IsHasColumnRayRod();//是否支持柱面射线构件
	static BOOL IsHasSphereDesign();//是否支持球节点
	static BOOL IsHasAutoArcLift();	//是否自动圆弧预拱
	static BOOL IsHasShadowObject();//是否支持影射对象
	static BOOL IsHasBlockSetup();	//是否支持部件组装
	static BOOL IsHasTidExport();	//是否支持导出Tid文件
	static BOOL IsHasInternalTest();//是否支持内部测试功能
	static BOOL IsHasNonlinearAnalysis();	//是否支持非线性梁杆结构分析
	static BOOL IsHasDynamicAnalysis();		//是否支持动力特性分析
	static BOOL IsHasCableAnalysis();		//是否支持非线性拉线塔分析
};
extern CTower Ta;
extern CSysParaLibrary g_sysPara;
extern LPCLASSFACTORY solid3dClf;
#ifndef __DRAWING_CONTEXT_
extern ISolidDraw *g_pSolidDraw;
extern ISolidSet *g_pSolidSet;
extern ISolidSnap *g_pSolidSnap;
extern ISolidOper *g_pSolidOper;
GEPOINT GetCurrViewNormal();	//返回当前显示环境下的视平面法线(屏幕朝外方向) wjh-2017.5.25
#endif
extern CEvent LButtonDown,RButtonDown;
void NewNode(CLDSNode *pNode);
void NewLinePart(CLDSLinePart *pJg,bool updatesolid=true);
void NewPlate(CLDSGeneralPlate* pParamPlate);
SOLIDMODEL_DISPLAY_PARA GetCurrSolidModelDisplayPara();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void NewArcLinePart(CLDSArcPart *pArcPart);
void RefLsToPart(CLsRefList *pLsRefList,CSuperSmartPtr<CLDSPart> pDestPart,BOOL bUpdateL0=TRUE);
//绘制构件轮廓边以及轮廓点,可指定当前激活轮廓点 wht 10-09-02
void DrawPartLineMap(CLDSPart *pPart,int iActiveVertex=-1);
#endif

#endif