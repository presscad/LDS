//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "MainFrm.h"
#include "env_def.h"
#include "CmdLineDlg.h"
#include "TtaDataFile.h"
#include "SlotArm.h"
#include "LocalFeatureDef.h"
#include "LicFuncDef.h"

//直接执行指定命令
BOOL CLDSView::Command(CString cmdStr)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	return CmdLineAffirmed((WPARAM)&cmdStr,0);
}
void CLDSView::InitSysCmdList()
{
	listSysCmds.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __SMART_DRAW_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::CorrectNodeFatherRodCfgWord,"CorrectNodeFatherRodCfgWord","CNFRC"))->descript.Copy("correct node father rod configure word");
	listSysCmds.append(SYS_CMD(&CLDSView::ShrinkObjHandle,"Shrink",""))->descript.Copy("shrink handle");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::MultiLegSchema,"MultiLegSchema","LegSchema"))->descript.Copy("module leg configure word placeholder mode");
	listSysCmds.append(SYS_CMD(&CLDSView::CopyLeg,"CopyLeg","CL"))->descript.Copy("copy leg");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFoot,"DesignFoot","DF"))->descript.Copy("design foot base");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOldFoot,"DesignOldFoot","DOF"))->descript.Copy("design foot base by old schema");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignHoof,"Hoof","DH"))->descript.Copy("design hoof plate");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignBasePlate,"BasePlate","BP"))->descript.Copy("design base plate");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignShoePlate,"ShoePlate","SP"))->descript.Copy("design shoe plate");
	listSysCmds.append(SYS_CMD(&CLDSView::HidePart,"Hide","H"))->descript.Copy("Hide selected objects");
	listSysCmds.append(SYS_CMD(&CLDSView::CheckModel,"CheckModel","CK"))->descript.Copy("Check model data");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportFootTemplate,"FootTemplate","FT"))->descript.Copy("Export foot template");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportConnectionTemplate,"ConnectTemplate","CT"))->descript.Copy("Export connection template");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectFromTemplate,"DesignConnect","DC"))->descript.Copy("Design Conect From Template");
	listSysCmds.append(SYS_CMD(&CLDSView::StdPanel,"Panel","pa"))->descript.Copy("Insert standard panel");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFitting,"Fitting",""))->descript.Copy("Design Fitting");
#else
#ifdef __SMART_DRAW_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::CorrectNodeFatherRodCfgWord,"CorrectNodeFatherRodCfgWord","CNFRC"))->descript.Copy("修正节点父杆件配材号");
	listSysCmds.append(SYS_CMD(&CLDSView::ShrinkObjHandle,"Shrink",""))->descript.Copy("收缩句柄号");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::MultiLegSchema,"MultiLegSchema","LegSchema"))->descript.Copy("呼高接腿配材号占位模式");
#ifndef __STATION_
	listSysCmds.append(SYS_CMD(&CLDSView::CopyLeg,"CopyLeg","CL"))->descript.Copy("复制接腿");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFoot,"DesignFoot","DF"))->descript.Copy("设计塔脚底座");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOldFoot,"DesignOldFoot","DOF"))->descript.Copy("design foot base by old schema");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignHoof,"Hoof","DH"))->descript.Copy("设计牛蹄板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignBasePlate,"BasePlate","BP"))->descript.Copy("设计基板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignShoePlate,"ShoePlate","SP"))->descript.Copy("设计靴板");
	listSysCmds.append(SYS_CMD(&CLDSView::HidePart,"Hide","H"))->descript.Copy("隐藏选中对象");
	listSysCmds.append(SYS_CMD(&CLDSView::CheckModel,"Check","ck"))->descript.Copy("检查模型");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportFootTemplate,"FootTemplate","FT"))->descript.Copy("生成塔脚模板");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportConnectionTemplate,"ConnectTemplate","CT"))->descript.Copy("导出设计模板");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectFromTemplate,"DesignConnect","DC"))->descript.Copy("模板连接设计");
	listSysCmds.append(SYS_CMD(&CLDSView::StdPanel,"Panel","pa"))->descript.Copy("插入标准模板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFitting,"Fitting",""))->descript.Copy("设计金具");
#endif
#ifdef __ANGLE_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::RoutingWarningCheck,"RoutineCheck","RC"))->descript.Copy("Internal test");
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::InternalTest,"InternalTestFunc","IT"))->descript.Copy("Internal test func");
#endif
	//新建文件(模型)	
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __STATION_
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewTower","N"))->descript.Copy("Create new substation architecture model");
#elif defined(__TMA_)||defined(__LMA_)
	listSysCmds.append(SYS_CMD(&CLDSView::NewManuTowerModel,"NewTower","N"))->descript.Copy("Create new rod tower body model");
