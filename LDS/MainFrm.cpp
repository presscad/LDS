//<LOCALE_TRANSLATE Confirm by hxr/>
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "Splash.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "UserDefMsg.h"
#include "Tower.h"
#include "direct.h"
#include "database.h"
#include "Query.h"
#include "MsgDef.h"
#include "SendMsgDlg.h"
#include "dlg.h"
#include "TowerTreeDlg.h"
#include "TowerPropertyDlg.h"
#include "TowerPartGroupDlg.h"
#include "CmdLineDlg.h"
#include "RevisionOperDlg.h"
#include "LogViewerDlg.h"
#include "OutputInfoDlg.h"
#include "TowerPartGroupDlg.h"
#include "PartNoListDlg.h"
#include "PostProcess.h"
#include "PartLib.h"
#include "DelUndisplayPartsDlg.h"
#include "SortFunc.h"
#include "BeamPartSelMatDlg.h"
#include "SelOrderOfVibrationModeDlg.h"
#include "KeyboardMapDlg.h"
#include "SysParametersDlg.h"
#include "PartNoListDlg.h"
#include "OutputInfoDlg.h"
#include "LicFuncDef.h"
#include "LicenseAuthorizeDlg.h"
#ifdef __CONNECT_REMOTE_SERVER_
#include "RemoteServerDlg.h"
#endif
#if defined(__LDS_)||defined(__TSA_)
#include "Resolver.h"
#include "CfgCalScopeDlg.h"
#include "EqChkScopeDlg.h"
#include "CfgModuleScopeDlg.h"
#include "FreqAnalyzeDlg.h"
#include "PreProcessor.h"
#include "AnalyzeProcessor.h"
#include "PostProcessor.h"
#include "PostProcessReport.h"
#include "PartSizeOptimizor.h"
#include "FLSizeOptimizor.h"
#include "StubAngleCheckDlg.h"
#include "ExportRptFileOptionDlg.h"
#include "RevisionOperDlg.h"
#include "LogFile.h"
#include "RsltBufferDlg.h"
#include "LibraryQuery.h"
#endif
#include "LocalFeatureDef.h"
#include "ClientId.h"
#include "LdsPortalLocale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	// 全局帮助命令
	ON_COMMAND(ID_HELP_FINDER, CFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWndEx::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWndEx::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWndEx::OnHelpFinder)
	//其余命令
	ON_WM_CLOSE()
	ON_COMMAND(ID_RELEASE_NOTES, OnReleaseNotes)
	ON_COMMAND(ID_UPGRADE_SOFTWARE, OnUpgradeSoftware)
	ON_COMMAND(ID_SEND_MSG_TO_OTHER, OnSendMsgToOther)
	ON_WM_TIMER()
	ON_COMMAND(ID_PART_ENV, OnPartEnv)
	ON_UPDATE_COMMAND_UI(ID_SOLID_MODEL_ENV, OnUpdatePartEnv)
	ON_COMMAND(ID_APP_EXIT_PART_APP, OnAppExitPartApp)
	ON_NOTIFY(TBN_DROPDOWN,AFX_IDW_TOOLBAR,OnPartEditorToolBarDropDown)
	ON_COMMAND(ID_DOCKPAGE_MODEL, OnDockpageModel)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_MODEL, OnUpdateDockpageModel)
	ON_COMMAND(ID_DOCKPAGE_PROPERTY, OnDockpageProperty)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_PROPERTY, OnUpdateDockpageProperty)
	ON_COMMAND(ID_DOCKPAGE_CMDLINE, OnDockpageCmdline)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_CMDLINE, OnUpdateDockpageCmdline)
	ON_COMMAND(ID_DOCKPAGE_OUTPUT, OnDockpageOutput)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_SERVER, OnUpdateDockpageServer)
	ON_COMMAND(ID_DOCKPAGE_SERVER, OnDockpageServer)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_OUTPUT, OnUpdateDockpageOutput)
	ON_COMMAND(ID_DOCKPAGE_CHECK, OnDockdataCheck)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_CHECK, OnUpdateDockdataCheck)
	ON_COMMAND(ID_DOCKPAGE_REVISION, OnDockpageRevision)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_REVISION, OnUpdateDockpageRevision)
	ON_COMMAND(ID_DOCKPAGE_LOGGER, OnDockpageLogger)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_LOGGER, OnUpdateDockpageLogger)
	ON_COMMAND(ID_DOCKPAGE_WELDPART, OnDockpageWeldPart)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_WELDPART, OnUpdateDockpageWeldPart)
	ON_COMMAND(ID_DOCKPAGE_PARTLABEL, OnDockpagePartLabel)
	ON_UPDATE_COMMAND_UI(ID_DOCKPAGE_PARTLABEL, OnUpdateDockpagePartLabel)
	ON_COMMAND(ID_SOLID_MODE_DISPLAY, OnSolidModeDisplay)
	ON_UPDATE_COMMAND_UI(ID_SOLID_MODE_DISPLAY, OnUpdateSolidModeDisplay)
	ON_COMMAND(ID_LINE_MODEL_ENV, OnLineModeEnv)
	ON_UPDATE_COMMAND_UI(ID_LINE_MODEL_ENV, OnUpdateLineModeEnv)
	ON_COMMAND(ID_SOLID_MODEL_ENV, OnSolidModeEnv)
	ON_UPDATE_COMMAND_UI(ID_SOLID_MODEL_ENV, OnUpdateSolidModeEnv)
	ON_UPDATE_COMMAND_UI(ID_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_REDO, OnUpdateRedo)
	ON_COMMAND(ID_CODE_NODE_I, OnCodeNodeI)
	ON_COMMAND(ID_DES_WIRE_NODE, OnDesWireNode)
	ON_COMMAND(ID_IMPORT_LOAD_FILE, OnImportLoadFile)
	ON_COMMAND(ID_EXPORT_LOAD_FILE, OnExportLoadFile)
	ON_COMMAND(ID_DISPLAY_DEFORM_TOWER, OnDisplayDeformTower)
	ON_COMMAND(ID_MODEL_FREQS, OnModelFreqs)
	ON_COMMAND(ID_MODEL_DESIGN_CAL, OnModelDesignCal)
	ON_COMMAND(ID_INSERT_STD_FACE_PANEL, OnInsertStdFacePanel)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_DEFORM_TOWER, OnUpdateDisplayDeformTower)
	ON_COMMAND(ID_BN_BEND_PART_CHECK, OnBnBendPartCheck)
	ON_COMMAND(ID_IMPORT_VIBRATION_MODE, OnImportVibrationMode)
	ON_COMMAND(ID_DISPLAY_VIBRATION_MODE, OnDisplayVibrationMode)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_VIBRATION_MODE, OnUpdateDisplayVibrationMode)
	ON_COMMAND(ID_ANALYSIS_WORK_ENV, OnAnalysisWorkEnv)
	ON_UPDATE_COMMAND_UI(ID_ANALYSIS_WORK_ENV, OnUpdateAnalysisWorkEnv)
	ON_COMMAND(ID_FEM_ELEM_ENV, OnFemElemEnv)
	ON_UPDATE_COMMAND_UI(ID_FEM_ELEM_ENV, OnUpdateFemElemEnv)
	ON_COMMAND(ID_DRAWING2D_ENV, OnDrawing2dEnv)
	ON_UPDATE_COMMAND_UI(ID_DRAWING2D_ENV, OnUpdateDrawing2dEnv)
	ON_COMMAND(ID_KEYBOARD_MAP, OnKeyboardMap)
	ON_COMMAND(ID_SYS_PARAMETERS, OnSysParameters)
	ON_COMMAND(ID_STUB_ANGLE_CHECK, OnStubAngleCheck)
	ON_COMMAND(ID_FOOT_DESIGN_CAL, OnFootDesignCal)
	ON_COMMAND(ID_MODEL_EQ_CHECK, OnModelEqCheck)
	ON_COMMAND(ID_RESULT_BUFFER, OnResultBuffer)
	ON_COMMAND(ID_COMPARE_WITH_TTA_RESULTS, OnCompareWithTtaResults)
	ON_COMMAND(ID_EXPORT_RPT_FILE, OnExportRptFile)
	ON_COMMAND(ID_CORRECT_ERRORS, OnCorrectErrors)
	ON_COMMAND(ID_ROUTINE_CHECK, OnRoutineCheck)
	ON_COMMAND(ID_PARTNO_CHECK, OnPartNoCheck)
	ON_COMMAND(ID_COLLISION_CHECK, OnCollisionCheck)
	ON_COMMAND(ID_REDO, OnRedo)
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_MODEL_WIRE_LOAD_CAL, &CMainFrame::OnModelWireLoadCal)
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SEARCH_TEXT, OnClickEditBox)
	ON_COMMAND(ID_QUICK_SEARCH_PART, OnQuickSearchPart)
	ON_COMMAND(ID_LICENSE_CENTER, &CMainFrame::OnLicenseCenter)
	ON_COMMAND(ID_UI_OF_MECHANICS, &CMainFrame::OnShiftUiOfMechanics)
	ON_COMMAND(ID_UI_OF_RAPID_3D_MODEL, &CMainFrame::OnShiftUiOfRapid3dModel)
	ON_COMMAND(ID_UI_OF_DRAWING_MODEL,&CMainFrame::OnShiftUiOfDrawingModel)
	ON_COMMAND(ID_UI_OF_FINAL_3D_MODEL, &CMainFrame::OnShiftUiOfFinal3dModel)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MESSAGE, &CMainFrame::OnUpdateStatusBar)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_WORKENV, &CMainFrame::OnUpdateStatusBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_MOUSE,     // status line indicator
#ifdef __LDS_
	ID_INDICATOR_WORKENV,	// 工作环境
#endif // __LDS_
	ID_INDICATOR_MESSAGE,   // 消息
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_editButton=NULL;
	m_nTimerID=0;
	prev_part.SetSize(100);
	next_part.SetSize(100);
	m_bDisplayDeformedTower=FALSE;
	m_hMenuDefault=NULL;
	InitDataCenterCmdItems();
	UpdateDataCenterCmdGroupPara();
	theApp.m_nAppLook = ID_VIEW_APPLOOK_WINDOWS_7;//theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	m_towerPropertyView.Init(RUNTIME_CLASS(CTowerPropertyDlg), IDD_TOWER_PROPERTY_DLG);
	m_towerTreeView.Init(RUNTIME_CLASS(CTowerTreeDlg), IDD_TOWER_STRUCTURE_DLG);
	m_cmdLineView.Init(RUNTIME_CLASS(CCmdLineDlg), IDD_CMD_LINE_DLG);
#if defined(__LDS_)||defined(__TSA_)
	m_revisionOperView.Init(RUNTIME_CLASS(CRevisionOperDlg), IDD_REVISION_OPER_DLG);
	m_logView.Init(RUNTIME_CLASS(CLogViewerDlg), IDD_LOG_VIEWER_DLG);
#endif
	m_outputInfoView.Init(RUNTIME_CLASS(COutputInfoDlg), IDD_OUTPUT_INFO_DLG);
#ifdef __CONNECT_REMOTE_SERVER_
	//if(/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())
	//	m_remoteServerView.Init(RUNTIME_CLASS(CRemoteServerDlg),IDD_REMOTE_SERVER_DLG);
#endif
#ifdef __TMA_
	if(PRODUCT_FUNC::IsHasInternalTest())
		m_dataCheckView.Init(RUNTIME_CLASS(CDataCheckDlg), CDataCheckDlg::IDD);
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_towerPartGroupView.Init(RUNTIME_CLASS(CTowerPartGroupDlg), IDD_TOWER_PART_GROUP_DLG);
#ifdef __SMART_DRAW_INC_
	m_partNoListView.Init(RUNTIME_CLASS(CPartNoListDlg), IDD_PARTNO_LIST_DLG);
#endif
#endif
}

CMainFrame::~CMainFrame()
{
	if(NULL!=m_editButton)
	{
		delete m_editButton;
		m_editButton=NULL;
	}
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	try{
		return CFrameWndEx::OnCommand(wParam, lParam);
	}
	//afx.h
	//CATCH_ALL(e)
	//{
	//}
	//END_CATCH_ALL
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	catch(CXhChar16 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar50 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
	}
	catch(...)
	{
		AfxMessageBox("unexpected error");
	}
}


BOOL CMainFrame::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	try{
		return CFrameWndEx::OnWndMsg(message, wParam, lParam, pResult);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	catch(CXhChar16 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar50 sError)
	{
		AfxMessageBox(sError);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
	}
	catch(...)
	{
		AfxMessageBox("unexpected error");
	}
}
afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM wp, LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	if (uiToolBarId == IDR_MAINFRAME||uiToolBarId==IDR_MAINFRAME_TDA||
		uiToolBarId==IDR_MAINFRAME_TMA||uiToolBarId==IDR_MAINFRAME_LMA||
		uiToolBarId==IDR_MAINFRAME_CSCW_TMA||uiToolBarId==IDR_MAINFRAME_CSCW_LMA)
	{
		CString str;
		str.LoadString (ID_DISPLAY_FRAMELINE);
		CMenu menu;
		menu.LoadMenu (IDR_FRAMELINE);
		m_wndToolBar.ReplaceButton (ID_DISPLAY_FRAMELINE, 
			CMFCToolBarMenuButton (ID_DISPLAY_FRAMELINE, menu, 
			GetCmdMgr ()->GetCmdImage (ID_DISPLAY_FRAMELINE), str));
	}
	if (uiToolBarId == IDR_MAINFRAME)
	{
		CString str;
		str.LoadString (ID_FILE_NEW);
		CMenu menu;
		menu.LoadMenu (IDR_NEW_MODEL);
		m_wndToolBar.ReplaceButton (ID_FILE_NEW, 
			CMFCToolBarMenuButton (ID_FILE_NEW, menu, 
			GetCmdMgr ()->GetCmdImage (ID_FILE_NEW), str));
		//m_wndToolBar.SetToolBarBtnText (m_wndToolBar.CommandToIndex (ID_FILE_NEW));
	}
	else if(uiToolBarId == IDR_BAR_PART_SEARCH)
	{
		if(m_editButton==NULL)
			m_editButton=new CMFCToolBarEditBoxButton(ID_SEARCH_TEXT,GetCmdMgr ()->GetCmdImage(ID_SEARCH_TEXT, FALSE));
		m_editButton->EnableWindow(TRUE);
		m_editButton->SetStyle(TBBS_PRESSED);
		m_editButton->HaveHotBorder();
		m_editButton->SetFlatMode(true);
		m_wndPartSearch.ReplaceButton (ID_SEARCH_TEXT, *m_editButton);
	}
	return 0;
}
void InitDefaultPartLibray()
{
	/*theApp.env.m_sJgZhunJuTblPath = 
		AfxGetApp()->GetProfileString("Settings","ZhunJuTblPath",NULL);
	if(!theApp.env.m_sJgZhunJuTblPath.IsEmpty())
		JgZhunJuSerialize(theApp.env.m_sJgZhunJuTblPath,FALSE);
	*/
	//角钢准距表 wht 10-05-17
	CString sJgZhunJuTblPath;
	sJgZhunJuTblPath.Format("%sTower.jgg",APP_PATH);
	FILE *fp;
	char file_path[MAX_PATH];
	sprintf(file_path,"%s",sJgZhunJuTblPath);
	if((fp=fopen(file_path,"rt")))
	{	//测试系统目录下是否存在Tower.jgg
		fclose(fp);	//关闭
		JgZhunJuSerialize(sJgZhunJuTblPath,FALSE); //导入Tower.jgg中的数据
	}
	//角钢规格库
	theApp.env.m_sJgGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","JgGuiGeTblPath",NULL);
	if(!theApp.env.m_sJgGuiGeTblPath.IsEmpty())
		JgGuiGeSerialize(theApp.env.m_sJgGuiGeTblPath,FALSE);
	//钢管规格库
	theApp.env.m_sTubeGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","TubeGuiGeTblPath",NULL);
	if(!theApp.env.m_sTubeGuiGeTblPath.IsEmpty())
		TubeGuiGeSerialize(theApp.env.m_sTubeGuiGeTblPath,FALSE);
	//槽钢规格库
	theApp.env.m_sTubeGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","SlotGuiGeTblPath",NULL);
	if(!theApp.env.m_sTubeGuiGeTblPath.IsEmpty())
		SlotGuiGeSerialize(theApp.env.m_sSlotGuiGeTblPath,FALSE);
#ifdef __H_SHAPE_STEEL_
	//HW型钢规格库
	theApp.env.m_sHWGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","HWGuiGeTblPath",NULL);
	if(!theApp.env.m_sHWGuiGeTblPath.IsEmpty())
		HShapeGuiGeSerialize(theApp.env.m_sHWGuiGeTblPath,0,FALSE);
	//HM型钢规格库
	theApp.env.m_sHMGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","HMGuiGeTblPath",NULL);
	if(!theApp.env.m_sHMGuiGeTblPath.IsEmpty())
		HShapeGuiGeSerialize(theApp.env.m_sHMGuiGeTblPath,1,FALSE);
	//HN型钢规格库
	theApp.env.m_sHNGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","HNGuiGeTblPath",NULL);
	if(!theApp.env.m_sHNGuiGeTblPath.IsEmpty())
		HShapeGuiGeSerialize(theApp.env.m_sHNGuiGeTblPath,2,FALSE);
	//HP型钢规格库
	theApp.env.m_sHPGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","HPGuiGeTblPath",NULL);
	if(!theApp.env.m_sHPGuiGeTblPath.IsEmpty())
		HShapeGuiGeSerialize(theApp.env.m_sHPGuiGeTblPath,3,FALSE);
#endif
	//角钢接头库
	theApp.env.m_sJgJointParaTblPath = AfxGetApp()->GetProfileString("Settings","JgJointParaTblPath",NULL);
	if(!theApp.env.m_sJgJointParaTblPath.IsEmpty())
		JgJointParaSerialize(theApp.env.m_sJgJointParaTblPath,FALSE);
	//扁钢规格库
	theApp.env.m_sTubeGuiGeTblPath = AfxGetApp()->GetProfileString("Settings","FlatGuiGeTblPath",NULL);
	if(!theApp.env.m_sTubeGuiGeTblPath.IsEmpty())
		FlatGuiGeSerialize(theApp.env.m_sFlatGuiGeTblPath,FALSE);
	//螺栓间距库
	theApp.env.m_sLsSpaceTblPath = AfxGetApp()->GetProfileString("Settings","LsSpaceTblPath",NULL);
	if(!theApp.env.m_sLsSpaceTblPath.IsEmpty())
		LsSpaceSerialize(theApp.env.m_sLsSpaceTblPath,FALSE);
	//槽型插板库
	theApp.env.m_sCInsertPlateLibPath = AfxGetApp()->GetProfileString("Settings","CInsertPlateLibPath",NULL);
	if(!theApp.env.m_sCInsertPlateLibPath.IsEmpty())
		CInsertPlateSerialize(theApp.env.m_sCInsertPlateLibPath,FALSE);
	//U型插板库
	theApp.env.m_sUInsertPlateLibPath = AfxGetApp()->GetProfileString("Settings","UInsertPlateLibPath",NULL);
	if(!theApp.env.m_sUInsertPlateLibPath.IsEmpty())
		UInsertPlateSerialize(theApp.env.m_sUInsertPlateLibPath,FALSE);
	//十字插板库
	theApp.env.m_sXInsertPlateLibPath = AfxGetApp()->GetProfileString("Settings","XInsertPlateLibPath",NULL);
	if(!theApp.env.m_sXInsertPlateLibPath.IsEmpty())
		XInsertPlateSerialize(theApp.env.m_sXInsertPlateLibPath,FALSE);
	//标准法兰库
	theApp.env.m_sStdPartFlDLibPath = AfxGetApp()->GetProfileString("Settings","StdPartFlDLibPath",NULL);
	if(!theApp.env.m_sStdPartFlDLibPath.IsEmpty())
		FlDSerialize(theApp.env.m_sStdPartFlDLibPath,FALSE);
	theApp.env.m_sStdPartFlPLibPath = AfxGetApp()->GetProfileString("Settings","StdPartFlPLibPath",NULL);
	if(!theApp.env.m_sStdPartFlPLibPath.IsEmpty())
		FlPSerialize(theApp.env.m_sStdPartFlPLibPath,FALSE);
	theApp.env.m_sFlCoupleLevelPath = AfxGetApp()->GetProfileString("Settings","FlCoupleLevelPath",NULL);
	if(!theApp.env.m_sFlCoupleLevelPath.IsEmpty())
		FlCoupleLevelSerialize(theApp.env.m_sFlCoupleLevelPath,FALSE);
	theApp.env.m_sStdPartFlGLibPath = AfxGetApp()->GetProfileString("Settings","StdPartFlGLibPath",NULL);
	if(!theApp.env.m_sStdPartFlGLibPath.IsEmpty())
		FlGSerialize(theApp.env.m_sStdPartFlGLibPath,FALSE);
	theApp.env.m_sStdPartFlRlibPath = AfxGetApp()->GetProfileString("Settings","StdPartFlRLibPath",NULL);
	if(!theApp.env.m_sStdPartFlRlibPath.IsEmpty())
		FlRSerialize(theApp.env.m_sStdPartFlRlibPath,FALSE);
}
static BOOL CreateToolBar(CWnd *pParentWnd,CMFCToolBar &toolBar,UINT uToolBarID,UINT uToolBarNameID,BOOL bSupportCustomize)
{
	if (!toolBar.CreateEx(pParentWnd, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(1,1,1,1),uToolBarID) ||
		!toolBar.LoadToolBar(uToolBarID))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;   // fail to create
	}
	CString strToolBarName;
	BOOL bNameValid = strToolBarName.LoadString(uToolBarNameID);
	ASSERT(bNameValid);
	toolBar.SetWindowText(strToolBarName);

	if(bSupportCustomize)
	{
		CString strCustomize;
		bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
		ASSERT(bNameValid);
		toolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	}
	return TRUE;
}
static CCmdLineDlg *fpLocalCmdLine=NULL;
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, theApp.m_nMainMenuID))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		TRACE0("fail to creat menu bar\n");
#else
		TRACE0("未能创建菜单栏\n");
