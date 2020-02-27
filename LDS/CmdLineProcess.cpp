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

//ֱ��ִ��ָ������
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
	listSysCmds.append(SYS_CMD(&CLDSView::CorrectNodeFatherRodCfgWord,"CorrectNodeFatherRodCfgWord","CNFRC"))->descript.Copy("�����ڵ㸸�˼���ĺ�");
	listSysCmds.append(SYS_CMD(&CLDSView::ShrinkObjHandle,"Shrink",""))->descript.Copy("���������");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::MultiLegSchema,"MultiLegSchema","LegSchema"))->descript.Copy("���߽�����ĺ�ռλģʽ");
#ifndef __STATION_
	listSysCmds.append(SYS_CMD(&CLDSView::CopyLeg,"CopyLeg","CL"))->descript.Copy("���ƽ���");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFoot,"DesignFoot","DF"))->descript.Copy("������ŵ���");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOldFoot,"DesignOldFoot","DOF"))->descript.Copy("design foot base by old schema");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignHoof,"Hoof","DH"))->descript.Copy("���ţ���");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignBasePlate,"BasePlate","BP"))->descript.Copy("��ƻ���");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignShoePlate,"ShoePlate","SP"))->descript.Copy("���ѥ��");
	listSysCmds.append(SYS_CMD(&CLDSView::HidePart,"Hide","H"))->descript.Copy("����ѡ�ж���");
	listSysCmds.append(SYS_CMD(&CLDSView::CheckModel,"Check","ck"))->descript.Copy("���ģ��");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportFootTemplate,"FootTemplate","FT"))->descript.Copy("��������ģ��");
	listSysCmds.append(SYS_CMD(&CLDSView::ExportConnectionTemplate,"ConnectTemplate","CT"))->descript.Copy("�������ģ��");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectFromTemplate,"DesignConnect","DC"))->descript.Copy("ģ���������");
	listSysCmds.append(SYS_CMD(&CLDSView::StdPanel,"Panel","pa"))->descript.Copy("�����׼ģ��");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignFitting,"Fitting",""))->descript.Copy("��ƽ��");
#endif
#ifdef __ANGLE_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::RoutingWarningCheck,"RoutineCheck","RC"))->descript.Copy("Internal test");
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::InternalTest,"InternalTestFunc","IT"))->descript.Copy("Internal test func");
#endif
	//�½��ļ�(ģ��)	
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
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewTower","N"))->descript.Copy("�½���繹��ģ��");
#elif defined(__TMA_)||defined(__LMA_)
	listSysCmds.append(SYS_CMD(&CLDSView::NewManuTowerModel,"NewTower","N"))->descript.Copy("�½�����������ģ��");
#else
	listSysCmds.append(SYS_CMD(&CLDSView::NewDesignTowerModel,"NewTower","N"))->descript.Copy("�½���������ģ��");
#endif;
#endif

#ifdef __LMA_
#ifdef AFX_TARG_ENU_ENGLISH
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewSubstation","NS"))->descript.Copy("Create new substation architecture model");
#else
	listSysCmds.append(SYS_CMD(&CLDSView::NewSubstationModel,"NewSubstation","NS"))->descript.Copy("�½���繹��ģ��");
#endif
#endif
#ifdef __ALFA_TEST_VERSION_
	listSysCmds.append(SYS_CMD(&CLDSView::Test,"Test"))->descript.Copy("�ڲ�����");
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
	listSysCmds.append(SYS_CMD(&CLDSView::DisplayPartWorkUcs,"Ucs"))->descript.Copy("��ʾָ�������Ĺ�������ϵ");