#else
	listSysCmds.append(SYS_CMD(&CLDSView::NewDesignTowerModel,"NewTower","N"))->descript.Copy("Create new tower body model");
#endif;
#else 
#ifdef __STATION_
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewTower","N"))->descript.Copy("新建变电构架模型");
#elif defined(__TMA_)||defined(__LMA_)
	listSysCmds.append(SYS_CMD(&CLDSView::NewManuTowerModel,"NewTower","N"))->descript.Copy("新建杆塔架主体模型");
#else
	listSysCmds.append(SYS_CMD(&CLDSView::NewDesignTowerModel,"NewTower","N"))->descript.Copy("新建塔架主体模型");
#endif;
#endif

#ifdef __LMA_
#ifdef AFX_TARG_ENU_ENGLISH
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewSubstation","NS"))->descript.Copy("Create new substation architecture model");
#else
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewSubstation","NS"))->descript.Copy("新建变电构架模型");
#endif
#endif
#ifdef __ALFA_TEST_VERSION_
	listSysCmds.append(SYS_CMD(&CLDSView::Test,"Test"))->descript.Copy("内部测试");
#endif;
//
#ifdef AFX_TARG_ENU_ENGLISH
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	listSysCmds.append(SYS_CMD(&CLDSView::DisplayPartWorkUcs,"Ucs"))->descript.Copy("display work ucs");
#endif
#if defined(__TSA_)||defined(__LDS_)
	listSysCmds.append(SYS_CMD(&CLDSView::TtaFile,"Tta"))->descript.Copy("import/Export tta file");
	listSysCmds.append(SYS_CMD(&CLDSView::TowerFile,"Tower"))->descript.Copy("import/Export Tower file");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::DefControlNode,"ControlNode"))->descript.Copy("define control node");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderAll,"Refresh","Re"))->descript.Copy("refresh");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderMode,"RenderMode"))->descript.Copy("render mode");
	listSysCmds.append(SYS_CMD(&CLDSView::DistMeasure,"Dist"))->descript.Copy("cal distance");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishLinkPart,"Link","L"))->descript.Copy("link part");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishDefDivideScaleNode,"ScaleNode"))->descript.Copy("define scale node");
	listSysCmds.append(SYS_CMD(&CLDSView::FindPart,"Find","F"))->descript.Copy("find part");
	listSysCmds.append(SYS_CMD(&CLDSView::DeletePart,"Delete"))->descript.Copy("delete part");
	listSysCmds.append(SYS_CMD(&CLDSView::CalAngle,"Angle","A"))->descript.Copy("cal angle");
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	listSysCmds.append(SYS_CMD(&CLDSView::DelSpecClsPart,"DelSpecClsPart"))->descript.Copy("delete special part");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::ModifyShankSection,"Section"))->descript.Copy("section");
	listSysCmds.append(SYS_CMD(&CLDSView::DefOffsetNode,"OffsetNode"))->descript.Copy("offset node");
	listSysCmds.append(SYS_CMD(&CLDSView::DefIntersNode,"IntersNode"))->descript.Copy("inters node");
	listSysCmds.append(SYS_CMD(&CLDSView::DefHorizonNode,"HorizonNode"))->descript.Copy("horizon node");
	listSysCmds.append(SYS_CMD(&CLDSView::DefFixCoordNode,"FixCoordNode"))->descript.Copy("fix coord node");
	listSysCmds.append(SYS_CMD(&CLDSView::BreakLinePart,"Break","B"))->descript.Copy("bread line_part");
	listSysCmds.append(SYS_CMD(&CLDSView::DivideMainLinePart,"DivideMainLinePart"))->descript.Copy("divide line_part");
	listSysCmds.append(SYS_CMD(&CLDSView::JgKeyPoint,"AngleKeyPoint"))->descript.Copy("View angle's keypoint");
	listSysCmds.append(SYS_CMD(&CLDSView::AddLegAndBraceRods, "AddLeg","AL"))->descript.Copy("Add leg and brace rods");