#endif
		return -1;      // 未能创建
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_FIXED | CBRS_TOOLTIPS | CBRS_TOP);
	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	//标准工具栏
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_TRANSPARENT) || !m_wndToolBar.LoadToolBar(theApp.m_nMainMenuID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	CString strToolBarName;
	BOOL bNameValid = strToolBarName.LoadString(theApp.m_nMainMenuID);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);
	m_wndToolBar.SetBorders();
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	//
	if (!CreateToolBar(this, m_wndModelling, IDR_BAR_MODELLING, IDS_TOOLBAR_NODE, TRUE))
		return -1;
	/*可能以前是删除按钮是全塔单螺栓设计或交叉点定义的,现在已无意义 wjh-2016.9.20
	if(theApp.m_bCooperativeWork)
	{
		m_wndModelling.RemoveButton(13);
		m_wndModelling.RemoveButton(13);
	}
	*/
	UINT TOOLKIT_ID = 0;
	if (theApp.m_bAnalysisWorkEnvOnly)
		TOOLKIT_ID = IDR_BAR_TOOLKIT_TSA;
	else
		TOOLKIT_ID = IDR_BAR_TOOLKIT;
	if (!CreateToolBar(this, m_wndToolKit, TOOLKIT_ID, IDS_TOOLBAR_TOOLKITS, TRUE))
		return -1;
	//
	if (!CreateToolBar(this, m_wndGLBar, IDR_BAR_OPENGL, IDS_TOOLBAR_GL, TRUE))
		return -1;
	//生成搜索工具栏
	if (!CreateToolBar(this, m_wndPartSearch, IDR_BAR_PART_SEARCH, IDS_TOOLBAR_SEARCH, TRUE))
		return -1;
	//
	if (!CreateToolBar(this, m_wndViewBar, IDR_BAR_VIEW, IDS_TOOLBAR_VIEW, TRUE))
		return -1;
	//
	if (!CreateToolBar(this, m_wndDataCheckBar, IDR_BAR_DATA_CHECK, IDS_TOOLBAR_DATA_CHECK, TRUE))
		return -1;
	//
	if (!theApp.m_bAnalysisWorkEnvOnly)
	{
		UINT nPartDesignID = IDR_BAR_PART_DESIGN_LMA;
#ifdef __TMA_
		nPartDesignID = IDR_BAR_PART_DESIGN_TMA;
#else
		nPartDesignID = IDR_BAR_PART_DESIGN_LMA;
#endif
		if (!CreateToolBar(this, m_wndPartDesignBar, nPartDesignID, IDS_TOOLBAR_PART_DESIGN, TRUE))
			return -1;
		/*可能以前是删除按钮是全塔单螺栓设计或交叉点定义的,现在已无意义 wjh-2016.9.20
		if(theApp.m_bCooperativeWork)
		{
			m_wndPartDesignBar.RemoveButton(2);
		}
		*/
		if (!CreateToolBar(this, m_wndPartEditor, IDR_BAR_PART_EDITOR, IDS_TOOLBAR_PART_EDITOR, TRUE))
			return -1;
	}
	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneWidth(1, 200);

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除以下代码
	AddPane(&m_wndMenuBar);	//停靠菜单
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndGLBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPartSearch.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModelling.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolKit.EnableDocking(CBRS_ALIGN_ANY);
	m_wndViewBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDataCheckBar.EnableDocking(CBRS_ALIGN_ANY);
	if (!theApp.m_bAnalysisWorkEnvOnly)
	{
		m_wndPartDesignBar.EnableDocking(CBRS_ALIGN_ANY);
		m_wndPartEditor.EnableDocking(CBRS_ALIGN_ANY);
	}
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	SetMenuBarState(AFX_MBS_VISIBLE);

	DockPane(&m_wndToolBar);
	DockPane(&m_wndModelling);
	DockPane(&m_wndToolKit);
	DockPane(&m_wndGLBar);
	DockPane(&m_wndPartSearch);
	DockPane(&m_wndViewBar);
	DockPane(&m_wndDataCheckBar);
	if (!theApp.m_bAnalysisWorkEnvOnly)
	{
		DockPane(&m_wndPartEditor);
		ShowPane(&m_wndPartEditor, FALSE, FALSE, FALSE);
		DockPane(&m_wndViewBar);
		DockPaneLeftOf(&m_wndDataCheckBar, &m_wndViewBar);
		DockPaneLeftOf(&m_wndPartDesignBar, &m_wndDataCheckBar);
		DockPaneLeftOf(&m_wndPartSearch, &m_wndPartDesignBar);
		DockPaneLeftOf(&m_wndGLBar, &m_wndPartSearch);
		DockPaneLeftOf(&m_wndToolBar, &m_wndModelling);
		DockPaneLeftOf(&m_wndModelling, &m_wndToolKit);
	}

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_BAR_TOOLKIT, theApp.m_bHiColorIcons ? IDR_BAR_TOOLKIT : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		TRACE0("fail to creat dock window\n");
#else
		TRACE0("未能创建停靠窗口\n");
#endif
		return -1;
	}

	m_towerPropertyView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_towerPropertyView);
	m_cmdLineView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_cmdLineView);
	m_towerTreeView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_towerTreeView);
#if defined(__LDS_)||defined(__TSA_)
	m_revisionOperView.EnableDocking(CBRS_ALIGN_ANY);
	m_revisionOperView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
	m_logView.EnableDocking(CBRS_ALIGN_ANY);
	m_logView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
#endif
	m_outputInfoView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_outputInfoView);
	m_outputInfoView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
#ifdef __CONNECT_REMOTE_SERVER_
	if (/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())
	{
		m_remoteServerView.EnableDocking(CBRS_ALIGN_ANY);
		DockPane(&m_remoteServerView);
		m_remoteServerView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
	}
#endif
#ifdef __TMA_
	if (PRODUCT_FUNC::IsHasInternalTest())
	{
		m_dataCheckView.EnableWindow(CBRS_ALIGN_ANY);
		DockPane(&m_dataCheckView);
		m_dataCheckView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
	}
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_towerPartGroupView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_towerPartGroupView);
	m_towerPartGroupView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
#ifdef __SMART_DRAW_INC_
	m_partNoListView.EnableDocking(CBRS_ALIGN_ANY);
	m_partNoListView.AttachToTabWnd(&m_towerTreeView, DM_SHOW, FALSE);
#endif
#endif
	// 启用工具栏和停靠窗口菜单替换
	CString strCustomize;
	strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_userImages.Load(_T(".\\res\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_userImages);
		}
	}
	//设置正常字体
	/*LOGFONT logfont = {0};
	:: SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0);
	afxGlobalData.SetMenuFont(&logfont,true);*/
	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);
	lf.lfHeight = -12;
	lstrcpy(lf.lfFaceName, _T("宋体"));     // using without style office 2007   
	afxGlobalData.fontRegular.DeleteObject();
	afxGlobalData.fontRegular.CreateFontIndirect(&lf);

	m_pDoc = theApp.GetLDSDoc();
	// Get the main window's menu
#ifdef __LDS_
	if (theApp.m_bAnalysisWorkEnvOnly || theApp.m_bDrawingWorkEnvOnly || theApp.m_bCooperativeWork)
		LoadNewMenu(theApp.m_nMainMenuID);
#else
	if (theApp.m_bCooperativeWork)
		LoadNewMenu(theApp.m_nMainMenuID);
#endif
	HMENU hMainMenu = m_wndMenuBar.GetHMenu();
	if (theApp.m_nMainMenuID == IDR_MAINFRAME_TDA)
	{	//移除TDA中的［NC加工］菜单组 wjh-2017.12.8
		HMENU hMenu = m_wndMenuBar.GetHMenu();
#ifdef __LDS_
		if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT))
#endif
			::DeleteMenu(hMenu, 7, MF_BYPOSITION);
	}
#ifdef __LDS_
	if (theApp.m_nMainMenuID == IDR_MAINFRAME)
	{	//移除LDS中的［NC加工］菜单组 wjh-2017.12.8
		HMENU hMenu = m_wndMenuBar.GetHMenu();
		if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_PROCESS_EXPORT))
			::DeleteMenu(hMenu, 8, MF_BYPOSITION);
	}
#endif
	if (!PRODUCT_FUNC::IsHasStdTemplate())
	{
		HMENU hMenu = m_wndMenuBar.GetHMenu();
		::DeleteMenu(hMenu, ID_CREATE_FACEPANEL, MF_BYCOMMAND);
	}
	LOCALE_PROPITEM* pLocaleItem;
#ifdef __LDS_
	pLocaleItem=gxPortalLocalizer.GetLocaleItem("StructureCheck");
	if(!PRODUCT_FUNC::IsHasInternalTest()&&(pLocaleItem==NULL||!pLocaleItem->bVal))//支持结构强度校审
#endif
		::DeleteMenu(hMainMenu, ID_MECHANICS_CHECK, MF_BYCOMMAND);
#if defined(__TMA_)||defined(__LMA_)
	if (!PRODUCT_FUNC::IsHasInternalTest())
#endif
	{
		HMENU hMenu = m_wndMenuBar.GetHMenu();
#ifndef __LDS_
		if (!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))	//::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE) == 0
#endif
			::DeleteMenu(hMenu, ID_DATA_COMPARE, MF_BYCOMMAND);
		::DeleteMenu(hMenu, ID_CHECK_PART, MF_BYCOMMAND);
		::DeleteMenu(hMenu, ID_DOCKPAGE_CHECK, MF_BYCOMMAND);
		::DeleteMenu(hMenu, ID_DOCKPAGE_MANUMODEL_REVISION, MF_BYCOMMAND);
		//#ifdef __TMA_
		//		if(!theApp.m_bNetModelWork)
		//#endif
		::DeleteMenu(hMenu, ID_DOCKPAGE_SERVER, MF_BYCOMMAND);
	}
	if (!PRODUCT_FUNC::IsHasInternalTest())
	{
		HMENU hMenu = m_wndMenuBar.GetHMenu();
		::DeleteMenu(hMenu, ID_CHECK_MODEL_BY_CMDDRW, MF_BYCOMMAND);
	}
	if (!PRODUCT_FUNC::IsHasPPE())
	{
		HMENU hMenu = m_wndMenuBar.GetHMenu();
		::DeleteMenu(hMenu, ID_PPE_STARTER, MF_BYCOMMAND);
	}
	/*
	CMenu* pMenu = GetMenu();
#ifdef __LMA_
	if(!PRODUCT_FUNC::IsHasAngleFoot())	//角钢塔脚(海城银烽特殊要求)
	{
		pMenu->DeleteMenu(ID_FOOT_PLANK,MF_BYCOMMAND);
		pMenu->DeleteMenu(ID_DES_HOOF_PLANK,MF_BYCOMMAND);
		pMenu->DeleteMenu(ID_FOLD_PLANK,MF_BYCOMMAND);
	}
#elif defined( __LDS_)
	CString rStr;
	pMenu->GetMenuString(ID_APP_ABOUT,rStr,MF_BYCOMMAND);
	pMenu->ModifyMenu(ID_APP_ABOUT,MF_BYCOMMAND,ID_APP_ABOUT,"关于 LDS(&A)...");
#endif
#ifndef __GGG_
	pMenu->DeleteMenu(ID_DRAWING2D_ENV,MF_BYCOMMAND);
#endif
#ifdef __COMPLEX_PART_INC_
	if(!PRODUCT_FUNC::IsHasColumnRayRod())
		pMenu->DeleteMenu(ID_DESIGN_COLUMN_RAY_ROD,MF_BYCOMMAND);
#endif
	*/
	//m_strTitle+="(公测版)";
	if (theApp.m_bCooperativeWork)
		m_strTitle += "-" + theApp.m_sUserName;
	//由于导入证书时间较长，所以需要在导入证书之前启动（此时还没有主框架窗口） wjh-2019.2.11
	//if(!theApp.m_bStartAsFemResolver)
	//	CSplashWnd::ShowSplashScreen(this);
	InitDefaultPartLibray();
	CString szTitle = this->GetTitle();
	pLocaleItem = gxPortalLocalizer.GetLocaleItem("AppName");
	if (pLocaleItem)
		szTitle = pLocaleItem->szStrBuff;
	else
	{
		CXhChar50 sClientName = GetClientNameById(CLDSPart::CustomerSerial, '#');
		if (sClientName.GetLength() > 0)
			szTitle.Insert(0, sClientName);
	}
#ifdef _DEBUG
	szTitle.Append("(内测版Debug)");	//用于调试程序时区分Release进程还是Debug进程 wjh-2018.11.22
#else
	if(PRODUCT_FUNC::IsHasInternalTest())
		szTitle.Append("(内测版)");
#endif
	//if(CLDSPart::CustomerSerial>0)
		this->SetTitle(szTitle);
	fpLocalCmdLine=GetCmdLinePage();
	return 0;
} 

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	//  the CREATESTRUCT cs

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	return TRUE;
}
static BOOL CreateDockingWindow(CWnd *pParentWnd,UINT nDlgID,UINT nViewNameID,CDialogPanel &dlgPanel,DWORD dwPosStyle)
{
	CString sViewName="";
	BOOL bNameValid = sViewName.LoadString(nViewNameID);
	ASSERT(bNameValid);
	if (!dlgPanel.Create(sViewName, pParentWnd, CRect(0, 0, 200, 200), TRUE, nDlgID,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwPosStyle | CBRS_FLOAT_MULTI))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		TRACE0("fail to creat“"+sViewName+"”window\n");
#else
		TRACE0("未能创建“"+sViewName+"”窗口\n");
#endif
		return FALSE;
	}
	return TRUE;
}

static void SetDockingWindowIcon(CDialogPanel &dlgPanel,UINT nIdHC,UINT nCommonId,BOOL bHiColorIcons)
{
	HICON hViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), 
		MAKEINTRESOURCE(bHiColorIcons ? nIdHC : nCommonId), 
		IMAGE_ICON,::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	dlgPanel.SetIcon(hViewIcon, FALSE);
}