#endif
#ifdef __COMMON_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::StdComTmpl,"stdcom","st"))->descript.Copy("��׼���");
#endif
#if defined(__TSA_)||defined(__LDS_)
	listSysCmds.append(SYS_CMD(&CLDSView::TtaFile,"Tta"))->descript.Copy("����/����Tta�ļ�");
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::TowerFile,"Tower"))->descript.Copy("����/����Tta�ļ�");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::DefControlNode,"ControlNode"))->descript.Copy("������ƽڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderAll,"Refresh","Re"))->descript.Copy("ˢ��");
	listSysCmds.append(SYS_CMD(&CLDSView::RenderMode,"RenderMode"))->descript.Copy("�л���ʾģʽ");
	listSysCmds.append(SYS_CMD(&CLDSView::DistMeasure,"Dist"))->descript.Copy("��������ľ���");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishLinkPart,"Link","L"))->descript.Copy("��������");
	listSysCmds.append(SYS_CMD(&CLDSView::FinishDefDivideScaleNode,"ScaleNode"))->descript.Copy("����ȷֵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::FindPart,"Find","F"))->descript.Copy("����ָ������");
	listSysCmds.append(SYS_CMD(&CLDSView::DeletePart,"Delete"))->descript.Copy("ɾ��ָ������");
	listSysCmds.append(SYS_CMD(&CLDSView::CalAngle,"Angle","A"))->descript.Copy("���㹹���н�");
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	listSysCmds.append(SYS_CMD(&CLDSView::DelSpecClsPart,"DelSpecClsPart"))->descript.Copy("ɾ��ָ�����͹���");
#endif
	listSysCmds.append(SYS_CMD(&CLDSView::ModifyShankSection,"Section"))->descript.Copy("����ڼ�");
	listSysCmds.append(SYS_CMD(&CLDSView::DefOffsetNode,"OffsetNode"))->descript.Copy("ƫ�ƽڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::DefIntersNode,"IntersNode"))->descript.Copy("����ڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::DefHorizonNode,"HorizonNode"))->descript.Copy("�ȸ߽ڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::DefFixCoordNode,"FixCoordNode"))->descript.Copy("ָ����������ڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::BreakLinePart,"Break","B"))->descript.Copy("��ϸ˼�");
	listSysCmds.append(SYS_CMD(&CLDSView::DivideMainLinePart,"DivideMainLinePart"))->descript.Copy("���ķֶ�");
	listSysCmds.append(SYS_CMD(&CLDSView::JgKeyPoint,"AngleKeyPoint"))->descript.Copy("�鿴�ؼ���");
	listSysCmds.append(SYS_CMD(&CLDSView::AddLegAndBraceRods, "AddLeg","AL"))->descript.Copy("��ӽ��ȼ������༰V�渨��");
#ifndef __TSA_
	listSysCmds.append(SYS_CMD(&CLDSView::ComposeLinePart,"ComposeLinePart"))->descript.Copy("�ϲ��˼�");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutFootnail,"LayoutFootnail"))->descript.Copy("���ýŶ�");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutPedalPlate, "PedalPlate"))->descript.Copy("���ý�̤��");
	listSysCmds.append(SYS_CMD(&CLDSView::CommonJgJoint,"CommonJgJoint"))->descript.Copy("�Ǹֽ�ͷ");
	listSysCmds.append(SYS_CMD(&CLDSView::ConnectionPlate,"ConnectionPlate"))->descript.Copy("���Ӱ�");
	listSysCmds.append(SYS_CMD(&CLDSView::OverlappendPlate,"OverlappendPlate"))->descript.Copy("���Ű�");
	listSysCmds.append(SYS_CMD(&CLDSView::RibPlate,"RibPlate"))->descript.Copy("�߰�");
	listSysCmds.append(SYS_CMD(&CLDSView::RotatePartAroundAxis,"RotatePartAroundAxis"))->descript.Copy("��ָ������ת�ְ�");
	listSysCmds.append(SYS_CMD(&CLDSView::LayoutAngleBolts,"LayoutAngleBolts"))->descript.Copy("���ýǸ���˨");
	listSysCmds.append(SYS_CMD(&CLDSView::AngleBolts,"AngleBolts","AB"))->descript.Copy("���ٲ�����˨");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOddment,"DesignOddment"))->descript.Copy("�趨����ͷ");
	listSysCmds.append(SYS_CMD(&CLDSView::ShareJdbDesign,"GongYongBan","GY"))->descript.Copy("���ð�");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBoltEndAngle,"1BoltEndAngle"))->descript.Copy("����˨����");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignNodePlate,"DesignNodePlate"))->descript.Copy("��ƽڵ��");
	listSysCmds.append(SYS_CMD(&CLDSView::ExtendPlate,"ExtendPlate"))->descript.Copy("����ְ�");
	listSysCmds.append(SYS_CMD(&CLDSView::MergePlates,"MergePlates"))->descript.Copy("�ϲ����Ӱ�");
	listSysCmds.append(SYS_CMD(&CLDSView::MergeNodes,"MergeNodes","MN"))->descript.Copy("�ϲ��ڵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::BranchNodes,"BranchNodes","BN"))->descript.Copy("��ֶ˽ڵ�");
#if defined(__TMA_)||defined(__LMA_)
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		listSysCmds.append(SYS_CMD(&CLDSView::LabelInput,"Label","LI"))->descript.Copy("¼��˼����");
#endif
	if(PRODUCT_FUNC::IsHasInternalTest())
		listSysCmds.append(SYS_CMD(&CLDSView::DataOperation,"DataOperation","DO"))->descript.Copy("���ݲ���");
	listSysCmds.append(SYS_CMD(&CLDSView::WasherRevision,"Washer","Wa"))->descript.Copy("��Ȧ��������޶�");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSphere,"DesignSphere"))->descript.Copy("�ڵ�����");
	listSysCmds.append(SYS_CMD(&CLDSView::NewArcPart,"ArcPart","Arc"))->descript.Copy("���θ˼�");
	listSysCmds.append(SYS_CMD(&CLDSView::CutPlate,"CutPlate"))->descript.Copy("�и�ְ�");
	listSysCmds.append(SYS_CMD(&CLDSView::BoltPad,"BoltPad"))->descript.Copy("��˨���");
	listSysCmds.append(SYS_CMD(&CLDSView::ReplaceBoltDatumPoint,"ReplaceBoltDP"))->descript.Copy("�任��˨����");
	listSysCmds.append(SYS_CMD(&CLDSView::AlignBoltsAlongLenVec,"AlignBolts"))->descript.Copy("���������˨");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlateTubeInters,"InsertPlateTubeInters"))->descript.Copy("���ʽ�ֹܽ�������");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignOneBolt,"1Bolt"))->descript.Copy("��Ƶ���˨");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSpecNodeOneBolt,"Spec1Bolt"))->descript.Copy("������Ƶ���˨����");
	listSysCmds.append(SYS_CMD(&CLDSView::FilletPlateCorner,"DJ"))->descript.Copy("�ְ嵹��");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankVertex,"IV"))->descript.Copy("����������");
	listSysCmds.append(SYS_CMD(&CLDSView::InsertPlankBolt,"IB"))->descript.Copy("������˨��");
	listSysCmds.append(SYS_CMD(&CLDSView::OpenCutAngle, "OpenCut"))->descript.Copy("�Ǹֿ���");