#ifndef __TSA_
	listSysCmds.append(SYS_CMD(&CLDSView::ComposeLinePart,"ComposeLinePart"))->descript.Copy("compose line_part");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutFootnail,"LayoutFootnail"))->descript.Copy("layout foot nail");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutPedalPlate, "PedalPlate"))->descript.Copy("layout pedal plate");
	listSysCmds.append(SYS_CMD(&CLDSView::CommonJgJoint,"CommonJgJoint"))->descript.Copy("design CommonJg joint");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectionPlate,"ConnectionPlate"))->descript.Copy("connect palte");
	listSysCmds.append(SYS_CMD(&CLDSView::OverlappendPlate,"OverlappendPlate"))->descript.Copy("overlappend palte");
	listSysCmds.append(SYS_CMD(&CLDSView::RibPlate,"RibPlate"))->descript.Copy("rib plate");
	listSysCmds.append(SYS_CMD(&CLDSView::RotatePartAroundAxis,"RotatePartAroundAxis"))->descript.Copy("rotate");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutAngleBolts,"LayoutAngleBolts"))->descript.Copy("layout angle bolts");
	listSysCmds.append(SYS_CMD(&CLDSView::AngleBolts,"AngleBolts","AB"))->descript.Copy("layout angle bolts");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOddment,"DesignOddment"))->descript.Copy("design oddment");
	listSysCmds.append(SYS_CMD(&CLDSView::ShareJdbDesign,"GongYongBan","GY"))->descript.Copy("public plate");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBoltEndAngle,"1BoltEndAngle"))->descript.Copy("one bolt connect");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignNodePlate,"DesignNodePlate"))->descript.Copy("Design node plate");
	listSysCmds.append(SYS_CMD(&CLDSView::ExtendPlate,"ExtendPlate"))->descript.Copy("extend plate");
	listSysCmds.append(SYS_CMD(&CLDSView::MergePlates,"MergePlates"))->descript.Copy("merge plates");
	listSysCmds.append(SYS_CMD(&CLDSView::MergeNodes,"MergeNodes","MN"))->descript.Copy("merge nodes");
	listSysCmds.append(SYS_CMD(&CLDSView::BranchNodes,"BranchNodes","BN"))->descript.Copy("branch nodes");
	listSysCmds.append(SYS_CMD(&CLDSView::LabelInput,"Label","LI"))->descript.Copy("Input rod part label");
	listSysCmds.append(SYS_CMD(&CLDSView::WasherRevision,"Washer","Wa"))->descript.Copy("Washer revision");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSphere,"DesignSphere"))->descript.Copy("design sphere");
	listSysCmds.append(SYS_CMD(&CLDSView::NewArcPart,"ArcPart","Arc"))->descript.Copy("arc part");
	listSysCmds.append(SYS_CMD(&CLDSView::CutPlate,"CutPlate"))->descript.Copy("cut plate");
	listSysCmds.append(SYS_CMD(&CLDSView::BoltPad,"BoltPad"))->descript.Copy("bolt pad");
	listSysCmds.append(SYS_CMD(&CLDSView::ReplaceBoltDatumPoint,"ReplaceBoltDP"))->descript.Copy("replate bolt datumpoint ");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlateTubeInters,"InsertPlateTubeInters"))->descript.Copy("insert palte tube inters");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBolt,"1Bolt"))->descript.Copy("design one bolt");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSpecNodeOneBolt,"Spec1Bolt"))->descript.Copy("design spec one bolt");
	listSysCmds.append(SYS_CMD(&CLDSView::FilletPlateCorner,"DJ"))->descript.Copy("plate cut angle");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankVertex,"IV"))->descript.Copy("Insert Vertex");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankBolt,"IB"))->descript.Copy("Insert Bolt");