BOOL CMainFrame::CreateDockingWindows()
{
	CreateDockingWindow(this,IDD_TOWER_PROPERTY_DLG,IDS_TOWER_PROP_VIEW,m_towerPropertyView,CBRS_LEFT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
	CreateDockingWindow(this,IDD_TOWER_STRUCTURE_DLG,IDS_TOWER_CTRL_VIEW,m_towerTreeView,CBRS_RIGHT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
	CreateDockingWindow(this,IDD_CMD_LINE_DLG,IDS_CMD_LINE_VIEW,m_cmdLineView,CBRS_BOTTOM);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
#if defined(__LDS_)||defined(__TSA_)
	CreateDockingWindow(this,IDD_REVISION_OPER_DLG,IDS_REVISION_OPER_VIEW,m_revisionOperView,CBRS_RIGHT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
	CreateDockingWindow(this,IDD_LOG_VIEWER_DLG,IDS_LOG_VIEW,m_logView,CBRS_RIGHT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
#endif
	CreateDockingWindow(this,IDD_OUTPUT_INFO_DLG,IDS_OUTPUT_INFO_VIEW,m_outputInfoView,CBRS_RIGHT);
#ifdef __CONNECT_REMOTE_SERVER_
	if(/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())
		CreateDockingWindow(this,IDD_REMOTE_SERVER_DLG,IDS_REMOTE_SERVER_VIEW,m_remoteServerView,CBRS_RIGHT);
#endif
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
#ifdef __TMA_
	if(PRODUCT_FUNC::IsHasInternalTest())
		CreateDockingWindow(this,CDataCheckDlg::IDD,IDS_REVISION_OPER_VIEW,m_dataCheckView,CBRS_RIGHT);
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CreateDockingWindow(this,IDD_TOWER_PART_GROUP_DLG,IDS_TOWER_PARTGROUP_VIEW,m_towerPartGroupView,CBRS_RIGHT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
#ifdef __SMART_DRAW_INC_
	CreateDockingWindow(this,IDD_PARTNO_LIST_DLG,IDS_PARTNO_LIST_VIEW,m_partNoListView,CBRS_RIGHT);
	//SetDockingWindowIcon(m_towerPropertyView,ID_DD,ID_DD2,theApp.m_bHiColorIcons);
#endif
#endif
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnClickEditBox()
{	//响应回车查找事件
	CString sSeartStr;
	CMFCToolBarEditBoxButton* pSrcEdit = CMFCToolBarEditBoxButton::GetByCmd(ID_SEARCH_TEXT);
	pSrcEdit->GetEditBox()->GetWindowText(sSeartStr);
	CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	pLdsView->QuickFindPart(sSeartStr);
}
void CMainFrame::OnQuickSearchPart()
{
	CString sSearchText;
	CMFCToolBarEditBoxButton* pSrcEdit = CMFCToolBarEditBoxButton::GetByCmd(ID_SEARCH_TEXT);
	if(pSrcEdit==NULL)
	{	//个别 情况下可能导致搜索工具栏丢失编辑框情况出现 wjh-2018.8.23
		AfxMessageBox("请与软件供应商联系重置搜索工具栏");
		return;
	}
	pSrcEdit->GetEditBox()->GetWindowText(sSearchText);
	CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	if(sSearchText.GetLength()>0)
		pLdsView->QuickFindPart(sSearchText);
	else
		pLdsView->FindPart();
}
void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}
	//清掉最近一次搜索框内容 wjh-2016.6.27
	CMFCToolBarEditBoxButton* pSrcEdit = CMFCToolBarEditBoxButton::GetByCmd(ID_SEARCH_TEXT);
	if(pSrcEdit)
		pSrcEdit->GetEditBox()->SetWindowText("");
	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
}
//
bool CMainFrame::SetPaneTextById(UINT idPaneItemId,const char* sPaneText,BOOL bUpdate/*=TRUE*/)
{
	return m_wndStatusBar.SetPaneTextById(idPaneItemId,sPaneText,bUpdate);
}
void CMainFrame::SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
	m_wndStatusBar.SetPaneText(nIndex,lpszNewText,bUpdate);
}

void CMainFrame::LoadNewMenu(UINT idMenu)
{
   // Load the new menu.
	if(m_NewMenu.GetSafeHmenu())
		m_NewMenu.Detach();
	m_NewMenu.LoadMenu(idMenu);
	
	ASSERT(m_NewMenu);
	if(idMenu==theApp.m_nMainMenuID)
	{
	#ifdef __LMA_
		if(!PRODUCT_FUNC::IsHasAngleFoot())	//角钢塔脚(海城银烽特殊要求)
		{
			m_NewMenu.DeleteMenu(ID_FOOT_PLANK,MF_BYCOMMAND);
			m_NewMenu.DeleteMenu(ID_DES_HOOF_PLANK,MF_BYCOMMAND);
			m_NewMenu.DeleteMenu(ID_FOLD_PLANK,MF_BYCOMMAND);
		}
	#endif
	#ifndef __GGG_
		m_NewMenu.DeleteMenu(ID_DRAWING2D_ENV,MF_BYCOMMAND);
	#endif
	}
	// Remove and destroy the old menu
	SetMenu(NULL);
	DestroyMenu(m_hMenuDefault);

	// Add the new menu
	m_wndMenuBar.CreateFromMenu(m_NewMenu.m_hMenu,FALSE,TRUE);	//SetMenu(&m_NewMenu);
	// Assign default menu
	m_hMenuDefault = m_NewMenu.GetSafeHmenu();  // or m_NewMenu.m_hMenu;
}
void CMainFrame::SavePreWorkUIState()
{
	if (theApp.m_ePreUiType == ELEMENT_CAL_UI)
		((CWinAppEx*)&theApp)->SaveState(this, theApp.m_strRegSection_LXJS);
	else if (theApp.m_ePreUiType == RAPID_MODELING_UI)
		((CWinAppEx*)&theApp)->SaveState(this, theApp.m_strRegSection_KSJM);
	else if (theApp.m_ePreUiType == DRAWING_MODELING_UI)
		((CWinAppEx*)&theApp)->SaveState(this, theApp.m_strRegSection_JMHT);
	else if (theApp.m_ePreUiType == PRECISE_MODELING_UI)
		((CWinAppEx*)&theApp)->SaveState(this, theApp.m_strRegSection_JMFY);
	else
		theApp.SaveState();
}
//切换到力学分析界面
void CMainFrame::ShiftUiOfMechanics()
{
	//保存之前操作环境的界面布局
	theApp.m_ePreUiType = theApp.m_eUiType;
	theApp.m_eUiType = ELEMENT_CAL_UI;
	SetPaneTextById(ID_INDICATOR_WORKENV, "力学计算");
	SavePreWorkUIState();
	//加载当前操作环境的界面布局
	HKEY hKey;
	CXhChar100 sSubKey("Software\\Xerofox\\%s\\%s", theApp.m_pszAppName, theApp.m_strRegSection_LXJS);
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		theApp.LoadState(this, theApp.m_strRegSection_LXJS);
	}
	else
	{	//设置菜单栏|工具栏
		ShowPane(&m_wndPartEditor, FALSE, FALSE, TRUE);
		ShowPane(&m_wndDataCheckBar, FALSE, FALSE, TRUE);
		ShowPane(&m_wndPartDesignBar, FALSE, FALSE, TRUE);
		ShowPane(&m_wndPartSearch, FALSE, FALSE, TRUE);
		ShowPane(&m_wndToolBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndGLBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndModelling, TRUE, FALSE, TRUE);
		ShowPane(&m_wndToolKit, TRUE, FALSE, TRUE);
		ShowPane(&m_wndViewBar, TRUE, FALSE, TRUE);
		DockPaneLeftOf(&m_wndGLBar, &m_wndModelling);
		LoadNewMenu(IDR_MAINFRAME_TSA);
	}
	//设置视图环境
	theApp.m_eEnvType = FEMROD_ENV;
	theApp.m_bAnalysisWorkEnv = TRUE;
	theApp.m_bAnalysisWorkEnvOnly = TRUE;
	theApp.m_bDrawingWorkEnvOnly = FALSE;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
}
//切换到快速三维建模界面
void CMainFrame::ShiftUiOfRapid3dModel()
{
	//保存之前操作环境的界面布局
	theApp.m_ePreUiType = theApp.m_eUiType;
	theApp.m_eUiType = RAPID_MODELING_UI;
	SetPaneTextById(ID_INDICATOR_WORKENV, "快速建模");
	SavePreWorkUIState();
	//加载当前操作环境的界面布局
	HKEY hKey;
	CXhChar100 sSubKey("Software\\Xerofox\\%s\\%s", theApp.m_pszAppName, theApp.m_strRegSection_KSJM);
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		theApp.LoadState(this, theApp.m_strRegSection_KSJM);
	}	
	else
	{	//设置菜单栏|工具栏
		ShowPane(&m_wndPartEditor, FALSE, FALSE, TRUE);
		ShowPane(&m_wndDataCheckBar, FALSE, FALSE, TRUE);
		ShowPane(&m_wndPartDesignBar, FALSE, FALSE, TRUE);
		ShowPane(&m_wndToolBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndGLBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndModelling, TRUE, FALSE, TRUE);
		ShowPane(&m_wndToolKit, TRUE, FALSE, TRUE);
		ShowPane(&m_wndViewBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndPartSearch, TRUE, FALSE, TRUE);
		DockPaneLeftOf(&m_wndGLBar, &m_wndModelling);
		LoadNewMenu(IDR_MAINFRAME_RMA);
	}
	//设置视图环境
	theApp.m_eEnvType = SOLID_ENV;
	theApp.m_bAnalysisWorkEnv = FALSE;
	theApp.m_bAnalysisWorkEnvOnly = FALSE;
	theApp.m_bDrawingWorkEnvOnly = FALSE;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
}
void CMainFrame::ShiftUiOfDrawingModel()
{
	//保存之前操作环境的界面布局
	theApp.m_ePreUiType = theApp.m_eUiType;
	theApp.m_eUiType = DRAWING_MODELING_UI;
	SetPaneTextById(ID_INDICATOR_WORKENV, "建模绘图");
	SavePreWorkUIState();
	//加载当前操作环境的界面布局
	HKEY hKey;
	CXhChar100 sSubKey("Software\\Xerofox\\%s\\%s", theApp.m_pszAppName, theApp.m_strRegSection_JMHT);
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		theApp.LoadState(this, theApp.m_strRegSection_JMHT);
	}
	else
	{	//设置菜单栏|工具栏
		ShowPane(&m_wndPartEditor, FALSE, FALSE, TRUE);
		ShowPane(&m_wndToolBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndModelling, TRUE, FALSE, TRUE);
		ShowPane(&m_wndToolKit, TRUE, FALSE, TRUE);
		ShowPane(&m_wndGLBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndPartDesignBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndDataCheckBar, FALSE, FALSE, TRUE);
		DockPane(&m_wndToolBar);
		DockPane(&m_wndModelling);
		DockPane(&m_wndToolKit);
		DockPaneLeftOf(&m_wndToolBar, &m_wndModelling);
		DockPaneLeftOf(&m_wndModelling, &m_wndToolKit);
		DockPane(&m_wndGLBar);
		DockPane(&m_wndPartSearch);
		DockPane(&m_wndViewBar);
		DockPane(&m_wndPartDesignBar);
		DockPaneLeftOf(&m_wndPartDesignBar, &m_wndViewBar);
		DockPaneLeftOf(&m_wndPartSearch, &m_wndPartDesignBar);
		DockPaneLeftOf(&m_wndGLBar, &m_wndPartSearch);
		LoadNewMenu(IDR_MAINFRAME_TDA);
	}
	//设置视图环境
	theApp.m_eEnvType = LINE_ENV;
	theApp.m_bAnalysisWorkEnv = FALSE;
	theApp.m_bAnalysisWorkEnvOnly = FALSE;
	theApp.m_bDrawingWorkEnvOnly = TRUE;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
}
//切换到建模放样界面
void CMainFrame::ShiftUiOfFinal3dModel()
{
	//保存之前操作环境的界面布局
	theApp.m_ePreUiType = theApp.m_eUiType;
	theApp.m_eUiType = PRECISE_MODELING_UI;
	SetPaneTextById(ID_INDICATOR_WORKENV, "建模放样");
	SavePreWorkUIState();
	//加载当前操作环境的界面布局
	HKEY hKey;
	CXhChar100 sSubKey("Software\\Xerofox\\%s\\%s", theApp.m_pszAppName, theApp.m_strRegSection_JMFY);
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, sSubKey, 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		theApp.LoadState(this, theApp.m_strRegSection_JMFY);
	}
	else
	{	//设置菜单栏|工具栏
		ShowPane(&m_wndPartEditor, FALSE, FALSE, TRUE);
		ShowPane(&m_wndToolBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndGLBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndModelling, TRUE, FALSE, TRUE);
		ShowPane(&m_wndToolKit, TRUE, FALSE, TRUE);
		ShowPane(&m_wndViewBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndPartSearch, TRUE, FALSE, TRUE);
		ShowPane(&m_wndDataCheckBar, TRUE, FALSE, TRUE);
		ShowPane(&m_wndPartDesignBar, TRUE, FALSE, TRUE);
		DockPaneLeftOf(&m_wndGLBar, &m_wndModelling);
		LoadNewMenu(IDR_MAINFRAME_LMA);
	}
	//设置视图环境
	theApp.m_eEnvType = SOLID_ENV;
	theApp.m_bAnalysisWorkEnv = FALSE;
	theApp.m_bAnalysisWorkEnvOnly = FALSE;
	theApp.m_bDrawingWorkEnvOnly = FALSE;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
}
//切换到建模绘图界面
//切换到构件编辑器界面
void CMainFrame::ShiftToPartEditor()
{
	((CWinAppEx*)&theApp)->SaveState(this);
	ShowPane(&m_wndToolBar,FALSE,FALSE,TRUE);
	ShowPane(&m_wndGLBar,TRUE,FALSE,TRUE);
	ShowPane(&m_wndPartSearch,FALSE,FALSE,TRUE);
	ShowPane(&m_wndModelling,FALSE,FALSE,TRUE);
	ShowPane(&m_wndToolKit,FALSE,FALSE,TRUE);
	ShowPane(&m_wndPartDesignBar,FALSE,FALSE,TRUE);
	ShowPane(&m_wndViewBar,FALSE,FALSE,TRUE);
	ShowPane(&m_wndDataCheckBar,FALSE,FALSE,TRUE);
	ShowPane(&m_wndPartEditor,TRUE,FALSE,TRUE);
	DockPane(&m_wndPartEditor);
	RecalcLayout();
	DockPaneLeftOf(&m_wndGLBar, &m_wndPartEditor);
	LoadNewMenu(IDR_PART_ENV);
}

void CMainFrame::ExitPartApp() 
{
	prev_part.Empty();
	next_part.Empty();
	theApp.LoadState(this);
	if(theApp.m_eEnvType!=theApp.m_ePrevEnvType)
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
	else
		theApp.m_eEnvType=LINE_ENV;
	/*通过LoadState加载菜单工具栏显示状态，不再需要以下代码 wht 14-11-13
	ShowPane(&m_wndPartEditor,FALSE,FALSE,TRUE);
	ShowPane(&m_wndToolBar,TRUE,FALSE,TRUE);
	ShowPane(&m_wndGLBar,TRUE,FALSE,TRUE);
	ShowPane(&m_wndModelling,TRUE,FALSE,TRUE);
	ShowPane(&m_wndToolKit,TRUE,FALSE,TRUE);
	ShowPane(&m_wndPartDesignBar,TRUE,FALSE,TRUE);
	DockPaneLeftOf(&m_wndGLBar, &m_wndModelling);
	LoadNewMenu(theApp.m_nMainMenuID);*/
}

void CMainFrame::OnClose() 
{
	g_pSolidSet->ExitComponent();	//防止此时如SnapPoint,SnapLine等函数在关闭程序时依旧保持运行状态造成无法关闭程序
	if(fpLocalCmdLine&&fpLocalCmdLine->m_bGettingStrFromCmdLine)
	{	//当前有命令在执行不能直接退出关闭程序
		fpLocalCmdLine->m_bForceExitCommand=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("One command is running, terminating now, please close program later!");
#else
		AfxMessageBox("有一函数命令正在执行，正在终止，请稍后再关闭程序!");
#endif
		return;
	}
	if(m_pDoc&&!m_pDoc->IsModified()&&Ta.IsDataModified())
		m_pDoc->SetModifiedFlag(TRUE);
	UINT nTimer=theApp.GetLDSDoc()->m_nTimer;
	if(nTimer!=0)
		KillTimer(nTimer);
	//SaveBarState("General");
	CString sys_file_path;
	GetSysPath(APP_PATH);
#ifdef __TMA_
	sys_file_path.Format("%stma.cfg",APP_PATH);
#elif defined(__LMA_)
	sys_file_path.Format("%slma.cfg",APP_PATH);
#elif defined(__TSA_)
	sys_file_path.Format("%stsa.cfg",APP_PATH);
#elif defined(__TDA_)
	sys_file_path.Format("%stda.cfg",APP_PATH);
#else
#ifdef __LDS_TSA_
	sys_file_path.Format("%stsa.cfg",APP_PATH);
#else
	sys_file_path.Format("%slds.cfg",APP_PATH);
#endif
#endif
	/*
	RECT rect;
	CDockPageBar *pLeftDockBar=FindDockBarByID(AFX_IDW_DOCKBAR_LEFT);
	if(pLeftDockBar->GetSafeHwnd()!=NULL)
	{
		pLeftDockBar->GetWindowRect(&rect);
		g_sysPara.dock.m_nLeftPageWidth=(short)(rect.right-rect.left);
	}
	CDockPageBar *pRightDockBar=FindDockBarByID(AFX_IDW_DOCKBAR_RIGHT);
	if(pRightDockBar&&pRightDockBar->GetSafeHwnd()!=NULL)
	{
		pRightDockBar->GetWindowRect(&rect);
		g_sysPara.dock.m_nRightPageWidth=(short)(rect.right-rect.left);
	}
	CDockPageBar *pBottomDockBar=FindDockBarByID(AFX_IDW_DOCKBAR_BOTTOM);
	if(pBottomDockBar&&pBottomDockBar->GetSafeHwnd()!=NULL)
	{
		pBottomDockBar->GetWindowRect(&rect);
		g_sysPara.dock.m_nBottomPageHeight=(short)(rect.bottom-rect.top);
	}*/
	g_sysPara.Write(sys_file_path);
	CFrameWndEx::OnClose();
}
//设置标题
void CMainFrame::SetBottomWorkspaceTitle(LPCSTR sTitle)
{
	//m_wndWorkspaceBottom.SetTitle(sTitle);
}

//激活指定的DockPage
void CMainFrame::ActivateDockpage(CRuntimeClass *pRuntimeClass)
{
	ModifyDockpageStatus(pRuntimeClass,TRUE);
}
void CMainFrame::ShowTaskInfoList(CTaskOutputList &infoList)
{
	COutputInfoDlg *pTaskListDlg = GetOutputInfoPage();
	if(!g_sysPara.dock.pageOutput.bDisplay)
		OnDockpageOutput();
	pTaskListDlg->InitOutputInfoList(infoList);
	char sTitle[50]="";
#ifdef AFX_TARG_ENU_ENGLISH
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d task",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0 task");
#else 
	if(infoList.GetNodeNum()>0)
		sprintf(sTitle,"%d项任务",infoList.GetNodeNum());
	else 
		sprintf(sTitle,"0项任务");
#endif
	pTaskListDlg->SetInfoTitle(sTitle);
	ActivateDockpage(RUNTIME_CLASS(COutputInfoDlg));
}