#ifdef __COMPLEX_PART_INC_
	listSysCmds.append(SYS_CMD(&CLDSView::CircularPlate,"CircularPlate"))->descript.Copy("�����");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignElbowPlate,"ElbowPlate"))->descript.Copy("�ֹ����");
	listSysCmds.append(SYS_CMD(&CLDSView::TubeBetweenPlate,"TubeBetweenPlate"))->descript.Copy("�ֹܼа�");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignColumnRayRod,"DesignColumnRayRod"))->descript.Copy("�������߸˼�");
	listSysCmds.append(SYS_CMD(&CLDSView::NewProcessHole,"NewProcessHole"))->descript.Copy("��ˮ��");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeD,"D"))->descript.Copy("�ֹܶ��������");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeO,"O"))->descript.Copy("����");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeU,"U"))->descript.Copy("U�Ͳ��");	
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeC,"C"))->descript.Copy("���Ͳ��");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeX,"X"))->descript.Copy("��ͷ���-Uʮ�ֲ��");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeEndLjTypeTJ,"TJ"))->descript.Copy("��Ƹֹ�����");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignSimpleLadder,"PT"))->descript.Copy("���׸ֹ�����");
	listSysCmds.append(SYS_CMD(&CLDSView::DesignTubeWalkWay,"ZD"))->descript.Copy("��Ƹֹ��ߵ�");
	listSysCmds.append(SYS_CMD(&CLDSView::MigrateSlotNailParams,"MSN"))->descript.Copy("����Ǩ�ƾ�LMA�����ļ���ȱ�ٲ������ĽŶ�����");
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
//����������õ�ȷ��
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
			continue;	//��ѧ�����������������ڷ���ѧ���������²�����
		if(cmdStr.CompareNoCase(pCmd->cmdStr)!=0&&
			(strlen(pCmd->briefCmdStr)==0||cmdStr.CompareNoCase(pCmd->briefCmdStr)!=0))
			continue;	//�������Ʋ�ƥ��
		if(pCmdLine->m_bGettingStrFromCmdLine)
		{
			if(pCmd->dwTypeFlag&SYS_CMD::CMD_NEED_CMDINPUT)
			{	//��Ҫ���������������
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Please close the current command to follow new command again!");
#else 
				AfxMessageBox("������������,����ֹ��ǰ�������ִ��������!");
#endif
				return FALSE;
			}
			else 
				pCmdLine->m_bForceExitCommand=TRUE;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Command:",cmdStr);
#else 
		pCmdLine->FillCmdLine("����:",cmdStr);
#endif
		(this->*pCmd->execute)();	//ִ������
	}
	return TRUE;
}
//���������ȡ��
LRESULT CLDSView::CmdLineCanceled(WPARAM wParam, LPARAM lParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->CancelCmdLine();
	return 0;
}