#ifdef __COMPLEX_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::CircularPlate,"CircularPlate"))->descript.Copy("circular plate");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignElbowPlate,"ElbowPlate"))->descript.Copy("elbow palte");
	listSysCmds.append(SYS_CMD(&CLDSView::TubeBetweenPlate,"TubeBetweenPlate"))->descript.Copy("tube between palte");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignColumnRayRod,"DesignColumnRayRod"))->descript.Copy("column ray_rod");
	listSysCmds.append(SYS_CMD(&CLDSView::NewProcessHole,"NewProcessHole"))->descript.Copy("process hole");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeD,"D"))->descript.Copy("TubeEndLjTypeD");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeO,"O"))->descript.Copy("TubeEndLjTypeO");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeU,"U"))->descript.Copy("TubeEndLjTypeU");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeC,"C"))->descript.Copy("TubeEndLjTypeC");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeX,"X"))->descript.Copy("TubeEndLjTypeX");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeTJ,"TJ"))->descript.Copy("TubeEndLjTypeTJ");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSimpleLadder,"PT"))->descript.Copy("Simple Ladder");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeWalkWay,"ZD"))->descript.Copy("Tube WalkWay");
	listSysCmds.append(SYS_CMD(&CLDSView::MigrateSlotNailParams,"MSN"))->descript.Copy("Migrate slot nail params");
#endif
#endif
#else
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	listSysCmds.append(SYS_CMD(&CLDSView::DisplayPartWorkUcs,"Ucs"))->descript.Copy("显示指定构件的工作坐标系");
#endif
#ifdef __COMMON_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::StdComTmpl,"stdcom","st"))->descript.Copy("标准组件");
#endif
#if defined(__TSA_)||defined(__LDS_)
	listSysCmds.append(SYS_CMD(&CLDSView::TtaFile,"Tta"))->descript.Copy("导入/导出Tta文件");
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::TowerFile,"Tower"))->descript.Copy("导入/导出Tta文件");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::DefControlNode,"ControlNode"))->descript.Copy("定义控制节点");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderAll,"Refresh","Re"))->descript.Copy("刷新");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderMode,"RenderMode"))->descript.Copy("切换显示模式");
	listSysCmds.append(SYS_CMD(&CLDSView::DistMeasure,"Dist"))->descript.Copy("计算对象间的距离");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishLinkPart,"Link","L"))->descript.Copy("连接塔材");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishDefDivideScaleNode,"ScaleNode"))->descript.Copy("定义等分点");
	listSysCmds.append(SYS_CMD(&CLDSView::FindPart,"Find","F"))->descript.Copy("查找指定构件");
	listSysCmds.append(SYS_CMD(&CLDSView::DeletePart,"Delete"))->descript.Copy("删除指定构件");
	listSysCmds.append(SYS_CMD(&CLDSView::CalAngle,"Angle","A"))->descript.Copy("计算构件夹角");
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	listSysCmds.append(SYS_CMD(&CLDSView::DelSpecClsPart,"DelSpecClsPart"))->descript.Copy("删除指定类型构件");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::ModifyShankSection,"Section"))->descript.Copy("塔身节间");
	listSysCmds.append(SYS_CMD(&CLDSView::DefOffsetNode,"OffsetNode"))->descript.Copy("偏移节点");
	listSysCmds.append(SYS_CMD(&CLDSView::DefIntersNode,"IntersNode"))->descript.Copy("交叉节点");
	listSysCmds.append(SYS_CMD(&CLDSView::DefHorizonNode,"HorizonNode"))->descript.Copy("等高节点");
	listSysCmds.append(SYS_CMD(&CLDSView::DefFixCoordNode,"FixCoordNode"))->descript.Copy("指定坐标分量节点");
	listSysCmds.append(SYS_CMD(&CLDSView::BreakLinePart,"Break","B"))->descript.Copy("打断杆件");
	listSysCmds.append(SYS_CMD(&CLDSView::DivideMainLinePart,"DivideMainLinePart"))->descript.Copy("塔材分段");
	listSysCmds.append(SYS_CMD(&CLDSView::JgKeyPoint,"AngleKeyPoint"))->descript.Copy("查看关键点");
	listSysCmds.append(SYS_CMD(&CLDSView::AddLegAndBraceRods, "AddLeg","AL"))->descript.Copy("添加接腿及正、侧及V面辅材");