void CMainFrame::ModifyDockpageStatus(CRuntimeClass *pRuntimeClass, BOOL bShow)
{
	if(m_towerPropertyView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_towerPropertyView.ShowPane(bShow,FALSE,TRUE);
	else if(m_towerTreeView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_towerTreeView.ShowPane(bShow,FALSE,TRUE);
	else if(m_cmdLineView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_cmdLineView.ShowPane(bShow,FALSE,TRUE);
#if defined(__LDS_)||defined(__TSA_)
	else if(m_revisionOperView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_revisionOperView.ShowPane(bShow,FALSE,TRUE);
	else if(m_logView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_logView.ShowPane(bShow,FALSE,TRUE);
#endif
	else if(m_outputInfoView.GetDlgPtr()->IsKindOf(pRuntimeClass))
	{
		m_outputInfoView.ShowPane(bShow,FALSE,TRUE);
		COutputInfoDlg* pTaskDlg=(COutputInfoDlg*)m_outputInfoView.GetDlgPtr();
		if(bShow&&pTaskDlg!=NULL)
		{
			CString title;
			title.LoadString(IDS_OUTPUT_INFO_VIEW);
			if(pTaskDlg->GetInfoTitle().GetLength()>0)
				title+="-"+pTaskDlg->GetInfoTitle();
			//窗口处于隐藏状态时GetParentTabbedPane()为NULL wht 15-04-25
			if(m_outputInfoView.GetParentTabbedPane())
				m_outputInfoView.GetParentTabbedPane()->SetWindowText(title);
		}
	}
#ifdef __CONNECT_REMOTE_SERVER_
	else if((/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())&&m_remoteServerView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_logView.ShowPane(bShow,FALSE,TRUE);
#endif
#ifdef __TMA_
	else if(PRODUCT_FUNC::IsHasInternalTest()&&m_dataCheckView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_dataCheckView.ShowPane(bShow,FALSE,TRUE);
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(m_towerPartGroupView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_towerPartGroupView.ShowPane(bShow,FALSE,TRUE);
#ifdef __SMART_DRAW_INC_
	else if(m_partNoListView.GetDlgPtr()->IsKindOf(pRuntimeClass))
		m_partNoListView.ShowPane(bShow,FALSE,TRUE);
#endif
#endif
}
void CMainFrame::OnDockpageModel() 
{
	ShowPane(&m_towerTreeView,!m_towerTreeView.IsVisible(),FALSE,TRUE);
}

void CMainFrame::OnUpdateDockpageModel(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_towerTreeView.IsVisible());
}

void CMainFrame::OnDockpageProperty() 
{
	ShowPane(&m_towerPropertyView,!m_towerPropertyView.IsVisible(),FALSE,TRUE);
}

void CMainFrame::OnUpdateDockpageProperty(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_towerPropertyView.IsVisible());
}

void CMainFrame::OnDockpageCmdline() 
{
	ShowPane(&m_cmdLineView,!m_cmdLineView.IsVisible(),FALSE,TRUE);
}

void CMainFrame::OnUpdateDockpageCmdline(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_cmdLineView.IsVisible());
}
void CMainFrame::ShowDockpageOutput()
{
	ShowPane(&m_outputInfoView,!m_outputInfoView.IsVisible(),FALSE,TRUE);
}
void CMainFrame::OnDockpageOutput()
{
	ShowPane(&m_outputInfoView,!m_outputInfoView.IsVisible(),FALSE,TRUE);
}
void CMainFrame::OnUpdateDockpageServer(CCmdUI* pCmdUI)
{
#ifdef __CONNECT_REMOTE_SERVER_
	if(/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())
		pCmdUI->SetCheck(m_remoteServerView.IsVisible());
#endif
}
void CMainFrame::OnDockpageServer()
{
#ifdef __CONNECT_REMOTE_SERVER_
	if(/*theApp.m_bNetModelWork||*/PRODUCT_FUNC::IsHasInternalTest())
		ShowPane(&m_remoteServerView,!m_remoteServerView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockpageOutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_outputInfoView.IsVisible());
}
void CMainFrame::OnDockdataCheck()
{
#ifdef __TMA_
	if(PRODUCT_FUNC::IsHasInternalTest())
		ShowPane(&m_dataCheckView,!m_dataCheckView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockdataCheck(CCmdUI* pCmdUI)
{
#ifdef __TMA_
	if(PRODUCT_FUNC::IsHasInternalTest())
		pCmdUI->SetCheck(m_dataCheckView.IsVisible());
#endif
}
void CMainFrame::OnDockpageRevision()
{
#if defined(__LDS_)||defined(__TSA_)
	ShowPane(&m_revisionOperView,!m_revisionOperView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockpageRevision(CCmdUI* pCmdUI)
{
#if defined(__LDS_)||defined(__TSA_)
	pCmdUI->SetCheck(m_revisionOperView.IsVisible());
#endif
}
void CMainFrame::OnDockpageLogger()
{
#if defined(__LDS_)||defined(__TSA_)
	ShowPane(&m_logView,!m_logView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockpageLogger(CCmdUI* pCmdUI)
{
#if defined(__LDS_)||defined(__TSA_)
	pCmdUI->SetCheck(m_logView.IsVisible());
#endif
}
void CMainFrame::OnDockpageWeldPart()
{
#ifdef __COMMON_PART_INC_
	ShowPane(&m_towerPartGroupView,!m_towerPartGroupView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockpageWeldPart(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_towerPartGroupView.IsVisible());
}
void CMainFrame::OnDockpagePartLabel()
{
#ifdef __SMART_DRAW_INC_
	ShowPane(&m_partNoListView,!m_partNoListView.IsVisible(),FALSE,TRUE);
#endif
}
void CMainFrame::OnUpdateDockpagePartLabel(CCmdUI* pCmdUI)
{
#ifdef __SMART_DRAW_INC_
	pCmdUI->SetCheck(m_partNoListView.IsVisible());
#endif
}

void CMainFrame::OnReleaseNotes()
{
	theApp.GetVersionUpdateNotes(false);
}
void CMainFrame::OnUpgradeSoftware()
{
	HELPINFO helpInfo;
	helpInfo.dwContextId=ID_UPGRADE_SOFTWARE;
	helpInfo.iContextType=HELPINFO_MENUITEM;
	//OnHelpInfo(&helpInfo);
	CXhChar200 helpStr((char*)theApp.m_pszHelpFilePath);
	helpStr.Append("::/technical_support.htm");
	HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);//::GetDesktopWindow(),"D:\\Cimsforta\\LDS\\Debug\\lds.chm::\\设计原理\\基准图元.htm#基准线",HH_DISPLAY_TOPIC,NULL);
}

void CMainFrame::OnSendMsgToOther() 
{
	CSendMsgDlg msg_dlg;
	if(msg_dlg.DoModal()==IDOK)
		theApp.SendMsgToOther(msg_dlg.m_sMsgReceiver,msg_dlg.m_sMsgContent);
}

void CMainFrame::StartInfoPrompt()
{
	if(m_nTimerID==0)
		m_nTimerID=SetTimer(2,500,NULL);
}
void CMainFrame::KillInfoPrompt()
{
	if(theApp.m_dwUpdateWord==0&&theApp.m_arrMsg.GetNodeNum()==0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID=0;
#ifdef AFX_TARG_ENU_ENGLISH
		m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"no",TRUE);
#else
		m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"无",TRUE);
#endif
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	static int count;
	if(nIDEvent==2)
	{
		count++;
		count%=4;
		if(theApp.m_bNeedUpdatePerm)
		{
			m_wndStatusBar.m_crTxtColor=RGB(255,0,0);
			if(count%2==0)
				m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"",TRUE);
			else
#ifdef AFX_TARG_ENU_ENGLISH
				m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"update permission",TRUE);
#else
				m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"更新权限",TRUE);
#endif
		}
		else
		{
			m_wndStatusBar.m_crTxtColor=0;
			if(count%2==0)
				m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"",TRUE);
			else if(count==1)
			{
				if(theApp.m_arrMsg.GetNodeNum()>0)
#ifdef AFX_TARG_ENU_ENGLISH
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"refer to SMS",TRUE);
				else if(theApp.m_dwUpdateWord>0)
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"Update data",TRUE);
#else
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"查阅短信",TRUE);
				else if(theApp.m_dwUpdateWord>0)
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"更新数据",TRUE);
#endif
			}
			else if(count==3)
			{
				if(theApp.m_dwUpdateWord>0)
#ifdef AFX_TARG_ENU_ENGLISH
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"Update data",TRUE);
				else if(theApp.m_arrMsg.GetNodeNum()>0)
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"refer to SMS",TRUE);
#else
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"更新数据",TRUE);
				else if(theApp.m_arrMsg.GetNodeNum()>0)
					m_wndStatusBar.SetPaneTextById(ID_INDICATOR_MESSAGE,"查阅短信",TRUE);
#endif
			}
		}
	}
	
	CFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnPartEnv() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	theApp.m_ePrevEnvType=theApp.m_eEnvType;
	theApp.m_eEnvType=PARTEDITOR_ENV;
	if(GetLDSView()->m_curAppType==TA_ASSEMBLE_ENV)
		g_pSolidOper->PushEnv();
	ShiftToPartEditor();
	GetLDSView()->ShiftApplication(TA_PART_EDITOR);
#endif
}
void CMainFrame::OnUpdatePartEnv(CCmdUI* pCmdUI)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	pCmdUI->SetCheck(theApp.m_eEnvType==PARTEDITOR_ENV);
#endif
}
void CMainFrame::OnAppExitPartApp() 
{
#ifdef __ANGLE_PART_INC_
	ExitPartApp();
	GetLDSView()->ShiftApplication(TA_ASSEMBLE_ENV);
#endif
}

void CMainFrame::OnPartEditorToolBarDropDown(NMHDR *pnm,LRESULT *plr)
{
	NMTOOLBAR *pnmtb=(NMTOOLBAR*)pnm;
	switch(pnmtb->iItem)
	{
	case ID_BN_PREV_PART:
		break;
	case ID_BN_NEXT_PART:
		break;
	default:
		return;
	}
	// 装入和显示弹出菜单
	CMenu menu;
	menu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pPopup=menu.GetSubMenu(0);
	pPopup->DeleteMenu(0,MF_BYPOSITION);
	CRect rect;
	CWnd *pWnd=&m_wndPartEditor;
	pWnd->SendMessage(TB_GETRECT,pnmtb->iItem,(LPARAM)&rect);
	pWnd->ClientToScreen(&rect);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,rect.left,rect.bottom,this,&rect);
}

void CMainFrame::UpdateAllPages()
{
	CTowerTreeDlg *pTreePage = GetTowerTreePage();
	if(pTreePage)
		pTreePage->RefreshTreeCtrl();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CTowerPartGroupDlg *pPartGroupPage=GetTowerPartGroupPage();
	if(pPartGroupPage)
		pPartGroupPage->RefreshModel();
#ifdef __SMART_DRAW_INC_
	CPartNoListDlg *pPartNoListPage = GetPartNoListPage();
	if(pPartNoListPage)
		pPartNoListPage->EmptyPartNoList();
#endif
#endif
}

/*void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}*/

void CMainFrame::OnSolidModeDisplay() 
{
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		CLDSView* pWorkView=GetLDSView();
		if(pCmdLine && (!pCmdLine->m_bGettingStrFromCmdLine&&!pWorkView->IsCmdLocked()))
			pWorkView->Command("RenderMode");
		else
		{
			CLDSView *pWorkView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pWorkView->RenderMode();
		}
	}
}

void CMainFrame::OnUpdateSolidModeDisplay(CCmdUI* pCmdUI) 
{
	DISPLAY_TYPE type;
	g_pSolidSet->GetDisplayType(&type);
	pCmdUI->SetCheck(type==DISP_SOLID);
}

/* 目产实体数据占用内存问题已不突出，故暂去掉此函数 wjh-2012.12.17
void CMainFrame::OnClearMemoryBuffer() 
{
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		delete pPart->pSolidBody;
		pPart->pSolidBody=NULL;
		pPart->SetModified(TRUE);
	}
	g_pSolidSet->SetDisplayType(DISP_LINE);
	//清空显示缓存提高内存利用率
	g_pSolidDraw->EmptyDisplayBuffer();
	g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
	g_pSolidDraw->Draw();
}*/

void CMainFrame::OnRedo()
{
	CCmdLineDlg *pCmdLine=GetCmdLinePage();
	if(pCmdLine->GetSafeHwnd()&&pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("one command running, please terminate current command before executing a new command!");
#else
		AfxMessageBox("有命令在运行,请终止当前命令后再执行新命令!");
#endif
		return;
	}
	Ta.RedoTower();
	GetLDSView()->EmptySnapObjects();
	GetLDSView()->selectObjs.Empty();	//防止Redo时删除了选中的数据对象，进而导致死机 wjh-2018.10.8
	for(CLDSNode *pNode=Ta.Node.GetFirst(TRUE);pNode;pNode=Ta.Node.GetNext(TRUE))
	{
		if(Ta.Node.IsCursorDeleted())
		{	
			g_pSolidDraw->DelEnt(pNode->handle);
			console.DispNodeSet.DeleteNode(pNode->handle);
		}
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst(0,TRUE);pPart;pPart=Ta.Parts.GetNext(0,TRUE))
	{
		if(Ta.Parts.IsCursorDeleted())
		{
			g_pSolidDraw->DelEnt(pPart->handle);
			console.DispPartSet.DeleteNode(pPart->handle);
		}
	}
	Ta.Node.Clean();
	Ta.Parts.Clean();
	g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));	//只渲染发生改变的构件，同时重建显示构件清单 wjh-2011.11.25
	//GetLDSView()->RedrawAll();		//RedrawAll会重新渲染所有构件太慢
}

void CMainFrame::Undo()
{
	CCmdLineDlg *pCmdLine=GetCmdLinePage();
	if(pCmdLine->GetSafeHwnd()&&pCmdLine->m_bGettingStrFromCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("one command running, please terminate current command before executing a new command!!");
#else
		AfxMessageBox("有命令在运行,请终止当前命令后再执行新命令!");
#endif
		return;
	}
	Ta.UndoTower();
	GetLDSView()->EmptySnapObjects();
	GetLDSView()->selectObjs.Empty();	//防止Undo时删除了选中的数据对象，进而导致死机 wjh-2018.10.8
	for(CLDSNode *pNode=Ta.Node.GetFirst(TRUE);pNode;pNode=Ta.Node.GetNext(TRUE))
	{
		if(Ta.Node.IsCursorDeleted())
		{
			console.DispNodeSet.DeleteNode(pNode->handle);
			g_pSolidDraw->DelEnt(pNode->handle);
		}
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst(0,TRUE);pPart;pPart=Ta.Parts.GetNext(0,TRUE))
	{
		if(Ta.Parts.IsCursorDeleted())
		{
			console.DispPartSet.DeleteNode(pPart->handle);
			if(pPart->GetClassTypeId()==CLS_BOLT)
				Ta.DelBoltAndRelativeBoltRef(pPart->handle);
			g_pSolidDraw->DelEnt(pPart->handle);
		}
	}
	Ta.Node.Clean();
	Ta.Parts.Clean();
	g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
	//GetLDSView()->RedrawAll();
}
void CMainFrame::OnUndo()
{
	Undo();
}
void CMainFrame::OnUpdateUndo(CCmdUI* pCmdUI) 
{
	/*CCmdLineDlg *pCmdLine=GetCmdLinePage();
	if(pCmdLine->GetSafeHwnd()&&pCmdLine->m_bGettingStrFromCmdLine)
		pCmdUI->Enable(FALSE);
	else*/
		pCmdUI->Enable(Ta.CanUndoOper());
}

void CMainFrame::OnUpdateRedo(CCmdUI* pCmdUI) 
{
	/*CCmdLineDlg *pCmdLine=GetCmdLinePage();
	if(pCmdLine->GetSafeHwnd()&&pCmdLine->m_bGettingStrFromCmdLine)
		pCmdUI->Enable(FALSE);
	else*/
		pCmdUI->Enable(Ta.CanRedoOper());
}

#if defined(__LDS_)||defined(__TSA_)
void CMainFrame::OnImportLoadFile() 
{
	CAttachBuffer *pRsltBuffer=Ta.AttachBuffer.GetValue(CTower::RSLT_REPORT);
#ifdef AFX_TARG_ENU_ENGLISH
	if(pRsltBuffer&&pRsltBuffer->GetLength()>0 && MessageBox("current file includes past calculation results data."
		"To maintain data consistency, when importing the load file, system will automatically clear past results data!","",MB_OKCANCEL)!=IDOK)
#else
	if(pRsltBuffer&&pRsltBuffer->GetLength()>0 && MessageBox("当前文件中已包含过往计算结果数据。"
		"为保持数据一致性, 导入荷载文件时系统将自动清空过往计算结果数据！","提示",MB_OKCANCEL)!=IDOK)
#endif
		return;
	CFileDialog dlg(TRUE,"lod","load",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"load file(*.lod)|*.lod||");
#else
		"荷载文件(*.lod)|*.lod||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		char filename[MAX_PATH];
		_snprintf(filename,MAX_PATH-1,"%s",dlg.GetPathName());
		Ta.m_iLoadExecuteCode=3;	//导入荷载文件时，自动将当前荷载执行规范变为用户指定，必须先指定否则无法正确设定系数
		Ta.ImportLoadFile(filename,Ta.m_fGamaGForDesign);
		if(pRsltBuffer)
			pRsltBuffer->ClearBuffer();
		CTowerTreeDlg* pTree=GetTowerTreePage();
		pTree->RefreshTreeCtrl();
	}
}

void CMainFrame::OnExportLoadFile() 
{
	CFileDialog dlg(FALSE,"lod","load",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"load file(*.lod)|*.lod||");
#else
		"荷载文件(*.lod)|*.lod||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		char filename[MAX_PATH];
		_snprintf(filename,MAX_PATH,"%s",dlg.GetPathName());
		Ta.ExportLoadFile(filename,Ta.m_fGamaGForDesign);
	}
}

void CMainFrame::OnDisplayDeformTower() 
{
	m_bDisplayDeformedTower=!m_bDisplayDeformedTower;
	if(m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
		theApp.m_eEnvType=FEMELEM_ENV;
	GetLDSView()->RedrawAll();
}

void CMainFrame::OnUpdateDisplayDeformTower(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bDisplayDeformedTower);
}

bool IsNoInLimStr(int iNo,char* lim_str)
{
	if(strchr(lim_str,'*')!=NULL)
		return true;
	long iFindNo=FindAdaptNo(lim_str);
	while(iFindNo>0)
	{
		if(iFindNo==iNo)
			return true;
		iFindNo=FindAdaptNo(NULL);
	}
	return false;
}
//下面进行三维桁架应力分析的函数来自于truss3d.cpp
BOOL truss3d_analysis(void* pResolver,CAnalyzeProcessor *pAnalysis,COMPUTE_METHOD method,bool bAnalysisForFoundation,bool bRecurResolve);
int MemberCheck(CElement *pElem,BOOL bCheckAll,int iDesignCode,bool bEqCheck=false,bool bUpdateBoltInfo=false);
#ifdef __TIMER_COUNT_
#include "TimerCount.h"
#endif
bool SubBlockModelOptimizeCore(ANALYZE_SCOPE& analScope,CCmdLineDlg *pCmdLine)
{
	CBlockResolver* pResolver=CBlockResolver::CreateResolver();
	return false;
}
//from AnalyzeProcessor.cpp
FEM_NODE_PTR ConnectFemNode(void* pContext,long pointI);
FEM_ELEMENT_PTR ConnectFemElement(void* pContext,long idElem);
bool ModelCheckCore(ANALYZE_SCOPE& analScope,CCmdLineDlg *pCmdLine,bool bEqCheck=false,bool bUpdateBoltInfo=false)
{
	CLogErrorLife loglife;
	COMPUTE_METHOD method;
	if(g_sysPara.AnalysisPara.m_iResolverStyle==0)
		method=GAUSS_REDUCTION;
	else if(g_sysPara.AnalysisPara.m_iResolverStyle==1)
		method=WAVE_FRONT_METHOD;
	else	//子结构法
		method=SUB_BLOCK_METHOD;

	int iModuleNo=FindAdaptNo(analScope.csModule);
	CLDSModule *pModule;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		pModule->feature=0;
	int nModuleCount=0;
	CPreProcessor::ActiveDesignCode = Ta.DesignCode();
	analScope.cfgwordBody.Clear();
	analScope.cfgwordLeg.Clear();
	while(iModuleNo!=0)
	{
		for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->iNo==iModuleNo)
			{
				if(method==SUB_BLOCK_METHOD)
				{
					CXhChar50 error;
					NODESET boundary;
					if(!pModule->SubBlockCheck(boundary,&error))
					{
						AfxMessageBox(error);
						return false;
					}
				}
				//analScope.cfgwordBody.flag.bytes[pModule->GetBodyNo()-1]=0xff;
				analScope.cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
				analScope.cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
				pModule->feature=TRUE;
				nModuleCount++;
			}
		}
		iModuleNo=FindAdaptNo(NULL);
	}
	CString promptStr;
	if(pCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		promptStr.Format("module number %d"to be calculated,nModuleCount);
#else
		promptStr.Format("待计算模型数%d",nModuleCount);
#endif
		pCmdLine->PromptStrInCmdLine(promptStr);
	}
	CWorkStatus *pStatus;
	CHashTable<CWorkStatus*>hashWorkStatus;
	hashWorkStatus.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		pStatus->feature=FALSE;
		hashWorkStatus.SetValueAt(pStatus->iNo,pStatus);
	}
	int nCaseCount=0;
	int iStatusNo=FindAdaptNo(analScope.csStatus);
	while(iStatusNo!=0)
	{
		if(hashWorkStatus.GetValueAt(iStatusNo,pStatus))
		{
			pStatus->feature=TRUE;
			nCaseCount++;
		}
		iStatusNo=FindAdaptNo(NULL);
	}
	if(pCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		promptStr.Format("working conditions number %d",nCaseCount);
#else
		promptStr.Format("工况数%d",nCaseCount);
#endif
		pCmdLine->PromptStrInCmdLine(promptStr);
	}

	THANDLE bak_module_handle=Ta.m_hActiveModule;
	THANDLE bak_workstatus_handle=Ta.m_hActiveStatus;
	CBlockResolver* pSubBlkResolver=NULL;
	CFemResolver *pResolver=NULL;
	if(method==SUB_BLOCK_METHOD)
		pSubBlkResolver=CBlockResolver::CreateResolver();
	else
		pResolver=CFemResolver::CreateResolver();
	bool result=false;
	try
	{
		Ta.m_iIteration=0;
#if defined(__TSA_)||defined(__LDS_)
		if(g_sysPara.bCreateAnsysCmdFile||g_sysPara.bCreateDetailRptFile)
			theApp.GetWorkPathAuto();	//自动获取当前文件计算结果的输出目录
#endif
		CLDSLinePart *pPole;
		for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
		{
			pPole->feature=0;		//还未配材
			pPole->pCtrlElem=NULL;	//复原选材控制规则
		}
#ifdef __TIMER_COUNT_
		timer.Start();
#endif
		DWORD pre_ticks=0;
		DWORD start_ticks=GetTickCount();
		if(pCmdLine)
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->PromptStrInCmdLine("calculating model original design information extraction arithmetic!");
#else
			pCmdLine->PromptStrInCmdLine("进行计算模型原始设计信息提取运算!");
#endif
		if(pResolver)
			preprocessor.PrepareTower(&Ta,analScope.cfgwordBody,analScope.cfgwordLeg);
		else if(pSubBlkResolver)
			preprocessor.PrepareTowerSubBlock(pSubBlkResolver,&Ta,analScope.cfgwordBody,analScope.cfgwordLeg);
		preprocessor.SetAnalysisProcessor(&analysis);
		analysis.SetPostProcessor(&postprocessor);
		postprocessor.Empty();	//清空以前所有的杆件单元记录
		char filename[MAX_PATH];
		theApp.GetFileName(filename);
		postreport.InitProcTempFile(CXhChar200("%s",filename));
		if(pCmdLine)
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->PromptStrInCmdLine("Update calculating model unit cell lode!");
#else
			pCmdLine->PromptStrInCmdLine("更新计算模型单元体荷载!");
#endif
#ifdef __TIMER_COUNT_
		timer.Relay(1);
#endif
		preprocessor.UpdateInfoAtElemSizeChange();
#ifdef __TIMER_COUNT_
		timer.Relay(2);
#endif
		ARRAY_LIST<WORD>legArr;
		ARRAY_LIST<CFGWORD>arrCalModule;
		int iCalUnit=0;
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->feature==0)	//处于睡眠状态现不处理此计算模型
				continue;
			if(pSubBlkResolver)
				pSubBlkResolver->ClearNodeLoadBuffer();
			BOOL bPushModule=Ta.Module.push_stack();
			Ta.m_hActiveModule=pModule->handle;
			//计算当前呼高模型本体部分重量
			long key=pModule->GetBodyNo()*10;
			TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetValue(key);
			if(pWeight==NULL)
			{
				pWeight=postprocessor.TowerWeight.Add(key);
				pWeight->netWeight=pWeight->weight=0.0;
				pWeight->cfg_no =pModule->GetBodyNo();
				pWeight->iBody0Leg1 = 0;	//本体部分
				for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
				{
#ifndef __TSA_
					if(pPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pPole)->group_father_jg_h>0x20)
						continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
					if(pModule->IsSonPart(pPole)&&!pPole->IsLegObj())
					{
						double weight=pPole->GetWeight(FALSE);
						pWeight->netWeight+=weight;
						pWeight->weight+=weight*pPole->m_fWeightRaiseCoef;
					}
				}
			}
			CFGWORD legword;	//暂用配材号代替存储各象位接腿号
			legArr.SetSize(0,10);
			arrCalModule.SetSize(0,10);
			int i;
			if(g_sysPara.iLegShiftType==0)
			{	//不进行接腿轮换
				for(i=0;i<4;i++)
					legword.flag.word[i]=pModule->m_arrActiveQuadLegNo[i];
				arrCalModule.append(legword);
			}
			else
			{	//需要进行接腿轮换
				for(i=0;i<192;i++)
				{
					if(!pModule->m_dwLegCfgWord.IsHasNo(i+1))
						continue;
					if(g_sysPara.iLegShiftType==1)
					{	//只进行等高腿轮换
						legword.flag.word[0]=legword.flag.word[1]=legword.flag.word[2]=legword.flag.word[3]=i+1;
						arrCalModule.append(legword);
					}
					else
						legArr.append((WORD)i+1);
				}
				if(g_sysPara.iLegShiftType==2)
				{	//进行高低腿轮换计算
					for(i=0;i<legArr.GetSize();i++)
					{
						legword.flag.word[0]=legArr[i];
						for(int j=0;j<legArr.GetSize();j++)
						{
							legword.flag.word[1]=legArr[j];
							for(int k=0;k<legArr.GetSize();k++)
							{
								legword.flag.word[2]=legArr[k];
								for(int m=0;m<legArr.GetSize();m++)
								{
									legword.flag.word[3]=legArr[m];
									arrCalModule.append(legword);
								}
							}
						}
					}
				}
			}
			preprocessor.UpdateInfoAtModuleChange(pModule);
			if(pSubBlkResolver)
			{
				pSubBlkResolver->ResetStructure();
				CModuleFrame* pFrame = preprocessor.result.hashModuleFrames.GetValue(pModule->iNo);
				for(SUBBLOCKREF* pBlkRef=pFrame->blkrefs.GetFirst();pBlkRef;pBlkRef=pFrame->blkrefs.GetNext())
					pSubBlkResolver->AddSubBlockRef(pBlkRef->serial,pBlkRef->acs);
				pSubBlkResolver->SetConnectFemNodeFunc(ConnectFemNode);
				pSubBlkResolver->SetConnectFemElementFunc(ConnectFemElement);
			}
