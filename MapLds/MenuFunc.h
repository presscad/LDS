#ifndef __MENU_FUNC_H_
#define __MENU_FUNC_H_

extern int g_nCurMapSegNo;		//当前分段图的分段号
extern int g_nCurBodyNo;		//当前总图的杜体号
extern double g_fCurMapScale;	//绘图塔块中的当前绘图比例
extern char g_sFileName[100];     //在出材料汇总表时使用文件名匹配
extern char g_sHelpFilePath[MAX_PATH];//帮助文件路径
//数据部分
void ImportTma();		//导入文件
void CloseTma();		//关闭文件
void FileProp();		//文件属性
//总图部分
void SelModuleNo();		//选择模型号
void SumFrontFace();	//正面
void SumSideFace();		//侧面
void FrontMutiLeg();	//接腿正面
void SideMutiLeg();		//接腿侧面
void Draw3DLine();		//显示三维单线
void Draw3DLineBC();	//绘制博超要求的三维单线
void Draw3dLineGW();	//绘制国网要求的三维单线
void DrawSolidGW();     //绘制国网要求的三维实体
void Export3DFiles();	//导出塔形实体文件
void DrawSolid();        //显示实体
void TidToDwg();		//根据TID文件生成DWG(单线+实体)
void DimSegI();			//标注分段号
void FLPartBOM();		//法兰明细表
void SumTaMat();		//材料汇总表
void SumTaMatCfg();		//设置材料汇总表
void ConfigInfoBOM();	//配置信息表
void JgZhunJuTable();	//角钢准距表
void DrawSumMapByAnalysisMode();	//力学模式简图
//结构图部分
void SelSegI();			//选择分段号
void SpreadFaceLine();	//力学分析快捷展开图
void SegLine();			//单线图
void SegStru();			//分段结构图
void SpreadFaceStru();	//展开面结构图
void PartGroupSetupDrawing();//组焊装配图
void PartGroupBOM();	//组焊件构件明细表
void SegPartBOM();		//分段构件明细表
void BoltPadBOM();		//分段螺栓明细表
void DrawFootPlanform();//绘制底脚板
//插入图框
void InsertMapFrm();
//插入钢印区
void InsertMKRect();
//插入接头包板俯视图
void InsertWrapperSideView();
//检测出图资料中的件号
void CheckDwgFilePartNo();
void MoveTop();			//移动图元到前台
void HideLine();		//对直线消隐处理
//标注部分
void DimSpaceSize();	//模型尺寸
void DimMapSize();		//图面尺寸	
void DimFootNailSpace();//标注脚钉间距
void DimLsInfo();		//标注螺栓信息
void DimBoltPad();		//标注垫圈信息
void DimOddment();		//标注正负头
void PartSpecTbl();		//构件规格表	
void DimPartSpec();		//标注构件明细
void DimSimPolyCfg();	//相似形标注设置
void DimSimPoly();		//标注相似形
//构件图
void PartDrawingClassicAngle();	//批量生成典型角钢工艺卡
void PartDrawingOneByOne();		//逐个生成零件图
void BatchPartDrawing();		//批量生成零件图
void SpecialProcessStatCard();	//特殊工艺统计卡
void StdPartGroupProcessCard();	//标准组焊件工艺卡
void SetupAidedPlate();			//工装辅助钢板
void StdPartDrawingByReadLibFile();	//读取库文件生成标准构件零件图
void PolyJgBendDrawing();		//制弯角钢拓印图
//工具
void DefTable();			//定制表格
void RecogizePartBomToTxt();	//提取构件明细表
void RecogizePartBomToBom();
void CalTwoTubeAngle();		//计算两钢管间的夹角
void BreakSpecLine();		//截断指定线
void DrawSpecialWeld();		//绘制焊缝线
void DrawSpecTubeBreakLine();//绘制钢管破断线
void DrawSpecCutawayViewSymbol();//绘制剖视图符
void BatchPlotPartDrawing(); //批量打印
void PolarCsys();			 //极坐标转换
void LoadTMAMapArx();		 //加载TMA出图模块 wht 10-11-16
void LoadLMAMapArx();		 //加载LMA出图模块
void LoadLDSMapArx();		 //加载LDS出图模块
void LoadTSAMapArx();		 //加载TSA出图模块
void SmartDraw();			 //智能绘图
//配置部分
void SystemSetting();	//系统设置
void EnvGeneralSet();	//设置
void SysSetImport();	//导入设置
void SysSetExport();	//导出设置
void JgGuiGeFileImport();	//导入角钢规格库
void TubeGuiGeFileImport();	//导入钢管规格库
void CInsertPlateLibImport();//导入槽型插板库
void UInsertPlateLibImport();//导入U型插板库
void XInsertPlateLibImport();//导入X型插板库
void PFlLibImport();	//导入平焊法兰库
void DFlLibImport();	//导入对焊法兰库
void GFlLibImport();    //导入刚性法兰库
void RFlLibImport();    //导入柔性法兰库
void FlCoupleLevelFileImport();//对接法兰级别定义文件
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
void PartSpecTbl();
//void DimPartSpec();
void DrawLoadGraph();
#endif
//菜单中未显示的命令
void PartGroupStru();	//组焊图
void EnvScaleSet();		//设置绘图比例
void TransformProcessCard(); //转换工艺卡
void SysSetExportDefault(BOOL bPrompt);	//仅仅用于保存工装辅助钢板库文件目录到配置文件中
//钢板布局
void LayoutPlates();
void ExtractAndLayoutPlates();
#ifdef __BP_ONLY_
//安徽宏源定制的智能批量打印构件图功能
void BatchPrintPartProcessCards();
void OptimalSortSet();		//优化排序设置
#elif defined(__CARD_IMG_VIEWER_)
//青岛汇金通工艺卡提取上传功能
void AppendOrUpdateProcessCards();
void RetrievedProcessCards();
#endif

#endif