#ifndef __TSA_
	listSysCmds.append(SYS_CMD(&CLDSView::ComposeLinePart,"ComposeLinePart"))->descript.Copy("合并杆件");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutFootnail,"LayoutFootnail"))->descript.Copy("布置脚钉");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutPedalPlate, "PedalPlate"))->descript.Copy("布置脚踏板");
	listSysCmds.append(SYS_CMD(&CLDSView::CommonJgJoint,"CommonJgJoint"))->descript.Copy("角钢接头");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectionPlate,"ConnectionPlate"))->descript.Copy("连接板");
	listSysCmds.append(SYS_CMD(&CLDSView::OverlappendPlate,"OverlappendPlate"))->descript.Copy("叠放板");
	listSysCmds.append(SYS_CMD(&CLDSView::RibPlate,"RibPlate"))->descript.Copy("肋板");
	listSysCmds.append(SYS_CMD(&CLDSView::RotatePartAroundAxis,"RotatePartAroundAxis"))->descript.Copy("绕指定轴旋转钢板");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutAngleBolts,"LayoutAngleBolts"))->descript.Copy("布置角钢螺栓");
	listSysCmds.append(SYS_CMD(&CLDSView::AngleBolts,"AngleBolts","AB"))->descript.Copy("快速布置螺栓");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOddment,"DesignOddment"))->descript.Copy("设定正负头");
	listSysCmds.append(SYS_CMD(&CLDSView::ShareJdbDesign,"GongYongBan","GY"))->descript.Copy("共用板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBoltEndAngle,"1BoltEndAngle"))->descript.Copy("单螺栓连接");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignNodePlate,"DesignNodePlate"))->descript.Copy("设计节点板");
	listSysCmds.append(SYS_CMD(&CLDSView::ExtendPlate,"ExtendPlate"))->descript.Copy("延伸钢板");
	listSysCmds.append(SYS_CMD(&CLDSView::MergePlates,"MergePlates"))->descript.Copy("合并连接板");
	listSysCmds.append(SYS_CMD(&CLDSView::MergeNodes,"MergeNodes","MN"))->descript.Copy("合并节点");
	listSysCmds.append(SYS_CMD(&CLDSView::BranchNodes,"BranchNodes","BN"))->descript.Copy("拆分端节点");
#if defined(__TMA_)||defined(__LMA_)
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		listSysCmds.append(SYS_CMD(&CLDSView::LabelInput,"Label","LI"))->descript.Copy("录入杆件编号");
#endif
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::DataOperation,"DataOperation","DO"))->descript.Copy("数据操作");
	listSysCmds.append(SYS_CMD(&CLDSView::WasherRevision,"Washer","Wa"))->descript.Copy("垫圈垫板增补修订");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSphere,"DesignSphere"))->descript.Copy("节点球体");
	listSysCmds.append(SYS_CMD(&CLDSView::NewArcPart,"ArcPart","Arc"))->descript.Copy("环形杆件");
	listSysCmds.append(SYS_CMD(&CLDSView::CutPlate,"CutPlate"))->descript.Copy("切割钢板");
	listSysCmds.append(SYS_CMD(&CLDSView::BoltPad,"BoltPad"))->descript.Copy("螺栓垫板");
	listSysCmds.append(SYS_CMD(&CLDSView::ReplaceBoltDatumPoint,"ReplaceBoltDP"))->descript.Copy("变换螺栓基点");
	listSysCmds.append(SYS_CMD(&CLDSView::AlignBoltsAlongLenVec,"AlignBolts"))->descript.Copy("纵向对齐螺栓");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlateTubeInters,"InsertPlateTubeInters"))->descript.Copy("插板式钢管交叉连接");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBolt,"1Bolt"))->descript.Copy("设计单螺栓");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSpecNodeOneBolt,"Spec1Bolt"))->descript.Copy("批量设计单螺栓连接");
	listSysCmds.append(SYS_CMD(&CLDSView::FilletPlateCorner,"DJ"))->descript.Copy("钢板倒角");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankVertex,"IV"))->descript.Copy("插入轮廓点");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankBolt,"IB"))->descript.Copy("插入螺栓孔");
	listSysCmds.append(SYS_CMD(&CLDSView::OpenCutAngle, "OpenCut"))->descript.Copy("角钢开槽");