#ifdef __TIMER_COUNT_
			timer.Relay(3);
#endif
			postprocessor.Reaction.SetHashTableGrowSize(nModuleCount*arrCalModule.GetSize()*nCaseCount*4,false);
			for(i=0;i<arrCalModule.GetSize();i++)
			{
				WORD legQuadBak[4];
				memcpy(legQuadBak,pModule->m_arrActiveQuadLegNo,8);
				int iLeg;
				for(iLeg=0;iLeg<4;iLeg++)
					pModule->m_arrActiveQuadLegNo[iLeg]=(WORD)arrCalModule[i].flag.word[iLeg];
#ifdef __TIMER_COUNT_
				timer.Relay();
#endif
				preprocessor.RetrieveFemAnalInfo(pModule);
#ifdef __TIMER_COUNT_
				timer.Relay(41);
#endif
				preprocessor.UpdateInfoAtLegChange(pModule,pSubBlkResolver);
				if(pSubBlkResolver)
					pSubBlkResolver->ConnectFemNodeAndElement(&analysis);
#ifdef __TIMER_COUNT_
				timer.Relay(42);
#endif
				//计算当前呼高中各腿的重量
				for(iLeg=0;iLeg<4;iLeg++)
				{
					key=pModule->m_arrActiveQuadLegNo[iLeg]*10+1;
					TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetValue(key);
					if(pWeight==NULL)
					{
						pWeight=postprocessor.TowerWeight.Add(key);
						pWeight->netWeight=pWeight->weight=0.0;
						pWeight->cfg_no = pModule->m_arrActiveQuadLegNo[iLeg];
						pWeight->iBody0Leg1 = 1;	//接腿
						for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
						{
#ifndef __TSA_
							if(pPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pPole)->group_father_jg_h>0x20)
								continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
							if(pPole->layer(0)=='L'&&pPole->cfgword.IsHasNo(pWeight->cfg_no))
							{
								double weight=pPole->GetWeight(FALSE);
								pWeight->netWeight+=weight;
								pWeight->weight+=weight*pPole->m_fWeightRaiseCoef;
							}
						}
						pWeight->weight*=0.25;	//表示1/4组腿重
						pWeight->netWeight*=0.25;
					}
				}
#ifdef __TIMER_COUNT_
				timer.Relay();
#endif
//#ifdef lksd
				bool bFirstResolve=true;	//首次计算框架时需要初始化信息,之后只需继承计算即可
				if(pre_ticks==0)
					pre_ticks=GetTickCount();
				for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
				{
					if(pStatus->feature==0)	//处于睡眠状态现不处理此计算模型
						continue;
					if(!bEqCheck&&pStatus->m_cMainLoadType>=5&&pStatus->m_cMainLoadType<=8)	//抗震验算荷载
					//if(!IsNoInLimStr(pModule->iNo,pStatus->limit_str))
						continue;	//当前工况不适用于当前计算呼高模型
					BOOL bPushStatus=Ta.WorkStatus.push_stack();
					Ta.m_hActiveStatus=pStatus->handle;
					int slopes=max(1,preprocessor.GetEiffelSlopes(true,pStatus));
					if(slopes>15)	//TODO: 一般不会这样，但对于异型结构，如变电构架可能返回错误值，此时暂强制性赋1 wjh-2014.7.4
						slopes=1;
					int eiffel_cases=(slopes-1)*2+1;
					for(int eiffel=0;eiffel<eiffel_cases;eiffel++)
					{
#ifdef __TIMER_COUNT_
						timer.PushTicks();
#endif
					//清空风压数据，重新计算
					for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
					{
						pNode->WindLoad.Set();
						pNode->AutoCalLoad.Set();
					}
					pStatus->SetEiffelCaseNo(eiffel);
					BOOL bVerifyByStdLoad=(analScope.m_iCalMode==CCfgCalScopeDlg::CHK_DEFORM)||analScope.m_bVerifyFoundationByStdLoad;
					preprocessor.UpdateInfoAtStatusChange(pModule,pStatus,analScope.m_bVerifyFoundationMode,analScope.m_bVerifyFoundationByStdLoad,bEqCheck);
#ifdef __TIMER_COUNT_
					timer.Relay(72);
#endif
					if(pSubBlkResolver)
					{
						pSubBlkResolver->ApplyNodeLoad(pStatus->iNo);
#ifdef __TIMER_COUNT_
						if(i==0)
							timer.Relay(73);
						else
							timer.Relay(74);
#endif
					}
#ifdef __TIMER_COUNT_
					timer.PopTicks();
					timer.Relay(7);
					timer.PushTicks();
#endif
					BOOL analyzed=FALSE;
					if(pSubBlkResolver)
						analyzed=truss3d_analysis(pSubBlkResolver,&analysis,method,analScope.m_bVerifyFoundationMode,false);
					else if(pResolver)
						analyzed=truss3d_analysis(pResolver,&analysis,method,analScope.m_bVerifyFoundationMode,!bFirstResolve);
#ifdef __TIMER_COUNT_
					timer.PopTicks();
					timer.Relay(8);
#endif
					if(!analyzed)
					{
						if(bPushModule)
							Ta.Module.pop_stack();
						if(bPushStatus)
							Ta.WorkStatus.pop_stack();
#ifdef AFX_TARG_ENU_ENGLISH
						throw CString("A node truss system is variable geometry, transient system or lack of enough restraints, stress analysis cannot be done!");
#else
						throw CString("某节点处桁架体系为几何可变、瞬变体系或缺少足够的约束,无法进行应力分析!");
#endif
					}
					bFirstResolve=false;
					}
					if(bPushStatus)
						Ta.WorkStatus.pop_stack();
				}
				DWORD tick=GetTickCount();
				iCalUnit++;
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 prompt("%5d #has finished %2d module %C-%C-%C-%C leg's Mechanical Analysis,the analysis time consuming %5.3fs,total time consuming %8.3fs\n",
#else
				CXhChar100 prompt("%5d#已完成%2d号呼高%C-%C-%C-%C腿的力学分析,本次分析耗时%5.3fs,总耗时%8.3fs\n",
#endif
					iCalUnit,pModule->iNo,(arrCalModule[i].flag.word[0]-1)%8+'A',(arrCalModule[i].flag.word[1]-1)%8+'A',
					(arrCalModule[i].flag.word[2]-1)%8+'A',(arrCalModule[i].flag.word[3]-1)%8+'A',
					(tick-pre_ticks)*0.001,(tick-start_ticks)*0.001);
				pre_ticks=tick;
#ifdef __TIMER_COUNT_
				timer.Relay();
#endif
				DisplayPrompt(prompt);
				if(IsCanceledPromptDlg())
#ifdef AFX_TARG_ENU_ENGLISH
					throw CString("user terminates calculation!");
#else
					throw CString("用户中断计算过程!");
#endif
				analysis.UpdateInfoAtModuleChange(pModule);
//#endif
#ifdef __TIMER_COUNT_
				timer.Relay(9);
#endif
				memcpy(pModule->m_arrActiveQuadLegNo,legQuadBak,8);
			}
			if(bPushModule)
				Ta.Module.pop_stack();
		}
		for(pPole=preprocessor.result.allrodset.GetFirst();bUpdateBoltInfo&&pPole;pPole=preprocessor.result.allrodset.GetNext())
			pPole->feature=0;
		for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
		{
			postprocessor.hashElems.push_stack();
			//验证计算各单元的最大设计应力及参数
			pElem->m_iCtrlRule=0;			//清空单元的控制条件
			pElem->pParaType = NULL;
			MemberCheck(pElem,TRUE,Ta.m_iMemberSizeCheckCode,bEqCheck,bUpdateBoltInfo);
			postprocessor.hashElems.pop_stack();
			pElem->pOrgPole->feature=1;	//个位数为１表示验算更新过该杆件
		}
		//统一螺栓选材
		for(pPole=preprocessor.result.allrodset.GetFirst();bUpdateBoltInfo&&pPole;pPole=preprocessor.result.allrodset.GetNext())
		{
			if(pPole->feature/10>0)
				continue;	//表示已处理
			if(pPole->feature==0)
				continue;	//表示该杆件未直接验算更新过（上文中对称单元，只验算点号最小的杆件单元)
			if(pPole->pStart==NULL||pPole->pEnd==NULL)
				continue;
			preprocessor.result.allrodset.push_stack();
			LINEPARTSET poleset;
			poleset.append(pPole);
			CConnectInfo group_start_ls,group_end_ls;
			group_start_ls = pPole->connectStart;
			group_end_ls = pPole->connectEnd;
			GEPOINT rodvec=pPole->pEnd->Position(false)-pPole->pStart->Position(false);
			CXhSimpleList<CLDSLinePart*>listRelaRods;
			listRelaRods.AttachObject(pPole);
			for(RELATIVE_OBJECT *pMirObj=pPole->relativeObjs.GetFirst();pMirObj;pMirObj=pPole->relativeObjs.GetNext())
			{
				if(pMirObj->mirInfo.axis_flag&0x08)
					continue;	//搜索信息不包括旋转对称
				CLDSLinePart *pRelaMirRod=Ta.FromRodHandle(pMirObj->hObj);
				if(pRelaMirRod&&pRelaMirRod->pStart!=NULL&&pRelaMirRod->pEnd!=NULL)
				{
					GEPOINT mirrodvec=pMirObj->mirInfo.MirVector(rodvec);
					GEPOINT relarodvec=pRelaMirRod->pEnd->Position(false)-pRelaMirRod->pStart->Position(false);
					bool reverse=mirrodvec*relarodvec<0;
					if(reverse)
						pRelaMirRod->feature+=100;
					listRelaRods.AttachObject(pRelaMirRod);
					if(pRelaMirRod->feature%10>0)
					{
						if(reverse)
						{
							if(group_start_ls.wnSummBoltN<pRelaMirRod->connectEnd.wnSummBoltN)
								group_start_ls=pRelaMirRod->connectEnd;
							if(group_end_ls.wnSummBoltN<pRelaMirRod->connectStart.wnSummBoltN)
								group_end_ls=pRelaMirRod->connectStart;
						}
						else
						{
							if(group_start_ls.wnSummBoltN<pRelaMirRod->connectStart.wnSummBoltN)
								group_start_ls=pRelaMirRod->connectStart;
							if(group_end_ls.wnSummBoltN<pRelaMirRod->connectEnd.wnSummBoltN)
								group_end_ls=pRelaMirRod->connectEnd;
						}
					}
				}
			}
			for(LIST_NODE<CLDSLinePart*>* pListNode=listRelaRods.EnumFirst();pListNode;pListNode=listRelaRods.EnumNext())
			{
				if(pListNode->data->feature>=100)
				{
					pListNode->data->connectStart=group_end_ls;
					pListNode->data->connectEnd=group_start_ls;
				}
				else
				{
					pListNode->data->connectStart=group_start_ls;
					pListNode->data->connectEnd=group_end_ls;
				}
				pListNode->data->feature+=10;	//表示已处理
			}
		}
		if(g_sysPara.bUpdateRodDesignForce)
			analysis.UpdateElemRelaRodDesignForce();
		DisplayPrompt(NULL);
#ifdef __TIMER_COUNT_
		timer.PushTicks(true);
#endif
		postreport.CreateProcessReport();
#ifdef __TIMER_COUNT_
		timer.PopTicks();
		timer.Relay(10);
		timer.End();
		FILE* fp;
		if(method!=SUB_BLOCK_METHOD)
			fp=fopen("d:\\timer.txt","wt");
		else
			fp=fopen("d:\\timer_sub.txt","wt");
		fprintf(fp,"计算呼高:%s, 工况:%s\n",(char*)analScope.csModule,(char*)analScope.csStatus);
		fprintf(fp,"SUM time cost:%.3fs\n",(timer.GetEndTicks()-timer.GetStartTicks())*0.001);
		for(int iKey=1;iKey<1100;iKey++)
		{
			DWORD *pCost=timer.hashTicks.GetValue(iKey);
			if(pCost)
			{
				fprintf(fp,"%2d:%.3fs",iKey,*pCost*0.001);
				if(iKey==41)
					fprintf(fp," RetrieveFemAnalInfo\n");
				else if(iKey==42)
					fprintf(fp," UpdateInfoAtLegChange\n");
				else if(iKey==10)
					fprintf(fp," CreateProcessReport\n");
				else if(iKey==72)
					fprintf(fp," UpdateInfoAtStatusChange\n");
				else if(iKey==73)
					fprintf(fp," pSubBlkResolver->ApplyNodeLoad\n");
				else if(iKey==82)
					fprintf(fp," FemResolve\n");
				else if(iKey==84)
					fprintf(fp," AppendPrimaryNodeToAnalyzeBuffer\n");
				else if(iKey==85)
					fprintf(fp," AppendBraceNodeToAnalyzeBuffer\n");
				else if(iKey==86)
					fprintf(fp," AppendElemToAnalyzeBuffer\n");
				else if(iKey==88)
					fprintf(fp," pAnalysis->UpdateInfoAtStatusChange\n");
				else
					fprintf(fp,"\n");
			}
		}
		fclose(fp);
#endif
		result=true;
	}
	catch(CString sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	if(pResolver)
		CFemResolver::Destroy(pResolver->GetHandle());
	else if(pSubBlkResolver)
		CBlockResolver::Destroy(pSubBlkResolver->GetSerial());
	Ta.m_hActiveModule=bak_module_handle;
	Ta.m_hActiveStatus=bak_workstatus_handle;
	DisplayPrompt(NULL);
	return result;
}
#include "CompulsiveBreakDlg.h"
#include "WireLoadCalRunDlg.h"
void CMainFrame::OnModelWireLoadCal()
{
	CWireLoadCalRunDlg dlg;
	if(dlg.DoModal()==IDOK)
		GetTowerTreePage()->RefreshTreeCtrl();
}
void CMainFrame::ModelCheck(bool bEqCheck/*=false*/)
{
	BOOL bUpdateBoltInfo=m_xAnalyzeScope.m_bUpdateBoltInfo;
	//结果文件目录生成
	char file_path[MAX_PATH], file_name[MAX_PATH];
	CString ss=theApp.GetLDSDoc()->GetPathName();
	_snprintf(file_path,MAX_PATH-1,"%s",ss);
	GetSysPath(file_path,file_path,file_name);
	_chdir(file_path);
	_mkdir(file_name);
	_chdir(file_name);
	_mkdir("Step@0");
	CCmdLineDlg *pCmdLine=((CMainFrame*)theApp.m_pMainWnd)->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("check nodes number...","");
	if(!CheckNodeI())
	{
		pCmdLine->PromptStrInCmdLine("check nodes number...,not pass the check!");
		return;
	}
	pCmdLine->PromptStrInCmdLine("check nodes number...,pass!");
#else
	pCmdLine->FillCmdLine("进行节点编号检查...","");
	if(!CheckNodeI(&m_xAnalyzeScope.cfgwordBody,&m_xAnalyzeScope.cfgwordLeg))
	{
		pCmdLine->PromptStrInCmdLine("进行节点编号检查...，未通过节点编号检查！");
		return;
	}
	pCmdLine->PromptStrInCmdLine("进行节点编号检查...，通过！");
#endif
	if(ModelCheckCore(m_xAnalyzeScope,pCmdLine,bEqCheck,bUpdateBoltInfo!=FALSE))
	{
		//默认生成中文文本验算报告书文件
		strcat(file_path,file_name);
		strcat(file_path,"_worktemp\\");
		strcat(file_path,file_name);
		CreateDetailRptTxtFileByCN(file_path,m_xAnalyzeScope.cfgwordBody,m_xAnalyzeScope.cfgwordLeg,1);
		CRevisionOperDlg *pRevision=GetRevisionOperPage();
		pRevision->UpdateRevision(postreport);
	}
}
bool ModelOptimizeCore(ANALYZE_SCOPE& analScope,CCmdLineDlg *pCmdLine)
{
	CLogErrorLife loglife;
	COMPUTE_METHOD method;
	if(g_sysPara.AnalysisPara.m_iResolverStyle==0)
		method=GAUSS_REDUCTION;
	else if(g_sysPara.AnalysisPara.m_iResolverStyle==1)
		method=WAVE_FRONT_METHOD;
	else
		method=SUB_BLOCK_METHOD;

	int iModuleNo=FindAdaptNo(analScope.csModule);
	CLDSModule *pModule;
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		pModule->feature=0;
	CPreProcessor::ActiveDesignCode = Ta.DesignCode();
	//CFGWORD cfgwordBody,cfgwordLeg;
	analScope.cfgwordBody.Clear();
	analScope.cfgwordLeg.Clear();
	int nModuleCount=0;
	while(iModuleNo!=0)
	{
		for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->iNo==iModuleNo)
			{
				if(method==SUB_BLOCK_METHOD)
				{
					CXhChar50 error;
					NODESET boundary;
					if(!pModule->SubBlockCheck(boundary,&error))
					{
						AfxMessageBox(error);
						return false;
					}
				}
				//analScope.cfgwordBody.flag.bytes[pModule->GetBodyNo()-1]=0xff;
				analScope.cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
				analScope.cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
				pModule->feature=TRUE;
				nModuleCount++;
			}
		}
		iModuleNo=FindAdaptNo(NULL);
	}
	CString promptStr;
	if(pCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		promptStr.Format("model number %d to be calculated",nModuleCount);
#else
		promptStr.Format("待计算模型数%d",nModuleCount);
#endif
		pCmdLine->PromptStrInCmdLine(promptStr);
	}
	CWorkStatus *pStatus;
	CHashTable<CWorkStatus*>hashWorkStatus;
	hashWorkStatus.CreateHashTable(Ta.WorkStatus.GetNodeNum());
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		pStatus->feature=FALSE;
		hashWorkStatus.SetValueAt(pStatus->iNo,pStatus);
	}
	int nCaseCount=0;
	int iStatusNo=FindAdaptNo(analScope.csStatus);
	while(iStatusNo!=0)
	{
		if(hashWorkStatus.GetValueAt(iStatusNo,pStatus))
		{
			pStatus->feature=TRUE;
			nCaseCount++;
		}
		iStatusNo=FindAdaptNo(NULL);
	}
	if(pCmdLine)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		promptStr.Format("working conditions %d",nCaseCount);