#ifdef __COMPLEX_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::CircularPlate,"CircularPlate"))->descript.Copy("环向板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignElbowPlate,"ElbowPlate"))->descript.Copy("钢管肘板");
	listSysCmds.append(SYS_CMD(&CLDSView::TubeBetweenPlate,"TubeBetweenPlate"))->descript.Copy("钢管夹板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignColumnRayRod,"DesignColumnRayRod"))->descript.Copy("柱面射线杆件");
	listSysCmds.append(SYS_CMD(&CLDSView::NewProcessHole,"NewProcessHole"))->descript.Copy("流水孔");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeD,"D"))->descript.Copy("钢管端连接设计");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeO,"O"))->descript.Copy("法兰");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeU,"U"))->descript.Copy("U型插板");	
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeC,"C"))->descript.Copy("槽型插板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeX,"X"))->descript.Copy("接头设计-U十字插板");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeTJ,"TJ"))->descript.Copy("设计钢管塔脚");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSimpleLadder,"PT"))->descript.Copy("简易钢管爬梯");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeWalkWay,"ZD"))->descript.Copy("设计钢管走道");
	listSysCmds.append(SYS_CMD(&CLDSView::MigrateSlotNailParams,"MSN"))->descript.Copy("用于迁移旧LMA数据文件中缺少参数化的脚钉对象");
#endif
#endif
#endif
	CCmdLineDlg::propMenuHashtable.SetHashTableGrowSize(300);
	int id=1;
	for(SYS_CMD*pCMD=listSysCmds.GetFirst();pCMD;pCMD=listSysCmds.GetNext())
		CCmdLineDlg::AddCmdLineHelpItem(pCMD->cmdStr,PROPLIST_ITEM(id++,pCMD->descript));
/*#ifdef __GGG_
		else if(cmdStr.CompareNoCase("NSA")==0)
			NewSlotArm();
		else if(cmdStr.CompareNoCase("ESA")==0)
			EditSlotArm();
#endif*/
}
//命令行命令得到确认
CXhChar50 localPrevCmdStr;
LRESULT CLDSView::CmdLineAffirmed(WPARAM wParam, LPARAM lParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString *pCmdStr = (CString*)wParam;
	CString cmdStr;
	if(*pCmdStr)
		cmdStr=pCmdLine->m_sPreCmd = *pCmdStr;
	pCmdLine->FinishCmdLine();
	if(cmdStr.GetLength()>0)
		localPrevCmdStr=cmdStr;
	else
		cmdStr=localPrevCmdStr;
	for(SYS_CMD* pCmd=listSysCmds.GetFirst();pCmd;pCmd=listSysCmds.GetNext())
	{
		if((pCmd->dwTypeFlag&SYS_CMD::CMD_ANALYSIS_EXCLUSIVE)!=0&&!theApp.m_bAnalysisWorkEnv)
			continue;	//力学分析类排它型命令在非力学分析环境下不启动
		if(cmdStr.CompareNoCase(pCmd->cmdStr)!=0&&
			(strlen(pCmd->briefCmdStr)==0||cmdStr.CompareNoCase(pCmd->briefCmdStr)!=0))
			continue;	//命令名称不匹配
		if(pCmdLine->m_bGettingStrFromCmdLine)
		{
			if(pCmd->dwTypeFlag&SYS_CMD::CMD_NEED_CMDINPUT)
			{	//需要命令行输入的命令
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Please close the current command to follow new command again!");
#else 
				AfxMessageBox("有命令在运行,请终止当前命令后再执行新命令!");
#endif
				return FALSE;
			}
			else 
				pCmdLine->m_bForceExitCommand=TRUE;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:",cmdStr);
#else 
		pCmdLine->FillCmdLine("命令:",cmdStr);
#endif
		(this->*pCmd->execute)();	//执行命令
	}
	return TRUE;
}
//命令行命令被取消
LRESULT CLDSView::CmdLineCanceled(WPARAM wParam, LPARAM lParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->CancelCmdLine();
	return 0;
}