#else
		promptStr.Format("工况数%d",nCaseCount);
#endif
		pCmdLine->PromptStrInCmdLine(promptStr);
	}

	THANDLE bak_module_handle=Ta.m_hActiveModule;
	THANDLE bak_workstatus_handle=Ta.m_hActiveStatus;
	//以下是对角钢规格表进行排序
	CPartSizeOptimizor sizeOptimizor;
	sizeOptimizor.InitPartSizeOrder();
	CBlockResolver* pSubBlkResolver=NULL;
	CFemResolver *pResolver=NULL;
	if(method==SUB_BLOCK_METHOD)
		pSubBlkResolver=CBlockResolver::CreateResolver();
	else
		pResolver=CFemResolver::CreateResolver();
	bool result=false;
	try
	{
#if defined(__TSA_)||defined(__LDS_)
		if(g_sysPara.bCreateAnsysCmdFile||g_sysPara.bCreateDetailRptFile)
			theApp.GetWorkPathAuto();	//自动获取当前文件计算结果的输出目录
#endif
		CLDSLinePart *pPole;
		for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
		{
			pPole->feature=0;		//还未配材
			pPole->pCtrlElem=NULL;	//复原选材控制规则
		}
		double epsilon=0;	//选材残差值
		long iSelCount=1;
		//bool bVerifyFoundation=false;
		bool bExit=false;
		if(pResolver)
			preprocessor.PrepareTower(&Ta,analScope.cfgwordBody,analScope.cfgwordLeg);
		else if(pSubBlkResolver)
			preprocessor.PrepareTowerSubBlock(pSubBlkResolver,&Ta,analScope.cfgwordBody,analScope.cfgwordLeg);
		preprocessor.SetAnalysisProcessor(&analysis);
		analysis.SetPostProcessor(&postprocessor);
		char filename[MAX_PATH];
		theApp.GetFileName(filename);
		postreport.InitProcTempFile(CXhChar200("%s",filename));
		if(analScope.m_iCalMode==CCfgCalScopeDlg::UNI_CAL)
		{	//继承优选（单向增大选材）
			for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
			{
				if((pPole->iInitialSizeIndex=sizeOptimizor.GetPoleSizeOrderIndex(pPole))<0)
				{	//规格库中未找到该规格,可能无法正确单向继承该杆件的原有规格 wjh-2017.6.20
					pPole->xInheritSize._ciThick=(BYTE)ftoi(pPole->size_thick);
					pPole->xInheritSize._wiWidth=(WORD)ftoi(pPole->size_wide);
					pPole->xInheritSize._cSubClsType=pPole->size_cSubClassType;
					pPole->bSpecInheritSizeDetails=true;
				}
			}
		}
		while(true)
		{
			postprocessor.Empty();	//清空后处理器
			Ta.m_iIteration=iSelCount;
			postreport.ClearAnalInfoBuffer();
			for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
				pPole->feature=0;
			/*
			if(bVerifyFoundation)
			{
				if(pCmdLine)
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->PromptStrInCmdLine("In view of foundation design, carry out rod stress analysis computation!");
#else
					pCmdLine->PromptStrInCmdLine("针对基础设计，进行杆件应力分析运算!");
#endif
			}
			else
			*/
			{
#ifdef AFX_TARG_ENU_ENGLISH
				promptStr.Format("%d rod stress analysis computation!",iSelCount);
#else
				promptStr.Format("进行第%d次杆件应力分析运算!",iSelCount);
#endif
				if(pCmdLine)
					pCmdLine->PromptStrInCmdLine(promptStr);
				char step[MAX_TEMP_CHAR_50+1];
				_snprintf(step,MAX_TEMP_CHAR_50,"Step@%d",iSelCount);
				_mkdir(step);
			}

			preprocessor.UpdateInfoAtElemSizeChange();
			//每个选材循环都要清空原有单元最大拉力、压力 wjh-2010.11.13
			for(CElement* pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
				pElem->MaxCompression.N=pElem->MaxTension.N=0;

			int iCalUnit=0;
			for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			{
				if(pModule->feature==0)	//处于睡眠状态现不处理此计算模型
					continue;
				if(pSubBlkResolver)
					pSubBlkResolver->ClearNodeLoadBuffer();
				BOOL bPushModule=Ta.Module.push_stack();
				Ta.m_hActiveModule=pModule->handle;
				//计算当前呼高模型本体部分重量
				long key=pModule->GetBodyNo()*10;
				TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetValue(key);
				if(pWeight==NULL)
				{
					pWeight=postprocessor.TowerWeight.Add(key);
					pWeight->weight=pWeight->netWeight=0.0;
					pWeight->cfg_no =pModule->GetBodyNo();
					pWeight->iBody0Leg1 = 0;	//本体部分
					for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
					{
#ifndef __TSA_
						if(pPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pPole)->group_father_jg_h>0x20)
							continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
						if(pModule->IsSonPart(pPole)&&!pPole->IsLegObj())
						{
							double weight=pPole->GetWeight(FALSE);
							pWeight->weight+=weight*pPole->m_fWeightRaiseCoef;
							pWeight->netWeight+=weight;
						}
					}
				}
				CFGWORD legword;	//暂用配材号代替存储各象位接腿号
				ARRAY_LIST<WORD>legArr;
				ARRAY_LIST<CFGWORD>arrCalModule;
				legArr.SetSize(0,10);
				arrCalModule.SetSize(0,10);
				int i;
				if(g_sysPara.iLegShiftType==0)
				{	//不进行接腿轮换
					for(i=0;i<4;i++)
						legword.flag.word[i]=pModule->m_arrActiveQuadLegNo[i];
					arrCalModule.append(legword);
				}
				else
				{	//需要进行接腿轮换
					for(i=0;i<192;i++)
					{
						if(!pModule->m_dwLegCfgWord.IsHasNo(i+1))
							continue;
						if(g_sysPara.iLegShiftType==1)
						{	//只进行等高腿轮换
							legword.flag.word[0]=legword.flag.word[1]=legword.flag.word[2]=legword.flag.word[3]=i+1;
							arrCalModule.append(legword);
						}
						else
							legArr.append((WORD)i+1);
					}
					if(g_sysPara.iLegShiftType==2)
					{	//进行高低腿轮换计算
						for(i=0;i<legArr.GetSize();i++)
						{
							legword.flag.word[0]=legArr[i];
							for(int j=0;j<legArr.GetSize();j++)
							{
								legword.flag.word[1]=legArr[j];
								for(int k=0;k<legArr.GetSize();k++)
								{
									legword.flag.word[2]=legArr[k];
									for(int m=0;m<legArr.GetSize();m++)
									{
										legword.flag.word[3]=legArr[m];
										arrCalModule.append(legword);
									}
								}
							}
						}
					}
				}
				preprocessor.UpdateInfoAtModuleChange(pModule);
				if(pSubBlkResolver)
				{
					pSubBlkResolver->ResetStructure();
					CModuleFrame* pFrame = preprocessor.result.hashModuleFrames.GetValue(pModule->iNo);
					for(SUBBLOCKREF* pBlkRef=pFrame->blkrefs.GetFirst();pBlkRef;pBlkRef=pFrame->blkrefs.GetNext())
						pSubBlkResolver->AddSubBlockRef(pBlkRef->serial,pBlkRef->acs);
					pSubBlkResolver->SetConnectFemNodeFunc(ConnectFemNode);
					pSubBlkResolver->SetConnectFemElementFunc(ConnectFemElement);
				}
				postprocessor.Reaction.SetHashTableGrowSize(nModuleCount*arrCalModule.GetSize()*nCaseCount*4,false);
				for(i=0;i<arrCalModule.GetSize();i++)
				{
					WORD legQuadBak[4];
					memcpy(legQuadBak,pModule->m_arrActiveQuadLegNo,8);
					int iLeg;
					for(iLeg=0;iLeg<4;iLeg++)
						pModule->m_arrActiveQuadLegNo[iLeg]=(WORD)arrCalModule[i].flag.word[iLeg];
					preprocessor.RetrieveFemAnalInfo(pModule);
					preprocessor.UpdateInfoAtLegChange(pModule,pSubBlkResolver);
					if(pSubBlkResolver)
						pSubBlkResolver->ConnectFemNodeAndElement(&analysis);
					//计算当前呼高中各腿的重量
					for(iLeg=0;iLeg<4;iLeg++)
					{
						key=pModule->m_arrActiveQuadLegNo[iLeg]*10+1;
						TOWERWEIGHT *pWeight=postprocessor.TowerWeight.GetValue(key);
						if(pWeight==NULL)
						{
							pWeight=postprocessor.TowerWeight.Add(key);
							pWeight->netWeight=pWeight->weight=0.0;
							pWeight->cfg_no = pModule->m_arrActiveQuadLegNo[iLeg];
							pWeight->iBody0Leg1 = 1;	//接腿
							for(pPole=Ta.Parts.GetFirstLinePart();pPole;pPole=Ta.Parts.GetNextLinePart())
							{
#ifndef __TSA_
								if(pPole->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pPole)->group_father_jg_h>0x20)
									continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
								if(pPole->layer(0)=='L'&&pPole->cfgword.IsHasNo(pWeight->cfg_no))
								{
									double weight=pPole->GetWeight(FALSE);
									pWeight->netWeight+=weight;
									pWeight->weight+=weight*pPole->m_fWeightRaiseCoef;
								}
							}
							pWeight->weight*=0.25;	//表示1/4组腿重
							pWeight->netWeight*=0.25;
						}
					}
					bool bFirstResolve=true;	//首次计算框架时需要初始化信息,之后只需继承计算即可
					for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
					{
						if(pStatus->feature==0)	//处于睡眠状态现不处理
							continue;
						if(pStatus->m_cMainLoadType>=5&&pStatus->m_cMainLoadType<=8)	//抗震验算荷载
						//if(!IsNoInLimStr(pModule->iNo,pStatus->limit_str))
							continue;	//当前工况不适用于当前计算呼高模型
						BOOL bPushStatus=Ta.WorkStatus.push_stack();
						Ta.m_hActiveStatus=pStatus->handle;
						int slopes=max(1,preprocessor.GetEiffelSlopes(true,pStatus));
						int eiffel_cases=(slopes-1)*2+1;
						for(int eiffel=0;eiffel<eiffel_cases;eiffel++)
						{
							//清空风压数据，重新计算
							for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
							{
								pNode->WindLoad.Set();
								pNode->AutoCalLoad.Set();
							}
							pStatus->SetEiffelCaseNo(eiffel);
							preprocessor.UpdateInfoAtStatusChange(pModule,pStatus,false,false);
							TRACE("%d-%d analyzing\n",iSelCount,pStatus->iNo);
							if(pSubBlkResolver)
								pSubBlkResolver->ApplyNodeLoad(pStatus->iNo);
							BOOL analyzed=FALSE;
							if(pSubBlkResolver)
								analyzed=truss3d_analysis(pSubBlkResolver,&analysis,method,false,false);
							else if(pResolver)
								analyzed=truss3d_analysis(pResolver,&analysis,method,false,!bFirstResolve);
							if(!analyzed)
							{
								if(bPushModule)
									Ta.Module.pop_stack();
								if(bPushStatus)
									Ta.WorkStatus.pop_stack();
#ifdef AFX_TARG_ENU_ENGLISH
								throw CString("A node truss system is variable geometry, transient system or lack of enough restraints, stress analysis cannot be done!");
#else
								throw CString("某节点处桁架体系为几何可变、瞬变体系或缺少足够的约束,无法进行应力分析!");
#endif
							}
						}
						bFirstResolve=false;
						TRACE("%d-%d analyzed\n",iSelCount,pStatus->iNo);
						if(bPushStatus)
							Ta.WorkStatus.pop_stack();
					}
					//if(!bVerifyFoundation)
						analysis.UpdateInfoAtModuleChange(pModule);
					memcpy(pModule->m_arrActiveQuadLegNo,legQuadBak,8);
					iCalUnit++;
#ifdef AFX_TARG_ENU_ENGLISH
					CXhChar100 prompt("has finished the %2d material selection %5d#%2d module %2C-%2C-%2C-%2 Cleg'sMechanical Analysis \n",iSelCount,iCalUnit,pModule->iNo,
#else
					CXhChar100 prompt("已完成第%2d选材%5d#%2d号呼高%2C-%2C-%2C-%2C腿的力学分析\n",iSelCount,iCalUnit,pModule->iNo,
#endif
						(arrCalModule[i].flag.word[0]-1)%8+'A',(arrCalModule[i].flag.word[1]-1)%8+'A',
						(arrCalModule[i].flag.word[2]-1)%8+'A',(arrCalModule[i].flag.word[3]-1)%8+'A');
					DisplayPrompt(prompt);
					if(IsCanceledPromptDlg())
#ifdef AFX_TARG_ENU_ENGLISH
						throw CString("uses terminate computation!");
#else
						throw CString("用户中断计算过程!");
#endif
				}
				if(bPushModule)
					Ta.Module.pop_stack();
			}
			//if(!bVerifyFoundation)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				promptStr.Format("carrying on the %d initial material selection...",iSelCount);
#else
				promptStr.Format("正在进行第%d次初步选材...",iSelCount);
#endif
				if(pCmdLine)
					pCmdLine->PromptStrInCmdLine(promptStr);
				ATOM_LIST<SIZE_WTM> guigelist;
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					SIZE_WTM guige;
					guige.Set(pPole->cMaterial,pPole->GetWidth(),pPole->GetThick(),pPole->size_idClassType,pPole->size_cSubClassType);
					guigelist.append(guige);
					if(pPole->m_bNeedSelSize)
					{
						pPole->iSizeIndex=0;
						//由于选材前杆件规格要在计算螺栓减孔数时用到，
						//所以此处只能将索引清零而不能将规格清为最小 wjh-2010.11.13
						//sizeOptimizor.UpdatePoleSize(pPole,0);
					}
				}
				for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
				{
					pElem->m_iCtrlRule=0;	//清空控制条件
					BOOL bPush=postprocessor.hashElems.push_stack();
					if(!pElem->pOrgPole->m_bNeedSelSize||pElem->pOrgPole->size_idClassType==CLS_CABLE)
					{	//不需要重新选材
						MemberCheck(pElem,TRUE,Ta.m_iMemberSizeCheckCode);
						continue;
					}
					else
					{	//需要选材
						SIZESPEC_SELECT sizeSelect;
						BOOL bExecuteDichotomy=TRUE;
						if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
						{
							double wide,thick;
							if(pElem->pOrgPole->layer(1)=='Z')
								restore_JG_guige(Ta.SelMatPara.m_csMinMainAngleSize,wide,thick);
							else if(pElem->pOrgPole->layer(1)!='F')
								restore_JG_guige(Ta.SelMatPara.m_csMinForceAngleSize,wide,thick);
							else
								restore_JG_guige(Ta.SelMatPara.m_csMinAngleSize,wide,thick);
							
							//JG_PARA_TYPE *pNewType=FindJgType(wide,thick);
							//sizeOptimizor.m_fMinSizeArea=pNewType->area;
							//上面两行代码可能会出现当前规格库中无设定最小面积规格，导致死机 wjh-2010.11.14
							sizeOptimizor.m_fMinSizeArea=globalLibrary.GetPartArea(wide,thick,0,CLS_LINEANGLE)/100;
							bExecuteDichotomy=FALSE;	//角钢规格一般较少可不必二分法快速选材
						}
						else
							sizeOptimizor.m_fMinSizeArea=0;
						sizeOptimizor.QuickSelectElemSize(&sizeSelect,pElem,Ta.m_iMemberSizeCheckCode,bExecuteDichotomy);
						if(sizeSelect.orderI>pElem->pOrgPole->iSizeIndex)
						{
							pElem->pOrgPole->pCtrlElem=pElem;
							pElem->pOrgPole->iSizeIndex=sizeSelect.orderI;
							pElem->pOrgPole->size_wide=sizeSelect.wide;
							pElem->pOrgPole->size_thick=sizeSelect.thick;
							pElem->pOrgPole->size_height=sizeSelect.height;
							pElem->pOrgPole->size_idClassType=sizeSelect.idClassType;
							pElem->pOrgPole->size_cSubClassType=sizeSelect.cSubClassType;
						}
					}
					if(bPush)
						postprocessor.hashElems.pop_stack();
				}
				if(pCmdLine)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->PromptStrInCmdLine("finished!");
					pCmdLine->FillCmdLine("processing BOM Number...","");
#else
					pCmdLine->PromptStrInCmdLine("完毕！");
					pCmdLine->FillCmdLine("正在处理统材编号...","");
#endif
				}
				//统一同统材号及关联对称构件的规格
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					/* 仔细查验代码后，感觉以下这段代码是画蛇添足应该去掉　wjh-2015.3.5
					if(g_sysPara.bMergeRptFileMirObj)
					{
						CElement **ppSizeRelaElem=postprocessor.hashRodRelaElem.GetValue(pPole->handle);
						if(ppSizeRelaElem&&(*ppSizeRelaElem)->pOrgPole!=pPole)
						{	//更新对称关联的杆件规格
							pPole->iSizeIndex=(*ppSizeRelaElem)->pOrgPole->iSizeIndex;
							pPole->SetSizeSpec((*ppSizeRelaElem)->pOrgPole->GetSizeSpec());
						}
					}
					*/
					//上面清零规格索引时为了验算时计算减孔数需要保留旧有规格，在此需要根据规格索引更新实际规格　wjh-2013.1.08
					if(pPole->m_bNeedSelSize)
						sizeOptimizor.UpdatePoleSize(pPole,pPole->iSizeIndex);
					pPole->feature=pPole->feature%10;	//清除已处理标识,个位数可能另有它用
				}
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					if(!pPole->m_bNeedSelSize)
						continue;
					preprocessor.result.allrodset.push_stack();
					LINEPARTSET poleset;
					poleset.append(pPole);
					if(analScope.m_iCalMode==CCfgCalScopeDlg::UNI_CAL)
					{	//继承优选（单向增大选材）
						if(pPole->iSizeIndex<pPole->iInitialSizeIndex&&pPole->iInitialSizeIndex>0)
						{
							pPole->iSizeIndex=pPole->iInitialSizeIndex;
							sizeOptimizor.UpdatePoleSize(pPole,pPole->iSizeIndex);
						}
						else if(pPole->iInitialSizeIndex<0)
						{	//按面积排序
							CLDSLinePart rod;
							rod.size_idClassType=pPole->size_idClassType;
							sizeOptimizor.UpdatePoleSize(pPole,pPole->iSizeIndex);
							bool inherited=false;
							if(Ta.SelMatPara.m_iSizeOrderRule==0)
							{
								double area=0;
								if(CLDSPart::library)
									area=CLDSPart::library->GetPartArea(pPole->wiInheritWidth,pPole->ciInheritThick,0,pPole->size_idClassType,pPole->cInheritSubClsType);
								if(rod.GetArea()<area)
								{
									pPole->size_wide=pPole->wiInheritWidth;
									pPole->size_thick=pPole->ciInheritThick;
									pPole->size_cSubClassType=pPole->cInheritSubClsType;
									inherited=true;
								}
							}
							else if(Ta.SelMatPara.m_iSizeOrderRule==1)
							{	//按回转半径(也即肢宽)排序规格库
								if(rod.size_wide<pPole->wiInheritWidth)
								{
									pPole->size_wide=pPole->wiInheritWidth;
									pPole->size_thick=pPole->ciInheritThick;
									pPole->size_cSubClassType=pPole->cInheritSubClsType;
									inherited=true;
								}
							}
							else if(Ta.SelMatPara.m_iSizeOrderRule==2)
							{	//按肢宽肢厚排序规格库
								if( rod.size_wide<pPole->wiInheritWidth||(
									rod.size_wide==pPole->wiInheritWidth&&rod.size_thick<pPole->ciInheritThick))
								{
									pPole->size_wide=pPole->wiInheritWidth;
									pPole->size_thick=pPole->ciInheritThick;
									pPole->size_cSubClassType=pPole->cInheritSubClsType;
									inherited=true;
								}
							}
							if(!inherited)
								logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d-%d杆件的原始规格未在规格库中找到,无法单向继承,系统进行了重新优选",pPole->pStart->pointI,pPole->pEnd->pointI);
						}
					}
					int index=pPole->iSizeIndex;
					for(CLDSLinePart *pOthPole=preprocessor.result.allrodset.GetNext();pOthPole;pOthPole=preprocessor.result.allrodset.GetNext())
					{
						if(pOthPole->feature/10>0)
							continue;	//表示已处理
						if( (pOthPole->m_uStatMatNo==pPole->m_uStatMatNo&&pPole->m_uStatMatNo!=0)||
							(pPole->m_uStatMatNo==0&&pPole->GetRelativeObjInfo(pOthPole->handle,false)))
						{
							if(index<pOthPole->iSizeIndex)
								index=pOthPole->iSizeIndex;
							poleset.append(pOthPole);
						}
					}
					for(pOthPole=poleset.GetFirst();pOthPole;pOthPole=poleset.GetNext())
					{
						pOthPole->feature+=10;	//表示已处理
						pOthPole->iSizeIndex=index;
						if(pOthPole->m_bNeedSelSize)
							sizeOptimizor.UpdatePoleSize(pOthPole,index);
						pOthPole->pCtrlElem=NULL;
					}
					preprocessor.result.allrodset.pop_stack();
				}
				//更新螺栓
				for(pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
				{
					if(pElem->pStart!=pElem->pOrgPole->pStart&&pElem->pEnd!=pElem->pOrgPole->pEnd)
						continue;	//不是两端单元不用重新计算螺栓
					if(pElem->m_iSizeIndex!=pElem->pOrgPole->iSizeIndex)
					{
						sizeOptimizor.UpdateElemSize(pElem,pElem->pOrgPole->iSizeIndex);
						CalElemRelaBoltInfo(pElem);
					}
				}
				//更新螺栓选择
				/* 仔细查验代码后，感觉以下这段代码是画蛇添足应该去掉,也有可能以前出现过
				　 对称杆件螺栓信息不同的情况才加此代码　wjh-2015.3.5
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					if(g_sysPara.bMergeRptFileMirObj)
					{
						CElement **ppSizeRelaElem=postprocessor.hashRodRelaElem.GetValue(pPole->handle);
						if(ppSizeRelaElem&&(*ppSizeRelaElem)->pOrgPole!=pPole)
						{	//更新对称关联的杆件规格
							pPole->connectStart=(*ppSizeRelaElem)->pOrgPole->connectStart;
							pPole->connectEnd=(*ppSizeRelaElem)->pOrgPole->connectEnd;
							//减孔数一起更新,否则可能因只对合并后的父单元进行减孔计算,导致一些对称杆件减孔数显示值不匹配 wjh-2013.2.22
							pPole->hole_out=(*ppSizeRelaElem)->pOrgPole->hole_out;
						}
					}
					pPole->feature=pPole->feature%10;	//清除已处理标识,个位数可能另有它用
				}
				*/
				//统一螺栓选材
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					preprocessor.result.allrodset.push_stack();
					LINEPARTSET poleset;
					poleset.append(pPole);
					CConnectInfo group_start_ls,group_end_ls;
					group_start_ls = pPole->connectStart;
					group_end_ls = pPole->connectEnd;
					for(CLDSLinePart *pOthPole=preprocessor.result.allrodset.GetNext();pOthPole;pOthPole=preprocessor.result.allrodset.GetNext())
					{
						if(pOthPole->feature/10>0)
							continue;	//表示已处理
						if((pPole->m_uStatMatNo!=0&&pOthPole->m_uStatMatNo==pPole->m_uStatMatNo)||
						   (pPole->m_uStatMatNo==0&&pPole->GetRelativeObjInfo(pOthPole->handle,false)))
						{
							poleset.append(pPole);
							if(group_start_ls.wnSummBoltN<pOthPole->connectStart.wnSummBoltN)
								group_start_ls=pOthPole->connectStart;
							if(group_end_ls.wnSummBoltN<pOthPole->connectEnd.wnSummBoltN)
								group_end_ls=pOthPole->connectEnd;
						}
					}
					for(pOthPole=poleset.GetFirst();pOthPole;pOthPole=poleset.GetNext())
					{
						pOthPole->feature+=10;	//表示已处理
						pOthPole->connectStart=group_start_ls;
						pOthPole->connectEnd=group_end_ls;
					}
					preprocessor.result.allrodset.pop_stack();
				}
				if(pCmdLine)
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->PromptStrInCmdLine("processing BOM Number..., completed!");
#else
					pCmdLine->PromptStrInCmdLine("正在处理统材编号..., 完毕！");
#endif
				
				//epsilon反映收敛程度
				epsilon=0;
				CString prompt;
				SIZE_WTM* pGuige=guigelist.GetFirst();
				for(pPole=preprocessor.result.allrodset.GetFirst();pPole;pPole=preprocessor.result.allrodset.GetNext())
				{
					double old_wing_wide=pGuige->width;
					double old_wing_thick=pGuige->thick;
					double new_wing_wide=pPole->GetWidth();
					double new_wing_thick=pPole->GetThick();
					if(pGuige->idClsType==CLS_GROUPLINEANGLE)
					{
						if(pPole->size_cSubClassType=='X')
							old_wing_wide*=4;
						else
							old_wing_wide*=2;
					}
					if(pPole->size_idClassType==CLS_GROUPLINEANGLE)
					{
						if(pPole->size_cSubClassType=='X')
							new_wing_wide*=4;
						else
							new_wing_wide*=2;
					}
					double diff=fabs(old_wing_wide-new_wing_wide)+fabs(old_wing_thick-new_wing_thick);
					epsilon+=diff;
					if(diff>0)
					{
						char s_b[3]="",s_a[3]="";
						if(pGuige->idClsType==CLS_GROUPLINEANGLE)
							sprintf(s_b,"%CL",pGuige->cSubClassType);
						else if(pGuige->idClsType==CLS_LINEANGLE)
							strcpy(s_b,"L");
						if(pPole->size_idClassType==CLS_GROUPLINEANGLE)
							sprintf(s_a,"%CL",pPole->size_cSubClassType);
						else if(pPole->size_idClassType==CLS_LINEANGLE)
							strcpy(s_a,"L");
#ifdef AFX_TARG_ENU_ENGLISH
						prompt.Format("%d-%d rods specification %s%.0fX%.0f->%s%.0fX%.0f",
#else
						prompt.Format("%d-%d杆件规格%s%.0fX%.0f->%s%.0fX%.0f",
#endif
							pPole->pStart->point_i,pPole->pEnd->point_i,
							s_b,pGuige->width,pGuige->thick,s_a,pPole->size_wide,pPole->size_thick);
						if(pCmdLine)
							pCmdLine->PromptStrInCmdLine(prompt);
					}
					pGuige=guigelist.GetNext();
				}
#ifdef AFX_TARG_ENU_ENGLISH
				prompt.Format("convergence degree of residual %.0f",epsilon);
#else
				prompt.Format("收敛残差度%.0f",epsilon);
#endif
				if(pCmdLine)
					pCmdLine->PromptStrInCmdLine(prompt);
			}
			iSelCount++;
			if(epsilon<Ta.SelMatPara.m_nConvergeLim||iSelCount>Ta.SelMatPara.m_nMaxSelMatCount)
			{	//更新单元验算信息，结束优化选材
				for(CElement *pElem=postprocessor.hashElems.GetFirst();pElem;pElem=postprocessor.hashElems.GetNext())
				{
					sizeOptimizor.UpdateElemSize(pElem,pElem->pOrgPole->iSizeIndex);
					if(MemberCheck(pElem,TRUE,Ta.m_iMemberSizeCheckCode)==6)
					{	//螺栓强度不满足(可能由于统材问题导致肢厚降低,进而导致螺栓强度不满足)
						CalElemRelaBoltInfo(pElem);
						pElem->m_iCtrlRule=0;
					}
				}
				if(g_sysPara.bUpdateRodDesignForce)
					analysis.UpdateElemRelaRodDesignForce();
#if defined(__LDS_)||defined(__TSA_)
				CFLSizeOptimizor flOpt;
				//生成法兰匹配对
				CJointCouple::StatJointLinkPart(&flOpt.listCouple,CLS_LINETUBE);
				//生成法兰
				flOpt.GenFLSet();
#endif
				break;
			}
		}
		if(epsilon<Ta.SelMatPara.m_nConvergeLim)
		{
			if(pCmdLine)
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->PromptStrInCmdLine("Selection process is convergent!");
#else
				pCmdLine->PromptStrInCmdLine("选材过程已收敛！");
#endif
		}
		else// if(iSelCount<Ta.SelMatPara.m_nMaxSelMatCount)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			promptStr.Format("beyond the maximum times for material selection(%d times)",Ta.SelMatPara.m_nMaxSelMatCount);
#else
			promptStr.Format("超出选材最高次数限制(%d次)",Ta.SelMatPara.m_nMaxSelMatCount);
#endif
			if(pCmdLine)
				pCmdLine->PromptStrInCmdLine(promptStr);
		}
		DisplayPrompt(NULL);
		postreport.CreateProcessReport();
		result=true;
	}
	catch(CString sError)
	{ 
		//PrintPrompt(this,FALSE,"中途退出！\n");;
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	catch(CXhChar100 sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	catch(char* sError)
	{ 
		//PrintPrompt(this,FALSE,"中途退出！\n");;
		AfxMessageBox(sError);
		DisplayProcess(100);
	}
	if(pResolver)
		CFemResolver::Destroy(pResolver->GetHandle());
	else if(pSubBlkResolver)
		CBlockResolver::Destroy(pSubBlkResolver->GetSerial());
	DisplayPrompt(NULL);
	Ta.m_hActiveModule=bak_module_handle;
	Ta.m_hActiveStatus=bak_workstatus_handle;
	return result;
}

void CMainFrame::ModelOptimize()
{
	//结果文件目录生成
	CXhChar200 file_path, file_name;
	CString ss=theApp.GetLDSDoc()->GetPathName();
	file_path.Printf("%s",ss);
	GetSysPath(file_path,file_path,file_name);
	_chdir(file_path);
	_mkdir(file_name);
	_chdir(file_name);
	CCmdLineDlg *pCmdLine=((CMainFrame*)theApp.m_pMainWnd)->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("checking nodes Number...","");
	if(!CheckNodeI())
	{
		pCmdLine->PromptStrInCmdLine("checking nodes Number...,not pass the check!");
		return;
	}
	pCmdLine->PromptStrInCmdLine("checking nodes Number...,passed!");
#else
	pCmdLine->FillCmdLine("进行节点编号检查...","");
	if(!CheckNodeI(&m_xAnalyzeScope.cfgwordBody,&m_xAnalyzeScope.cfgwordLeg))
	{
		pCmdLine->PromptStrInCmdLine("进行节点编号检查...，未通过节点编号检查！");
		return;
	}
	pCmdLine->PromptStrInCmdLine("进行节点编号检查...，通过！");
#endif
	if(ModelOptimizeCore(m_xAnalyzeScope,pCmdLine))
	{	//默认生成中文文本满应力报告书文件
		file_path.Append(file_name);
		file_path.Append("_worktemp\\");
		file_path.Append(file_name);
		CreateDetailRptTxtFileByCN(file_path,m_xAnalyzeScope.cfgwordBody,m_xAnalyzeScope.cfgwordLeg,0);
		CRevisionOperDlg *pRevision=GetRevisionOperPage();
		pRevision->UpdateRevision(postreport);
	}
}

BOOL freqs_analysis(CFemResolver* pResolver,int nFreqsRank);
void CMainFrame::ModelFreqsAnalyze(int iElemProcessMode/*=0*/)
{
	CCmdLineDlg *pCmdLine=this->GetCmdLinePage();
	postprocessor.Empty();	//清空以前所有的杆件单元记录
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->PromptStrInCmdLine("checking nodes Number...");
	if(!CheckNodeI())
	{
		pCmdLine->PromptStrInCmdLine("not pass checking nodes Number!\n");
		return;
	}
	pCmdLine->PromptStrInCmdLine("passed!\n");
#else
	pCmdLine->PromptStrInCmdLine("进行节点编号检查...");
	if(!CheckNodeI())
	{
		pCmdLine->PromptStrInCmdLine("未通过节点编号检查！\n");
		return;
	}
	pCmdLine->PromptStrInCmdLine("通过！\n");
#endif
	int nCount=0;
	CFGWORD cfgwordBody,cfgwordLeg;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		if(pModule->feature!=0)
		{
			//cfgwordBody.flag.bytes[pModule->GetBodyNo()-1]=0xff;
			cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
			cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
			nCount++;
		}
	}
	pCmdLine->PromptStrInCmdLine("进行节点编号检查...");
	if(!CheckNodeI(&cfgwordBody,&cfgwordLeg))
	{
		pCmdLine->PromptStrInCmdLine("未通过节点编号检查！\n");
		return;
	}
	pCmdLine->PromptStrInCmdLine("通过！\n");

	THANDLE bak_module_handle=Ta.m_hActiveModule;
	THANDLE bak_workstatus_handle=Ta.m_hActiveStatus;
	CFemResolver *pResolver=CFemResolver::CreateResolver();
	try
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("rod stress analysis and computation!\n");
#else
		pCmdLine->PromptStrInCmdLine("进行杆件应力分析运算!\n");
#endif
		preprocessor.PrepareTower(&Ta,cfgwordBody,cfgwordLeg,2);
		preprocessor.SetAnalysisProcessor(&analysis);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->feature<=0)	//处于睡眠状态现不处理此计算模型
				continue;
			Ta.Module.push_stack();
			Ta.m_hActiveModule=pModule->handle;
			preprocessor.PreCalFreqNodeAllocAddedStruMass(pModule);
			preprocessor.RetrieveFemAnalInfo(pModule);
			analysis.AnalyzeElemDegree();
			if (iElemProcessMode == 1)
			{	//全部按梁单元处理
				for (FEM_ELEMENT* pFemElem = analysis.femElemList.GetFirst(); pFemElem != NULL; pFemElem = analysis.femElemList.GetNext())
				{
					pFemElem->start_joint_type = pFemElem->end_joint_type = JOINT_RIGID;
					pFemElem->pStart->dimension = pFemElem->pEnd->dimension = 6;
				}
			}
			Ta.Module.pop_stack();
			if(!freqs_analysis(pResolver,Ta.EarthQuake.nFreqsRank))
#ifdef AFX_TARG_ENU_ENGLISH
				throw CString("Dynamic Analysis failed!");
		}
		pCmdLine->PromptStrInCmdLine("tower Dynamic Analysis completed!");
		MessageBox("tower Dynamic Analysis completed!");
#else
				throw CString("动力特性分析失败！");
		}
		pCmdLine->PromptStrInCmdLine("整塔的动力特性分析完毕！");
		MessageBox("整塔的动力特性分析完毕！",theApp.m_pszAppName);
#endif
	}
	catch(CXhChar100 sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("quit halfway!\n");;
#else
		pCmdLine->PromptStrInCmdLine("中途退出！\n");;
#endif
		AfxMessageBox(sError);
	}
	catch(CString sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("quit halfway!\n");;
#else
		pCmdLine->PromptStrInCmdLine("中途退出！\n");;
#endif
		AfxMessageBox(sError);
	}
	CFemResolver::Destroy(pResolver->GetHandle());
	Ta.m_hActiveModule=bak_module_handle;
}

void CMainFrame::OnModelFreqs() 
{
	CLDSModule* pModule,*pHighestModule=NULL;
	double dfMaxHeight = 0;
	CXhChar200  szHeightList;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		szHeightList.Append(CXhChar16("%d",pModule->idBodyNo),',');
		double height= pModule->GetModuleScopeZ();
		if (pHighestModule == NULL)
		{
			pHighestModule = pModule;
			dfMaxHeight = height;
		}
		else if (height > dfMaxHeight)
		{
			pHighestModule = pModule;
			dfMaxHeight = height;
		}
	}
	static CFreqAnalyzeDlg freqcaldlg;
	freqcaldlg.m_sModuleScope=szHeightList;
	freqcaldlg.m_sHeightNo.Format("%d", pHighestModule->idBodyNo);
	freqcaldlg.m_bAreaAmplifyCoefIncBraceRods = Ta.m_bAreaAmplifyCoefIncBraceRods;
	freqcaldlg.m_bWeightAmplifyCoefIncBraceRods = Ta.m_bWeightAmplifyCoefIncBraceRods;
	if(freqcaldlg.m_nFreqOrderCount==0)
		freqcaldlg.m_nFreqOrderCount=Ta.EarthQuake.nFreqsRank;
	if(freqcaldlg.DoModal()==IDOK)
	{
		Ta.m_bAreaAmplifyCoefIncBraceRods = (freqcaldlg.m_bAreaAmplifyCoefIncBraceRods!=FALSE);
		Ta.m_bWeightAmplifyCoefIncBraceRods = (freqcaldlg.m_bWeightAmplifyCoefIncBraceRods != FALSE);
		int iModuleNo=atoi(freqcaldlg.m_sHeightNo);
		for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
			pModule->feature= (pModule->iNo == iModuleNo)?TRUE:FALSE;
		CStackVariant<int> stackRank(&Ta.EarthQuake.nFreqsRank, freqcaldlg.m_nFreqOrderCount);
		ModelFreqsAnalyze(freqcaldlg.m_iElemProcessMode);
	}
}
void CMainFrame::OnModelDesignCal()
{
	ModelDesignCal(0);
}
void RetrieveCfgWordFromModuleScopeStr(const char* bodyStr,CFGWORD* pCfgBody,CFGWORD* pCfgLeg)
{
	pCfgBody->Clear();
	pCfgLeg->Clear();
	int iModuleNo=FindAdaptNo(bodyStr);
	while(iModuleNo!=0)
	{
		for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->iNo!=iModuleNo)
				continue;
			pCfgBody->SetBodyLegs(pModule->GetBodyNo());
			pCfgLeg->AddSpecWord(pModule->m_dwLegCfgWord);
		}
		iModuleNo=FindAdaptNo(NULL);
	}
}
void CMainFrame::ModelDesignCal(int iSelModuleNo/* = 0*/,bool bEqCheck/*=false*/) 
{
	if(!bEqCheck)
	{
		static CCfgCalScopeDlg anal_scope_dlg;
		if(anal_scope_dlg.DoModal()!=IDOK)
			return;
		RetrieveCfgWordFromModuleScopeStr(anal_scope_dlg.m_sModuleScope,&m_xAnalyzeScope.cfgwordBody,&m_xAnalyzeScope.cfgwordLeg);
		m_xAnalyzeScope.csModule.Copy(anal_scope_dlg.m_sModuleScope);
		m_xAnalyzeScope.csStatus.Copy(anal_scope_dlg.m_sWorkStatusScope);
		m_xAnalyzeScope.m_iCalMode=anal_scope_dlg.m_iCalMode;
		m_xAnalyzeScope.m_bUpdateBoltInfo=anal_scope_dlg.m_bUpdateBoltInfo;
		m_xAnalyzeScope.m_bVerifyFoundationMode=!m_xAnalyzeScope.m_bUpdateBoltInfo&&anal_scope_dlg.m_bVerifyFoundationMode;
		m_xAnalyzeScope.m_bVerifyFoundationByStdLoad=anal_scope_dlg.m_iFoundationByStdLoad0==0;
		if(m_xAnalyzeScope.m_iCalMode<=CCfgCalScopeDlg::UNI_CAL)	//优化选材
			ModelOptimize();
		else
			ModelCheck();
	}
	else //if(bEqCheck)
	{
		static CEqChkScopeDlg eq_scope_dlg;
		if(iSelModuleNo>0)
			eq_scope_dlg.m_iSelModelNo=iSelModuleNo;
		if(eq_scope_dlg.DoModal()!=IDOK)
			return;
		m_xAnalyzeScope.csModule.Printf("%d",iSelModuleNo);
		RetrieveCfgWordFromModuleScopeStr(m_xAnalyzeScope.csModule,&m_xAnalyzeScope.cfgwordBody,&m_xAnalyzeScope.cfgwordLeg);
		m_xAnalyzeScope.csStatus.Copy(eq_scope_dlg.m_sWorkStatusScope.GetBuffer());
		m_xAnalyzeScope.m_bUpdateBoltInfo=eq_scope_dlg.m_bUpdateBoltInfo;
		ModelCheck(true);
	}
}

void CMainFrame::OnModelEqCheck() 
{
	ModelDesignCal(0,true);
}

void CMainFrame::OnTrussStableCheck() 
{
}
bool InsertStdFacePanel(BOOL bNewCmd=TRUE);
void CorrectJgLaystyle(long *id_arr,long n);	//defined in LDSView.cpp
void CMainFrame::OnInsertStdFacePanel() 
{
#if defined(__TSA_)||defined(__LDS_)||defined(__TDA_)
	g_pSolidDraw->ReleaseSnapStatus();
	CUndoOperObject undo(&Ta,true);
	if(InsertStdFacePanel(TRUE))
	{
#ifdef __ANGLE_PART_INC_
		long *id_arr=NULL;
		long nn=g_pSolidSnap->GetLastSelectEnts(id_arr);
		CorrectJgLaystyle(id_arr,nn);
#endif
	}
#endif
}

static int vibration_phase;
static double amplititude_manify_coef=1.0;
static int current_sel_vibra_mode=0;
void CMainFrame::OnImportVibrationMode() 
{
	char file_path[MAX_PATH];
	theApp.GetFileName(file_path);
	strcat(file_path,".frq");
	//_snprintf(file_path,MAX_PATH-1,"%s\\freqs.frq",theApp.work_path);
	FILE* fp=fopen(file_path,"rb");
	if(fp!=NULL)
	{
		long i,h,version,nFreqsRank=0,nNodeNum=0,seek_offset=0;
		fread(&version,sizeof(long),1,fp);
		fread(&nFreqsRank,sizeof(long),1,fp);
		fread(&nNodeNum,sizeof(long),1,fp);
		CSelOrderOfVibrationModeDlg order_dlg;
		order_dlg.m_fMaxAmplitude=theApp.m_fMaxAmplitude;
		order_dlg.m_nOrderNum=nFreqsRank;
		order_dlg.m_iSelOrder=current_sel_vibra_mode;
		order_dlg.modes_arr=Ta.EarthQuake.modes_arr;
		order_dlg.m_nSampleTimerInterval = theApp.m_nSampleTimerInterval;
		order_dlg.m_nSimulateVibraModePeriod = theApp.m_nSimulateVibraModePeriod;
		long head_final=12+(4+8)*nNodeNum;	//12+(handle+mass)*nNodeNum
		for(i=0;i<order_dlg.m_nOrderNum;i++)
		{
			seek_offset=head_final+i*(24+nNodeNum*28);//(sizeof(double)*3+nNodeNum*(sizeof(long)+sizeof(double)*3)
			fseek(fp,seek_offset,SEEK_SET);
			fread(&Ta.EarthQuake.modes_arr[i].freqs,sizeof(double),1,fp);	//该阶振型的自振频率
			fread(&Ta.EarthQuake.modes_arr[i].gama_x,sizeof(double),1,fp);	//该阶振型X向振型参预系数
			fread(&Ta.EarthQuake.modes_arr[i].gama_y,sizeof(double),1,fp);	//该阶振型Y向振型参预系数
		}
		if(order_dlg.DoModal()!=IDOK)
			return;
		theApp.m_fMaxAmplitude=order_dlg.m_fMaxAmplitude;
		theApp.m_nSampleTimerInterval = order_dlg.m_nSampleTimerInterval;
		theApp.m_nSimulateVibraModePeriod = order_dlg.m_nSimulateVibraModePeriod;
		current_sel_vibra_mode=order_dlg.m_iSelOrder;
		seek_offset=head_final+24+order_dlg.m_iSelOrder*(24+nNodeNum*28);//head_final+24+(sizeof(double)*3+nNodeNum*(sizeof(long)+sizeof(double)*3)
		fseek(fp,seek_offset,SEEK_SET);
		f3dPoint offset;
		//double freqs;
		//fread(&freqs,sizeof(double),1,fp);	//该阶振型的自振频率
		double max_value=0;
		for(i=0;i<nNodeNum;i++)
		{
			fread(&h,sizeof(long),1,fp);
			fread(&offset.x,sizeof(double),1,fp);
			fread(&offset.y,sizeof(double),1,fp);
			fread(&offset.z,sizeof(double),1,fp);
			max_value=__max(max_value,fabs(offset.x));
			max_value=__max(max_value,fabs(offset.y));
			max_value=__max(max_value,fabs(offset.z));
			CLDSNode *pNode=Ta.Node.FromHandle(h);
			if(pNode)
				pNode->m_offsetAmplitude=offset;
		}
		amplititude_manify_coef=theApp.m_fMaxAmplitude/max_value;
		fclose(fp);
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Didn't find vibration mode calculation results file!");
#else
		AfxMessageBox("没有找到振型计算结果文件！");
#endif
}

void CALLBACK EXPORT VibrationAnimationProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(nIDEvent==theApp.m_uAnimateTimer)
	{
		CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		double phasePositionCoef=sin(2*vibration_phase*Pi/ theApp.m_nSimulateVibraModePeriod);
		pLdsView->VibrationAnimate(phasePositionCoef*amplititude_manify_coef);
		theApp.m_uAnimateTimer=AfxGetMainWnd()->SetTimer(nIDEvent, theApp.m_nSampleTimerInterval,VibrationAnimationProc);
		vibration_phase+= theApp.m_nSampleTimerInterval;
		vibration_phase%= theApp.m_nSimulateVibraModePeriod;
	}
}
void CMainFrame::OnDisplayVibrationMode() 
{
	if(theApp.m_uAnimateTimer==0)	//启动振型动态显示
	{
		theApp.m_uAnimateTimer=2;
		vibration_phase=0;
		VibrationAnimationProc(GetSafeHwnd(),0,theApp.m_uAnimateTimer,0);
	}
	else
	{
		KillTimer(theApp.m_uAnimateTimer);
		theApp.m_uAnimateTimer=0;
		CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pLdsView->RedrawAll();
	}
}

void CMainFrame::OnUpdateDisplayVibrationMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.m_uAnimateTimer>0);
}
#else	//以下函数都是LDS中内力分析时用到的函数，如去掉链接会报错，所以只能置空
void CMainFrame::OnCodeNodeI(){;}
void CMainFrame::OnDesWireNode(){;}
void CMainFrame::OnImportLoadFile(){;}
void CMainFrame::OnExportLoadFile(){;}
void CMainFrame::OnDisplayDeformTower(){;}
void CMainFrame::OnUpdateDisplayDeformTower(CCmdUI* pCmdUI){;}
void CMainFrame::OnModelFreqs(){;}
void CMainFrame::OnModelDesignCal(){;}
void CMainFrame::OnModelEqCheck(){;}
void CMainFrame::OnTrussStableCheck(){;}
void CMainFrame::OnUpdateDisplayVibrationMode(CCmdUI* pCmdUI){;} 
void CMainFrame::OnInsertStdFacePanel(){;}
void CMainFrame::OnImportVibrationMode(){;}
void CMainFrame::OnDisplayVibrationMode(){;}
void CMainFrame::OnModelWireLoadCal(){;}
#endif

void CMainFrame::OnBnBendPartCheck() 
{
#ifdef __PART_DESIGN_INC_
	static CBeamPartSelMatDlg dlg;
	dlg.DoModal();
#endif
}
void CMainFrame::OnLineModeEnv()
{
#ifdef __COMMON_PART_INC_
	if(theApp.m_eEnvType!=LINE_ENV)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=LINE_ENV;
	}
	else
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
#endif
}
void CMainFrame::OnUpdateLineModeEnv(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_eEnvType==LINE_ENV);
}
void CMainFrame::OnSolidModeEnv()
{
#ifdef __COMMON_PART_INC_
	if(theApp.m_eEnvType!=SOLID_ENV)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=SOLID_ENV;
	}
	else
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
	theApp.m_bAnalysisWorkEnv=FALSE;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
#endif
}
void CMainFrame::OnUpdateSolidModeEnv(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_eEnvType==SOLID_ENV);
}

void CMainFrame::OnAnalysisWorkEnv() 
{
	if( theApp.m_eEnvType==FEMELEM_ENV && 
		m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Previewing calculation results by color plan,can't change to rod displaying mode");
#else
		AfxMessageBox("正在通过颜色方案预览计算结果，不能转到杆件显示模式");
#endif
		return;
	}
	if(theApp.m_eEnvType!=FEMROD_ENV)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=FEMROD_ENV;
		theApp.m_bAnalysisWorkEnv=TRUE;
	}
	else
	{
		WORKENV_TYPE env_type=theApp.m_eEnvType;
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
		theApp.m_ePrevEnvType=env_type;
	}
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
}

void CMainFrame::OnUpdateAnalysisWorkEnv(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.m_eEnvType==FEMROD_ENV);
}
void CMainFrame::OnFemElemEnv()
{
	if( theApp.m_eEnvType==FEMELEM_ENV && 
		m_bDisplayDeformedTower&&g_sysPara.iShowDeformedMapPlan>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Previewing calculation results,can't change to rod displaying mode");
#else
		AfxMessageBox("正在预览计算结果，不能转到杆件显示模式");
#endif
		return;
	}
#ifdef __PART_DESIGN_INC_
	if(theApp.m_eEnvType!=FEMELEM_ENV)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=FEMELEM_ENV;
	}
	else
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
#endif
}
void CMainFrame::OnUpdateFemElemEnv(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_eEnvType==FEMELEM_ENV);
}

void CMainFrame::OnDrawing2dEnv()
{
#ifdef __GGG_
	if(theApp.m_eEnvType!=DRAWING2D_ENV)
	{
		theApp.m_ePrevEnvType=theApp.m_eEnvType;
		theApp.m_eEnvType=DRAWING2D_ENV;
	}
	else
		theApp.m_eEnvType=theApp.m_ePrevEnvType;
	GetLDSView()->LdsWorkEnvShift(theApp.m_eEnvType);
#endif
}
void CMainFrame::OnUpdateDrawing2dEnv(CCmdUI *pCmdUI)
{
#ifdef __GGG_
	pCmdUI->SetCheck(theApp.m_eEnvType==DRAWING2D_ENV);
#endif
}
void CMainFrame::OnUpdateStatusBar(CCmdUI *pCmdUI)
{
	if( pCmdUI->m_nID==ID_INDICATOR_WORKENV||(
		pCmdUI->m_nID==ID_INDICATOR_MESSAGE&& theApp.m_bCooperativeWork))
		pCmdUI->Enable(true);
	if(pCmdUI->m_nID == ID_INDICATOR_WORKENV)
	{
		if (theApp.m_bAnalysisWorkEnvOnly)
			pCmdUI->SetText("力学计算");
	}
}

//键盘快捷键列表
void CMainFrame::OnKeyboardMap() 
{
	CKeyboardMapDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnSysParameters() 
{
#if defined(__TSA_)||defined(__LDS_)
	CSysParametersDlg sys_dlg;
	sys_dlg.DoModal();	
#endif
}

void CMainFrame::OnStubAngleCheck() 
{
#if defined(__TSA_)||defined(__LDS_)
	CStubAngleCheckDlg dlg;
	dlg.DoModal();
#endif
}
void CMainFrame::OnFootDesignCal()
{
	char vector_path[MAX_PATH];
	strcpy(vector_path,APP_PATH);
	strcat(vector_path,"FootCal.exe");
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	StartupInfo.cb=sizeof(STARTUPINFO);
	StartupInfo.lpReserved=NULL;
	StartupInfo.lpDesktop=NULL;
	StartupInfo.lpTitle=NULL;
	StartupInfo.dwX=100;
	StartupInfo.dwY=100;
	StartupInfo.dwXSize=100;
	StartupInfo.dwYSize=100;
	StartupInfo.dwXCountChars=0;
	StartupInfo.dwYCountChars=0;
	StartupInfo.dwFillAttribute=0;
	StartupInfo.dwFlags=STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow=SW_SHOWDEFAULT;
	StartupInfo.cbReserved2=0;
	StartupInfo.lpReserved2=NULL;
 	StartupInfo.hStdInput=StartupInfo.hStdOutput=StartupInfo.hStdError=NULL;
	CreateProcess(NULL,vector_path,NULL,NULL,FALSE,0,NULL,NULL,&StartupInfo,&ProcessInfo);
}

void CMainFrame::UpdateGlToolBarCmdUI()
{
	if(m_wndGLBar.GetSafeHwnd()!=NULL)
	{
		OPER_TYPE oper_type;
		oper_type = g_pSolidSet->GetOperType();
		int nMinID=min(ID_GL_PAN,min(ID_GL_ROTATED,ID_GL_ZOOM));
		int nMaxID=max(ID_GL_PAN,max(ID_GL_ROTATED,ID_GL_ZOOM));
		int nCheckedID=ID_GL_PAN;
		if(oper_type==OPER_ROTATE)
			nCheckedID=ID_GL_ROTATED;
		else if(oper_type==OPER_ZOOM)
			nCheckedID=ID_GL_ZOOM;
		m_wndGLBar.CheckRadioButton(nMinID,nMaxID,nCheckedID);
	}
	/*CToolBarCtrl& barCtrl=m_wndGLBar.GetToolBarCtrl();
	if(barCtrl.GetSafeHwnd()!=NULL)
	{
		OPER_TYPE oper_type;
		oper_type = g_pSolidSet->GetOperType();
		barCtrl.CheckButton(ID_GL_ROTATED,oper_type==OPER_ROTATE);
		barCtrl.CheckButton(ID_GL_PAN,oper_type==OPER_PAN);
		barCtrl.CheckButton(ID_GL_ZOOM,oper_type==OPER_ZOOM);
	}*/
}

void CMainFrame::OnResultBuffer()
{
#if defined(__LDS_)||defined(__TSA_)
	CRsltBufferDlg rslt_dlg;
	rslt_dlg.m_sActiveRsltItemKeyStr=postreport.m_sRsltItem;
	rslt_dlg.m_cmbRsltItems="";
	CAttachBuffer *pRsltBuffer=Ta.AttachBuffer.GetValue(CTower::RSLT_REPORT);
	int item_n=0;
	if(pRsltBuffer)
	{
		pRsltBuffer->SeekToBegin();
		pRsltBuffer->ReadInteger(&item_n);
	}
	CXhChar16 rsltItemKeyStr;
	for(int i=0;i<item_n;i++)
	{
		if(!rslt_dlg.m_cmbRsltItems.IsEmpty())
			rslt_dlg.m_cmbRsltItems.Append("|");
		if(pRsltBuffer->ReadString(rsltItemKeyStr,17)>0)
		{
			int length=0;
			pRsltBuffer->ReadInteger(&length);
			pRsltBuffer->Offset(length);
			rslt_dlg.m_cmbRsltItems.Append((char*)rsltItemKeyStr);
		}
	}
	rslt_dlg.DoModal();
#endif
}
void CMainFrame::OnCompareWithTtaResults()
{
#if defined(__LDS_)||defined(__TSA_)
	CPostProcessReport ttareport;
	if(!ttareport.ReadFromTtaRsltOutFile())
		return;
	//TODO:1.遍历ttareport所有单元的材质符是否合法，只要有一个不合法就弹出所有材质符匹配
	//TODO:2.弹出对话框进行人工材质符匹配
	//TODO:3.根据人工设定的所有单元材质符映射匹配表更所有构件的材质符
	theApp.GetWorkPathAuto();
	CXhChar200 filepath("%s/FemTestReport.txt",theApp.work_path);
	CLogFile rpt(filepath);
	postreport.CompareWithOtherProgramReport(ttareport,rpt);
#endif
}

void CMainFrame::OnExportRptFile() 
{
#if defined(__LDS_)||defined(__TSA_)
	static CExportRptFileOptionDlg rpt_dlg;
	rpt_dlg.m_iEfficType=g_sysPara.iEfficacyCeofType;
	if(rpt_dlg.DoModal()==IDOK)
	{
		//结果文件目录生成
		char file_path[MAX_PATH], file_name[MAX_PATH];
		CString ss=theApp.GetLDSDoc()->GetPathName();
		_snprintf(file_path,MAX_PATH-1,"%s",ss);
		GetSysPath(file_path,file_path,file_name);
		strcat(file_path,file_name);
		strcat(file_path,"\\");
		strcat(file_path,file_name);
		g_sysPara.iEfficacyCeofType=rpt_dlg.m_iEfficType;
		if(rpt_dlg.m_iFileLanguage == 0)
			CreateDetailRptFileFromReportByCN(file_path,rpt_dlg.m_iDocFileType);
		else if(rpt_dlg.m_iFileLanguage == 1)
			CreateDetailRptFileFromReportByEN(file_path,rpt_dlg.m_iDocFileType);
	}
#endif
}

void CMainFrame::HtmlHelp(DWORD_PTR dwData, UINT nCmd)
{
	//在此添加专用代码和/或调用基类
	if(nCmd==HH_HELP_CONTEXT)
	{	//重载系统默认的菜单命令及对话框上下文帮助触发操作 wjh-2015.11.19
		PROPLIST_ITEM* pHelpItem=NULL;
		if((dwData&HID_BASE_COMMAND)>0)
		{	//ID and IDM菜单命令
			DWORD uCmd=dwData-HID_BASE_COMMAND;
			pHelpItem=theApp.hashMenuHelpItemById.GetValue(uCmd);
		}
		else if((dwData&HID_BASE_RESOURCE)>0)
		{	//IDR and IDD对话框资源
			DWORD uCmd=dwData-HID_BASE_RESOURCE;
			pHelpItem=theApp.hashDialogHelpItemById.GetValue(uCmd);
		}
		else if((dwData&HID_BASE_CONTROL)>0)
		{	//IDC对话框控件
		}
		if(pHelpItem!=NULL&&pHelpItem->Url.GetLength()>0)
		{
			CXhChar200 helpStr("%s::/%s",theApp.m_pszHelpFilePath,(char*)pHelpItem->Url);
			HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
			return;
		}
	}

	CFrameWndEx::HtmlHelp(dwData, nCmd);
}


void CMainFrame::OnLicenseCenter()
{
	CLicenseAuthorizeDlg authdlg;
	authdlg.m_bRunning=true;
	authdlg.InitLicenseModeByLicFile(NULL);
	authdlg.DoModal();
}

void CMainFrame::OnShiftUiOfMechanics()
{
	ShiftUiOfMechanics();
}

void CMainFrame::OnShiftUiOfRapid3dModel()
{
	ShiftUiOfRapid3dModel();
}

void CMainFrame::OnShiftUiOfFinal3dModel()
{
	ShiftUiOfFinal3dModel();
}

void CMainFrame::OnShiftUiOfDrawingModel()
{
	ShiftUiOfDrawingModel();
}
