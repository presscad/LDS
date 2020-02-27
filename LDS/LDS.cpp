// LDS.cpp : Defines the class behaviors for the application.
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "LDS.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Splash.h"
#include "direct.h"
#include "MD5.h"
#include "Signature.h"
#include "ReConnServerDlg.h"
#include "MsgDef.h"
#include "ProcessMsgPackDlg.h"
#include "LmaDlg.h"
#include "XhCharString.h"
#include "FLSizeOptimizor.h"
#ifdef __PART_DESIGN_INC_
#include "FemSolverLoop.h"
#include "./Codes/Codes.h"
#endif
#ifndef _LEGACY_LICENSE
#include "XhLdsLm.h"
#include "LicenseAuthorizeDlg.h"
#else
#include "Lic.h"
#endif
#include "PartLib.h"
#include ".\Log\XhLog.h"
#include "Expression.h"
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"
#include "LicFileProblemDlg.h"
#include "IntelliSight.h"
#include "LogFile.h"
#include "SystemSettingDlg.h"
#include "LibraryQuery.h"
#include "Buffer.h"
#include "ClientId.h"
#include "LdsLocale.h"
#include "LdsPortalLocale.h"
#include "Global.h"
#ifdef __SMART_DRAW_INC_
#include "BriefWeldPart.h"
#endif
#ifdef __CONNECT_REMOTE_SERVER_
#include "TMS.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLDSApp
char APP_PATH[MAX_PATH];
char PIPE_CORE_NAME[MAX_PATH],PIPE_SERVER_LISTEN[MAX_PATH],PIPE_SERVER_SEND[MAX_PATH];
//
DefGetSupportBOMType CLDSApp::GetSupportBOMType=NULL;
DefGetSupportDataBufferVersion CLDSApp::GetSupportDataBufferVersion=NULL;
DefCreateExcelBomFile CLDSApp::CreateExcelBomFile = NULL;
DefRecogniseReport CLDSApp::RecogniseReport =NULL;
DefGetExcelFormat CLDSApp::GetBomExcelFormat=NULL;
DefGetExcelFormatEx CLDSApp::GetBomExcelFormatEx=NULL;


#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.Lib")  //MiniDumpWriteDump链接时用到
LONG WINAPI  LDSExceptionFilter(EXCEPTION_POINTERS *pExptInfo)
{
	if(strlen(APP_PATH)<=0)
		GetSysPath(APP_PATH);
	// 程序崩溃时，将写入程序目录下的ExceptionDump.dmp文件  
	CXhChar500 sExceptionFilePath("%s\\%sException.dmp", theApp.m_pszAppName,APP_PATH);
	size_t size = sExceptionFilePath.Length;
	if (size > 0)
	{
		HANDLE hFile = ::CreateFile(sExceptionFilePath, GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION exptInfo;
			exptInfo.ThreadId = ::GetCurrentThreadId();
			exptInfo.ExceptionPointers = pExptInfo;
			//将内存堆栈dump到文件中
			//MiniDumpWriteDump需引入dbghelp头文件
			BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(),
				::GetCurrentProcessId(), hFile, MiniDumpNormal,
				&exptInfo, NULL, NULL);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
		return 0;
}
DWORD GetPermWord(long nUserId)
{
	if(nUserId==0)
		return PERM_TOWER_ADMIN;
	else
		return GetSingleWord(nUserId);
}
THANDLE GetCooperativeNewHandle()
{
	DWORD nBytes;
	long h;
	LDS_MSGHEAD msg_head;
	msg_head.command_id=ASK_NEW_HANDLE;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	msg_head.msg_length=0;
	if(!WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&nBytes,NULL))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Server is closed！";
#else 
		throw "服务器已关闭！";
#endif
	if(!ReadFile(theApp.m_hPipeServerListen,&h,sizeof(long),&nBytes,NULL))
#ifdef AFX_TARG_ENU_ENGLISH
		throw "Server is closed！";
#else 
		throw "服务器已关闭！";
#endif
	return h;
}
PROPLIST_ITEM* AddUniversalPropItem(char* propKey,PROPLIST_ITEM& item,CHashStrList<PROPLIST_ITEM>* pPropHashtable,CHashSet<PROPLIST_ITEM*>* pHashPropById)
{
	PROPLIST_ITEM* pItem=pPropHashtable->GetValue(propKey);
	if(pItem!=NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Attribute primary key string %s has repeated",propKey);
#else 
		logerr.Log("属性主键字符串%s出现重复",propKey);
#endif
	}
	else
	{
		pItem=pPropHashtable->SetValue(propKey,item);
		pItem->propKeyStr.Copy(propKey);
	}
	if(pHashPropById)
		return pHashPropById->SetValue(item.id,pItem);
	else
		return pItem;
}
BOOL IsValidModifyPerm(CLDSObject *pObj)
{
	if(!theApp.m_bCooperativeWork||pObj->dwPermission&theApp.GetPermDword())
		return TRUE;
	else
		return FALSE;
}
void LoadDefaultObjectInfo(CLDSObject *pObj)
{
	if(!Ta.IsNeedInitNewObjDefaultInfo())
		return;
	DWORD dwPermission=GetSingleWord(theApp.m_nUserId);
	CLDSModule *pActiveModule=Ta.GetActiveModule();
	if(pActiveModule&&pObj->IsDbObject())
	{
		((CLDSDbObject*)pObj)->cfgword=CFGWORD(pActiveModule->GetBodyNo(),0);
		((CLDSDbObject*)pObj)->cfgword.AddBodyLegs(pActiveModule->GetBodyNo());
	}
	if(pObj->IsDbObject())
	{
		((CLDSDbObject*)pObj)->SetLayer(theApp.env.layer);
		((CLDSDbObject*)pObj)->iSeg = theApp.env.iSeg;
		if(pObj->BelongModel()!=NULL)
		{
			if(pObj->GetClassTypeId()==CLS_NODE)
				console.DispNodeSet.append((CLDSNode*)pObj);			//pObj->BelongModel()->DispNodeSet
			else if(pObj->GetClassTypeId()==CLS_BLOCKREF)
				console.DispBlockRefSet.append((CBlockReference*)pObj);	//pObj->BelongModel()->DispBlockRefSet
			else if(pObj->GetClassTypeId()==CLS_FOUNDATION)
				console.DispFoundationSet.append((CLDSFoundation*)pObj);
			else
				console.DispPartSet.append((CLDSPart*)pObj);			//pObj->BelongModel()->DispPartSet
			//如果为定制视图将新生成的构件添加到定制视图的显示集合中
			CDisplayView *pView=console.GetActiveView();
			if(pView&&pView->m_iViewType==1)
				pView->AppendViewPart(pObj->handle);
		}
		if(((CLDSDbObject*)pObj)->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
		{
			((CLDSLinePart*)pObj)->dwStartPermission=dwPermission;
			((CLDSLinePart*)pObj)->dwEndPermission=dwPermission;
		}
	}
	pObj->SetPermission(dwPermission);
}
CProcBarDlg *pProcDlg=NULL;
void DisplayProcess(int percent,const char *sTitle)
{
	if(percent>=100)
	{
		if(pProcDlg!=NULL)
		{
			pProcDlg->DestroyWindow();
			delete pProcDlg;
			pProcDlg=NULL;
		}
		return;
	}
	else if(pProcDlg==NULL)
		pProcDlg=new CProcBarDlg(NULL);
	if(pProcDlg->GetSafeHwnd()==NULL)
		pProcDlg->Create();
	static int prevPercent;
	if(percent!=0&&percent==prevPercent)
		return;	//跟上次进度一致不需要更新
	else
		prevPercent=percent;
	if(sTitle)
		pProcDlg->SetTitle(CString(sTitle));
	else
#ifdef AFX_TARG_ENU_ENGLISH
		pProcDlg->SetTitle("Process");
#else 
		pProcDlg->SetTitle("进度");
#endif
	pProcDlg->Refresh(percent);
}
CPromptBarDlg *pPromptDlg=NULL;
BOOL IsCanceledPromptDlg()
{
	if(pPromptDlg)
		return pPromptDlg->CanceledProc();
	else
		return FALSE;
}
void DisplayPrompt(char *sTitle/*=NULL*/)
{
	if(sTitle==NULL||strlen(sTitle)==0)
	{
		if(pPromptDlg!=NULL)
		{
			pPromptDlg->DestroyWindow();
			delete pPromptDlg;
			pPromptDlg=NULL;
		}
		return;
	}
	else if(pPromptDlg==NULL)
		pPromptDlg=new CPromptBarDlg(NULL);
	if(pPromptDlg->GetSafeHwnd()==NULL)
		pPromptDlg->Create();
	else
		pPromptDlg->ShowWindow(SW_SHOW);
	pPromptDlg->SetPromptStr(CString(sTitle));
}
void CopyPartlib(CTowerArchiveLibrary& lib)
{
	//槽型插板库
	lib.rollend_param_N=rollend_param_N;
	lib.rollend_param_table.SetSize(rollend_param_N,sizeof(INSERT_PLATE_PARAM)*rollend_param_N);
	memcpy(lib.rollend_param_table.m_pData,(void*)rollend_param_table,sizeof(INSERT_PLATE_PARAM)*rollend_param_N);
	//U型插板库
	lib.uend_param_N=uend_param_N;
	lib.uend_param_table.SetSize(uend_param_N,sizeof(INSERT_PLATE_PARAM)*uend_param_N);
	memcpy(lib.uend_param_table.m_pData,(void*)uend_param_table,sizeof(INSERT_PLATE_PARAM)*uend_param_N);
	//十字型插板库
	lib.xend_param_N=xend_param_N;	
	lib.xend_param_table.SetSize(xend_param_N,sizeof(INSERT_PLATE_PARAM)*xend_param_N);
	memcpy(lib.xend_param_table.m_pData,(void*)xend_param_table,sizeof(INSERT_PLATE_PARAM)*xend_param_N);
	//法兰对接级别表
	lib.fl_couple_rule_N=fl_couple_rule_N;
	lib.fl_couple_rule_table.SetSize(fl_couple_rule_N,sizeof(FL_COUPLE_RULE)*fl_couple_rule_N);
	memcpy(lib.fl_couple_rule_table.m_pData,(void*)fl_couple_rule_table,sizeof(FL_COUPLE_RULE)*fl_couple_rule_N);
	//对焊法兰
	lib.fld_param_N=fld_param_N;
	lib.fld_param_table.SetSize(fld_param_N,sizeof(FL_PARAM));
	memcpy(lib.fld_param_table.m_pData,(void*)fld_param_table,sizeof(FL_PARAM)*fld_param_N);
	//平焊法兰
	lib.flp_param_N=flp_param_N;
	lib.flp_param_table.SetSize(flp_param_N,sizeof(FL_PARAM));
	memcpy(lib.flp_param_table.m_pData,(void*)flp_param_table,sizeof(FL_PARAM)*flp_param_N);
	//刚性法兰
	lib.flg_param_N=flg_param_N;
	lib.flg_param_tbl.SetSize(flg_param_N,sizeof(FL_PARAM2));
	memcpy(lib.flg_param_tbl.m_pData,(void*)flg_param_tbl,sizeof(FL_PARAM2)*flg_param_N);
	//柔性法兰
	lib.flr_param_N=flr_param_N;
	lib.flr_param_tbl.SetSize(flr_param_N,sizeof(FL_PARAM2)*flr_param_N);
	memcpy(lib.flr_param_tbl.m_pData,(void*)flr_param_tbl,sizeof(FL_PARAM2)*flr_param_N);
}
#include "ElectricTower.h"
void CLDSApp::InitTowerModel()
{
	TCHAR cmd_str[MAX_PATH]="",lib_path[MAX_PATH]="";
	GetSysPath(cmd_str);
#ifdef __PART_DESIGN_INC_
	IMemberDesignCode::SetHostErrorLogger(&logerr);
	sprintf(lib_path,"%sWireLayoutEvolver.dll",cmd_str);
	m_hWireLayoutModule=LoadLibrary(lib_path);
	Ta.m_pExterModel=&gxExterModel;
	gxExterModel.SetBelongTower(&Ta);
	if(m_hWireLayoutModule!=NULL)
	{
		CTower::WriteWireLayoutWeather =(DefWriteWeatherFunc)GetProcAddress(m_hWireLayoutModule,"WriteWeather");
		CTower::ReadWireLayoutData =(DefReadWireLayoutDataFunc)GetProcAddress(m_hWireLayoutModule,"ReadWireLayoutData");
	}
#endif
	sprintf(lib_path,"%sBomExport.dll",cmd_str);
	m_hBomExport=LoadLibrary(lib_path);
	if(m_hBomExport)
	{
		CLDSApp::GetSupportBOMType=(DefGetSupportBOMType)GetProcAddress(m_hBomExport,"GetSupportBOMType");
		CLDSApp::GetSupportDataBufferVersion=(DefGetSupportDataBufferVersion)GetProcAddress(m_hBomExport,"GetSupportDataBufferVersion");
		CLDSApp::CreateExcelBomFile = (DefCreateExcelBomFile)GetProcAddress(m_hBomExport, "CreateExcelBomFile");
		CLDSApp::RecogniseReport =(DefRecogniseReport)GetProcAddress(m_hBomExport,"RecogniseReport");
		CLDSApp::GetBomExcelFormat=(DefGetExcelFormat)GetProcAddress(m_hBomExport,"GetExcelFormat");
		CLDSApp::GetBomExcelFormatEx=(DefGetExcelFormatEx)GetProcAddress(m_hBomExport,"GetExcelFormatEx");
	}
	CLDSObject::_console=&console;		//操作控制台
	CLDSPart::library=&globalLibrary;	//构件库
	CWorkStatus::sysPara=&g_sysPara;	//环境配置
	CLDSLinePart::sysPara=&g_sysPara;	//环境配置
	CDesignThickPara::m_pTower=&Ta;		
	CLDSPoint::m_pTower=&Ta;
	CLDSVector::m_pTower=&Ta;
	CLDSObject::log2file=&logerr;
#ifdef __SMART_DRAW_INC_
	WELDSONPART::globalModel=&Ta;
#endif
	//显示构件细节类型范围设定
	//显示比例及圆弧抽样尺寸设定
	//钢管相邻直角边最小夹角设定
	//初始化铁塔模型的默认信息
	Ta.user_pipeline_no=DogSerialNo();
	Ta.InitTower();
	console.ActivateModel(&Ta);
	console.InitTowerModel();
	//读取文件进度条回调函数设定
	Ta.DisplayProcess=DisplayProcess;
	//新加构件缺省信息赋值回调函数设定
	Ta.View.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Point.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Line.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Plane.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Module.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.ArcLift.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	//Ta.PartGroup.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Node.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Parts.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.BlockRef.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	Ta.Foundations.LoadDefaultObjectInfo=LoadDefaultObjectInfo;
	//权限验证函数设定
	Ta.View.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Point.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Line.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Plane.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Module.IsValidModifyPerm=IsValidModifyPerm;
	//Ta.PartGroup.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Node.IsValidModifyPerm=IsValidModifyPerm;
	Ta.Parts.IsValidModifyPerm=IsValidModifyPerm;
	//获取协同构件新句柄函数设定
	Ta.GetCooperativeNewHandle=GetCooperativeNewHandle;
	Ta.m_bCooperativeWork=theApp.m_bCooperativeWork;
}
void InitClassTypePropHashtable()
{
	CLDSApp::InitSyssettingPropHashtable();
	CLDSView::InitWorkViewPropHashtable();
	CLDSDbObject::InitDbObjectPropHashtable();
	CLDSNode::InitNodePropHashtable();
	CLDSPart::InitPartPropHashtable();
	CLDSPoint::InitPropHashtable();
	CLDSVector::InitPropHashtable();
	//非线型构件
#ifdef __COMMON_PART_INC_
	CLDSPlate::InitPropHashtable();
	CLDSParamPlate::InitPropHashtable();
	CLDSBolt::InitBoltPropHashtable();
	CLDSSphere::InitSpherePropHashtable();
	CLDSFitting::InitPropHashtable();
	CLDSAnchorBolt::InitPropHashtable();
	CXhWirePoint::InitPropHashtable();
#endif
	//直线构件
	CLDSLinePart::InitLinePartPropHashtable();
#ifdef __PART_DESIGN_INC_
	CElemInfo::InitElemInfoPropHashtable();
#endif
#ifdef __ANGLE_PART_INC_
	CLDSLineFlat::InitLineFlatPropHashtable();
	CLDSLineTube::InitLineTubePropHashtable();
#ifdef __GGG_
	CLDSConeTube::InitConeTubePropHashtable();
#endif
	CLDSLineAngle::InitLineAnglePropHashtable();
	CLDSLineSlot::InitLineSlotPropHashtable();
	CLDSGroupLineAngle::InitGroupLineAnglePropHashtable();
	//圆弧构件
	CLDSArcPart::InitArcPartPropHashtable();
	CLDSArcAngle::InitArcAnglePropHashtable();
	CLDSArcFlat::InitArcFlatPropHashtable();
	CLDSArcSlot::InitArcSlotPropHashtable();
	//CLDSLineHShape::InitLineHShapePropHashtable();
	//火曲变形点
	CLDSPolyJoint::InitPolyJointPropHashtable();
#endif
	CBlockModel::InitBlockModelPropHashtable();
	CBlockReference::InitBlockReferencePropHashtable();
	PROFILE_VERTEX::InitProfileVertexPropHashtable();
}

BEGIN_MESSAGE_MAP(CLDSApp, CWinAppEx)
	//{{AFX_MSG_MAP(CLDSApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_APP_ABOUT, OnUpdateAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLDSApp construction
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
long CLDSApp::MAX_USER_PROPID = 1000;
long CLDSApp::MIN_IDLE_PROPID = 100;
IMPLEMENT_PROP_FUNC(CLDSApp);
void CLDSApp::InitSyssettingPropHashtable()
{
	m_iCurDisplayPropGroup=1;	//默认设定<显示>页为当前显示属性页,以便直接设定段号 wjh-2017.8.6
	InitPropHashtable();
}
void CLDSApp::InitPropHashtable()
{
	int id=1;
	CXhChar100 helpStr;
	CLDSApp::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSApp::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef  AFX_TARG_ENU_ENGLISH
	CLDSApp::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Basic Info"));
	CLDSApp::AddPropItem("DrawingInfo",PROPLIST_ITEM(id++,"Basic Info"));
	CLDSApp::AddPropItem("PartLibrary",PROPLIST_ITEM(id++,"Part Library"));
	CLDSApp::AddPropItem("g_sysPara.nAutoSaveTime",PROPLIST_ITEM(id++,"Auto Save Time"));
	CLDSApp::AddPropItem("g_sysPara.iAutoSaveType",PROPLIST_ITEM(id++,"Auto Save Type"));
	CLDSApp::AddPropItem("g_sysPara.uFileSaveVersion",PROPLIST_ITEM(id++,"Save as previous version"));
	CLDSApp::AddPropItem("BatchSaveAsOldFile",PROPLIST_ITEM(id++,"Batch save as previous version"));
	CLDSApp::AddPropItem("g_sysPara.iFileSaveDocType",PROPLIST_ITEM(id++,"Save as specified file type"));
	CLDSApp::AddPropItem("Ta.m_fTowerHeight",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("Ta.segstr",PROPLIST_ITEM(id++,"Display Segment Number Range","","Exclude Segments"));
	CLDSApp::AddPropItem("env.m_sUserName",PROPLIST_ITEM(id++,"User Name"));
	CLDSApp::AddPropItem("env.m_sShortUserName",PROPLIST_ITEM(id++,"User Name Abbreviation"));
	CLDSApp::AddPropItem("env.m_sAcadPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.m_ciLowestWarningLevel",PROPLIST_ITEM(id++,"Warning Level"));
	CLDSApp::AddPropItem("FileRW.BUFFSIZE",PROPLIST_ITEM(id++,"File read-write work buffer size(MB)"));
	CLDSApp::AddPropItem("env.m_sJgZhunJuTbl",PROPLIST_ITEM(id++,"Angle Stadia Library","Angle stadia library is saved with file。"));
	CLDSApp::AddPropItem("env.m_sJgGuiGeTbl",PROPLIST_ITEM(id++,"Angle Specification Library"));
	CLDSApp::AddPropItem("env.m_sJgGuiGeTblPath",PROPLIST_ITEM(id++,"Angle Specification File","New open program will initialize angle specification file。"));
	CLDSApp::AddPropItem("env.m_sTubeGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sTubeGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sFlatGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sFlatGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sSlotGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sSlotGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHWGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHWGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHMGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHMGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHNGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHNGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHPGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sHPGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sFittingGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sEBGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sGDGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sURGuiGeTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sJgJointParaTbl",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sJgJointParaTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("ANCHOR_BOLT_LIB",PROPLIST_ITEM(id++,"Anchor Bolt Library"));
	CLDSApp::AddPropItem("env.m_sLsSpaceTbl",PROPLIST_ITEM(id++,"Bolt Interval Library"));
	CLDSApp::AddPropItem("env.m_sLsSpaceTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sLsGuiGeTbl",PROPLIST_ITEM(id++,"Bolt Specification Series"));
	CLDSApp::AddPropItem("env.m_sLsSubPartTbl",PROPLIST_ITEM(id++,"Bolt Sub Part Library"));
	CLDSApp::AddPropItem("TwoCapM16IncreL",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("TwoCapM20IncreL",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("TwoCapM24IncreL",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("LsGuiGeTblPath",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("env.m_sWireGuiGeTbl",PROPLIST_ITEM(id++,"Cable specification warehouse"));
	CLDSApp::AddPropItem("env.m_sLsSelectTbl",PROPLIST_ITEM(id++,""));	//法兰螺栓选材表
	CLDSApp::AddPropItem("env.m_sSteelMatTbl",PROPLIST_ITEM(id++,"Material Library"));	//材料库
	CLDSApp::AddPropItem("env.m_sInterpolateBetaZTbl",PROPLIST_ITEM(id++,"Bolt Select Table"));	//风振系数插值表
	CLDSApp::AddPropItem("env.m_sInterpIceLoadCoefTbl",PROPLIST_ITEM(id++,""));	//杆塔覆冰重力荷载系数表
	CLDSApp::AddPropItem("Ta.listSelBoltCond",PROPLIST_ITEM(id++,"Bolt Select Table"));	//螺栓选择条件
	CLDSApp::AddPropItem("InsertPlateLib",PROPLIST_ITEM(id++,""));		//钢管管头插板库
	CLDSApp::AddPropItem("env.m_sCInsertPlateLib",PROPLIST_ITEM(id++,""));	//槽型插板库
	CLDSApp::AddPropItem("env.m_sUInsertPlateLib",PROPLIST_ITEM(id++,""));	//U型插板库
	CLDSApp::AddPropItem("env.m_sXInsertPlateLib",PROPLIST_ITEM(id++,""));	//十字插板库
	CLDSApp::AddPropItem("env.m_sCInsertPlateLibPath",PROPLIST_ITEM(id++,""));	//槽型插板库路径
	CLDSApp::AddPropItem("env.m_sUInsertPlateLibPath",PROPLIST_ITEM(id++,""));  //U型插板库路径
	CLDSApp::AddPropItem("env.m_sXInsertPlateLibPath",PROPLIST_ITEM(id++,""));	//十字型插板路径
	CLDSApp::AddPropItem("StdPartLib",PROPLIST_ITEM(id++,"Standard parts warehouse"));			//标准构件库
	CLDSApp::AddPropItem("StdPartFlLib",PROPLIST_ITEM(id++,""));			//法兰规格库
	CLDSApp::AddPropItem("env.m_sStdPartFlDLibPath",PROPLIST_ITEM(id++,""));//对焊法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlPLibPath",PROPLIST_ITEM(id++,""));//平焊法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlGLibPath",PROPLIST_ITEM(id++,""));//刚性法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlRLibPath",PROPLIST_ITEM(id++,""));//柔性法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sFlCoupleLevelPath",PROPLIST_ITEM(id++,""));//对接法兰大小头级别定义文件路径
	CLDSApp::AddPropItem("m_dwSelectPartsType",PROPLIST_ITEM(id++,"Part Filter"));
	CLDSApp::AddPropItem("CanSelectNode",PROPLIST_ITEM(id++,"Node","If can select node","Yes|No"));
	CLDSApp::AddPropItem("CanSelectLineAngle",PROPLIST_ITEM(id++,"Angle","If can select angle","Yes|No"));
	CLDSApp::AddPropItem("CanSelectLineTube",PROPLIST_ITEM(id++,"Tube","If can select tube","Yes|No"));
	CLDSApp::AddPropItem("CanSelectPlate",PROPLIST_ITEM(id++,"Plate","If can select plate","Yes|No"));
	CLDSApp::AddPropItem("CanSelectParamPlate",PROPLIST_ITEM(id++,"Parametric Plate","If can select paramtric plate","Yes|No"));
	CLDSApp::AddPropItem("CanSelectBolt",PROPLIST_ITEM(id++,"Bolt","If can select Bolt","Yes|No"));
	CLDSApp::AddPropItem("CanSelectSphere",PROPLIST_ITEM(id++,"Sphere","If can select Sphere","Yes|No"));
	CLDSApp::AddPropItem("CanSelectLineSlot",PROPLIST_ITEM(id++,"Slot","If can select Slot","Yes|No"));
	CLDSApp::AddPropItem("CanSelectLineFlat",PROPLIST_ITEM(id++,"Flat","If can select Flat","Yes|No"));
	CLDSApp::AddPropItem("CanSelectArcAngle",PROPLIST_ITEM(id++,"Arc Angle","If can select Arc Angle","Yes|No"));
	CLDSApp::AddPropItem("CanSelectArcFlat",PROPLIST_ITEM(id++,"Arc Flat","If can select Arc Flat","Yes|No"));
	CLDSApp::AddPropItem("CanSelectArcSlot",PROPLIST_ITEM(id++,"Arc Slot","If can select Arc Slot","Yes|No"));
	CLDSApp::AddPropItem("CanSelectStdPart",PROPLIST_ITEM(id++,"Std Part","If can select Std Part","Yes|No"));
	CLDSApp::AddPropItem("CanSelectEdgeLine",PROPLIST_ITEM(id++,"Edge Line","If can select Edge Line","Yes|No")); //轮廓面边界线
	CLDSApp::AddPropItem("DisplayScope",PROPLIST_ITEM(id++,"Display Range"));
	CLDSApp::AddPropItem("DisplayMode",PROPLIST_ITEM(id++,"Display Mode"));
	CLDSApp::AddPropItem("env.layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSApp::AddPropItem("env.flieProp",PROPLIST_ITEM(id++,"File Property"));
	CLDSApp::AddPropItem("Ta.version",PROPLIST_ITEM(id++,"File Version","Current file's format version"));
	CLDSApp::AddPropItem("Ta.Node.GetNodeNum",PROPLIST_ITEM(id++,"Count of Nodes"));
	CLDSApp::AddPropItem("Ta.Parts.GetNodeNum",PROPLIST_ITEM(id++,"Count of Part"));
	CLDSApp::AddPropItem("Ta.m_sPrjName",PROPLIST_ITEM(id++,"Project Name"));
	CLDSApp::AddPropItem("Ta.m_sTaType",PROPLIST_ITEM(id++,"Tower Type"));
	CLDSApp::AddPropItem("Ta.m_nTaNum",PROPLIST_ITEM(id++,"Tower Num"));
	CLDSApp::AddPropItem("Ta.m_sTaAlias",PROPLIST_ITEM(id++,"Tower Alias Code"));
	CLDSApp::AddPropItem("Ta.m_sTaStampNo",PROPLIST_ITEM(id++,"Stamp Mark"));
	CLDSApp::AddPropItem("Ta.m_sOperator",PROPLIST_ITEM(id++,"Operator"));
	CLDSApp::AddPropItem("Ta.m_sAuditor",PROPLIST_ITEM(id++,"Auditor"));
	CLDSApp::AddPropItem("Ta.m_ciModelArchitecture",PROPLIST_ITEM(id++,"Architecture"));
	CLDSApp::AddPropItem("Ta.m_bRepositionByModeBeforeV51",PROPLIST_ITEM(id++,"Old Reposition Mode","","Recommended mode|V2 deprecated mode|V1 deprecated mode"));
	CLDSApp::AddPropItem("m_iPlankWeightStatType",PROPLIST_ITEM(id++,"Prea Calculation Style","","0.Envelope rectangle;|1.Net area;"));
	CLDSApp::AddPropItem("m_iCirclePlankAreaType",PROPLIST_ITEM(id++,"Polygon Area Calculation Style","","0.Envelope rectangle area;1.Net area;"));
	CLDSApp::AddPropItem("g_sysPara.m_bSaveLogVersion",PROPLIST_ITEM(id++,"Save modification log"));
	CLDSApp::AddPropItem("g_sysPara.m_iModFileCodeType",PROPLIST_ITEM(id++,"Save Mod File Coding Type"));
	CLDSApp::AddPropItem("g_sysPara.fRotateVelocity",PROPLIST_ITEM(id++,"Rotate Speed"));
	CLDSApp::AddPropItem("g_sysPara.m_bLineDepthTest",PROPLIST_ITEM(id++,"Line model depth test","","Enable|Disable"));
	CLDSApp::AddPropItem("g_sysPara.m_bDragOpenWndSel",PROPLIST_ITEM(id++,"Rapid window select","","Enable|Disable"));
	CLDSApp::AddPropItem("g_sysPara.m_bDblClkStartRotateOper",PROPLIST_ITEM(id++,"Double Click Trigger Rotate","","Enable|Disable"));
	CLDSApp::AddPropItem("lineDisplay",PROPLIST_ITEM(id++,"Single Line Display"));
	CLDSApp::AddPropItem("g_sysPara.display.bNodePnt",PROPLIST_ITEM(id++,"Node","Display the single line's node","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bPoleLine",PROPLIST_ITEM(id++,"Rod Simplify Line","Whether to display the rod's simplify line.","Yes|No"));
	CLDSApp::AddPropItem("CLDSLinePart::bDisplayLineVecMark",PROPLIST_ITEM(id++,"Rod Direction Mark","Whether to display rod's direction(start->end) mark","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bPartSketch",PROPLIST_ITEM(id++,"Part Outline Edge","Display outline edge or vertex when selected one part,disappear after refresh!","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bLoadArrow",PROPLIST_ITEM(id++,"Load arrow","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.nLoadArrowScale",PROPLIST_ITEM(id++,"size of load arrow"));
	CLDSApp::AddPropItem("g_sysPara.display.bWirePoint",PROPLIST_ITEM(id++,"Wire point","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.xPointStyle.cbPointSize",PROPLIST_ITEM(id++,"Node Display Size"));
	CLDSApp::AddPropItem("g_sysPara.display.xPointStyle.ciPointStyle",PROPLIST_ITEM(id++,"Node Display Style","","0.Square Point|1.Circular Point"));
	CLDSApp::AddPropItem("g_sysPara.display.bDimNodeLabel",PROPLIST_ITEM(id++,"Node Numbers","Whether to display node numbers in mechanical analysis in single line model ","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.nNodeLabelTextHeight",PROPLIST_ITEM(id++,"word height of node numbers"));
	CLDSApp::AddPropItem("g_sysPara.display.bDimRodDesignInfo",PROPLIST_ITEM(id++,"Rod Design Info"));
	CLDSApp::AddPropItem("g_sysPara.display.nRodDesignInfoTextHeight",PROPLIST_ITEM(id++,"Rod Info Word Height"));
	CLDSApp::AddPropItem("g_sysPara.m_fMaxScaleScr2User",PROPLIST_ITEM(id++,"Largest Display Scale"));
	CLDSApp::AddPropItem("CLDSNode::m_bDisplayBeforeArcLift",PROPLIST_ITEM(id++,"Pre-Lift Display State","The rod's nodes display position before or after pre-lift int single view ","Before Arc Lift|After Arc Lift"));
	CLDSApp::AddPropItem("solidDisplay",PROPLIST_ITEM(id++,"Display Solid Model"));
	CLDSApp::AddPropItem("g_sysPara.display.bDispVirtualPart",PROPLIST_ITEM(id++,"Virtual Objects","Whether display the virtual anxiliary parts","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidJg",PROPLIST_ITEM(id++,"Angle","Display the solid angle","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidTube",PROPLIST_ITEM(id++,"Tube","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLjb",PROPLIST_ITEM(id++,"Connecting Plate","Display the solid connect plate","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidJoint",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLs",PROPLIST_ITEM(id++,"Bolt","Display the solid bolt","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidSphere",PROPLIST_ITEM(id++,"Sphere"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidSlot",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidFlat",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcAngle",PROPLIST_ITEM(id++,"Arc Angle"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcFlat",PROPLIST_ITEM(id++,"Arc Flat"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcSlot",PROPLIST_ITEM(id++,"Arc Slot"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidStdPart",PROPLIST_ITEM(id++,"Std Part"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidFoundation",PROPLIST_ITEM(id++,"Foundation"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidTransectFace",PROPLIST_ITEM(id++,"Transect Face"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLine",PROPLIST_ITEM(id++,"Single Line","Display the single line when display the solid model.","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.display.nSmoothness",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("CLDSLineAngle::m_bDisplaySolidAxisLine",PROPLIST_ITEM(id++,"Angle Sign Line","Display angle's center line、height and so on","Yes|No"));
	CLDSApp::AddPropItem("CLDSGeneralPlate::m_ciDisplayVertexType",PROPLIST_ITEM(id++,"Plate Edge Vertex","Display palte's vertex","0.No|1.All|2.Vertex"));
	//文档输出
	CLDSApp::AddPropItem("DocumentExport",PROPLIST_ITEM(id++,"result output"));//求解器
	CLDSApp::AddPropItem("g_sysPara.iShowDeformedMapPlan",PROPLIST_ITEM(id++,"calculation result preview plan"));
	CLDSApp::AddPropItem("g_sysPara.bUpdateRodDesignForce",PROPLIST_ITEM(id++,"Update rod force design value"));
	CLDSApp::AddPropItem("g_sysPara.bCalStdFoundationForce",PROPLIST_ITEM(id++,"calculating basic force standard value"));
	CLDSApp::AddPropItem("g_sysPara.bRecNodeOffset",PROPLIST_ITEM(id++,"Recording the information of analysis procedure"));
	CLDSApp::AddPropItem("g_sysPara.iBriefDocType",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.iDetailDocType",PROPLIST_ITEM(id++,"document type"));
	CLDSApp::AddPropItem("g_sysPara.bMergeRptFileMirObj",PROPLIST_ITEM(id++,"Merge symmetrical items"));
	CLDSApp::AddPropItem("g_sysPara.iLegShiftType",PROPLIST_ITEM(id++,"high-low legs rotating calculation"));
	CLDSApp::AddPropItem("g_sysPara.reactionExportType",PROPLIST_ITEM(id++,"basic forces filtrating"));
	CLDSApp::AddPropItem("g_sysPara.bCreateAnsysCmdFile",PROPLIST_ITEM(id++,"Generating ANSYS command line files"));
	CLDSApp::AddPropItem("g_sysPara.iLanguage",PROPLIST_ITEM(id++,"output language"));//计算书输出语言
	CLDSApp::AddPropItem("g_sysPara.reactionExportType",PROPLIST_ITEM(id++,"basic forces filtrating"));
	CLDSApp::AddPropItem("g_sysPara.putOutM",PROPLIST_ITEM(id++,"Output of bending moment"));
	CLDSApp::AddPropItem("g_sysPara.iEfficacyCeofType",PROPLIST_ITEM(id++,"Type of margin coefficient"));
	//荷载计算
	CLDSApp::AddPropItem("WireLoadCal",PROPLIST_ITEM(id++,"Load calculation"));	//荷载计算
	CLDSApp::AddPropItem("m_cLandscape",PROPLIST_ITEM(id++,"landform of tower location"));	//电压等级
	CLDSApp::AddPropItem("m_cLineRoadType",PROPLIST_ITEM(id++,"line road condition"));	//线路路况
	CLDSApp::AddPropItem("WeatherCondition",PROPLIST_ITEM(id++,"Weather condition"));		//气象条件
	CLDSApp::AddPropItem("m_nHighestTemperature",PROPLIST_ITEM(id++,"Maximum air temperature(°C)"));//最高气温
	CLDSApp::AddPropItem("m_nLowestTemperature",PROPLIST_ITEM(id++,"Minimum air temperature(°C)"));	//最低气温
	CLDSApp::AddPropItem("m_nAverageTemperature",PROPLIST_ITEM(id++,"average temperature(°C)"));//平均温度
	CLDSApp::AddPropItem("m_nMaxWindVelocityTemperature",PROPLIST_ITEM(id++,"termperature in maximum wind velocity(°C)"));	//最大风速温度
	CLDSApp::AddPropItem("m_fMaxWindVelocity",PROPLIST_ITEM(id++,"maximum wind velocity(m/s)"));		//最大风速
	CLDSApp::AddPropItem("m_fMaxWindVelocityCheck",PROPLIST_ITEM(id++,""));	//最大风速验算
	CLDSApp::AddPropItem("m_nMaxIceTemperature",PROPLIST_ITEM(id++,"temperature in maximum icing(°C)"));	//最大覆冰温度
	CLDSApp::AddPropItem("m_fMaxIceWindVelocity",PROPLIST_ITEM(id++,"wind speed in maximum icing(m/s)"));//最大覆冰风速
	CLDSApp::AddPropItem("m_fMaxIceThick",PROPLIST_ITEM(id++,"thickness of icing(mm)"));			//最大覆冰厚度
	CLDSApp::AddPropItem("m_fMaxIceThickCheck",PROPLIST_ITEM(id++,""));		//最大覆冰厚度验算
	CLDSApp::AddPropItem("m_nSetupTemperature",PROPLIST_ITEM(id++,"temperature while installing(°C)"));	//安装温度
	CLDSApp::AddPropItem("m_fSetupWindVelocity",PROPLIST_ITEM(id++,"wind speed while installing(m/s)"));	//安装风速
	CLDSApp::AddPropItem("Ta.m_fIceDensity",PROPLIST_ITEM(id++,""));		//覆冰重度
	//单元验算
	CLDSApp::AddPropItem("MemberCheck",PROPLIST_ITEM(id++,"Unit calculation"));		//单元验算
	CLDSApp::AddPropItem("Ta.m_cStructureType",PROPLIST_ITEM(id++,"The type of structure"));		//0.单塔身主体(考虑埃菲尔效应);1.复合主体
	//应力分析计算
	CLDSApp::AddPropItem("EquationSolver",PROPLIST_ITEM(id++,"finite element solver"));//求解器
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc",PROPLIST_ITEM(id++,"Bracing rod participates in mechanical analysis"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_iResolverStyle",PROPLIST_ITEM(id++,"choose solver mode"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_iNonlinearStyle",PROPLIST_ITEM(id++,"stress analysis and calculation method"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_nLoadStepCounts",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_nIterationCounts",PROPLIST_ITEM(id++,"times of iteration"));
	CLDSApp::AddPropItem("DynamicAnalysis",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fConvergeLim",PROPLIST_ITEM(id++,"Iterations"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_nSubSpaceIterCounts",PROPLIST_ITEM(id++,"iterations of modal analysis"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fFreqsTolerance",PROPLIST_ITEM(id++,"convergence error for natural vibration frequency"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef",PROPLIST_ITEM(id++,"Reduction factor of beam transverse stiffness"));
	//设计规定
	CLDSApp::AddPropItem("TowerProperty",PROPLIST_ITEM(id++,"attribute of tower"));
	CLDSApp::AddPropItem("m_iLoadExecuteCode",PROPLIST_ITEM(id++,"rules for carrying out load calculation"));
	CLDSApp::AddPropItem("m_iMemberSizeCheckCode",PROPLIST_ITEM(id++,"rules for parts material selection"));
	CLDSApp::AddPropItem("m_iSlendernessLimCode",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("Ta.m_fVibrationT1",PROPLIST_ITEM(id++,"Fundamental natural vibration period"));
	CLDSApp::AddPropItem("SectionType",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("Ta.m_cLandscape",PROPLIST_ITEM(id++,"landform of tower location"));
	CLDSApp::AddPropItem("Ta.m_siSafetyGrade",PROPLIST_ITEM(id++,"Security level"));
	CLDSApp::AddPropItem("Ta.m_fSafetyCoef",PROPLIST_ITEM(id++,"strcucture importance coefficient"));
	CLDSApp::AddPropItem("BasicRegulations",PROPLIST_ITEM(id++,"Basic rules"));
	CLDSApp::AddPropItem("m_fLimitBearCoef",PROPLIST_ITEM(id++,"maximum design value of loading coefficient"));
	CLDSApp::AddPropItem("m_fGamaGForDesign",PROPLIST_ITEM(id++,"Optimize gravity load γG when selecting material"));
	CLDSApp::AddPropItem("m_fGamaGForFoundation",PROPLIST_ITEM(id++,"Gravity loadγG during calculation"));
	CLDSApp::AddPropItem("m_fGamaQForTowerWindLoad",PROPLIST_ITEM(id++,"Tower wind load subentry coefficientγQ"));
	//选材
	CLDSApp::AddPropItem("SelectMatRegulations",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SlendernessLimit",PROPLIST_ITEM(id++, "length and slenderness ration limit  for the other rods"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMainPoleSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure legs' tube"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nXiePoleSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure tilted material"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nAuxPoleSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure auxiliary steel"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nTensivePoleSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of the pulled steel"));
	CLDSApp::AddPropItem("TubeSlendernessLimit",PROPLIST_ITEM(id++,"proportion limit for tube's length and slenderness"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMainTubeSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure legs' tube"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nXieTubeSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure tilted material"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nAuxTubeSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of pressure auxiliary tube"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit",PROPLIST_ITEM(id++,"length and slenderness ration threshold  of the pulled tube"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_bPreferSingleAngle",PROPLIST_ITEM(id++,"Priority given to singular-angle in angle parts"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_iSizeOrderRule",PROPLIST_ITEM(id++,"rules for specification prioirity order"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nConvergeLim",PROPLIST_ITEM(id++,"accuracy of material selection"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMaxSelMatCount",PROPLIST_ITEM(id++,"Times of material selection"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinAngleSize",PROPLIST_ITEM(id++,"minimum cross-section for the other steel materials"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinForceAngleSize",PROPLIST_ITEM(id++,"minimum cross-section of the forced steel material"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinMainAngleSize",PROPLIST_ITEM(id++,"minimun cross-section of steel material"));

	CLDSApp::AddPropItem("ColorScheme",PROPLIST_ITEM(id++,"Color Scheme"));
	CLDSApp::AddPropItem("env.mcs",PROPLIST_ITEM(id++,"Model Coordinate System","","Visible|Invisible"));
	CLDSApp::AddPropItem("env.mcs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSApp::AddPropItem("env.mcs.axis_x.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("env.mcs.axis_x.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("env.mcs.axis_x.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("env.mcs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSApp::AddPropItem("env.mcs.axis_y.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("env.mcs.axis_y.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("env.mcs.axis_y.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("env.mcs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSApp::AddPropItem("env.mcs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("env.mcs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("env.mcs.axis_z.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispPartACS",PROPLIST_ITEM(id++,"Part Assemble CS","The assemble CS of selected part。","Visible|Invisible"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispWorkUcs",PROPLIST_ITEM(id++,"WCS","Operator input some space position or direction data, in order to determine, can enter relative coordinates int work CS。","Visible|Invisible"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispBlockLCS",PROPLIST_ITEM(id++,"Block Location CS","The assemble location CS of  block","Visible|Invisible"));
	//自定义工作坐标系
	CLDSApp::AddPropItem("work_ucs.m_eDatumPlane",PROPLIST_ITEM(id++,"Datum Plane","Specify two axis, automatically calculate the perpendicular with current direction,the first axis direction invariable fixed the second axis direction. Such as: x-y plane, first in terms of X&Y to calculate Z axis direction, with X&Z axis fixed Y direction。"));
	CLDSApp::AddPropItem("work_ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSApp::AddPropItem("work_ucs.origin.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("work_ucs.origin.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("work_ucs.origin.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("work_ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSApp::AddPropItem("work_ucs.axis_x.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("work_ucs.axis_x.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("work_ucs.axis_x.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("work_ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSApp::AddPropItem("work_ucs.axis_y.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("work_ucs.axis_y.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("work_ucs.axis_y.z",PROPLIST_ITEM(id++,"Z"));
	CLDSApp::AddPropItem("work_ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSApp::AddPropItem("work_ucs.axis_z.x",PROPLIST_ITEM(id++,"X"));
	CLDSApp::AddPropItem("work_ucs.axis_z.y",PROPLIST_ITEM(id++,"Y"));
	CLDSApp::AddPropItem("work_ucs.axis_z.z",PROPLIST_ITEM(id++,"Z"));

	CLDSApp::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.TubeParam",PROPLIST_ITEM(id++,"Tube's parameters"));
	CLDSApp::AddPropItem("g_sysPara.PlateParam",PROPLIST_ITEM(id++,"Plate's parameters"));
	CLDSApp::AddPropItem("g_sysPara.AngleParam",PROPLIST_ITEM(id++,"Angle's parameters"));
	CLDSApp::AddPropItem("g_sysPara.BoltParam",PROPLIST_ITEM(id++,"Bolts and Nails Parameters"));
	CLDSApp::AddPropItem("g_sysPara.m_bModifyCoupledFLTubeOddment",PROPLIST_ITEM(id++,"Modify Coupled FL Tube Oddment","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.HuoQuDist",PROPLIST_ITEM(id++,"Bending Line Clearance"));
	CLDSApp::AddPropItem("g_sysPara.JgEndDist",PROPLIST_ITEM(id++,"Angle Dock Clearance"));
	CLDSApp::AddPropItem("g_sysPara.JointPlankDist",PROPLIST_ITEM(id++,"Joint & Plate Clearance"));
	CLDSApp::AddPropItem("g_sysPara.TubeFlDist",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("CLDSLineTube::ciFlTubeOddment",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.VertexDist",PROPLIST_ITEM(id++,"Vertex & Vertex Clearance"));
	CLDSApp::AddPropItem("g_sysPara.FootNailDist",PROPLIST_ITEM(id++,"Footnail Distance"));
	CLDSApp::AddPropItem("g_sysPara.fStressDiffuseAngle",PROPLIST_ITEM(id++,"Stress Dispersion Angle"));
	CLDSApp::AddPropItem("g_sysPara.fInsertPlateEdgeSpace",PROPLIST_ITEM(id++,"Board Edge Space","The edge space of board affects outline design of board"));
	CLDSApp::AddPropItem("g_sysPara.m_bDelBoltsResideOnPlate",PROPLIST_ITEM(id++,"Delete Bolts Reside On Plate","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.ciLinkAngleDlgFocusCtrlMode",PROPLIST_ITEM(id++,"New angle dialog focus control","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.fNailRepScope",PROPLIST_ITEM(id++,"FootNail Replace Hole Scope"));
	CLDSApp::AddPropItem("g_sysPara.fTubeWeldRoadMinD",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("MaxWasherN",PROPLIST_ITEM(id++,"Max washer count on 1 bolt"));
	CLDSApp::AddPropItem("BoltPadThick",PROPLIST_ITEM(id++,"Single Bolt Washer Minimum Thickness"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM12",PROPLIST_ITEM(id++,"M12"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM16",PROPLIST_ITEM(id++,"M16"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM20",PROPLIST_ITEM(id++,"M20"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM24",PROPLIST_ITEM(id++,"M24"));
	CLDSApp::AddPropItem("g_sysPara.m_nPlankEdgeAngleThreshold",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("m_iProfileAlgorithm",PROPLIST_ITEM(id++,"Profile Create Algorithm","","0.Traditional Algorithm|1.Improved Algorithm"));
	CLDSApp::AddPropItem("g_sysPara.nAutoAdjustTubePos",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("CLDSPlate::m_cRollProcessType",PROPLIST_ITEM(id++,"Rolling Style of Plate Edge"));
	CLDSApp::AddPropItem("g_sysPara.m_fKaiHeJiaoThreshold",PROPLIST_ITEM(id++,"Angle open and merge dimension threshold(°)"));
	CLDSApp::AddPropItem("g_sysPara.m_nGroupJgMachiningGap",PROPLIST_ITEM(id++,"Group Angle machining gap"));
	CLDSApp::AddPropItem("g_sysPara.m_fKaiHeJiaoMinDeita",PROPLIST_ITEM(id++,"Min security clearance of open-close","","0.None|1.Before Label|2.After Label"));
	CLDSApp::AddPropItem("g_sysPara.m_nMatCharPosType",PROPLIST_ITEM(id++,"Part Material Word Position","0.No|1.Spacing"));
	CLDSApp::AddPropItem("g_sysPara.m_cMatCharSeparator",PROPLIST_ITEM(id++,"Part Material Word Separator"));
	CLDSApp::AddPropItem("g_sysPara.m_fMinCutAngleLen",PROPLIST_ITEM(id++,"The minimum of cut angle"));
	CLDSApp::AddPropItem("g_sysPara.m_fMaxIgnoreCutAngleLen",PROPLIST_ITEM(id++,"Maximum ignore cut angle"));
	CLDSApp::AddPropItem("g_sysPara.b1BoltDesignPrompt",PROPLIST_ITEM(id++,"Gasket Parameter Dialog","","Yes|No"));
	CLDSApp::AddPropItem("CLDSBolt::BOLTL0_CALMODE", PROPLIST_ITEM(id++, "Computing Model", "", "0.Sum Part Thick|1.Include Part Gap"));
	CLDSApp::AddPropItem("CLDSBolt::BOLTL0_PREFER_MODE", PROPLIST_ITEM(id++, "Computing Model Init Value", "", "0.Sum Part Thick|1.Include Part Gap|2.Bolt Self Property"));
	CLDSApp::AddPropItem("g_sysPara.m_b45CutAngleNeedCoefR",PROPLIST_ITEM(id++,"Is Need Inner Arc Cut Ratio","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.m_fCutAngleCoefR",PROPLIST_ITEM(id++,"Inner Arc Cut Ratio","","1/3|2/3"));
	CLDSApp::AddPropItem("g_sysPara.m_bAnglePreferSimplePosMode",PROPLIST_ITEM(id++,"Angle's prefer location mode","","Simple|Parametric"));
	CLDSApp::AddPropItem("g_sysPara.display.cHoleWallMode",PROPLIST_ITEM(id++,"Hole wall visible","The display mode of holes in all of parts","0.None|1.Virtual bolt hole|2.All holes"));
	CLDSApp::AddPropItem("CLDSPart::UPDATE_PARTNO_AND_SEG",PROPLIST_ITEM(id++,"SegI&Label	of part synchornize","","Yes|No"));
	CLDSApp::AddPropItem("Ta.m_cRollProcessType",PROPLIST_ITEM(id++,"Roll Process Type","","0.Mechanical Priority|1.Processing Priority 1|2.Processing Priority 2"));
	CLDSApp::AddPropItem("Ta.m_cRollOffsetType",PROPLIST_ITEM(id++,"RollLine Cal Type","","0.Outside|1.Inside"));
	CLDSApp::AddPropItem("Ta.m_cDeformType",PROPLIST_ITEM(id++,"Deform Type","","0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane"));
	CLDSApp::AddPropItem("Ta.m_fDeformCoef",PROPLIST_ITEM(id++,"Deform Coef",""));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bOnlyDesignCurView",PROPLIST_ITEM(id++,"Only Design Current View","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignCommonConnect",PROPLIST_ITEM(id++,"Desing Common Connect","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignBendConnect",PROPLIST_ITEM(id++,"Desing Connect With Two Connect Planes","","Yes|No"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignAngleJoint",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignFL",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SyncProp",PROPLIST_ITEM(id++,"Associate Object Synchronous Property"));
	//显示提示框
	CLDSApp::AddPropItem("g_sysPara.m_bEnablePrompt",PROPLIST_ITEM(id++,"Whether Enable Prompt Dialog","","Yes|No"));
	//抗震分析
	CLDSApp::AddPropItem("EarthQuake",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.grade",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.AlfaMax",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.Tg",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.zeta",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.gamaRE",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.nFreqsRank",PROPLIST_ITEM(id++,""));
	//同步属性设置
	CLDSApp::AddPropItem("SyncProp",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SyncProp.Node",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SyncProp.Angle",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SyncProp.Tube",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("SyncProp.Plate",PROPLIST_ITEM(id++,""));
#else
	CLDSApp::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息"));
	CLDSApp::AddPropItem("DrawingInfo",PROPLIST_ITEM(id++,"绘图信息"));
	CLDSApp::AddPropItem("PartLibrary",PROPLIST_ITEM(id++,"构件库"));
	CLDSApp::AddPropItem("g_sysPara.nAutoSaveTime",PROPLIST_ITEM(id++,"自动存盘间隔时间"));
	CLDSApp::AddPropItem("g_sysPara.iAutoSaveType",PROPLIST_ITEM(id++,"自动存盘方式"));
	CLDSApp::AddPropItem("g_sysPara.uFileSaveVersion",PROPLIST_ITEM(id++,"另存旧版本"));
	CLDSApp::AddPropItem("BatchSaveAsOldFile",PROPLIST_ITEM(id++,"批量另存旧版本"));
	CLDSApp::AddPropItem("g_sysPara.iFileSaveDocType",PROPLIST_ITEM(id++,"另存其余文件格式"));
	CLDSApp::AddPropItem("Ta.m_fTowerHeight",PROPLIST_ITEM(id++,"杆塔高度"));
	CLDSApp::AddPropItem("Ta.segstr",PROPLIST_ITEM(id++,"显示段号范围","当前显示段号范围，*表示全部显示","排除段号"));
	CLDSApp::AddPropItem("env.m_sUserName",PROPLIST_ITEM(id++,"用户名"));

	CLDSApp::AddPropItem("env.m_sShortUserName",PROPLIST_ITEM(id++,"用户名缩写"));
	CLDSApp::AddPropItem("env.m_sAcadPath",PROPLIST_ITEM(id++,"定位绘图程序"));
	CLDSApp::AddPropItem("g_sysPara.m_ciLowestWarningLevel",PROPLIST_ITEM(id++,"警告等级"));
	CLDSApp::AddPropItem("FileRW.BUFFSIZE",PROPLIST_ITEM(id++,"文件读写工作缓存(MB)"));
	CLDSApp::AddPropItem("env.m_sJgZhunJuTbl",PROPLIST_ITEM(id++,"角钢准距库","角钢准距库,随文件保存。"));
	CLDSApp::AddPropItem("env.m_sJgGuiGeTbl",PROPLIST_ITEM(id++,"角钢规格库"));
	CLDSApp::AddPropItem("env.m_sJgGuiGeTblPath",PROPLIST_ITEM(id++,"角钢规格库文件","指定角钢规格库文件将会在程序新打开时初始化角钢规格库。"));
	CLDSApp::AddPropItem("env.m_sTubeGuiGeTbl",PROPLIST_ITEM(id++,"钢管规格库"));
	CLDSApp::AddPropItem("env.m_sTubeGuiGeTblPath",PROPLIST_ITEM(id++,"钢管规格库文件","指定钢管规格库文件将会在程序新打开时初始化钢管规格库。"));
	CLDSApp::AddPropItem("env.m_sFlatGuiGeTbl",PROPLIST_ITEM(id++,"扁铁规格库"));
	CLDSApp::AddPropItem("env.m_sFlatGuiGeTblPath",PROPLIST_ITEM(id++,"扁铁规格库文件"));
	CLDSApp::AddPropItem("env.m_sSlotGuiGeTbl",PROPLIST_ITEM(id++,"槽钢规格库"));
	CLDSApp::AddPropItem("env.m_sSlotGuiGeTblPath",PROPLIST_ITEM(id++,"槽钢规格库文件"));
	CLDSApp::AddPropItem("env.m_sHWGuiGeTbl",PROPLIST_ITEM(id++,"HW型钢规格库"));
	CLDSApp::AddPropItem("env.m_sHWGuiGeTblPath",PROPLIST_ITEM(id++,"HW型钢规格库文件"));
	CLDSApp::AddPropItem("env.m_sHMGuiGeTbl",PROPLIST_ITEM(id++,"HM型钢规格库"));
	CLDSApp::AddPropItem("env.m_sHMGuiGeTblPath",PROPLIST_ITEM(id++,"HM型钢规格库文件"));
	CLDSApp::AddPropItem("env.m_sHNGuiGeTbl",PROPLIST_ITEM(id++,"HN型钢规格库"));
	CLDSApp::AddPropItem("env.m_sHNGuiGeTblPath",PROPLIST_ITEM(id++,"HN型钢规格库文件"));
	CLDSApp::AddPropItem("env.m_sHPGuiGeTbl",PROPLIST_ITEM(id++,"HP型钢规格库"));
	CLDSApp::AddPropItem("env.m_sHPGuiGeTblPath",PROPLIST_ITEM(id++,"HP型钢规格库文件"));
	CLDSApp::AddPropItem("env.m_sFittingGuiGeTbl",PROPLIST_ITEM(id++,"金具规格库"));
	CLDSApp::AddPropItem("env.m_sEBGuiGeTbl",PROPLIST_ITEM(id++,"EB挂板库"));
	CLDSApp::AddPropItem("env.m_sGDGuiGeTbl",PROPLIST_ITEM(id++,"GD挂板库"));
	CLDSApp::AddPropItem("env.m_sURGuiGeTbl",PROPLIST_ITEM(id++,"U型环库"));
	CLDSApp::AddPropItem("env.m_sJgJointParaTbl",PROPLIST_ITEM(id++,"角钢接头库"));
	CLDSApp::AddPropItem("env.m_sJgJointParaTblPath",PROPLIST_ITEM(id++,"角钢接头库文件"));
	CLDSApp::AddPropItem("ANCHOR_BOLT_LIB",PROPLIST_ITEM(id++,"地脚螺栓","地脚螺栓库"));
	CLDSApp::AddPropItem("env.m_sLsSpaceTbl",PROPLIST_ITEM(id++,"螺栓间距库","螺栓间距库,随文件保存。"));
	CLDSApp::AddPropItem("env.m_sLsSpaceTblPath",PROPLIST_ITEM(id++,"螺栓间距库文件"));
	CLDSApp::AddPropItem("env.m_sLsGuiGeTbl",PROPLIST_ITEM(id++,"螺栓规格系列","螺栓规格系列,随文件保存。"));
	CLDSApp::AddPropItem("env.m_sLsSubPartTbl",PROPLIST_ITEM(id++,"螺栓零件库"));
	CLDSApp::AddPropItem("TwoCapM16IncreL",PROPLIST_ITEM(id++,"M16双帽螺栓长增量"));
	CLDSApp::AddPropItem("TwoCapM20IncreL",PROPLIST_ITEM(id++,"M20双帽螺栓长增量"));
	CLDSApp::AddPropItem("TwoCapM24IncreL",PROPLIST_ITEM(id++,"M24双帽螺栓长增量"));
	CLDSApp::AddPropItem("env.m_sWireGuiGeTbl",PROPLIST_ITEM(id++,"线缆规格库"));
	CLDSApp::AddPropItem("env.m_sLsSelectTbl",PROPLIST_ITEM(id++,"法兰螺栓选择表"));	//法兰螺栓选材表
	CLDSApp::AddPropItem("env.m_sSteelMatTbl",PROPLIST_ITEM(id++,"材料库"));	//材料库
	CLDSApp::AddPropItem("env.m_sInterpolateBetaZTbl",PROPLIST_ITEM(id++,"风振系数插值表","风振系数(即杆塔风荷载调整系数)插值计算时的,高度插值表"));	//风振系数插值表
	CLDSApp::AddPropItem("env.m_sInterpIceLoadCoefTbl",PROPLIST_ITEM(id++,"杆塔覆冰重力荷载系数表"));	//杆塔覆冰重力荷载系数表
	CLDSApp::AddPropItem("Ta.listSelBoltCond",PROPLIST_ITEM(id++,"角钢螺栓选择表","根据角钢肢宽选择螺栓规格及等级"));	//螺栓选择条件
	CLDSApp::AddPropItem("InsertPlateLib",PROPLIST_ITEM(id++,"钢管管头插板库"));		//钢管管头插板库
	CLDSApp::AddPropItem("env.m_sCInsertPlateLib",PROPLIST_ITEM(id++,"槽型插板库"));	//槽型插板库
	CLDSApp::AddPropItem("env.m_sUInsertPlateLib",PROPLIST_ITEM(id++,"U型插板库"));	//U型插板库
	CLDSApp::AddPropItem("env.m_sXInsertPlateLib",PROPLIST_ITEM(id++,"十字插板库"));	//十字插板库
	CLDSApp::AddPropItem("env.m_sCInsertPlateLibPath",PROPLIST_ITEM(id++,"槽型插板库路径"));	//槽型插板库路径
	CLDSApp::AddPropItem("env.m_sUInsertPlateLibPath",PROPLIST_ITEM(id++,"U型插板库路径"));  //U型插板库路径
	CLDSApp::AddPropItem("env.m_sXInsertPlateLibPath",PROPLIST_ITEM(id++,"十字型插板路径"));	//十字型插板路径
	CLDSApp::AddPropItem("StdPartLib",PROPLIST_ITEM(id++,"标准构件库","标准构件库,随文件保存。"));			//标准构件库
	CLDSApp::AddPropItem("StdPartFlLib",PROPLIST_ITEM(id++,"法兰规格库"));			//法兰规格库
	CLDSApp::AddPropItem("env.m_sStdPartFlDLibPath",PROPLIST_ITEM(id++,"对焊法兰规格库文件路径"));//对焊法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlPLibPath",PROPLIST_ITEM(id++,"平焊法兰规格库文件路径"));//平焊法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlGLibPath",PROPLIST_ITEM(id++,"刚性法兰规格库文件路径"));//刚性法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sStdPartFlRLibPath",PROPLIST_ITEM(id++,"柔性法兰规格库文件路径"));//柔性法兰规格库文件路径
	CLDSApp::AddPropItem("env.m_sFlCoupleLevelPath",PROPLIST_ITEM(id++,"对接法兰大小头级别定义文件路径"));//对接法兰大小头级别定义文件路径
	CLDSApp::AddPropItem("m_dwSelectPartsType",PROPLIST_ITEM(id++,"构件选择过滤器","选择构件时只能选择通过过滤器的构件"));
	CLDSApp::AddPropItem("CanSelectNode",PROPLIST_ITEM(id++,"节点","是否可选中节点","是|否"));
	CLDSApp::AddPropItem("CanSelectLineAngle",PROPLIST_ITEM(id++,"角钢","是否可选中角钢","是|否"));
	CLDSApp::AddPropItem("CanSelectLineTube",PROPLIST_ITEM(id++,"钢管","是否可选中钢管","是|否"));
	CLDSApp::AddPropItem("CanSelectPlate",PROPLIST_ITEM(id++,"钢板","是否可选中钢板","是|否"));
	CLDSApp::AddPropItem("CanSelectParamPlate",PROPLIST_ITEM(id++,"参数化钢板","是否可选中参数化板","是|否"));
	CLDSApp::AddPropItem("CanSelectBolt",PROPLIST_ITEM(id++,"螺栓","是否可选中螺栓","是|否"));
	CLDSApp::AddPropItem("CanSelectSphere",PROPLIST_ITEM(id++,"球体","是否可选中球体","是|否"));
	CLDSApp::AddPropItem("CanSelectLineSlot",PROPLIST_ITEM(id++,"槽钢","是否可选中槽钢","是|否"));
	CLDSApp::AddPropItem("CanSelectLineFlat",PROPLIST_ITEM(id++,"扁钢","是否可选中扁钢","是|否"));
	CLDSApp::AddPropItem("CanSelectArcAngle",PROPLIST_ITEM(id++,"环形角钢","是否可选中环形角钢","是|否"));
	CLDSApp::AddPropItem("CanSelectArcFlat",PROPLIST_ITEM(id++,"环形扁钢","是否可选中环形扁钢","是|否"));
	CLDSApp::AddPropItem("CanSelectArcSlot",PROPLIST_ITEM(id++,"环形槽钢","是否可选中环形槽钢","是|否"));
	CLDSApp::AddPropItem("CanSelectStdPart",PROPLIST_ITEM(id++,"标准构件","是否可选中标准构件","是|否"));
	CLDSApp::AddPropItem("CanSelectEdgeLine",PROPLIST_ITEM(id++,"轮廓面边界线","是否可选中轮廓面边界线","是|否")); //轮廓面边界线
	CLDSApp::AddPropItem("DisplayScope",PROPLIST_ITEM(id++,"显示范围"));
	CLDSApp::AddPropItem("DisplayMode",PROPLIST_ITEM(id++,"显示模式"));
	CLDSApp::AddPropItem("env.layer",PROPLIST_ITEM(id++,"当前图层"));
	CLDSApp::AddPropItem("env.flieProp",PROPLIST_ITEM(id++,"文件属性"));
	CLDSApp::AddPropItem("Ta.version",PROPLIST_ITEM(id++,"文件版本号","文件存储格式的版本号"));
	CLDSApp::AddPropItem("Ta.m_uiOriginalDogKey",PROPLIST_ITEM(id++,"原始加密锁号","文件最初创建时的加密锁号"));
	CLDSApp::AddPropItem("Ta.m_uiLastSaveDogKey",PROPLIST_ITEM(id++,"当前加密锁号","文件最近一次存储使用的加密锁号"));
	CLDSApp::AddPropItem("ModelInfo",PROPLIST_ITEM(id++,"模型信息"));
	CLDSApp::AddPropItem("Ta.Node.GetNodeNum",PROPLIST_ITEM(id++,"节点数量"));
	CLDSApp::AddPropItem("Ta.Parts.GetNodeNum",PROPLIST_ITEM(id++,"构件数量"));
	CLDSApp::AddPropItem("Ta.m_fNamedHeightZeroZ",PROPLIST_ITEM(id++,"呼高基点Z值"));
	CLDSApp::AddPropItem("Ta.m_sPrjName",PROPLIST_ITEM(id++,"工程名称"));
	CLDSApp::AddPropItem("Ta.m_sTaType",PROPLIST_ITEM(id++,"塔型"));
	CLDSApp::AddPropItem("Ta.m_nTaNum",PROPLIST_ITEM(id++,"基数"));
	CLDSApp::AddPropItem("Ta.m_sTaAlias",PROPLIST_ITEM(id++,"代号","塔型代号"));
	CLDSApp::AddPropItem("Ta.m_sTaStampNo",PROPLIST_ITEM(id++,"钢印号","构件上冲压的工程塔型及件号标识"));
	CLDSApp::AddPropItem("Ta.m_sOperator",PROPLIST_ITEM(id++,"操作员","放样人员"));
	CLDSApp::AddPropItem("Ta.m_sAuditor",PROPLIST_ITEM(id++,"审核员"));
	CLDSApp::AddPropItem("Ta.m_ciModelArchitecture",PROPLIST_ITEM(id++,"结构类型"));
	helpStr.Copy("V2(TMAV 5.1之后)到位算法，对心线交点计算及搭接角钢工作肢法向偏移的到位算法进行了细微调整,"\
				 "最新算法(TMAV 5.1.7之后)，对含有圆弧预拱项的搭接于平推位置的角钢到位算法进行了微调。");
	CLDSApp::AddPropItem("Ta.m_bRepositionByModeBeforeV51",PROPLIST_ITEM(id++,"旧算法到位计算",helpStr,"新算法|V2 旧算法|V1 旧算法"));
	CLDSApp::AddPropItem("m_iPlankWeightStatType",PROPLIST_ITEM(id++,"连扳面积统计方式","","0.最小包络矩形面积|1.净面积"));
	CLDSApp::AddPropItem("m_iCirclePlankAreaType",PROPLIST_ITEM(id++,"环扳面积计算方式","","0.最小包络形面积|1.净面积"));
	helpStr.Copy("最佳受力:展开后倒角处为两条直线, 最少刀数1:保证卷边外侧边长度为L-N,最少刀数2:保证卷边内侧边长度为L-N。钢板只有 最少刀数1，最少刀数1=最少刀数2");
	CLDSApp::AddPropItem("Ta.m_cRollProcessType",PROPLIST_ITEM(id++,"卷边处理方式",helpStr,"0.最佳受力|1.最少刀数1|2.最少刀数2"));
	CLDSApp::AddPropItem("Ta.m_cRollOffsetType",PROPLIST_ITEM(id++,"卷边偏移方式","","0.外侧|1.内侧"));
	CLDSApp::AddPropItem("Ta.m_cDeformType",PROPLIST_ITEM(id++,"火曲变形处理方式","","0.装配面|1.中性层|2.凸面|3.凹面"));
	CLDSApp::AddPropItem("Ta.m_fDeformCoef",PROPLIST_ITEM(id++,"中性层系数",""));
	CLDSApp::AddPropItem("g_sysPara.m_bSaveLogVersion",PROPLIST_ITEM(id++,"保存更改日志"));
	CLDSApp::AddPropItem("g_sysPara.m_iModFileCodeType",PROPLIST_ITEM(id++,"Mod文件编码","","0.UTF-8|1.ANSI"));
	CLDSApp::AddPropItem("g_sysPara.fRotateVelocity",PROPLIST_ITEM(id++,"旋转速度"));
	CLDSApp::AddPropItem("g_sysPara.m_bLineDepthTest",PROPLIST_ITEM(id++,"单线模型自动消隐","","开启|关闭"));
	CLDSApp::AddPropItem("g_sysPara.m_bDragOpenWndSel",PROPLIST_ITEM(id++,"鼠标拖拽开窗","","开启|关闭"));
	CLDSApp::AddPropItem("g_sysPara.m_bDblClkStartRotateOper",PROPLIST_ITEM(id++,"双击启动旋转","","开启|关闭"));
	CLDSApp::AddPropItem("lineDisplay",PROPLIST_ITEM(id++,"单线显示"));
	CLDSApp::AddPropItem("g_sysPara.display.bNodePnt",PROPLIST_ITEM(id++,"节点","显示单线节点","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bPoleLine",PROPLIST_ITEM(id++,"杆件简化线","显示杆件简化线","是|否"));
	CLDSApp::AddPropItem("CLDSLinePart::bDisplayLineVecMark",PROPLIST_ITEM(id++,"杆件起始端标记","显示杆件起始端标记","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bPartSketch",PROPLIST_ITEM(id++,"构件轮廓边","选中单个构件时显示构件轮廓边以及轮廓顶点,刷新后消失!","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bLoadArrow",PROPLIST_ITEM(id++,"荷载箭头","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.nLoadArrowScale",PROPLIST_ITEM(id++,"荷载箭头大小"));
	CLDSApp::AddPropItem("g_sysPara.display.bWirePoint",PROPLIST_ITEM(id++,"挂点箭头","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.xPointStyle.cbPointSize",PROPLIST_ITEM(id++,"节点显示大小"));
	CLDSApp::AddPropItem("g_sysPara.display.xPointStyle.ciPointStyle",PROPLIST_ITEM(id++,"节点显示类型","","0.方点|1.圆点"));
	CLDSApp::AddPropItem("g_sysPara.display.bDimNodeLabel",PROPLIST_ITEM(id++,"节点编号","是否在力学分析单线模式显示节点编号","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.nNodeLabelTextHeight",PROPLIST_ITEM(id++,"节点编号字高"));
	CLDSApp::AddPropItem("g_sysPara.display.bDimRodDesignInfo",PROPLIST_ITEM(id++,"杆件规格信息"));
	CLDSApp::AddPropItem("g_sysPara.display.nRodDesignInfoTextHeight",PROPLIST_ITEM(id++,"杆件设计信息字高"));
	CLDSApp::AddPropItem("g_sysPara.m_fMaxScaleScr2User",PROPLIST_ITEM(id++,"最大缩放显示比例","局部放大定位时的最大缩放显示比例"));
	CLDSApp::AddPropItem("CLDSNode::m_bDisplayBeforeArcLift",PROPLIST_ITEM(id++,"预拱显示状态","单线视图中杆件节点显示为预拱前或预拱后位置","预拱前|预拱后"));
	CLDSApp::AddPropItem("solidDisplay",PROPLIST_ITEM(id++,"实体显示"));
	CLDSApp::AddPropItem("g_sysPara.display.bDispVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件","是否显示虚拟辅助构件","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidJg",PROPLIST_ITEM(id++,"角钢","显示实体角钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidTube",PROPLIST_ITEM(id++,"钢管","显示实体钢管","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLjb",PROPLIST_ITEM(id++,"连接板","显示实体连接板","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLs",PROPLIST_ITEM(id++,"螺栓","显示实体螺栓","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidSphere",PROPLIST_ITEM(id++,"球体","显示球体","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidSlot",PROPLIST_ITEM(id++,"槽钢","显示实体槽钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidFlat",PROPLIST_ITEM(id++,"扁钢","显示实体扁钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcAngle",PROPLIST_ITEM(id++,"环形角钢","显示实体环形角钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcFlat",PROPLIST_ITEM(id++,"环形扁钢","显示实体环形扁钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidArcSlot",PROPLIST_ITEM(id++,"环形槽钢","显示实体环形槽钢","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidStdPart",PROPLIST_ITEM(id++,"标准构件","显示实体标准构件","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidFoundation",PROPLIST_ITEM(id++,"基础/地脚螺栓","显示铁塔基础及地脚螺栓","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidTransectFace",PROPLIST_ITEM(id++,"相贯面","显示实体相贯时产生的相贯面，尤其是指钢管间的焊接相贯面。","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.bSolidLine",PROPLIST_ITEM(id++,"单线","显示实体同时显示单线","是|否"));
	CLDSApp::AddPropItem("g_sysPara.display.nSmoothness",PROPLIST_ITEM(id++,"光滑度","实体显示时圆弧分解成的最小面数,面数越多显示越细腻但显示速度会变慢"));
	CLDSApp::AddPropItem("CLDSLineAngle::m_bDisplaySolidAxisLine",PROPLIST_ITEM(id++,"角钢标识线","显示角钢心线、厚度边消隐线等","是|否"));
	CLDSApp::AddPropItem("CLDSGeneralPlate::m_ciDisplayVertexType",PROPLIST_ITEM(id++,"钢板轮廓点","显示钢板轮廓点","0.不显示轮廓点|1.显示带标签轮廓点|2.只显示轮廓点"));
	//文档输出
	CLDSApp::AddPropItem("DocumentExport",PROPLIST_ITEM(id++,"结果输出"));//结果输出
	CLDSApp::AddPropItem("g_sysPara.iShowDeformedMapPlan",PROPLIST_ITEM(id++,"计算结果预览方案"));
	CLDSApp::AddPropItem("g_sysPara.bUpdateRodDesignForce",PROPLIST_ITEM(id++,"更新杆件作用力设计值"));
	CLDSApp::AddPropItem("g_sysPara.bCalStdFoundationForce",PROPLIST_ITEM(id++,"计算基础作用力标准值"));
	CLDSApp::AddPropItem("g_sysPara.bRecNodeOffset",PROPLIST_ITEM(id++,"记录分析过程信息"));
	CLDSApp::AddPropItem("g_sysPara.iDetailDocType",PROPLIST_ITEM(id++,"文档类型"));
	CLDSApp::AddPropItem("g_sysPara.bMergeRptFileMirObj",PROPLIST_ITEM(id++,"合并对称项"));
	CLDSApp::AddPropItem("g_sysPara.iLegShiftType",PROPLIST_ITEM(id++,"高低腿轮换计算"));
	CLDSApp::AddPropItem("g_sysPara.reactionExportType",PROPLIST_ITEM(id++,"基础作用力筛选"));
	CLDSApp::AddPropItem("g_sysPara.bCreateAnsysCmdFile",PROPLIST_ITEM(id++,"生成ANSYS命令行文件"));
	CLDSApp::AddPropItem("g_sysPara.iLanguage",PROPLIST_ITEM(id++,"输出语言"));//计算书输出语言
	CLDSApp::AddPropItem("g_sysPara.putOutM",PROPLIST_ITEM(id++,"弯矩项输出"));
	CLDSApp::AddPropItem("g_sysPara.iEfficacyCeofType",PROPLIST_ITEM(id++,"单元效率系数类型"));
	//荷载计算
	CLDSApp::AddPropItem("WireLoadCal",PROPLIST_ITEM(id++,"荷载计算"));	//荷载计算
	CLDSApp::AddPropItem("m_cLandscape",PROPLIST_ITEM(id++,"塔位地貌"));	//电压等级
	CLDSApp::AddPropItem("m_cLineRoadType",PROPLIST_ITEM(id++,"线路路况"));	//线路路况
	CLDSApp::AddPropItem("WeatherCondition",PROPLIST_ITEM(id++,"气象条件"));		//气象条件
	CLDSApp::AddPropItem("m_nHighestTemperature",PROPLIST_ITEM(id++,"最高气温"));//最高气温
	CLDSApp::AddPropItem("m_nLowestTemperature",PROPLIST_ITEM(id++,"最低气温"));	//最低气温
	CLDSApp::AddPropItem("m_nAverageTemperature",PROPLIST_ITEM(id++,"平均温度"));//平均温度
	CLDSApp::AddPropItem("m_nMaxWindVelocityTemperature",PROPLIST_ITEM(id++,"最大风速温度"));	//最大风速温度
	CLDSApp::AddPropItem("m_fMaxWindVelocity",PROPLIST_ITEM(id++,"最大风速"));		//最大风速
	CLDSApp::AddPropItem("m_fMaxWindVelocityCheck",PROPLIST_ITEM(id++,"最大风速验算"));	//最大风速验算
	CLDSApp::AddPropItem("m_nMaxIceTemperature",PROPLIST_ITEM(id++,"最大覆冰温度"));	//最大覆冰温度
	CLDSApp::AddPropItem("m_fMaxIceWindVelocity",PROPLIST_ITEM(id++,"最大覆冰风速"));//最大覆冰风速
	CLDSApp::AddPropItem("m_fMaxIceThick",PROPLIST_ITEM(id++,"最大覆冰厚度"));			//最大覆冰厚度
	CLDSApp::AddPropItem("m_fMaxIceThickCheck",PROPLIST_ITEM(id++,"最大覆冰厚度验算"));		//最大覆冰厚度验算
	CLDSApp::AddPropItem("m_nSetupTemperature",PROPLIST_ITEM(id++,"安装温度"));	//安装温度
	CLDSApp::AddPropItem("m_fSetupWindVelocity",PROPLIST_ITEM(id++,"安装风速"));	//安装风速
	CLDSApp::AddPropItem("Ta.m_fIceDensity",PROPLIST_ITEM(id++,"覆冰重度"));		//覆冰重度
	//单元验算
	CLDSApp::AddPropItem("MemberCheck",PROPLIST_ITEM(id++,"单元验算"));		//单元验算
	CLDSApp::AddPropItem("Ta.m_cStructureType",PROPLIST_ITEM(id++,"结构类型"));		//0.单塔身主体(考虑埃菲尔效应);1.复合主体
	//应力分析计算
	CLDSApp::AddPropItem("EquationSolver",PROPLIST_ITEM(id++,"有限元求解器"));//求解器
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc",PROPLIST_ITEM(id++,"辅助材参预力学分析"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_iResolverStyle",PROPLIST_ITEM(id++,"求解器方式选择"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_iNonlinearStyle",PROPLIST_ITEM(id++,"应力分析计算方式"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_nIterationCounts",PROPLIST_ITEM(id++,"迭代次数"));
    CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fConvergeLim",PROPLIST_ITEM(id++,"收敛阈值"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_nSubSpaceIterCounts",PROPLIST_ITEM(id++,"模态分析迭代次数"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fFreqsTolerance",PROPLIST_ITEM(id++,"自振频率收敛误差"));
	CLDSApp::AddPropItem("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef",PROPLIST_ITEM(id++,"梁横向刚度折减系数"));
	//设计规定
	CLDSApp::AddPropItem("TowerProperty",PROPLIST_ITEM(id++,"杆塔属性"));
	CLDSApp::AddPropItem("m_iLoadExecuteCode",PROPLIST_ITEM(id++,"荷载计算执行规范"));
	CLDSApp::AddPropItem("m_iMemberSizeCheckCode",PROPLIST_ITEM(id++,"构件选材计算执行规范"));
	CLDSApp::AddPropItem("Ta.m_fVibrationT1",PROPLIST_ITEM(id++,"基本自振周期"));
	CLDSApp::AddPropItem("Ta.m_cLandscape",PROPLIST_ITEM(id++,"塔位地形"));
	CLDSApp::AddPropItem("Ta.m_siSafetyGrade",PROPLIST_ITEM(id++,"安全等级"));
	CLDSApp::AddPropItem("Ta.m_fSafetyCoef",PROPLIST_ITEM(id++,"结构重要性系数"));
	CLDSApp::AddPropItem("BasicRegulations",PROPLIST_ITEM(id++,"基本规定"));
	CLDSApp::AddPropItem("m_fLimitBearCoef",PROPLIST_ITEM(id++,"承载系数最大设计值"));
	CLDSApp::AddPropItem("m_fGamaGForDesign",PROPLIST_ITEM(id++,"优化选材时重力荷载γG"));
	CLDSApp::AddPropItem("m_fGamaGForFoundation",PROPLIST_ITEM(id++,"基础验算时重力荷载γG"));
	CLDSApp::AddPropItem("m_fGamaQForTowerWindLoad",PROPLIST_ITEM(id++,"塔身风荷载分项系数γQ"));
	//选材
	CLDSApp::AddPropItem("SlendernessLimit",PROPLIST_ITEM(id++,"其余杆件长细比限制"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMainPoleSlendernessLimit",PROPLIST_ITEM(id++,"受压主材长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nXiePoleSlendernessLimit",PROPLIST_ITEM(id++,"受压斜材长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nAuxPoleSlendernessLimit",PROPLIST_ITEM(id++,"受压辅助材长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nTensivePoleSlendernessLimit",PROPLIST_ITEM(id++,"受拉材长细比阈值"));
	CLDSApp::AddPropItem("TubeSlendernessLimit",PROPLIST_ITEM(id++,"钢管长细比限制"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMainTubeSlendernessLimit",PROPLIST_ITEM(id++,"受压主材钢管长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nXieTubeSlendernessLimit",PROPLIST_ITEM(id++, "受压斜材钢管长细比阈值"	));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nAuxTubeSlendernessLimit",PROPLIST_ITEM(id++,"受压辅助钢管长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit",PROPLIST_ITEM(id++,"受拉钢管长细比阈值"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_bPreferSingleAngle",PROPLIST_ITEM(id++,"角钢构件单角钢优先"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_iSizeOrderRule",PROPLIST_ITEM(id++,"规格优选排序规则"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nConvergeLim",PROPLIST_ITEM(id++,"选材精度"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_nMaxSelMatCount",PROPLIST_ITEM(id++,"选材次数"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinAngleSize",PROPLIST_ITEM(id++,"其它材最小截面积"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinForceAngleSize",PROPLIST_ITEM(id++,"受力材最小截面积"));
	CLDSApp::AddPropItem("Ta.SelMatPara.m_csMinMainAngleSize",PROPLIST_ITEM(id++,"主材最小截面积"));

	CLDSApp::AddPropItem("ColorScheme",PROPLIST_ITEM(id++,"颜色方案"));
	CLDSApp::AddPropItem("env.mcs",PROPLIST_ITEM(id++,"模型坐标系","模型坐标系是整个结构体的的坐标确定基准。构件在整个结构体中的装配位置和方向都应当用模型坐标系下的坐标和向量来标定。","显示|隐藏"));
	CLDSApp::AddPropItem("env.mcs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSApp::AddPropItem("env.mcs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSApp::AddPropItem("env.mcs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSApp::AddPropItem("env.mcs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("env.mcs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispPartACS",PROPLIST_ITEM(id++,"构件装配坐标系","当前选中构件的装配坐标系。","显示|隐藏"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispWorkUcs",PROPLIST_ITEM(id++,"工作坐标系","操作者输入一些空间位置或方向数据时，为了便于确定，可以输入工作坐标系下的相对坐标值。","显示|隐藏"));
	CLDSApp::AddPropItem("g_sysPara.m_bDispBlockLCS",PROPLIST_ITEM(id++,"部件定位坐标系","部件空间中的部件装配定位坐标系。","显示|隐藏"));
	//自定义工作坐标系
	CLDSApp::AddPropItem("work_ucs.m_eDatumPlane",PROPLIST_ITEM(id++,"基准面","指定两个轴方向，自动计算出垂直与当前面的方向，第一个轴方向不变修正第二个轴方向。如:X-Y面,先以X、Y轴计算Z轴方向，以X、Z轴修正Y轴方向。"));
	CLDSApp::AddPropItem("work_ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSApp::AddPropItem("work_ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSApp::AddPropItem("work_ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSApp::AddPropItem("work_ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSApp::AddPropItem("work_ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSApp::AddPropItem("work_ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSApp::AddPropItem("work_ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSApp::AddPropItem("work_ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSApp::AddPropItem("work_ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSApp::AddPropItem("g_sysPara.TubeParam",PROPLIST_ITEM(id++,"钢管及插板参数"));
	CLDSApp::AddPropItem("g_sysPara.PlateParam",PROPLIST_ITEM(id++,"钢板参数"));
	CLDSApp::AddPropItem("g_sysPara.AngleParam",PROPLIST_ITEM(id++,"角钢参数"));
	CLDSApp::AddPropItem("g_sysPara.BoltParam",PROPLIST_ITEM(id++,"螺栓及脚钉参数"));
	CLDSApp::AddPropItem("g_sysPara.m_bModifyCoupledFLTubeOddment",PROPLIST_ITEM(id++,"修改对接法兰钢管正负头","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.HuoQuDist",PROPLIST_ITEM(id++,"火曲间隙","火曲线到角钢边缘距离"));
	CLDSApp::AddPropItem("g_sysPara.JgEndDist",PROPLIST_ITEM(id++,"角钢对接间隙","角钢与角钢对接时两者之间的距离"));
	CLDSApp::AddPropItem("g_sysPara.JointPlankDist",PROPLIST_ITEM(id++,"接头与板间隙","接头与板连接时两者之间的距离"));
	CLDSApp::AddPropItem("g_sysPara.TubeFlDist",PROPLIST_ITEM(id++,"法兰与钢管间隙","法兰与钢管连接时两者之间的距离"));
	CLDSApp::AddPropItem("CLDSLineTube::ciFlTubeOddment",PROPLIST_ITEM(id++,"法兰连接钢管的正负头基准","端头为法兰的钢管,其正负头的计算显示方式","0.至钢管端面|1.至法兰端面"));
	CLDSApp::AddPropItem("g_sysPara.VertexDist",PROPLIST_ITEM(id++,"顶点间隙","顶点与顶点之间的距离"));
	CLDSApp::AddPropItem("g_sysPara.FootNailDist",PROPLIST_ITEM(id++,"脚钉间距"));
	CLDSApp::AddPropItem("g_sysPara.fStressDiffuseAngle",PROPLIST_ITEM(id++,"应力扩散角"));
	CLDSApp::AddPropItem("g_sysPara.fInsertPlateEdgeSpace",PROPLIST_ITEM(id++,"插板边距","连接板插板边距，影响连接板插板外形设计"));
	CLDSApp::AddPropItem("g_sysPara.m_bDelBoltsResideOnPlate",PROPLIST_ITEM(id++,"同步删除钢板螺栓","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.ciLinkAngleDlgFocusCtrlMode",PROPLIST_ITEM(id++,"连材框默认光标位置","","自动设定|-|规格框优先|编号框优先"));
	CLDSApp::AddPropItem("g_sysPara.fNailRepScope",PROPLIST_ITEM(id++,"脚钉代孔范围"));
	CLDSApp::AddPropItem("g_sysPara.fTubeWeldRoadMinD",PROPLIST_ITEM(id++,"无缝钢管阈值","小于该直径的钢管默认为无缝钢管，无缝钢管不存在焊道线。"));
	CLDSApp::AddPropItem("MaxWasherN",PROPLIST_ITEM(id++,"垫圈最大叠加数量"));
	CLDSApp::AddPropItem("BoltPadThick",PROPLIST_ITEM(id++,"螺栓垫圈厚度"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM12",PROPLIST_ITEM(id++,"M12","M12螺栓垫圈厚度，多种厚度可用逗号分隔"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM16",PROPLIST_ITEM(id++,"M16","M16螺栓垫圈厚度，多种厚度可用逗号分隔"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM20",PROPLIST_ITEM(id++,"M20","M20螺栓垫圈厚度，多种厚度可用逗号分隔"));
	CLDSApp::AddPropItem("g_sysPara.BoltPadThick.m_nM24",PROPLIST_ITEM(id++,"M24","M24螺栓垫圈厚度，多种厚度可用逗号分隔"));
	CLDSApp::AddPropItem("g_sysPara.m_nPlankEdgeAngleThreshold",PROPLIST_ITEM(id++,"连接板邻边最小角度","包络多边形设计连板外形时，相邻边间的最小夹角（°）。"));
	helpStr.Copy("根据连接板连接杆件及螺栓生成的包络外形顶点, 生成最终无凹角钢板的外形生成算法");
	CLDSApp::AddPropItem("m_iProfileAlgorithm",PROPLIST_ITEM(id++,"外形生成算法",helpStr,"0.传统算法|1.改进算法"));
	helpStr.Copy("使用连接板命令生成钢板时自动调整钢板或槽型插板位置,可以选择钢管偏移或者槽型插板偏移或者选择不启用自动调整钢管位置");
	CLDSApp::AddPropItem("g_sysPara.nAutoAdjustTubePos",PROPLIST_ITEM(id++,"自动调整钢管位置",helpStr,"不启用|插板偏移|钢管偏移"));
	CLDSApp::AddPropItem("CLDSPlate::m_cRollProcessType",PROPLIST_ITEM(id++,"钢板卷边处理方式"));
	CLDSApp::AddPropItem("g_sysPara.m_fKaiHeJiaoThreshold",PROPLIST_ITEM(id++,"角钢开合角阈值(°)","角钢两肢夹角与90°的偏差值大于该阈值时认为需要进行开合角"));
	CLDSApp::AddPropItem("g_sysPara.m_fKaiHeJiaoMinDeita",PROPLIST_ITEM(id++,"开合角最小安全间隙","角钢肢开合角时连接构件外缘与开合角起止点间的最小距离"));
	CLDSApp::AddPropItem("g_sysPara.m_nGroupJgMachiningGap",PROPLIST_ITEM(id++,"组合角钢加工间隙","组合加工间隙用于初始化组合角钢上的连接板的厚度"));
	CLDSApp::AddPropItem("g_sysPara.m_nMatCharPosType",PROPLIST_ITEM(id++,"构件编号材质字符位置","","0.不需要|1.构件编号前|2.构件编号后"));
	CLDSApp::AddPropItem("g_sysPara.m_cMatCharSeparator",PROPLIST_ITEM(id++,"材质字符间隔符","","无|空格"));
	CLDSApp::AddPropItem("g_sysPara.m_fMinCutAngleLen",PROPLIST_ITEM(id++,"最小切角值"));
	CLDSApp::AddPropItem("g_sysPara.m_fMaxIgnoreCutAngleLen",PROPLIST_ITEM(id++,"最大忽略切角值"));
	CLDSApp::AddPropItem("g_sysPara.b1BoltDesignPrompt",PROPLIST_ITEM(id++,"显示垫板参数框","是否在单螺栓连接时显示垫板,螺栓,垫圈信息的对话框","是|否"));
	CLDSApp::AddPropItem("CLDSBolt::BOLTL0_CALMODE", PROPLIST_ITEM(id++, "通厚计算执行规则", "0.累加通厚：累加穿过构件厚度计算通厚；1.计入缺失垫板间隙：累加穿过构件厚度及相邻构件间>2mm的间隙值计算通厚；2.螺栓自身属性:以螺栓自身属性为准", "0.累加通厚|1.计入缺失垫板间隙|2.螺栓自身属性"));
	CLDSApp::AddPropItem("CLDSBolt::BOLTL0_PREFER_MODE", PROPLIST_ITEM(id++, "通厚计算规则初始值", "0.累加通厚：累加穿过构件厚度计算通厚；1.计入缺失垫板间隙：累加穿过构件厚度及相邻构件间>2mm的间隙值计算通厚", "0.累加通厚|1.计入缺失垫板间隙"));
	CLDSApp::AddPropItem("g_sysPara.m_b45CutAngleNeedCoefR",PROPLIST_ITEM(id++,"45度切角需要内圆弧比例系数","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.m_fCutAngleCoefR",PROPLIST_ITEM(id++,"切角间隙占内圆弧比例系数","","1/3|2/3"));
	CLDSApp::AddPropItem("g_sysPara.m_bAnglePreferSimplePosMode",PROPLIST_ITEM(id++,"角钢默认定位方式","","简单定位|参数化定位"));
	CLDSApp::AddPropItem("g_sysPara.display.cHoleWallMode",PROPLIST_ITEM(id++,"孔壁显示方式","构件上螺栓孔、挂线孔及工艺孔的显示方式","0.不显示|1.仅虚拟螺栓孔壁|2.全部孔壁"));
	CLDSApp::AddPropItem("CLDSPart::UPDATE_PARTNO_AND_SEG",PROPLIST_ITEM(id++,"构件段号编号同步更新","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect",PROPLIST_ITEM(id++,"智能连接设计参数"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bOnlyDesignCurView",PROPLIST_ITEM(id++,"仅设计当前视图连接","智能设计时仅对当前视图平面的连接进行设计","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignCommonConnect",PROPLIST_ITEM(id++,"智能设计普通连接","智能设计时包括普通基准材射线材平面连接设计","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignBendConnect",PROPLIST_ITEM(id++,"智能设计制弯板连接","智能设计时包括基准材射线材制弯板连接设计","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignAngleJoint",PROPLIST_ITEM(id++,"智能设计角钢接头","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignFL",PROPLIST_ITEM(id++,"智能设计钢管法兰","","是|否"));
	CLDSApp::AddPropItem("g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate",PROPLIST_ITEM(id++,"智能设计环向封板","","是|否"));
	CLDSApp::AddPropItem("SyncProp",PROPLIST_ITEM(id++,"关联对象同步属性"));
	//显示提示框
	CLDSApp::AddPropItem("g_sysPara.m_bEnablePrompt",PROPLIST_ITEM(id++,"启用提示框","建模命令操作过程中是否启用示意图提示框","是|否"));
	//抗震分析
	CLDSApp::AddPropItem("EarthQuake",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.grade",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.AlfaMax",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.Tg",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.zeta",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.gamaRE",PROPLIST_ITEM(id++,""));
	CLDSApp::AddPropItem("EarthQuake.nFreqsRank",PROPLIST_ITEM(id++,""));
	//同步属性设置
	CLDSApp::AddPropItem("SyncProp",PROPLIST_ITEM(id++,"关联对象同步属性"));
	CLDSApp::AddPropItem("SyncProp.Node",PROPLIST_ITEM(id++,"节点","设置关联节点同步属性"));
	CLDSApp::AddPropItem("SyncProp.Angle",PROPLIST_ITEM(id++,"角钢","设置关联角钢同步属性"));
	CLDSApp::AddPropItem("SyncProp.Tube",PROPLIST_ITEM(id++,"钢管","设置关联钢管同步属性"));
	CLDSApp::AddPropItem("SyncProp.Plate",PROPLIST_ITEM(id++,"钢板","设置关联钢板同步属性"));
	id=InitDesignParamsToSyssettingPropHashtable(id);
	MIN_IDLE_PROPID=id++;
#endif
}
long CLDSApp::InitDesignParamsToSyssettingPropHashtable(long iInitId)
{
	for(DESIGN_PARAM_ITEM* pParam=Ta.hashParams.GetFirst();pParam;pParam=Ta.hashParams.GetNext())
	{
		CXhChar50 label("model.%s",(char*)pParam->sLabel);
		if(CLDSApp::GetPropID(label)==0)
			CLDSApp::AddPropItem(label,PROPLIST_ITEM(iInitId++,label));
	}
	return iInitId++;
}
CHashStrList<PROPLIST_ITEM> CLDSApp::propMenuHashtable;
CHashSet<PROPLIST_ITEM*>CLDSApp::hashMenuHelpItemById;
void CLDSApp::InitMenuHelpHashtable()
{
	CLDSApp::propMenuHashtable.SetHashTableGrowSize(100);//HASHTABLESIZE);
	//文件带单
	CLDSApp::AddMenuHelpItem("ID_FILE_NEW",PROPLIST_ITEM(ID_FILE_NEW,"文件->新建"));
	CLDSApp::AddMenuHelpItem("ID_NEW_TOWER_MODEL",PROPLIST_ITEM(ID_NEW_TOWER_MODEL,"文件->新建->杆塔模型"));
	CLDSApp::AddMenuHelpItem("ID_NEW_SUBSTATION_MODEL",PROPLIST_ITEM(ID_NEW_TOWER_MODEL,"文件->新建->变电构架模型"));
	CLDSApp::AddMenuHelpItem("ID_FILE_OPEN",PROPLIST_ITEM(ID_FILE_OPEN,"文件->打开(O)"));
	CLDSApp::AddMenuHelpItem("ID_FILE_SAVE",PROPLIST_ITEM(ID_FILE_SAVE,"文件->保存(S)"));
	CLDSApp::AddMenuHelpItem("ID_FILE_SAVE_AS",PROPLIST_ITEM(ID_FILE_SAVE_AS,"文件->另存为"));
	CLDSApp::AddMenuHelpItem("ID_FILE_PROPERTY",PROPLIST_ITEM(ID_FILE_PROPERTY,"文件->文件属性"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_BMP_FILE",PROPLIST_ITEM(ID_EXPORT_BMP_FILE,"文件->输出BMP"));
	CLDSApp::AddMenuHelpItem("ID_FILE_MRU_FILE1",PROPLIST_ITEM(ID_FILE_MRU_FILE1,"文件->最近文件"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_SWAP_INFO_FILE",PROPLIST_ITEM(ID_EXPORT_SWAP_INFO_FILE,"文件->输出"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_EXCEL_FILE",PROPLIST_ITEM(ID_EXPORT_EXCEL_FILE,"文件->材料表->EXCEL材料表"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_EXCEL_FILE2",PROPLIST_ITEM( ID_EXPORT_EXCEL_FILE2,"文件->材料表->EXCEL材料表二"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_EXCEL_FILE3",PROPLIST_ITEM(ID_EXPORT_EXCEL_FILE3,"文件->材料表->EXCEL材料表三"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TXT_PART_DATA",PROPLIST_ITEM(ID_EXPORT_TXT_PART_DATA,"文件->材料表->文本材料表数据(P)"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TXT_PART_DATA2",PROPLIST_ITEM(ID_EXPORT_TXT_PART_DATA2,"文件->材料表->文本材料表数据二)"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TXT_PART_DATA3",PROPLIST_ITEM(ID_EXPORT_TXT_PART_DATA3,"文件->材料表->文本材料表数三"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TXT_PART_DATA4",PROPLIST_ITEM(ID_EXPORT_TXT_PART_DATA4,"文件->材料表->文本材料表数四"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TXT_LS_DATA",PROPLIST_ITEM( ID_EXPORT_TXT_LS_DATA,"文件->材料表->文本螺栓表数据"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_TUBE_SPECIAL_PROCESS_TABLE",PROPLIST_ITEM(ID_EXPORT_TUBE_SPECIAL_PROCESS_TABLE,"文件->材料表->钢管特殊工艺表"));
	CLDSApp::AddMenuHelpItem("ID_APP_EXIT",PROPLIST_ITEM(ID_APP_EXIT,"文件->退出"));
	//编辑菜单
	CLDSApp::AddMenuHelpItem("ID_UNDO",PROPLIST_ITEM(ID_UNDO,"编辑->撤销"));
	CLDSApp::AddMenuHelpItem("ID_REDO",PROPLIST_ITEM(ID_REDO,"编辑->重复"));
	CLDSApp::AddMenuHelpItem("ID_EDIT_FIND_PART",PROPLIST_ITEM(ID_EDIT_FIND_PART,"编辑->搜素构件"));
	CLDSApp::AddMenuHelpItem("ID_DEL_SPECIFY_PART",PROPLIST_ITEM(ID_DEL_SPECIFY_PART,"编辑->删除指定构件"));
	CLDSApp::AddMenuHelpItem("ID_COPY_PARTSET",PROPLIST_ITEM(ID_COPY_PARTSET,"编辑->复制对象"));
	CLDSApp::AddMenuHelpItem("ID_PASTE_PARTSET",PROPLIST_ITEM(ID_PASTE_PARTSET,"编辑->粘贴对象"));
	CLDSApp::AddMenuHelpItem("ID_HANDLE_REPLACE",PROPLIST_ITEM(ID_HANDLE_REPLACE,"编辑->句柄替换"));
	//视图菜单
	CLDSApp::AddMenuHelpItem("ID_DOCKPAGE_CMDLINE",PROPLIST_ITEM( ID_DOCKPAGE_CMDLINE,"视图->命令行"));
	CLDSApp::AddMenuHelpItem("ID_DOCKPAGE_MODEL",PROPLIST_ITEM( ID_DOCKPAGE_MODEL,"视图->模型视图"));
	CLDSApp::AddMenuHelpItem("ID_DOCKPAGE_PROPERTY",PROPLIST_ITEM(ID_DOCKPAGE_PROPERTY,"视图->属性视图"));
	CLDSApp::AddMenuHelpItem("ID_DOCKPAGE_OUTPUT",PROPLIST_ITEM(ID_DOCKPAGE_OUTPUT,"视图->其他视图->输出窗口"));
	CLDSApp::AddMenuHelpItem("ID_DOCKPAGE_WELDPART",PROPLIST_ITEM(ID_DOCKPAGE_WELDPART,"视图->其他视图->组合焊件"));
	CLDSApp::AddMenuHelpItem("ID_LINE_MODEL_ENV",PROPLIST_ITEM(ID_LINE_MODEL_ENV,"视图->工作环境->单线模型"));
	CLDSApp::AddMenuHelpItem("ID_SOLID_MODEL_ENV",PROPLIST_ITEM(ID_SOLID_MODEL_ENV,"视图->工作环境->实体模型"));
	CLDSApp::AddMenuHelpItem("ID_PART_ENV",PROPLIST_ITEM(ID_PART_ENV,"视图->工作环境->构件编辑器"));
	CLDSApp::AddMenuHelpItem("ID_GL_ALL_ZOOM",PROPLIST_ITEM(ID_GL_ALL_ZOOM,"视图->全显"));
	CLDSApp::AddMenuHelpItem("ID_GL_OPEN_WINDOW",PROPLIST_ITEM(ID_GL_OPEN_WINDOW,"视图->开窗"));
	CLDSApp::AddMenuHelpItem("ID_GL_PAN",PROPLIST_ITEM(ID_GL_PAN,"视图->平移"));
	CLDSApp::AddMenuHelpItem("ID_GL_ZOOM",PROPLIST_ITEM(ID_GL_ZOOM,"视图->缩放"));
	CLDSApp::AddMenuHelpItem("ID_GL_ROTATED",PROPLIST_ITEM(ID_GL_ROTATED,"视图->旋转"));
	CLDSApp::AddMenuHelpItem("ID_RESET_VIEW",PROPLIST_ITEM(ID_RESET_VIEW,"视图->复位"));
	CLDSApp::AddMenuHelpItem("ID_OPER_OTHER",PROPLIST_ITEM(ID_OPER_OTHER,"视图->控制中心"));
	CLDSApp::AddMenuHelpItem("ID_NODE_CENTER",PROPLIST_ITEM(ID_NODE_CENTER,"视图->节点中心"));
	CLDSApp::AddMenuHelpItem("ID_REDRAW_ALL",PROPLIST_ITEM(ID_REDRAW_ALL,"视图->刷新屏幕"));
	CLDSApp::AddMenuHelpItem("ID_SPREAD_FACE",PROPLIST_ITEM(ID_SPREAD_FACE,"视图->展开工作平面"));
	CLDSApp::AddMenuHelpItem("ID_SPREAD_NODE_FACE",PROPLIST_ITEM(ID_SPREAD_NODE_FACE,"视图->展开轮廓面"));
	CLDSApp::AddMenuHelpItem("ID_SELECT_NODE_FACE",PROPLIST_ITEM(ID_SELECT_NODE_FACE,"视图->选择轮廓面构件"));
	//建模菜单
	CLDSApp::AddMenuHelpItem("ID_OPER_SHANK_SECTION",PROPLIST_ITEM(ID_OPER_SHANK_SECTION,"建模->塔身节间"));
	CLDSApp::AddMenuHelpItem("ID_DEF_CTRL_PT",PROPLIST_ITEM(ID_PART_ENV,"建模->控制节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_DIVIDE_SCALE_NODE",PROPLIST_ITEM(ID_DEF_DIVIDE_SCALE_NODE,"建模->等分节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_OFFSET_NODE",PROPLIST_ITEM(ID_DEF_OFFSET_NODE,"建模->偏移节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_INTERS_NODE",PROPLIST_ITEM(ID_DEF_INTERS_NODE,"建模->交叉节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_HORIZON_NODE",PROPLIST_ITEM(ID_DEF_HORIZON_NODE,"建模->等高节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_AXIS_VAL_FIX_NODE",PROPLIST_ITEM(ID_DEF_AXIS_VAL_FIX_NODE,"建模->指定坐标分量节点"));
	CLDSApp::AddMenuHelpItem("ID_DEF_ALL_INTERS_NODE",PROPLIST_ITEM(ID_DEF_ALL_INTERS_NODE,"建模->生成所有交叉节点"));
	CLDSApp::AddMenuHelpItem("ID_LINK_LINEPART",PROPLIST_ITEM(ID_LINK_LINEPART,"建模->连接杆件"));
	CLDSApp::AddMenuHelpItem("ID_NEW_ARCPART",PROPLIST_ITEM(ID_NEW_ARCPART,"建模->环形杆件"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_COLUMN_RAY_ROD",PROPLIST_ITEM(ID_DESIGN_COLUMN_RAY_ROD,"建模->柱面射线杆件"));
	CLDSApp::AddMenuHelpItem("ID_ONEBOLT_DESIGN",PROPLIST_ITEM( ID_ONEBOLT_DESIGN,"建模->角钢单螺栓连接"));
	CLDSApp::AddMenuHelpItem("ID_SPEC_NODE_ONEBOLT_DESIGN",PROPLIST_ITEM(ID_SPEC_NODE_ONEBOLT_DESIGN,"建模->生成选定节点单螺栓连接"));
	CLDSApp::AddMenuHelpItem("ID_LAY_JG_END_LS",PROPLIST_ITEM(ID_LAY_JG_END_LS,"建模->布置单角钢螺栓"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_JDB",PROPLIST_ITEM(ID_DESIGN_JDB,"建模->节点板"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_LJ_PLATE",PROPLIST_ITEM(ID_DESIGN_LJ_PLATE,"建模->连接板"));
	CLDSApp::AddMenuHelpItem("ID_SHARE_JDB_DESIGN",PROPLIST_ITEM(ID_SHARE_JDB_DESIGN,"建模->共用板"));
	CLDSApp::AddMenuHelpItem("ID_CIRCULAR_PLATE",PROPLIST_ITEM(ID_CIRCULAR_PLATE,"建模->环向板"));
	CLDSApp::AddMenuHelpItem("ID_FOLD_PLANK",PROPLIST_ITEM(ID_FOLD_PLANK,"建模->折叠板"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_OVERLAPPED_PLATE",PROPLIST_ITEM(ID_DESIGN_OVERLAPPED_PLATE,"建模->叠放板"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_RIB_PLATE",PROPLIST_ITEM(ID_DESIGN_RIB_PLATE,"建模->肋板"));
	CLDSApp::AddMenuHelpItem("ID_TUBE_BETWEEN_PLATE",PROPLIST_ITEM(ID_TUBE_BETWEEN_PLATE,"建模->钢管夹板"));
	CLDSApp::AddMenuHelpItem("ID_BOLT_PAD",PROPLIST_ITEM(ID_BOLT_PAD,"建模->螺栓垫板"));
	CLDSApp::AddMenuHelpItem("ID_INSERT_PLATE_STYLE_TUBE_INTERS",PROPLIST_ITEM(ID_INSERT_PLATE_STYLE_TUBE_INTERS,"建模->插板式交叉钢管连接"));
	CLDSApp::AddMenuHelpItem("ID_LAYOUT_FOOTNAIL",PROPLIST_ITEM(ID_LAYOUT_FOOTNAIL,"建模->布置脚钉"));
	CLDSApp::AddMenuHelpItem("ID_COMMON_JG_JOINT",PROPLIST_ITEM(ID_COMMON_JG_JOINT,"建模->角钢接头"));
	CLDSApp::AddMenuHelpItem("ID_LAY_FILL_PLANK",PROPLIST_ITEM(ID_LAY_FILL_PLANK,"建模->填板布置"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_SPHERE",PROPLIST_ITEM(ID_DESIGN_SPHERE,"建模->节点球体"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_SIMPLE_LADDER",PROPLIST_ITEM(ID_DESIGN_SIMPLE_LADDER,"建模->简易钢管爬梯"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_TUBE_TOWER_SLOPE",PROPLIST_ITEM(ID_DESIGN_TUBE_TOWER_SLOPE,"建模->异型法兰设计"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_TUBE_TOWER_FOOT",PROPLIST_ITEM(ID_DESIGN_TUBE_TOWER_FOOT,"建模->钢管塔脚设计"));
	CLDSApp::AddMenuHelpItem("ID_FOOT_PLANK",PROPLIST_ITEM(ID_FOOT_PLANK,"建模->角钢塔脚设计"));
	CLDSApp::AddMenuHelpItem("ID_INTELLIGENT_CONNECT_DESIGN",PROPLIST_ITEM(ID_INTELLIGENT_CONNECT_DESIGN,"建模->智能连接设计"));
	CLDSApp::AddMenuHelpItem("ID_GY_HOLE",PROPLIST_ITEM(ID_GY_HOLE,"建模->流水孔"));
	CLDSApp::AddMenuHelpItem("ID_MIRROR_CREATE",PROPLIST_ITEM(ID_MIRROR_CREATE,"建模->对称生成"));
	//分析菜单
	CLDSApp::AddMenuHelpItem("ID_CODE_NODE_I",PROPLIST_ITEM(ID_CODE_NODE_I,"分析->节点编号"));
	CLDSApp::AddMenuHelpItem("ID_MODEL_WIRE_LOAD_CAL",PROPLIST_ITEM(ID_MODEL_WIRE_LOAD_CAL,"分析->线条荷载计算"));
	CLDSApp::AddMenuHelpItem("ID_IMPORT_LOAD_FILE",PROPLIST_ITEM(ID_IMPORT_LOAD_FILE,"分析->导入外荷载文件"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_LOAD_FILE",PROPLIST_ITEM(ID_EXPORT_LOAD_FILE,"分析->导出外荷载文件"));
	CLDSApp::AddMenuHelpItem("ID_RETRIEVE_SRC_DES_INFO",PROPLIST_ITEM(ID_RETRIEVE_SRC_DES_INFO,"分析->提取原始设计信息"));
	CLDSApp::AddMenuHelpItem("ID_MODEL_DESIGN_CAL",PROPLIST_ITEM(ID_MODEL_DESIGN_CAL,"分析->验算选材"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_RPT_FILE",PROPLIST_ITEM(ID_EXPORT_RPT_FILE,"分析->计算报告->生成计算书"));
	CLDSApp::AddMenuHelpItem("ID_RESULT_BUFFER",PROPLIST_ITEM(ID_RESULT_BUFFER,"分析->计算报告->计算结果库"));
	CLDSApp::AddMenuHelpItem("ID_ANGLE_JOINT_DESIGN_CAL",PROPLIST_ITEM(ID_ANGLE_JOINT_DESIGN_CAL,"分析->辅助结构计算->接头设计"));
	CLDSApp::AddMenuHelpItem("ID_BILL_OF_FL",PROPLIST_ITEM(ID_BILL_OF_FL,"分析->辅助结构计算->法兰设计"));
	CLDSApp::AddMenuHelpItem("ID_FOOT_DESIGN_CAL",PROPLIST_ITEM(ID_FOOT_DESIGN_CAL,"分析->辅助结构计算->底脚板设计"));
	CLDSApp::AddMenuHelpItem("ID_STUB_ANGLE_CHECK",PROPLIST_ITEM(ID_STUB_ANGLE_CHECK,"分析->辅助结构计算->插入角钢验算"));
	CLDSApp::AddMenuHelpItem("ID_BN_BEND_PART_CHECK",PROPLIST_ITEM(ID_BN_BEND_PART_CHECK,"分析->辅助结构计算->受弯构件验算"));
	CLDSApp::AddMenuHelpItem("ID_MODEL_FREQS",PROPLIST_ITEM(ID_MODEL_FREQS,"分析->动力特性分析"));
	CLDSApp::AddMenuHelpItem("ID_MODEL_EQ_CHECK",PROPLIST_ITEM(ID_MODEL_EQ_CHECK,"分析->抗震验算"));
	CLDSApp::AddMenuHelpItem("ID_DISPLAY_DEFORM_TOWER",PROPLIST_ITEM(ID_DISPLAY_DEFORM_TOWER,"分析->预览计算结果"));
	CLDSApp::AddMenuHelpItem("ID_IMPORT_VIBRATION_MODE",PROPLIST_ITEM(ID_IMPORT_VIBRATION_MODE,"分析->当前显示振型"));
	CLDSApp::AddMenuHelpItem("ID_DISPLAY_VIBRATION_MODE",PROPLIST_ITEM(ID_DISPLAY_VIBRATION_MODE,"分析->显示杆塔振型"));
	//操作菜单
	CLDSApp::AddMenuHelpItem("ID_TA_DAO_WEI",PROPLIST_ITEM(ID_TA_DAO_WEI,"操作->更新塔位"));
	CLDSApp::AddMenuHelpItem("ID_DEF_ZHU_JG_POLY",PROPLIST_ITEM(ID_DEF_ZHU_JG_POLY,"操作->定义主材四边形"));
	CLDSApp::AddMenuHelpItem("ID_DISPLAY_OR_HIDE_ALL_HOLE",PROPLIST_ITEM(ID_DISPLAY_OR_HIDE_ALL_HOLE,"操作->显示所有孔"));
	CLDSApp::AddMenuHelpItem("ID_BREAK_LINEPART",PROPLIST_ITEM(ID_BREAK_LINEPART,"操作->打断杆件"));
	CLDSApp::AddMenuHelpItem("ID_COMPOSE_LINEPART",PROPLIST_ITEM(ID_COMPOSE_LINEPART,"操作->合并杆件"));
	CLDSApp::AddMenuHelpItem("ID_DIVIDE_MAIN_LINEPART",PROPLIST_ITEM(ID_DIVIDE_MAIN_LINEPART,"操作->塔材分段"));
	CLDSApp::AddMenuHelpItem("ID_DESIGN_LINEPART_ODDMENT",PROPLIST_ITEM(ID_DESIGN_LINEPART_ODDMENT,"操作->设定正负头"));
	CLDSApp::AddMenuHelpItem("ID_MAKE_PLATE_BEND",PROPLIST_ITEM(ID_MAKE_PLATE_BEND,"操作->制弯钢板"));
	CLDSApp::AddMenuHelpItem("ID_EXTEND_PLATE",PROPLIST_ITEM(ID_EXTEND_PLATE,"操作->延伸钢板"));
	CLDSApp::AddMenuHelpItem("ID_MERGE_LJB",PROPLIST_ITEM(ID_MERGE_LJB,"操作->合并连接板"));
	CLDSApp::AddMenuHelpItem("ID_CUT_PLATE_BY_PLANE",PROPLIST_ITEM(ID_CUT_PLATE_BY_PLANE,"操作->切割钢板"));
	CLDSApp::AddMenuHelpItem("ID_CUT_PLATE_BY_COLUMN_FACE",PROPLIST_ITEM(ID_CUT_PLATE_BY_COLUMN_FACE,"操作->圆曲面切割钢板"));
	CLDSApp::AddMenuHelpItem("ID_CONVERT_TO_PLATE",PROPLIST_ITEM(ID_CONVERT_TO_PLATE,"操作->转换为普通钢板"));
	CLDSApp::AddMenuHelpItem("ID_ROTATE_PART_AROUND_AXIS",PROPLIST_ITEM( ID_ROTATE_PART_AROUND_AXIS,"操作->绕指定轴旋转钢板"));
	CLDSApp::AddMenuHelpItem("ID_ADJUST_PLATE_WELD_LEN",PROPLIST_ITEM(ID_ADJUST_PLATE_WELD_LEN,"操作->调整连接板焊缝长度"));
	CLDSApp::AddMenuHelpItem("ID_CAL_1JG_NORM",PROPLIST_ITEM(ID_CAL_1JG_NORM,"操作->计算角钢肢方向"));
	CLDSApp::AddMenuHelpItem("ID_TURN_ANGLE_INOUT_SIDE",PROPLIST_ITEM(ID_TURN_ANGLE_INOUT_SIDE,"操作->角钢里外铁翻转"));
	CLDSApp::AddMenuHelpItem("ID_TURN_ANGLE_WING",PROPLIST_ITEM(ID_TURN_ANGLE_WING,"操作->角钢左右面翻转"));
	CLDSApp::AddMenuHelpItem("ID_CUT_JG",PROPLIST_ITEM(ID_CUT_JG,"操作->基准角钢切角切肢"));
	CLDSApp::AddMenuHelpItem("ID_SPEC_CUT_FACE_CUT_JG",PROPLIST_ITEM(ID_SPEC_CUT_FACE_CUT_JG,"操作->指定切割面切角切肢"));
	CLDSApp::AddMenuHelpItem("ID_MAKE_JG_BEND",PROPLIST_ITEM(ID_MAKE_JG_BEND,"操作->角钢制弯"));
	CLDSApp::AddMenuHelpItem("ID_EXPLODE_HUOQU",PROPLIST_ITEM(ID_EXPLODE_HUOQU,"操作->打碎制弯组合"));
	CLDSApp::AddMenuHelpItem("ID_MAKE_END_BEND",PROPLIST_ITEM(ID_MAKE_END_BEND,"操作->端头制弯"));
	CLDSApp::AddMenuHelpItem("ID_HAND_SHORT_JG",PROPLIST_ITEM(ID_HAND_SHORT_JG,"操作->定制短角钢"));
	//数据带单
	CLDSApp::AddMenuHelpItem("ID_CORRECT_ERRORS",PROPLIST_ITEM(ID_CORRECT_ERRORS,"数据->智能纠错"));
	CLDSApp::AddMenuHelpItem("ID_ROUTINE_CHECK",PROPLIST_ITEM(ID_ROUTINE_CHECK,"数据->常规校核"));
	CLDSApp::AddMenuHelpItem("ID_PARTNO_CHECK",PROPLIST_ITEM(ID_PARTNO_CHECK,"数据->编号校核"));
	CLDSApp::AddMenuHelpItem("ID_COLLISION_CHECK",PROPLIST_ITEM(ID_COLLISION_CHECK,"数据->碰撞校核"));
	CLDSApp::AddMenuHelpItem("ID_ONE_KEY_CUT_ANGLE_CHECK",PROPLIST_ITEM(ID_ONE_KEY_CUT_ANGLE_CHECK,"数据->一键切角检测"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_SPEC_PART_EXCEL_FILE",PROPLIST_ITEM(ID_EXPORT_SPEC_PART_EXCEL_FILE,"数据->输出选中构件统计表"));
	CLDSApp::AddMenuHelpItem("ID_CLEAR_NAIL",PROPLIST_ITEM(ID_CLEAR_NAIL,"数据->清除脚钉设计"));
	CLDSApp::AddMenuHelpItem("ID_REFRESH_BURGLAR_LS",PROPLIST_ITEM(ID_REFRESH_BURGLAR_LS,"数据->刷新防盗螺栓"));
	CLDSApp::AddMenuHelpItem("ID_REMOVE_RELA_OBJECTS",PROPLIST_ITEM(ID_REMOVE_RELA_OBJECTS,"数据->去除关联对称信息"));
	//绘图菜单
	CLDSApp::AddMenuHelpItem("ID_CLEAR_PART_NO_I",PROPLIST_ITEM(ID_CLEAR_PART_NO_I,"绘图->清空构件编号"));
	CLDSApp::AddMenuHelpItem("ID_SEG_I_CHECK",PROPLIST_ITEM(ID_SEG_I_CHECK,"绘图->段号检查"));
	CLDSApp::AddMenuHelpItem("ID_CODE_PART_NO_RULE",PROPLIST_ITEM(ID_CODE_PART_NO_RULE,"绘图->编号规则"));
	CLDSApp::AddMenuHelpItem("ID_CODE_PART_NO",PROPLIST_ITEM(ID_CODE_PART_NO,"绘图->构件编号"));
	CLDSApp::AddMenuHelpItem("ID_ATTACH_PART_TBL",PROPLIST_ITEM(ID_ATTACH_PART_TBL,"绘图->附加构件表"));
	CLDSApp::AddMenuHelpItem("ID_DRAW_TA_IN_ACAD",PROPLIST_ITEM(ID_DRAW_TA_IN_ACAD,"绘图->Acad绘图"));
	//NC加工菜单
	CLDSApp::AddMenuHelpItem("ID_SET_NC_DRIVER",PROPLIST_ITEM(ID_SET_NC_DRIVER,"NC加工->设置NC驱动"));
	CLDSApp::AddMenuHelpItem("ID_GEN_JG_NC_FILE",PROPLIST_ITEM(ID_GEN_JG_NC_FILE,"NC加工->生成角钢NC数据"));
	CLDSApp::AddMenuHelpItem("ID_GEN_PLANK_NC_DATA",PROPLIST_ITEM(ID_GEN_PLANK_NC_DATA,"NC加工->生成板NC数据"));
	CLDSApp::AddMenuHelpItem("ID_GEN_PART_PROCESSINFO_FILE",PROPLIST_ITEM(ID_GEN_PART_PROCESSINFO_FILE,"NC加工->生成构件中性文件"));
	//工具菜单
	CLDSApp::AddMenuHelpItem("ID_MEASURE",PROPLIST_ITEM(ID_MEASURE,"工具->测量点距离"));
	CLDSApp::AddMenuHelpItem("ID_VIEW_JG_KEY_POINT",PROPLIST_ITEM(ID_VIEW_JG_KEY_POINT,"工具->查看角钢关键点"));
	CLDSApp::AddMenuHelpItem("ID_LINEPART_EXTEND_VEC",PROPLIST_ITEM(ID_LINEPART_EXTEND_VEC,"工具->杆件延伸方向"));
	CLDSApp::AddMenuHelpItem("ID_CAL_WORK_FACE_NORM",PROPLIST_ITEM(ID_CAL_WORK_FACE_NORM,"工具->计算工作面法线"));
	CLDSApp::AddMenuHelpItem("ID_CAL_POLE_INTERS_PT",PROPLIST_ITEM(ID_CAL_POLE_INTERS_PT,"工具->计算杆件心线交点"));
	CLDSApp::AddMenuHelpItem("ID_ANGLE_FACE_OFFSET_ODD_DIFF",PROPLIST_ITEM(ID_ANGLE_FACE_OFFSET_ODD_DIFF,"工具->角钢平推正负头计算"));
	CLDSApp::AddMenuHelpItem("ID_CAL_DIST_OF_SOLID",PROPLIST_ITEM(ID_CAL_DIST_OF_SOLID,"工具->计算两实体间的距离"));
	CLDSApp::AddMenuHelpItem("ID_CREATE_FACEPANEL",PROPLIST_ITEM(ID_CREATE_FACEPANEL,"工具->定制标准模板"));
	CLDSApp::AddMenuHelpItem("ID_PPE_STARTER",PROPLIST_ITEM(ID_PPE_STARTER,"工具->工艺编辑管理器"));
	CLDSApp::AddMenuHelpItem("ID_SPEC_WCS_ORGIN",PROPLIST_ITEM(ID_SPEC_WCS_ORGIN,"工具->选择工作原点"));
	CLDSApp::AddMenuHelpItem("ID_SPEC_AXIS_X_VERTEX",PROPLIST_ITEM(ID_SPEC_AXIS_X_VERTEX,"工具->选择X轴标定点"));
	CLDSApp::AddMenuHelpItem("ID_TOOL_KIT",PROPLIST_ITEM(ID_TOOL_KIT,"工具->工具箱"));
	//帮助菜单
	CLDSApp::AddMenuHelpItem("ID_HELP_FINDER",PROPLIST_ITEM(ID_HELP_FINDER,"帮助->帮助主题"));
	CLDSApp::AddMenuHelpItem("ID_KEYBOARD_MAP",PROPLIST_ITEM(ID_KEYBOARD_MAP,"帮助->快捷键列表"));
	CLDSApp::AddMenuHelpItem("ID_APP_ABOUT",PROPLIST_ITEM(ID_APP_ABOUT,"帮助->关于"));
	CLDSApp::AddMenuHelpItem("ID_RELEASE_NOTES",PROPLIST_ITEM(ID_RELEASE_NOTES,"帮助->版本升级信息"));
	CLDSApp::AddMenuHelpItem("ID_UPGRADE_SOFTWARE",PROPLIST_ITEM(ID_UPGRADE_SOFTWARE,"帮助->技术支持"));
	//工具栏快捷菜单
	CLDSApp::AddMenuHelpItem("ID_QUICK_SEARCH_PART",PROPLIST_ITEM(ID_QUICK_SEARCH_PART,"工具栏->快速查找"));
	CLDSApp::AddMenuHelpItem("ID_SOLID_MODE_DISPLAY",PROPLIST_ITEM(ID_SOLID_MODE_DISPLAY,"工具栏->显示实体模型"));
	CLDSApp::AddMenuHelpItem("ID_OPEN_WND_SEL",PROPLIST_ITEM(ID_OPEN_WND_SEL,"工具栏->开窗选择构件"));
	CLDSApp::AddMenuHelpItem("ID_HIDE_PART",PROPLIST_ITEM(ID_HIDE_PART,"工具栏->隐去构件"));
	CLDSApp::AddMenuHelpItem("ID_DISPLAY_LINE_VEC",PROPLIST_ITEM(ID_DISPLAY_LINE_VEC,"工具栏->显示杆件起始标记"));
	CLDSApp::AddMenuHelpItem("ID_DISPLAY_COORD_AXIS",PROPLIST_ITEM(ID_DISPLAY_COORD_AXIS,"工具栏->显示坐标系"));
	//主窗口的右键菜单
	CLDSApp::AddMenuHelpItem("ID_AFFIRM_CUR_COMMAND",PROPLIST_ITEM(ID_AFFIRM_CUR_COMMAND,"右键菜单->确认当前命令"));
	CLDSApp::AddMenuHelpItem("ID_CANCEL_CUR_COMMAND",PROPLIST_ITEM(ID_CANCEL_CUR_COMMAND,"右键菜单->取消当前命令"));
	CLDSApp::AddMenuHelpItem("ID_REPEATED_COMMAND",PROPLIST_ITEM(ID_REPEATED_COMMAND,"右键菜单->重复上一次的命令"));
	//CLDSApp::AddMenuHelpItem("ID_HIDE_PART",PROPLIST_ITEM(ID_HIDE_PART,"右键菜单->隐去构件"));
	CLDSApp::AddMenuHelpItem("ID_HANDOVER_PERMISSION",PROPLIST_ITEM(ID_HANDOVER_PERMISSION,"右键菜单->构件授权"));
	CLDSApp::AddMenuHelpItem("ID_AUTO_DESIGN_SELECTED_PARTS",PROPLIST_ITEM(ID_AUTO_DESIGN_SELECTED_PARTS,"右键菜单->设计指定对象"));
	CLDSApp::AddMenuHelpItem("ID_REVISION_BOLT_L0nZPOS",PROPLIST_ITEM(ID_REVISION_BOLT_L0nZPOS,"右键菜单->修正螺栓Z向偏移"));
	CLDSApp::AddMenuHelpItem("ID_NEW_LOAD_NODE",PROPLIST_ITEM(ID_NEW_LOAD_NODE,"右键菜单->新建荷载节点"));
	CLDSApp::AddMenuHelpItem("ID_SEND_PART_EDITOR",PROPLIST_ITEM(ID_SEND_PART_EDITOR,"右键菜单->发送构件编辑器"));
	CLDSApp::AddMenuHelpItem("ID_GROUP_EDIT_PART_LAYER",PROPLIST_ITEM(ID_GROUP_EDIT_PART_LAYER,"右键菜单->成组修改图层名"));
	CLDSApp::AddMenuHelpItem("ID_OBJECT_PROPERTY",PROPLIST_ITEM(ID_OBJECT_PROPERTY,"右键菜单->显示属性"));
	//CLDSApp::AddMenuHelpItem("ID_INTELLIGENT_CONNECT_DESIGN",PROPLIST_ITEM(ID_INTELLIGENT_CONNECT_DESIGN,"右键菜单->智能连接设计"));
	CLDSApp::AddMenuHelpItem("ID_COMPARE_LINEPART_POS",PROPLIST_ITEM(ID_COMPARE_LINEPART_POS,"右键菜单->对比杆件位置"));
	CLDSApp::AddMenuHelpItem("ID_SYNC_MIR_OBJ_PROP",PROPLIST_ITEM(ID_SYNC_MIR_OBJ_PROP,"右键菜单->更新关联对象"));
	CLDSApp::AddMenuHelpItem("ID_ADD_RELATIVE_OBJECT",PROPLIST_ITEM(ID_ADD_RELATIVE_OBJECT,"右键菜单->添加关联对象"));
	CLDSApp::AddMenuHelpItem("ID_SHADOW_CREATE",PROPLIST_ITEM(ID_SHADOW_CREATE,"右键菜单->影射构件"));
	CLDSApp::AddMenuHelpItem("ID_REUSE_PARTS",PROPLIST_ITEM(ID_REUSE_PARTS,"右键菜单->复用构件"));
	CLDSApp::AddMenuHelpItem("ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(ID_EDIT_ITEM_PROP_ITEM,"右键菜单->属性"));
	CLDSApp::AddMenuHelpItem("ID_EXPORT_BLOCK_FILE",PROPLIST_ITEM(ID_EXPORT_BLOCK_FILE,"右键菜单->导出部件分段"));
	CLDSApp::AddMenuHelpItem("ID_OUTER_ATTACH_ANGLE",PROPLIST_ITEM(ID_OUTER_ATTACH_ANGLE,"右键菜单->添加外贴角钢"));
	CLDSApp::AddMenuHelpItem("ID_UPGRADE_CTRL_NODE",PROPLIST_ITEM(ID_UPGRADE_CTRL_NODE,"右键菜单->转为控制节点"));
	CLDSApp::AddMenuHelpItem("ID_FAST_CIRCOVER_PLATE",PROPLIST_ITEM(ID_FAST_CIRCOVER_PLATE,"右键菜单->环向封板"));
	CLDSApp::AddMenuHelpItem("ID_FAST_CIRRIB_PLATE",PROPLIST_ITEM(ID_FAST_CIRRIB_PLATE,"右键菜单->环向肋板"));
	CLDSApp::AddMenuHelpItem("ID_FAST_TUBERIB_PLATE",PROPLIST_ITEM(ID_FAST_TUBERIB_PLATE,"右键菜单->跨越连板"));
	CLDSApp::AddMenuHelpItem("ID_MERGE_NODES",PROPLIST_ITEM(ID_MERGE_NODES,"右键菜单->合并节点"));
	CLDSApp::AddMenuHelpItem("ID_MOVE_DBOBJECT",PROPLIST_ITEM(ID_MOVE_DBOBJECT,"右键菜单->移动位置"));
	CLDSApp::AddMenuHelpItem("ID_LOOK_DESIGN_PARAM",PROPLIST_ITEM(ID_LOOK_DESIGN_PARAM,"右键菜单->查看设计参数"));
	CLDSApp::AddMenuHelpItem("ID_PLATE_PROFILE_RECT",PROPLIST_ITEM(ID_PLATE_PROFILE_RECT,"右键菜单->钢板外形矩形化"));
	CLDSApp::AddMenuHelpItem("ID_BOLT_PUNCH_HOLE",PROPLIST_ITEM(ID_BOLT_PUNCH_HOLE,"右键菜单->螺栓穿孔"));
	CLDSApp::AddMenuHelpItem("ID_IMPORT_BOLT_REF_TO_PART",PROPLIST_ITEM(ID_IMPORT_BOLT_REF_TO_PART,"右键菜单->引入螺栓"));
	CLDSApp::AddMenuHelpItem("ID_SWAP_G",PROPLIST_ITEM(ID_SWAP_G,"右键菜单->交换心距"));
	CLDSApp::AddMenuHelpItem("ID_UNIFY_ANGLE_BOLT_G",PROPLIST_ITEM(ID_UNIFY_ANGLE_BOLT_G,"右键菜单->规范螺栓心距参数"));
	CLDSApp::AddMenuHelpItem("ID_CREATE_BOLT_IN_PART",PROPLIST_ITEM(ID_CREATE_BOLT_IN_PART,"右键菜单->生成螺栓"));
	CLDSApp::AddMenuHelpItem("ID_LOOK_BOLT_IN_PART",PROPLIST_ITEM(ID_LOOK_BOLT_IN_PART,"右键菜单->查看螺栓"));
	CLDSApp::AddMenuHelpItem("ID_REPLACE_BOLT_DATUMPOINT",PROPLIST_ITEM(ID_REPLACE_BOLT_DATUMPOINT,"右键菜单->变换螺栓基点"));
	CLDSApp::AddMenuHelpItem("ID_MOVE_BOLT_SET",PROPLIST_ITEM(ID_MOVE_BOLT_SET,"右键菜单->移动螺栓"));
	CLDSApp::AddMenuHelpItem("ID_LOOK_LINEPART_IN_PLATE",PROPLIST_ITEM(ID_LOOK_LINEPART_IN_PLATE,"右键菜单->查看构件"));
	CLDSApp::AddMenuHelpItem("ID_CONVERT_TO_GROUP_JG",PROPLIST_ITEM(ID_CONVERT_TO_GROUP_JG,"右键菜单->转换为组合角钢"));
	CLDSApp::AddMenuHelpItem("ID_CONVERT_TO_SINGLE_JG",PROPLIST_ITEM(ID_CONVERT_TO_SINGLE_JG,"右键菜单->转换为单角钢"));
	CLDSApp::AddMenuHelpItem("ID_CONVERT_TO_TUBE",PROPLIST_ITEM(ID_CONVERT_TO_TUBE,"右键菜单->转换为钢管"));
	CLDSApp::AddMenuHelpItem("ID_AUTO_CORRECT_JG_LAYSTYLE",PROPLIST_ITEM(ID_AUTO_CORRECT_JG_LAYSTYLE,"右键菜单->修正角钢摆放位置"));
	CLDSApp::AddMenuHelpItem("ID_REMOVE_KAIHE_JIAO",PROPLIST_ITEM(ID_REMOVE_KAIHE_JIAO,"右键菜单->去除角钢开合角"));
	CLDSApp::AddMenuHelpItem("ID_IMPORT_LINEPART_TO_PLATE",PROPLIST_ITEM(ID_IMPORT_LINEPART_TO_PLATE,"右键菜单->加入射线杆件"));
	CLDSApp::AddMenuHelpItem("ID_START_WINGX_CUT_ANGLE",PROPLIST_ITEM(ID_START_WINGX_CUT_ANGLE,"右键菜单->始端X肢"));
	CLDSApp::AddMenuHelpItem("ID_START_WINGY_CUT_ANGLE",PROPLIST_ITEM(ID_START_WINGY_CUT_ANGLE,"右键菜单->始端Y肢"));
	CLDSApp::AddMenuHelpItem("ID_END_WINGX_CUT_ANGLE",PROPLIST_ITEM(ID_END_WINGX_CUT_ANGLE,"右键菜单->终端X肢"));
	CLDSApp::AddMenuHelpItem("ID_END_WINGY_CUT_ANGLE",PROPLIST_ITEM(ID_END_WINGY_CUT_ANGLE,"右键菜单->终端Y肢"));
	CLDSApp::AddMenuHelpItem("ID_WINGX_CUT_ANGLE",PROPLIST_ITEM(ID_WINGX_CUT_ANGLE,"右键菜单->两端X肢"));
	CLDSApp::AddMenuHelpItem("ID_WINGY_CUT_ANGLE",PROPLIST_ITEM(ID_WINGY_CUT_ANGLE,"右键菜单->两端Y肢"));
	CLDSApp::AddMenuHelpItem("ID_SELECT_RELA_BOLTS",PROPLIST_ITEM(ID_SELECT_RELA_BOLTS,"右键菜单->选择关联螺栓"));
	CLDSApp::AddMenuHelpItem("ID_ADD_RELA_BOLTS_TO_VIEW",PROPLIST_ITEM(ID_ADD_RELA_BOLTS_TO_VIEW,"右键菜单->导入关联螺栓"));
	CLDSApp::AddMenuHelpItem("ID_INSERT_JGNODE_TO_VIEW",PROPLIST_ITEM(ID_INSERT_JGNODE_TO_VIEW,"右键菜单->导入指定构件"));
	CLDSApp::AddMenuHelpItem("ID_REMOVE_OBJS_FROM_VIEW",PROPLIST_ITEM(ID_REMOVE_OBJS_FROM_VIEW,"右键菜单->移出当前视图"));
	CLDSApp::AddMenuHelpItem("ID_SET_SPREAD_FACE",PROPLIST_ITEM(ID_SET_SPREAD_FACE,"右键菜单->设定绘图平面"));
	CLDSApp::AddMenuHelpItem("ID_SAVE_AS_NEW_VIEW",PROPLIST_ITEM(ID_SAVE_AS_NEW_VIEW,"右键菜单->另存为新视图"));
	CLDSApp::AddMenuHelpItem("ID_SET_ARC_LIFT",PROPLIST_ITEM(ID_SET_ARC_LIFT,"右键菜单->预拱设定"));
	CLDSApp::AddMenuHelpItem("ID_SAVE_AS_NEW_BLOCK",PROPLIST_ITEM( ID_SAVE_AS_NEW_BLOCK,"右键菜单->定义为新部件"));
	CLDSApp::AddMenuHelpItem("ID_EXPLODE_BLOCKREF",PROPLIST_ITEM( ID_EXPLODE_BLOCKREF,"右键菜单->炸开部件引用"));
}
PROPLIST_ITEM* CLDSApp::AddMenuHelpItem(char* keyStr,PROPLIST_ITEM item)
{
	return AddUniversalPropItem(keyStr,item,&propMenuHashtable,&hashMenuHelpItemById);
}
CHashStrList<PROPLIST_ITEM> CLDSApp::propDialogHashtable;
CHashSet<PROPLIST_ITEM*>CLDSApp::hashDialogHelpItemById;
void CLDSApp::InitDialogHelpHashtable()
{
	CLDSApp::propDialogHashtable.SetHashTableGrowSize(100);//HASHTABLESIZE);
	CLDSApp::AddDialogHelpItem("IDD_TOWER_STRUCTURE_DLG",PROPLIST_ITEM(IDD_TOWER_STRUCTURE_DLG,"模型窗口*"));
	CLDSApp::AddDialogHelpItem("IDD_OUTPUT_INFO_DLG",PROPLIST_ITEM(IDD_OUTPUT_INFO_DLG,"任务输出窗口*"));
	CLDSApp::AddDialogHelpItem("IDD_PARTNO_LIST_DLG",PROPLIST_ITEM(IDD_PARTNO_LIST_DLG,"构件编号窗口*"));
	CLDSApp::AddDialogHelpItem("IDD_REVISION_OPER_DLG",PROPLIST_ITEM(IDD_REVISION_OPER_DLG,"校审窗口*"));
	CLDSApp::AddDialogHelpItem("IDD_TOWER_PART_GROUP_DLG",PROPLIST_ITEM(IDD_TOWER_PART_GROUP_DLG,"组焊件窗口*"));
	CLDSApp::AddDialogHelpItem("ID_DESIGN_TUBE_BETWEEN_PLATE_DLG",PROPLIST_ITEM(ID_DESIGN_TUBE_BETWEEN_PLATE_DLG,"钢管夹板"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_CIRCULAR_COVER_PLATE_DLG",PROPLIST_ITEM(IDD_ADD_CIRCULAR_COVER_PLATE_DLG,"添加环向封板"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_CIRCULAR_PLATE_DLG",PROPLIST_ITEM(IDD_ADD_CIRCULAR_PLATE_DLG,"添加环向肋板"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_DIJIAO_LS_DLG",PROPLIST_ITEM(IDD_ADD_DIJIAO_LS_DLG,"添加底脚孔"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_EIFFIEL_DLG",PROPLIST_ITEM(IDD_ADD_EIFFIEL_DLG,"添加风压折减系数"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_FILL_PLANK_DLG",PROPLIST_ITEM(IDD_ADD_FILL_PLANK_DLG,"添加填板"));
	CLDSApp::AddDialogHelpItem("IDD_ADD_WORKSTATUS_DLG",PROPLIST_ITEM(IDD_ADD_WORKSTATUS_DLG,"工况基本属性"));
	CLDSApp::AddDialogHelpItem("IDD_ARC_LIFT_DLG",PROPLIST_ITEM(IDD_ARC_LIFT_DLG,"圆弧预拱"));
	CLDSApp::AddDialogHelpItem("IDD_ATTACH_WIND_LOAD_DLG",PROPLIST_ITEM(IDD_ATTACH_WIND_LOAD_DLG,"附加荷载"));
	CLDSApp::AddDialogHelpItem("IDD_BEND_PLATE_BY_ANGLE_DLG",PROPLIST_ITEM(IDD_BEND_PLATE_BY_ANGLE_DLG,"制弯钢板"));
	CLDSApp::AddDialogHelpItem("IDD_BOLT_THROUGH_THICK_DLG",PROPLIST_ITEM(IDD_BOLT_THROUGH_THICK_DLG,"螺栓通厚*"));
	CLDSApp::AddDialogHelpItem("IDD_CMD_LINE_DLG",PROPLIST_ITEM(IDD_CMD_LINE_DLG,"命令行*"));
	CLDSApp::AddDialogHelpItem("IDD_CORRECT_PLATE_PARA_DLG",PROPLIST_ITEM(IDD_CORRECT_PLATE_PARA_DLG,"更新钢板设计参数*"));
	CLDSApp::AddDialogHelpItem("IDD_CUT_PLATE_OPTION_DLG",PROPLIST_ITEM(IDD_CUT_PLATE_OPTION_DLG,"切割钢板*"));
	CLDSApp::AddDialogHelpItem("IDD_DATUM_PLANE_DLG",PROPLIST_ITEM(IDD_DATUM_PLANE_DLG,"基准面定义*"));
	CLDSApp::AddDialogHelpItem("IDD_DATUM_POINT_PROPERTY_DLG",PROPLIST_ITEM(IDD_DATUM_POINT_PROPERTY_DLG,"基准点定义*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_COLUMN_RAY_ROD_DLG",PROPLIST_ITEM(IDD_DESIGN_COLUMN_RAY_ROD_DLG,"柱面射线杆件*"));
	CLDSApp::AddDialogHelpItem("IDD_DEFINED_VIEW_PROP_DLG",PROPLIST_ITEM(IDD_DEFINED_VIEW_PROP_DLG,"定制视图*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_ARC_PART",PROPLIST_ITEM(IDD_DESIGN_ARC_PART,"环形构件*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_BUSHTUBE_DLG",PROPLIST_ITEM(IDD_DESIGN_BUSHTUBE_DLG,"设计套管*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_CIRCULAR_PLATE_DLG",PROPLIST_ITEM(IDD_DESIGN_CIRCULAR_PLATE_DLG,"环向板*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_CONNECTION_PLATE_DLG",PROPLIST_ITEM(IDD_DESIGN_CONNECTION_PLATE_DLG,"连接板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_ONEBOLT_DLG",PROPLIST_ITEM(IDD_DESIGN_ONEBOLT_DLG,"单螺栓连接*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_OUTER_ATTACH_DLG",PROPLIST_ITEM(IDD_DESIGN_OUTER_ATTACH_DLG,"外贴材设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_RIBBED_PLATE_DLG",PROPLIST_ITEM(IDD_DESIGN_RIBBED_PLATE_DLG,"肋板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_SHADOW_DLG",PROPLIST_ITEM(IDD_DESIGN_SHADOW_DLG,"影射构件设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_SIMPLE_LADDER_DLG",PROPLIST_ITEM(IDD_DESIGN_SIMPLE_LADDER_DLG,"设计简易爬梯*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_SPECIAL_FL_DLG",PROPLIST_ITEM(IDD_DESIGN_SPECIAL_FL_DLG,"特殊法兰设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_TUBE_ENDLJ_DLG",PROPLIST_ITEM(IDD_DESIGN_TUBE_ENDLJ_DLG,"钢管端头设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_TUBE_RIB_PLATE_DLG",PROPLIST_ITEM(IDD_DESIGN_TUBE_RIB_PLATE_DLG,"跨越钢管连接*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_VECTOR_PROP_DLG",PROPLIST_ITEM(IDD_DESIGN_VECTOR_PROP_DLG,"方向设计*"));
	CLDSApp::AddDialogHelpItem("IDD_DESIGN_XEND_RIB_PLATE_DLG",PROPLIST_ITEM(IDD_DESIGN_XEND_RIB_PLATE_DLG,"十字插板加劲*"));
	CLDSApp::AddDialogHelpItem("IDD_DIANBAN_PARA_DLG",PROPLIST_ITEM(IDD_DIANBAN_PARA_DLG,"输入垫板构件*"));
	CLDSApp::AddDialogHelpItem("IDD_EXTRA_ANGLEODD_SET_DLG",PROPLIST_ITEM(IDD_EXTRA_ANGLEODD_SET_DLG,"角钢固定正负头设计*"));
	CLDSApp::AddDialogHelpItem("IDD_FACE_OFFSET_DLG",PROPLIST_ITEM(IDD_FACE_OFFSET_DLG,"平推面定义*"));
	CLDSApp::AddDialogHelpItem("IDD_FILE_PROPERTY_DLG",PROPLIST_ITEM(IDD_FILE_PROPERTY_DLG,"塔属性*"));
	CLDSApp::AddDialogHelpItem("IDD_FOOT_DESIGN_TEMPL_DLG",PROPLIST_ITEM(IDD_FOOT_DESIGN_TEMPL_DLG,"底脚板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_FOOTNAIL_SPACE_DLG",PROPLIST_ITEM(IDD_FOOTNAIL_SPACE_DLG,"脚钉间距*"));
	CLDSApp::AddDialogHelpItem("IDD_GROUP_SET_LAYER_DLG",PROPLIST_ITEM(IDD_GROUP_SET_LAYER_DLG,"成组修改图层名*"));
	CLDSApp::AddDialogHelpItem("IDD_INTELLIGENT_CONNECT_PARA_DLG",PROPLIST_ITEM(IDD_INTELLIGENT_CONNECT_PARA_DLG,"智能连接设置*"));
	CLDSApp::AddDialogHelpItem("IDD_JDB_DESIGN_DLG",PROPLIST_ITEM(IDD_JDB_DESIGN_DLG,"节点板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_LAY_FILL_PLANK_DLG",PROPLIST_ITEM(IDD_LAY_FILL_PLANK_DLG,"填板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_LAY_ANGLE_BOLT_DLG",PROPLIST_ITEM(IDD_LAY_ANGLE_BOLT_DLG,"布置角钢螺栓*"));
	CLDSApp::AddDialogHelpItem("IDD_LINE_ANGLE_RIB_PLATE_DLG",PROPLIST_ITEM(IDD_LINE_ANGLE_RIB_PLATE_DLG,"角钢肋板*"));
	CLDSApp::AddDialogHelpItem("IDD_LINK_TA_JG_DLG",PROPLIST_ITEM(IDD_LINK_TA_JG_DLG,"连接塔材(角钢)*"));
	CLDSApp::AddDialogHelpItem("IDD_LINK_TSA_POLE_DLG",PROPLIST_ITEM(IDD_LINK_TSA_POLE_DLG,"连接杆件(TSA)*"));
	CLDSApp::AddDialogHelpItem("IDD_MAIN_LINEPART_FENG_DUAN_DLG",PROPLIST_ITEM(IDD_MAIN_LINEPART_FENG_DUAN_DLG,"塔材分段*"));
	CLDSApp::AddDialogHelpItem("IDD_JOINT_DESIGN_PARA_DLG",PROPLIST_ITEM(IDD_JOINT_DESIGN_PARA_DLG,"角钢接头*"));
	CLDSApp::AddDialogHelpItem("IDD_MERGE_LJB_DLG",PROPLIST_ITEM(IDD_MERGE_LJB_DLG,"合并连接板*"));
	CLDSApp::AddDialogHelpItem("IDD_MIR_MSG_DLG",PROPLIST_ITEM(IDD_MIR_MSG_DLG,"对称信息*"));
	CLDSApp::AddDialogHelpItem("IDD_MODEL_PROP_DLG",PROPLIST_ITEM(IDD_MODEL_PROP_DLG,"呼高组属性*"));
	CLDSApp::AddDialogHelpItem("IDD_MOVE_BOLT_DLG",PROPLIST_ITEM(IDD_MOVE_BOLT_DLG,"移动螺栓*"));
	CLDSApp::AddDialogHelpItem("IDD_NEW_FILE_DLG",PROPLIST_ITEM(IDD_NEW_FILE_DLG,"新建模型(放样)*"));
	CLDSApp::AddDialogHelpItem("IDD_NEW_TOWER_DLG",PROPLIST_ITEM(IDD_NEW_TOWER_DLG,"新建模型(LDS)*"));
	CLDSApp::AddDialogHelpItem("IDD_OBJECT_SYNC_PROP_DETAILS_DLG",PROPLIST_ITEM(IDD_OBJECT_SYNC_PROP_DETAILS_DLG,"设置对象同步属性*"));
	CLDSApp::AddDialogHelpItem("IDD_ODDMENT_DESIGN_DLG",PROPLIST_ITEM(IDD_ODDMENT_DESIGN_DLG,"正负头设计*"));
	CLDSApp::AddDialogHelpItem("IDD_PART_AUTH_PERM_DLG",PROPLIST_ITEM(IDD_PART_AUTH_PERM_DLG,"构件授权*"));
	CLDSApp::AddDialogHelpItem("IDD_PART_WORK_PLANE_DLG",PROPLIST_ITEM(IDD_PART_WORK_PLANE_DLG,"构件工作平面*"));
	CLDSApp::AddDialogHelpItem("IDD_PLATE_BASIC_PARA_DLG",PROPLIST_ITEM(IDD_PLATE_BASIC_PARA_DLG,"特殊板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_RELATIVE_OBJECT_DLG",PROPLIST_ITEM(IDD_RELATIVE_OBJECT_DLG,"关联对象*"));
	CLDSApp::AddDialogHelpItem("IDD_REUSE_PARTS_DLG",PROPLIST_ITEM(IDD_REUSE_PARTS_DLG,"复用构件*"));
	CLDSApp::AddDialogHelpItem("IDD_RSLT_BUFFER_DLG",PROPLIST_ITEM(IDD_RSLT_BUFFER_DLG,"计算结果*"));
	CLDSApp::AddDialogHelpItem("IDD_SECTION_VIEW_PROPERTY_DLG",PROPLIST_ITEM(IDD_SECTION_VIEW_PROPERTY_DLG,"组焊件附加视图*"));
	CLDSApp::AddDialogHelpItem("IDD_SEL_FACE_PANEL_DLG",PROPLIST_ITEM(IDD_SEL_FACE_PANEL_DLG,"选择标准面结构*"));
	CLDSApp::AddDialogHelpItem("IDD_SET_ARC_LIFT_DLG",PROPLIST_ITEM(IDD_SET_ARC_LIFT_DLG,"设置预拱项*"));
	CLDSApp::AddDialogHelpItem("IDD_SET_NC_DRIVER_DLG",PROPLIST_ITEM(IDD_SET_NC_DRIVER_DLG,"设置NC驱动*"));
	CLDSApp::AddDialogHelpItem("IDD_SHADOW_PART_CREATE_DLG",PROPLIST_ITEM(IDD_SHADOW_PART_CREATE_DLG,"生成影射构件*"));
	CLDSApp::AddDialogHelpItem("IDD_SHORTJG_DEF_DLG",PROPLIST_ITEM(IDD_SHORTJG_DEF_DLG,"短角钢定义*"));
	CLDSApp::AddDialogHelpItem("IDD_JG_GUIGE_TABLE_DLG",PROPLIST_ITEM(IDD_JG_GUIGE_TABLE_DLG,"角钢规格表*"));
	CLDSApp::AddDialogHelpItem("IDD_JG_ZHUN_JU_LIB_DLG",PROPLIST_ITEM(IDD_JG_ZHUN_JU_LIB_DLG,"角钢准距表*"));
	CLDSApp::AddDialogHelpItem("IDD_WIRE_LIB_DLG",PROPLIST_ITEM(IDD_WIRE_LIB_DLG,"线缆规格库*"));
	CLDSApp::AddDialogHelpItem("IDD_TUBE_END_LJ_GUIGE_TBL_DLG",PROPLIST_ITEM(IDD_TUBE_END_LJ_GUIGE_TBL_DLG,"钢管端连接规格表*"));
	CLDSApp::AddDialogHelpItem("IDD_TUBE_ENDLJ_STDPART_LIB_DLG",PROPLIST_ITEM(IDD_TUBE_ENDLJ_STDPART_LIB_DLG,"钢管端连接标准库*"));
	CLDSApp::AddDialogHelpItem("IDD_TUBE_GUIGE_LIB_DLG",PROPLIST_ITEM(IDD_TUBE_GUIGE_LIB_DLG,"钢管规格表*"));
	CLDSApp::AddDialogHelpItem("IDD_USERDEF_CMD_GROUP_DLG",PROPLIST_ITEM(IDD_USERDEF_CMD_GROUP_DLG,"自定义命令组*"));
	CLDSApp::AddDialogHelpItem("IDD_V_FACE_JDB_DLG",PROPLIST_ITEM(IDD_V_FACE_JDB_DLG,"V面板设计*"));
	CLDSApp::AddDialogHelpItem("IDD_WINDLOAD_SEG_DLG",PROPLIST_ITEM(IDD_WINDLOAD_SEG_DLG,"风压段属性*"));
	CLDSApp::AddDialogHelpItem("IDD_WORK_STATUS_DLG",PROPLIST_ITEM(IDD_WORK_STATUS_DLG,"工况属性*"));
	CLDSApp::AddDialogHelpItem("IDD_WORKENVLOAD_MODEL_DLG",PROPLIST_ITEM(IDD_WORKENVLOAD_MODEL_DLG,"线条原始计算荷载*"));
	CLDSApp::AddDialogHelpItem("IDD_X_INSERT_PLATE_INTERS_TUBE_DLG",PROPLIST_ITEM(IDD_X_INSERT_PLATE_INTERS_TUBE_DLG,"插板式钢管交叉构造*"));
}
PROPLIST_ITEM* CLDSApp::AddDialogHelpItem(char* keyStr,PROPLIST_ITEM item)
{
	return AddUniversalPropItem(keyStr,item,&propDialogHashtable,&hashDialogHelpItemById);
}
//绘图模块对话框帮助项导出管理
CHashStrList<PROPLIST_ITEM> CLDSApp::propCadDialogHashtable;
CHashSet<PROPLIST_ITEM*>CLDSApp::hashCadDialogHelpItemById;
void CLDSApp::InitCadDialogHelpHashtable()
{
	int id=1;
	CLDSApp::propCadDialogHashtable.SetHashTableGrowSize(100);//HASHTABLESIZE);
	AddCadDialogHelpItem("IDD_SYSTEM_SETTING_DLG",PROPLIST_ITEM(id++,"系统设置*"));
	AddCadDialogHelpItem("IDD_BOM_CFG_DLG",PROPLIST_ITEM(id++,"模型窗口*"));
	AddCadDialogHelpItem("IDD_BREAK_LINE_DLG",PROPLIST_ITEM(id++,"破断线*"));
	AddCadDialogHelpItem("IDD_DRAW_MAP_SCRIPT_DLG",PROPLIST_ITEM(id++,"智绘图命令列表*"));
	AddCadDialogHelpItem("IDD_NEW_DRAWINGVIEW_DLG",PROPLIST_ITEM(id++,"新建图纸*"));
	AddCadDialogHelpItem("IDD_PART_SET_CREATOR_DLG",PROPLIST_ITEM(id++,"构件汇总*"));
	AddCadDialogHelpItem("IDD_SEL_PART_GROUP_DLG",PROPLIST_ITEM(id++,"选择组焊件*"));
	AddCadDialogHelpItem("IDD_SELECT_DRAWING_DLG",PROPLIST_ITEM(id++,"图纸列表*"));
	AddCadDialogHelpItem("IDD_SELECT_SIMPLE_SPEC_DLG",PROPLIST_ITEM(id++,"选择简化规格"));
	AddCadDialogHelpItem("IDD_SET_ACTIVE_MODULE_DLG",PROPLIST_ITEM(id++,"设置当前呼高*"));
}
PROPLIST_ITEM* CLDSApp::AddCadDialogHelpItem(char* keyStr,PROPLIST_ITEM item)
{
	return AddUniversalPropItem(keyStr,item,&propCadDialogHashtable,&hashCadDialogHelpItemById);
}

CHashStrList<PROPLIST_ITEM> CLDSApp::propSubGroupHelpHashtable;	//属性字符串哈希表
CHashSet<PROPLIST_ITEM*> CLDSApp::hashSubGroupHlpHelpItemById;
void CLDSApp::InitSubGroupHelpHashtable()
{
	int id=1;
	AddSubGroupHelpItem("IDD_TOWER_STRUCTURE_DLG",PROPLIST_ITEM(IDD_TOWER_STRUCTURE_DLG,"模型窗口"));
	CTowerTreeDlg::InitMenuHelpHashtable();
	//AddSubGroupHelpItem("IDD_OUTPUT_INFO_DLG",PROPLIST_ITEM(IDD_OUTPUT_INFO_DLG,"任务输出窗口"));
	AddSubGroupHelpItem("IDD_CMD_LINE_DLG",PROPLIST_ITEM(IDD_CMD_LINE_DLG,"命令行窗口"));
	//AddSubGroupHelpItem("IDD_REVISION_OPER_DLG",PROPLIST_ITEM(IDD_REVISION_OPER_DLG,"校审窗口"));
}
PROPLIST_ITEM* CLDSApp::AddSubGroupHelpItem(char* keyStr,PROPLIST_ITEM item)
{
	return AddUniversalPropItem(keyStr,item,&propSubGroupHelpHashtable,&hashSubGroupHlpHelpItemById);
}
CHashStrList<PROPLIST_ITEM>* CLDSApp::GetSubGroupHelpPropHashtable(long idSubGroup)
{
	switch(idSubGroup)
	{
	case IDD_TOWER_STRUCTURE_DLG:
		return &CTowerTreeDlg::propMenuHashtable;
	//case IDD_OUTPUT_INFO_DLG:
	//	return &COutputInfo::propMenuHashtable;
	case IDD_CMD_LINE_DLG:
    	return &CCmdLineDlg::propMenuHashtable;
#ifdef __PART_DESIGN_INC_
	//case IDD_REVISION_OPER_DLG:
	//	return &CRevisionOperDlg::propMenuHashtable;
#endif
	default:
		break;
	}
	return NULL;
}

CLDSApp::CLDSApp()
{
#ifdef __CONNECT_REMOTE_SERVER_
	m_bNetModelWork=FALSE;
#endif
	m_bHiColorIcons = TRUE;
	EnableHtmlHelp();
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
	m_bStartAsFemResolver=false;
	m_bCooperativeWork=FALSE;
	m_wDogModule=0;
	m_bDrawingWorkEnvOnly=m_bAnalysisWorkEnvOnly=m_bManuRevisionMode=FALSE;
	m_hBomExport=NULL;
#ifdef __PART_DESIGN_INC_
	m_hWireLayoutModule=NULL;
	m_eEnvType=FEMROD_ENV;
	m_ePrevEnvType=FEMELEM_ENV;	//默认前一环境为单元设计环境
	m_bAnalysisWorkEnv=TRUE;
#else
	m_eEnvType=LINE_ENV;	//单线模型环境
	m_ePrevEnvType=SOLID_ENV;	//默认前一环境为实体环境
	m_bAnalysisWorkEnv=FALSE;
#endif
	m_eUiType = DEFAULT_MODELING_UI;
	m_ePreUiType = DEFAULT_MODELING_UI;
	m_hPipeServerListen=m_hPipeServerSend=NULL;
	env.iSeg=SEGI();
	strcpy(env.layer,"SPQ");

	env.m_bDisplayLineVecMark = TRUE;
	env.m_bDisplayWorldUcs = TRUE;
	env.m_iNcGroupType = 0;
	g_sysPara.m_bDispPartACS = FALSE;
	g_sysPara.m_bDispWorkUcs = FALSE;
	g_sysPara.m_bDispBlockLCS = FALSE;
	env.mcs.axis_x.Set(1,0,0);
	env.mcs.axis_y.Set(0,0,1);
	env.mcs.axis_z.Set(0,-1,0);
	console.m_curWCS = env.mcs;
	strcpy(work_path,"");
	strcpy(execute_path,"");
	m_uAnimateTimer=0;
	m_fMaxAmplitude = 500;
	m_nSampleTimerInterval = 100;		//动画模拟振型显示时取样间隔（ms）
	m_nSimulateVibraModePeriod = 1000;//动画模拟振型显示时的周期值（ms）

	m_dwUpdateWord=0;	//待更新用户数据列表
	m_bDisplayAllPart=TRUE;//显示所有构件
	m_bNeedUpdatePerm=FALSE;//不需更新权限
	m_nUserId=1;		//默认用户为1号用户Admin
	m_version[0]=m_version[1]=0;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CLDSApp object

CLDSApp theApp;
CTowerArchiveLibrary g_orgPartLib;
//#define _LOCAL_ENCRYPT
/////////////////////////////////////////////////////////////////////////////
// CLDSApp initialization
static BOOL SERVR_SELF_MODE;
void TestPortal();	//From TestMain.cpp仅为了临时快速测试一些功能代码用wjh -2013.9.8
void ExportPropListMapFile();
void ImportPropHelpListMapFile(char* bmp_file);	//from Tower.cpp
#ifdef __PART_DESIGN_INC_
CFemSolverLoop _WorkLoop;	//作为子进程进行有限元分析计算时，启动的数据收发监听线程 wjh-2016.2.17
#endif
bool DetectSpecifiedHaspKeyFile(const char* default_file)
{
	FILE* fp=fopen(default_file,"rt");
	if(fp==NULL)
		return false;
	bool detected=false;
	CXhChar200 line_txt;//[MAX_PATH];
	CXhChar200 scope_xmlstr;
	scope_xmlstr.Append(
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
		"<haspscope>");
	while(!feof(fp))
	{
		if(fgets(line_txt,line_txt.GetLengthMax(),fp)==NULL)
			break;
		line_txt.Replace("＝","=");
		char* final=SearchChar(line_txt,';');
		if(final!=NULL)
			*final=0;
		char *skey=strtok(line_txt," =,");
		//常规设置
		if(_stricmp(skey,"Key")==0)
		{
			if(skey=strtok(NULL,"=,"))
			{
				scope_xmlstr.Append("<hasp id=\"");
				scope_xmlstr.Append(skey);
				scope_xmlstr.Append("\" />");
				detected=true;
			}
		}
	}
	fclose(fp);
	scope_xmlstr.Append("</haspscope>");
	if(detected)
		SetHaspLoginScope(scope_xmlstr);
	return detected;
}
void InitCustomizeConfiguration(UINT uiCustomizeSerial)
{
	//CPlankDrawing::PREFER_STYLEFLAG.SetMaskBitCount(0);
	//sys.customize.ciCustomerSerial=(BYTE)uiCustomizeSerial;	//客户定制标识序号
	gxLocalizer.InitCustomerSerial(uiCustomizeSerial);
	gxPortalLocalizer.InitCustomerSerial(uiCustomizeSerial);
	CLDSPart::CustomerSerial=CTowerType::CustomerSerial=uiCustomizeSerial;
	if(uiCustomizeSerial==3)
	{	//青岛武晓，定制项如下：
	}
}
bool VerifyDllVersionCompatible()
{
	DWORD dwXeroCadMajorVersion=XeroCadDllGetVersion()&0xffffff00;
	DWORD dwDrawSolidMajorVersion=CDrawSolidFactory::DrawSolidDllGetVersion()&0xffffff00;
	if(dwXeroCadMajorVersion!=0x01000200)
	{
		AfxMessageBox("XeroCad.dll库版本与当前主程序版本不匹配，存在兼容性问题!");
		return false;
	}
	if(dwDrawSolidMajorVersion!=0x01000100)
	{
		AfxMessageBox("DrawSolid.dll库版本与当前主程序版本不匹配，存在兼容性问题!");
		return false;
	}
	return true;
}

BOOL CLDSApp::ImportLicense()
{
	char lic_file[MAX_PATH];
	char default_file[MAX_PATH];	//默认当前执行目录
	DWORD version[2];
	BYTE* byteVer=(BYTE*)version;
	WORD wModule=0;
	version[1]=20180130;//20170616;	//版本发布日期
	BYTE cProductType=PRODUCT_TMA;
#ifdef __TMA_
#ifdef _LOCAL_ENCRYPT
	sprintf(lic_file,"%sCMA.lic",APP_PATH);
	cProductType=PRODUCT_CTMA;
#else
	sprintf(lic_file,"%sTMA.lic",APP_PATH);
	cProductType=PRODUCT_TMA;
#endif
	byteVer[0]=5;
	byteVer[1]=2;
	byteVer[2]=0;
	byteVer[3]=0;
#elif defined(__LMA_)||defined(__STATION_)
//#ifdef __LMA_
	sprintf(lic_file,"%sLMA.lic",APP_PATH);
//#else
//	sprintf(lic_file,"%sStation.lic",APP_PATH);
//#endif
	wModule=1;
	byteVer[0]=2;
	byteVer[1]=0;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_)
	sprintf(lic_file,"%sTSA.lic",APP_PATH);
	wModule=2;
	byteVer[0]=1;
	byteVer[1]=8;
	byteVer[2]=1;
	byteVer[3]=2;
	cProductType=PRODUCT_TSA;
#elif defined(__LDS_)
	sprintf(lic_file,"%sLDS.lic",APP_PATH);
	wModule=3;
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_LDS;
#elif defined(__TDA_)
	sprintf(lic_file,"%sTDA.lic",APP_PATH);
	wModule=3;
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_TDA;
#endif
	m_version[0]=version[0];
	m_version[1]=version[1];
	strcpy(default_file,lic_file);
	//查找是否存在指定加密锁号的文件 wjh-2016.9.20
	char* separator=SearchChar(default_file,'.',true);
	strcpy(separator,".key");
	DetectSpecifiedHaspKeyFile(default_file);
	DWORD retCode=0;
	bool passed_liccheck=true;
	for(int i=0;true;i++){
#ifndef _LEGACY_LICENSE
		retCode=ImportLicFile(lic_file,cProductType,version);	
#else
#ifdef _NET_DOG
		m_wDogModule=NetDogModule(lic_file);
		if(!InitNetLicEnv(m_wDogModule))
			retCode=-1;
		else
#endif
			retCode=ImportLicFile(lic_file,version);
#endif
		CXhChar50 errormsgstr("未知证书读取错误");
		if(retCode==0)
		{
			passed_liccheck=false;
			if(GetLicVersion()<1000005)
#ifdef AFX_TARG_ENU_ENGLISH
				errormsgstr.Copy("The certificate file has been out of date, the current software's version must use the new certificate file！");
#else 
				errormsgstr.Copy("该证书文件已过时，当前软件版本必须使用新证书！");
#endif
			else if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_BASIC))
#ifdef AFX_TARG_ENU_ENGLISH
				errormsgstr.Copy("Software Lacks of legitimate use authorized!");
#else 
				errormsgstr.Copy("软件缺少合法使用授权!");
#endif
			else
			{
				passed_liccheck=true;
				WriteProfileString("Settings","lic_file2",lic_file);
			}
			if(!passed_liccheck)
			{
#ifndef _LEGACY_LICENSE
				ExitCurrentDogSession();
#elif defined(_NET_DOG)
				ExitNetLicEnv(m_wDogModule);
#endif
			}
			else
				break;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(retCode==-1)
				errormsgstr.Copy("0# Hard dog failed to initialize!");
			else if(retCode==1)
				errormsgstr.Copy("1# Unable to open the license file!");
			else if(retCode==2)
				errormsgstr.Copy("2# License file was damaged!");
			else if(retCode==3||retCode==4)
				errormsgstr.Copy("3# License file not found or does'nt match the hard lock!");
			else if(retCode==5)
				errormsgstr.Copy("5# License file doesn't match the authorized products in hard lock!");
			else if(retCode==6)
				errormsgstr.Copy("6# Beyond the scope of authorized version !");
			else if(retCode==7)
				errormsgstr.Copy("7# Beyond the scope of free authorize version !");
			else if(retCode==8)
				errormsgstr.Copy("8# The serial number of license file does'nt match the hard lock!");
#else 
			if(retCode==-1)
				errormsgstr.Copy("0#加密狗初始化失败!");
			else if(retCode==1)
				errormsgstr.Copy("1#无法打开证书文件");
			else if(retCode==2)
				errormsgstr.Copy("2#证书文件遭到破坏");
			else if(retCode==3||retCode==4)
				errormsgstr.Copy("3#执行目录下不存在证书文件或证书文件与加密狗不匹配");
			else if(retCode==5)
				errormsgstr.Copy("5#证书与加密狗产品授权版本不匹配");
			else if(retCode==6)
				errormsgstr.Copy("6#超出版本使用授权范围");
			else if(retCode==7)
				errormsgstr.Copy("7#超出免费版本升级授权范围");
			else if(retCode==8)
				errormsgstr.Copy("8#证书序号与当前加密狗不一致");
			else if(retCode==9)
				errormsgstr.Copy("9#授权过期，请续借授权");
			else if(retCode==10)
				errormsgstr.Copy("10#程序缺少相应执行权限，请以管理员权限运行程序");
			else if (retCode == 11)
				errormsgstr.Copy("11#授权异常，请使用管理员权限重新申请证书");
			else
				errormsgstr.Printf("未知错误，错误代码%d#", retCode);
#endif
#ifndef _LEGACY_LICENSE
			ExitCurrentDogSession();
#elif defined(_NET_DOG)
			ExitNetLicEnv(m_wDogModule);
#endif
			passed_liccheck=false;
			//return FALSE;
		}
		if(!passed_liccheck)
		{
			CLicenseAuthorizeDlg authdlg;
			CSplashWnd::RevokeHideSplashScreen();	//不关闭开机画面窗口会阻塞所有对话框 wjh-2019.2.11
			authdlg.m_sErrorMsg=errormsgstr;
			authdlg.m_nLicRetCode = retCode;
			if(retCode!=1)
				authdlg.InitLicenseModeByLicFile(lic_file);
			if(authdlg.DoModal(retCode==9)!=IDOK)
				return FALSE;
			if(passed_liccheck=authdlg.IsLicfileImported())
				break;	//内部已成功导入证书文件
		}
	}
	if(!passed_liccheck)
	{
#ifndef _LEGACY_LICENSE
		ExitCurrentDogSession();
#elif defined(_NET_DOG)
		ExitNetLicEnv(m_wDogModule);
#endif
		return FALSE;
	}
	CSplashWnd::RevokeHideSplashScreen();	//不关闭开机画面窗口会阻塞所有对话框 wjh-2019.2.11
	return TRUE;
}
UINT LoginToServer(LPVOID pParam)
{
	CLicenseVirtualRentalPage::TestLogin(true, true);
	return TRUE;
}
BOOL CLDSApp::InitInstance()
{
\
	{
\
		CCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
\
	}

#ifdef AFX_TARG_ENU_ENGLISH
	//SetThreadLocale(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
	SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
#endif
	CWinAppEx::InitInstance();
	AfxEnableControlContainer();
	EnableTaskbarInteraction(FALSE);
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

/*#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif*/
	if(!VerifyDllVersionCompatible())
		return FALSE;
	::SetUnhandledExceptionFilter(LDSExceptionFilter);	//设置未处理异常过滤器生成dmp文件 wht 19-07-08
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	free((void*)m_pszAppName);
#ifdef __TMA_
#ifdef AFX_TARG_ENU_ENGLISH
	m_pszAppName=_tcsdup(_T("TMA-EN"));
#else
	m_pszAppName=_tcsdup(_T("TMA"));
#endif
#elif defined(__LMA_)
	m_pszAppName=_tcsdup(_T("LMA"));
#elif defined(__TSA_)
	m_pszAppName=_tcsdup(_T("TSA"));
#elif defined(__LDS_)
#ifdef __LDS_TSA_	//LDS的内力分析模块
	m_pszAppName=_tcsdup(_T("TSA"));
#else
	m_pszAppName=_tcsdup(_T("LDS"));
#endif
#else
	m_pszAppName=_tcsdup(_T("TDA"));
#endif
	SetRegistryKey(_T("Xerofox"));
	GetSysPath(APP_PATH);
	char system_path[MAX_PATH];
	m_bCooperativeWork=FALSE;	//以单机方式进行工作
#ifdef __LDS_TSA_	//LDS的内力分析模块
	m_bAnalysisWorkEnvOnly=m_bAnalysisWorkEnv=TRUE;
#endif
	//判断LDS是否以网络协同的方式进行工作
	m_bStartAsFemResolver=false;
	if(__argc>1&&(__argv[1][0]=='-'||__argv[1][0]=='/'))
	{
		for(int i=1;i<__argc;i++)
		{
			strcpy(system_path,__argv[i]);
			char *key=strtok(system_path,"-/");
			if(key&&stricmp(key,"c")==0)//以网络协同的方式进行工作
				m_bCooperativeWork=TRUE;
#ifdef __PART_DESIGN_INC_
			else if(key&&stricmp(key,"fem")==0)
				m_bStartAsFemResolver=true;
#endif
			else if(key&&stricmp(key,"dc")==0)//以网络协同的方式进行工作
			{
				m_bCooperativeWork=TRUE;
				SERVR_SELF_MODE=TRUE;
			}
			else if(key&&stricmp(key,"public")==0)	//临时阻止进程内的内部测试授权
				PRODUCT_FUNC::m_bDisableInternalTest=true;
#ifndef __LDS_TSA_	//非LDS的内力分析模块
			else if(key&&stricmp(key,"D")==0)
				m_bDrawingWorkEnvOnly=TRUE;
#endif
#ifdef __CONNECT_REMOTE_SERVER_
			else if(key&&stricmp(key,"net")==0)
				m_bNetModelWork=TRUE;
#endif
			else					//以单机方式进行工作
				m_bCooperativeWork=FALSE;
		}
	}
#ifndef __COMMON_PART_INC_
	m_bAnalysisWorkEnvOnly=TRUE;
#endif
#ifndef __PART_DESIGN_INC_
	m_bDrawingWorkEnvOnly=TRUE;
#endif
	if(m_bDrawingWorkEnvOnly)
	{
		m_eEnvType=LINE_ENV;	//单线模型环境
		m_ePrevEnvType=SOLID_ENV;	//默认前一环境为实体环境
		m_bAnalysisWorkEnv=FALSE;
	}

	
	CString sys_path;
	sys_path = APP_PATH;
	sys_path += "Sys\\";
	strcpy(execute_path,APP_PATH);
	WriteProfileString("SETUP","SYS_PATH", sys_path);
	WriteProfileString("SETUP","APP_PATH", APP_PATH);	//应用程序路径
	GetSystemDirectory(system_path,MAX_PATH);
	strcat(system_path,"\\Tower.sys");
	FILE* fp = fopen(system_path,"w+t");
	if(fp!=NULL)	//win7下可能不允许访问系统目录
	{
		fputs(APP_PATH,fp);
		fputs("\n",fp);
		fputs(sys_path,fp);
		fclose(fp);
	}
	//为避免与其余信狐软件的日志文件冲突，需要在当前目录下生成日志文件 wjh-2017.3.20
	CXhChar200 logfilename("%swarning.log",execute_path);
	if(!logerr.InitLogFile(logfilename))
		AfxMessageBox(CXhChar500("临时日志文件{%s}创建失败,请确认是否为管理员身份运行!",(char*)logfilename));
	CLogErrorLife loglife(&logerr);

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	char lic_file[MAX_PATH];
	char default_file[MAX_PATH];	//默认当前执行目录
	DWORD version[2];
	BYTE* byteVer=(BYTE*)version;
	WORD wModule=0;
	version[1]=20180130;//20170616;	//版本发布日期
	BYTE cProductType=PRODUCT_TMA;
#ifdef __TMA_
#ifdef _LOCAL_ENCRYPT
	sprintf(lic_file,"%sCMA.lic",APP_PATH);
	cProductType=PRODUCT_CTMA;
#else
	sprintf(lic_file,"%sTMA.lic",APP_PATH);
	cProductType=PRODUCT_TMA;
#endif
	byteVer[0]=5;
	byteVer[1]=2;
	byteVer[2]=0;
	byteVer[3]=0;
#elif defined(__LMA_)||defined(__STATION_)
//#ifdef __LMA_
	sprintf(lic_file,"%sLMA.lic",APP_PATH);
//#else
//	sprintf(lic_file,"%sStation.lic",APP_PATH);
//#endif
	wModule=1;
	byteVer[0]=2;
	byteVer[1]=0;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_)
	sprintf(lic_file,"%sTSA.lic",APP_PATH);
	wModule=2;
	byteVer[0]=1;
	byteVer[1]=8;
	byteVer[2]=1;
	byteVer[3]=2;
	cProductType=PRODUCT_TSA;
#elif defined(__LDS_)
	sprintf(lic_file,"%sLDS.lic",APP_PATH);
	wModule=3;
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_LDS;
#elif defined(__TDA_)
	sprintf(lic_file,"%sTDA.lic",APP_PATH);
	wModule=3;
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=8;
	byteVer[3]=0;
	cProductType=PRODUCT_TDA;
#endif
	m_version[0]=version[0];
	m_version[1]=version[1];
	strcpy(default_file,lic_file);
	//查找是否存在指定加密锁号的文件 wjh-2016.9.20
	char* separator=SearchChar(default_file,'.',true);
	strcpy(separator,".key");
	DetectSpecifiedHaspKeyFile(default_file);
	if(!m_bStartAsFemResolver)
		CSplashWnd::ShowSplashScreen(NULL);
	if(!theApp.ImportLicense())
		return FALSE;
#ifdef __ALFA_TEST_VERSION_
	TestPortal();
#endif
#ifdef __PART_DESIGN_INC_
	if(m_bStartAsFemResolver)
		_WorkLoop.StartLoop();
#endif

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	strcpy(m_sLicFile,lic_file);
	//获取最新版本升级提示信息
	GetVersionUpdateNotes();
	CString versionStr;
	GetProductVersion(versionStr);
#ifdef AFX_TARG_ENU_ENGLISH
	m_strRegSection="EV"+versionStr+"\\Workspace";
	m_strRegSection_LXJS = "EV" + versionStr + "\\LXJS\\Workspace";
	m_strRegSection_KSJM = "EV" + versionStr + "\\KSJM\\Workspace";
	m_strRegSection_JMHT = "EV" + versionStr + "\\JMHT\\Workspace";
	m_strRegSection_JMFY = "EV" + versionStr + "\\JMFY\\Workspace";
#else
	m_strRegSection = "V" + versionStr + "\\Workspace";
	m_strRegSection_LXJS = "V" + versionStr + "\\LXJS\\Workspace";
	m_strRegSection_KSJM = "V" + versionStr + "\\KSJM\\Workspace";
	m_strRegSection_JMHT = "V" + versionStr + "\\JMHT\\Workspace";
	m_strRegSection_JMFY = "V" + versionStr + "\\JMFY\\Workspace";
#endif
	InitTowerModel();
	globalVars.InitialzeGlobalVars();
	InitClassTypePropHashtable();
	separator=SearchChar(default_file,'.',true);
	//strchr(lic_file,'.');strchr从前向后搜，无法避免文件夹中含'.'情况 wjh-2012.2.21
#if defined(__TMA_)&&defined(_LOCAL_ENCRYPT)
	char* head=separator-3;
	*head='T';	//CMA->TMA
#endif
#ifdef __LDS_TSA_
	separator-=3;
	strcpy(separator,"tsa.cfg");
#else
	strcpy(separator,".cfg");
#endif
	if(g_sysPara.Read(default_file))
	{
		WriteProfileString("Settings","cfg_file",default_file);
		logerr.SetWarningLevel(g_sysPara.m_ciLowestWarningLevel);
	}
	//读取数据库文件
#ifdef __LDS_TSA_
	strcpy(separator,"lds.cfg");
	separator+=3;
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(separator,"E.plb");
#else
	strcpy(separator,".plb");
#endif
	InitPartLibrary(default_file);
	CopyPartlib(g_orgPartLib);
	//读取本地螺栓系列文件
	CString sBoltLibWorkFolder=APP_PATH;
	sBoltLibWorkFolder+="BoltCodeSeries";
	InitLocalBoltSeries(sBoltLibWorkFolder);
	//读取本地化特征功能配置文件
	strcpy(separator,CXhChar16("%d.fac",DogSerialNo()));
	int retcode=ImportLocalFeatureAccessControlFile(default_file);
	//初始化企业定制配置信息
	InitCustomizeConfiguration(ValidateLocalizeFeature(FEATURE::LOCALIZE_CUSTOMIZE_CLIENT));
	//if(retcode==-1)//文件不存在，按无本地化特征功能授权处理
	if(retcode==-2)
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"本地化功能授权文件类型不对");
	else if(retcode==-3)
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"本地化功能授权文件与当前加密锁不匹配");
	else if(retcode==-3)
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"本地化功能授权文件内容有误");
	CLDSPart::INTERNAL_TEST_VERSION=(PRODUCT_FUNC::IsHasInternalTest()==TRUE);

	char hlp_file[MAX_PATH];
	//更改联机帮助文档的默认路径
	free((void*)m_pszHelpFilePath);
#ifdef __TMA_FILE_
	sprintf(hlp_file,"%sDocs\\TMA.chm",APP_PATH);
#else
	sprintf(hlp_file,"%sDocs\\LDS.chm",APP_PATH);
#endif
	m_pszHelpFilePath=_tcsdup(_T(hlp_file));

	BOOL bRetCode=FALSE;
	if(m_bCooperativeWork)	//LDS以网络协同的方式工作
		bRetCode=LogonServer();
#ifdef __CONNECT_REMOTE_SERVER_
	else if(m_bNetModelWork)
		bRetCode=LogonTMSServer();
	if((m_bNetModelWork||m_bCooperativeWork)&&!bRetCode)
#else
	if(m_bCooperativeWork&&!bRetCode)
#endif
	{
		m_bCooperativeWork=FALSE;
#ifdef __CONNECT_REMOTE_SERVER_
		m_bNetModelWork=FALSE;
#endif
#ifndef _LEGACY_LICENSE
		ExitCurrentDogSession();
#elif defined(_NET_DOG)
		ExitNetLicEnv(m_wDogModule);
#endif
		return FALSE;
	}

	if(m_bCooperativeWork)
	{
#if defined(__LMA_)||defined(__STATION_)
		m_nMainMenuID=IDR_MAINFRAME_CSCW_LMA;
#elif defined __TMA_
		m_nMainMenuID=IDR_MAINFRAME_CSCW_TMA;
#elif defined __LDS_
		if(m_bAnalysisWorkEnvOnly)
			m_nMainMenuID=IDR_MAINFRAME_CSCW_TSA;
		else if(m_bDrawingWorkEnvOnly)
			m_nMainMenuID=IDR_MAINFRAME_CSCW_TDA;
		else
			m_nMainMenuID=IDR_MAINFRAME_CSCW;
#elif defined __TSA_
		m_nMainMenuID=IDR_MAINFRAME_CSCW_TSA;
#else //defined __TDA_
		m_nMainMenuID=IDR_MAINFRAME_CSCW_TDA;
#endif
	}
	else
	{
#if defined(__LMA_)||defined(__STATION_)
		m_nMainMenuID=IDR_MAINFRAME_LMA;
#elif defined __TMA_
		m_nMainMenuID=IDR_MAINFRAME_TMA;
#elif defined __LDS_
#ifdef __LDS_TSA_	//LDS的内力分析模块
		m_nMainMenuID=IDR_MAINFRAME_TSA;
#else
		if(m_bDrawingWorkEnvOnly)
			m_nMainMenuID=IDR_MAINFRAME_TDA;
		else
			m_nMainMenuID=IDR_MAINFRAME;
#endif
#elif defined __TSA_
		m_nMainMenuID=IDR_MAINFRAME_TSA;
#else //defined __TDA_
		m_nMainMenuID=IDR_MAINFRAME_TDA;
#endif
	}
	if(!AfxInitRichEdit())
		return FALSE;
	AfxOleInit();				// 添加此函数，用于调用COM组件  王金合--02.10.31

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* 
	pDocTemplate = new CSingleDocTemplate(
		m_nMainMenuID,
		RUNTIME_CLASS(CLDSDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLDSView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
#if defined(__TMA_)||defined(__LMA_)||defined(__TDA_)||defined(__LDS_)||defined(__TSA_)
	strcpy(separator,".exe");
	CXhChar50 szSubKey="TMA.Document\\DefaultIcon";
	CXhChar200 szLdsExePath=default_file;
	//-Resouce Icon ID表示根据Id关联相应图标，正楼则表示索引(0 based index) wjh-2017.2.20
#ifdef __TMA_
	szLdsExePath.Append(CXhChar16(",%d",-IDR_MAINFRAME_TMA));	//-216
	szSubKey.Copy("TMA.Document\\DefaultIcon");
#elif defined(__LMA_)
	szLdsExePath.Append(CXhChar16(",%d",-IDR_MAINFRAME_LMA));	//-128
	szSubKey.Copy("LMA.Document\\DefaultIcon");
#elif defined(__TDA_)
	szLdsExePath.Append(CXhChar16(",%d",-IDR_MAINFRAME_TDA));	//-568
	szSubKey.Copy("TDA.Document\\DefaultIcon");
#elif defined(__LDS_)||defined(__TSA_)
	szLdsExePath.Append(CXhChar16(",%d",-IDR_MAINFRAME_TSA));	//-217
	szSubKey.Copy("LDS.Document\\DefaultIcon");
#endif
	AfxRegSetValue(HKEY_CLASSES_ROOT,szSubKey,REG_SZ,szLdsExePath,lstrlen(szLdsExePath)*sizeof(TCHAR));
#endif
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	/*以下代码目前已不合理而且导致双击文件无法打开程序故去掉 wjh-2015.3.8
	if(__argc==1)//&&__argv[1][0]!='-')	//带参数执行命令行
		cmdInfo.m_strFileName = GetProfileString("Recent File List","File1");
    m_bLoadLastFile = GetProfileInt("Settings","StartLoadData",0);
    if(!cmdInfo.m_strFileName.IsEmpty()&&(m_bLoadLastFile||__argc>1))
	{
		if(!ProcessShellCommand(cmdInfo))	//预生成一个文档对象
		{
#ifndef _LEGACY_LICENSE
			ExitCurrentDogSession();
#elif defined(_NET_DOG)
			ExitNetLicEnv(m_wDogModule);
#endif
			return FALSE;
		}
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
		ParseCommandLine(cmdInfo);
		if(!ProcessShellCommand(cmdInfo))
		{
	        cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
			ProcessShellCommand(cmdInfo);
		}
	}
	else*/
		ProcessShellCommand(cmdInfo);
	//因涉及命令行项（CLDSView::InitSysCmdList中初始化），必须放在CLDSView生成之后　wjh-2016.1.07
	sprintf(hlp_file,"%sDocs\\LDS.hmp",APP_PATH);
	InitMenuHelpHashtable();
	InitDialogHelpHashtable();
	InitCadDialogHelpHashtable();
	InitSubGroupHelpHashtable();
	CSystemSettingDlg::InitPropHashtable();
	ImportPropHelpListMapFile(hlp_file);
#ifdef _DEBUG
	ExportPropListMapFile();
#endif
	m_hResHandle = AfxGetResourceHandle();
	
	// The one and only window has been initialized, so show and update it.
	if(m_bStartAsFemResolver)	//外挂有限元模式暂时不显示窗口
		m_pMainWnd->ShowWindow(SW_HIDE);
	else
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	intelli.Start();
	//测试汇集用户活跃度信息
	AfxBeginThread(LoginToServer, &m_hPipeServerSend, THREAD_PRIORITY_NORMAL);
	return TRUE;
}
CString CLDSApp::GetPropValueStr(long id)
{
	CString valueStr;
	CXhChar100 sText;
	bool bContinueJustify=false;
	if(GetPropID("basicInfo")==0)
		return "";
	else if(GetPropID("g_sysPara.nAutoSaveTime")==id)
		valueStr.Format("%d", g_sysPara.nAutoSaveTime/60000);
	else if(GetPropID("g_sysPara.iAutoSaveType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.iAutoSaveType==0)
			valueStr="Single File Backup";
		else if(g_sysPara.iAutoSaveType==1)
			valueStr="Multiple File Backup";
#else 
		if(g_sysPara.iAutoSaveType==0)
			valueStr="单文件备份";
		else if(g_sysPara.iAutoSaveType==1)
			valueStr="多文件备份";
#endif
	}
	else if(GetPropID("g_sysPara.uFileSaveVersion")==id)
	{
		if(g_sysPara.uFileSaveVersion==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Program Version";
#else 
			valueStr="程序版本";
#endif
		else if(ToStringVersion(g_sysPara.uFileSaveVersion,sText)>0)
			valueStr.Format("V %s",(char*)sText);
	}
	else if(GetPropID("g_sysPara.iFileSaveDocType")==id)
	{
		if(g_sysPara.iFileSaveDocType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Prefer File Format";
#else 
			valueStr="当前格式";
#endif
		else if(g_sysPara.iFileSaveDocType==1)
			valueStr="*.tma";
		else if(g_sysPara.iFileSaveDocType==2)
			valueStr="*.lma";
		else if(g_sysPara.iFileSaveDocType==4)
			valueStr="*.lds";
		else if(g_sysPara.iFileSaveDocType==5)
			valueStr="*.tda";
		else
			valueStr="当前格式";
	}
	else if(GetPropID("env.m_sUserName")==id)
		valueStr = env.m_sUserName;
	else if(GetPropID("env.m_sShortUserName")==id)
		valueStr = env.m_sShortUserName;
	else if(GetPropID("env.m_sAcadPath")==id)
		valueStr = AfxGetApp()->GetProfileString("ACAD","ACAD_PATH","");
	else if(GetPropID("g_sysPara.m_ciLowestWarningLevel")==id)
	{
		if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL_ALL)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="4.All warnings output";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL0_NONE)
			valueStr="0.Close warnings";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL1_IMPORTANT)
			valueStr="1.Important warnings";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL2_COMMON)
			valueStr="2.General warnings";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL3_UNIMPORTANT)
			valueStr="3.Unimportant warnings";
#else 
			valueStr="4.输出全部警示";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL0_NONE)
			valueStr="0.关闭所有警示";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL1_IMPORTANT)
			valueStr="1.重要警示信息";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL2_COMMON)
			valueStr="2.一般警示信息";
		else if(g_sysPara.m_ciLowestWarningLevel==ILog2File::WARNING_LEVEL3_UNIMPORTANT)
			valueStr="3.次要警示信息";
#endif
	}
	else if (GetPropID("FileRW.BUFFSIZE")==id)
	{
		UINT uiRWBuffSize= AfxGetApp()->GetProfileInt("Settings","FileRW_BUFFSIZE",50);
		valueStr.Format("%d",uiRWBuffSize);
	}
	else if(GetPropID("Ta.version")==id)
		valueStr.Format("%s",Ta.version);
	else if(GetPropID("Ta.m_uiOriginalDogKey")==id)
		valueStr.Format("%d",Ta.m_uiOriginalDogKey);
	else if(GetPropID("Ta.m_uiLastSaveDogKey")==id)
		valueStr.Format("%d",Ta.m_uiLastSaveDogKey);
	else if (GetPropID("Ta.Parts.GetNodeNum")==id)
	  valueStr.Format("%d",Ta.Parts.GetNodeNum());
	else if (GetPropID("Ta.Node.GetNodeNum")==id)
		valueStr.Format("%d",Ta.Node.GetNodeNum());
	else if (GetPropID("Ta.m_fNamedHeightZeroZ")==id)
		valueStr.Format("%g",Ta.NamedHeightZeroZ);
	else if (GetPropID("Ta.m_sPrjName")==id)
		valueStr.Format("%s",Ta.m_sPrjName);
	else if (GetPropID("Ta.m_sTaType")==id)
		valueStr.Format("%s",Ta.m_sTaType);
	else if (GetPropID("Ta.m_nTaNum")==id)
		valueStr.Format("%d",Ta.m_nTaNum);
	else if (GetPropID("Ta.m_sTaAlias")==id)
		valueStr.Format("%s",Ta.m_sTaAlias);
	else if (GetPropID("Ta.m_sTaStampNo")==id)
		valueStr.Format("%s",Ta.m_sTaStampNo);
	else if (GetPropID("Ta.m_sOperator")==id)
		valueStr.Format("%s",Ta.m_sOperator);
	else if (GetPropID("Ta.m_sAuditor")==id)
		valueStr.Format("%s",Ta.m_sAuditor);
	else if (GetPropID("Ta.m_bRepositionByModeBeforeV51")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(Ta.m_ciRepositionAlgor==1)
			valueStr="V1 deprecated mode";
		else if(Ta.m_ciRepositionAlgor==2)
			valueStr="V2 deprecated mode";
		else
			valueStr="Recommended mode";
#else
		if(Ta.m_ciRepositionAlgor==1)
			valueStr="V1 旧算法";
		else if(Ta.m_ciRepositionAlgor==2)
			valueStr="V2 旧算法";
		else
			valueStr="新算法";
#endif
	}
	else if (GetPropID("m_iPlankWeightStatType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(CLDSPlate::m_iPlankWeightStatType==1)
			valueStr.Format("%s","0.Envelope rectangle");
		else if(CLDSPlate::m_iPlankWeightStatType==2)
			valueStr.Format("%s","1.Net area");
#else
		if(CLDSPlate::m_iPlankWeightStatType==1)
			valueStr.Format("%s","0.最小包络矩形面积");
		else if(CLDSPlate::m_iPlankWeightStatType==2)
			valueStr.Format("%s","1.净面积");
#endif
	}
	else if (GetPropID("m_iCirclePlankAreaType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(CLDSParamPlate::m_iCirclePlankAreaType==1)
			valueStr.Format("%s","0.Envelope rectangle area");
		else if(CLDSParamPlate::m_iCirclePlankAreaType==2)
			valueStr.Format("%s","1.Net area");
#else
		if(CLDSParamPlate::m_iCirclePlankAreaType==1)
			valueStr.Format("%s","0.最小包络形面积");
		else if(CLDSParamPlate::m_iCirclePlankAreaType==2)
			valueStr.Format("%s","1.净面积");
#endif
	}
	else if (GetPropID("Ta.m_cRollProcessType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(Ta.m_ciRollProcessType==2)
			valueStr="2.Processing Priority 2";
		else if(Ta.m_ciRollProcessType==1)
			valueStr="1.Processing Priority 1";
		else 
			valueStr="0.Mechanical Priority";
#else   
		if(Ta.m_ciRollProcessType==2)
			valueStr="2.最少刀数2";
		else if(Ta.m_ciRollProcessType==1)
			valueStr="1.最少刀数1";
		else 
			valueStr="0.最佳受力";
#endif
	}
	else if(GetPropID("Ta.m_cRollOffsetType")==id)
	{
		if(Ta.m_ciRollOffsetType==0)
#ifdef	AFX_TARG_ENU_ENGLISH
			valueStr="0.Outside";
		else
			valueStr="1.Inside";
#else
			valueStr="0.外侧";
		else
			valueStr="1.内侧";
#endif
	}
	else if(GetPropID("Ta.m_cDeformType")==id)
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		if(Ta.m_ciDeformType==0)
			valueStr="0.Assembly Plane";
		else if(Ta.m_ciDeformType==1)
			valueStr="1.Middle Plane";
		else if(Ta.m_ciDeformType==2)
			valueStr="2.Raised Plane";
		else if(Ta.m_ciDeformType==3)
			valueStr="3.Sunk Plane";
#else
		if(Ta.m_ciDeformType==0)
			valueStr="0.装配面";
		else if(Ta.m_ciDeformType==1)
			valueStr="1.中性层";
		else if(Ta.m_ciDeformType==2)
			valueStr="2.凸面";
		else if(Ta.m_ciDeformType==3)
			valueStr="3.凹面";
#endif
	}
	else if(GetPropID("Ta.m_fDeformCoef")==id)
	{
		valueStr.Format("%f",Ta.m_fDeformCoef);
		SimplifiedNumString(valueStr);
	}
	else if (GetPropID("g_sysPara.m_bSaveLogVersion")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(!g_sysPara.m_bSaveLogVersion)
			valueStr.Format("%s","0.None");
		else
			valueStr.Format("%s","1.Save");
#else
		if(!g_sysPara.m_bSaveLogVersion)
			valueStr.Format("%s","0.不保存");
		else
			valueStr.Format("%s","1.保存");
#endif
	}
	else if(GetPropID("g_sysPara.m_iModFileCodeType")==id)
	{
		if(g_sysPara.m_iModFileCodeType==0)
			valueStr="0.UTF-8";
		else
			valueStr="1.ANSI";
	}
	else if(GetPropID("env.m_sJgGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","JgGuiGeTblPath","");
	else if(GetPropID("env.m_sTubeGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","TubeGuiGeTblPath","");
	else if(GetPropID("env.m_sFlatGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","FlatGuiGeTblPath","");
	else if(GetPropID("env.m_sJgJointParaTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","JgJointParaTblPath","");
	else if(GetPropID("env.m_sSlotGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","SlotGuiGeTblPath","");
	else if(GetPropID("env.m_sHWGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","HWGuiGeTblPath","");
	else if(GetPropID("env.m_sHMGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","HMGuiGeTblPath","");
	else if(GetPropID("env.m_sHNGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","HNGuiGeTblPath","");
	else if(GetPropID("env.m_sHPGuiGeTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","HPGuiGeTblPath","");
	else if(GetPropID("env.m_sLsSpaceTblPath")==id)
		valueStr = theApp.GetProfileString("Settings","LsSpaceTblPath","");
	else if(GetPropID("env.m_sCInsertPlateLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","CInsertPlateLibPath","");
	else if(GetPropID("env.m_sUInsertPlateLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","UInsertPlateLibPath","");
	else if(GetPropID("env.m_sXInsertPlateLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","XInsertPlateLibPath","");
	else if(GetPropID("env.m_sStdPartFlDLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","StdPartFlDLibPath","");
	else if(GetPropID("env.m_sStdPartFlPLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","StdPartFlPLibPath","");
	else if(GetPropID("env.m_sStdPartFlGLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","StdPartFlGLibPath","");
	else if(GetPropID("env.m_sStdPartFlRLibPath")==id)
		valueStr = theApp.GetProfileString("Settings","StdPartFlRLibPath","");
	else if(GetPropID("env.m_sFlCoupleLevelPath")==id)
		valueStr = theApp.GetProfileString("Settings","FlCoupleLevelPath","");
	else
		bContinueJustify=true;
	if(!bContinueJustify)	//if-else判断太多编译通不过去，所以只能在中间断开
		return valueStr;
	bContinueJustify=false;	//重新启动检查
	if(GetPropID("Ta.segstr")==id)
		valueStr = Ta.segstr;
	else if(GetPropID("env.layer")==id)
		valueStr = env.layer;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	else if(GetPropID("volt_grade")==id)
		valueStr.Format("%d",Ta.volt_grade);
	else if(GetPropID("m_cLandscape")==id)
	{
		if(Ta.m_cLandscape=='A')
			valueStr="A:近海或沙漠平坦地区";
		else if(Ta.m_cLandscape=='B')
			valueStr="B:内陆建筑物稀疏地区";
		else if(Ta.m_cLandscape=='C')
			valueStr="C:有密集建筑群市区";
		else //if(Ta.m_cLandscape=='D')
			valueStr="D:有密集建筑群和高层建筑市区";
	}
	else if(GetPropID("m_cLineRoadType")==id)
	{
		if(Ta.m_cLineRoadType==0)
			valueStr="平地线路";
		else if(Ta.m_cLineRoadType==1)
			valueStr="丘陵线路";
		else
			valueStr="山地线路";
	}
	else if(GetPropID("WeatherCondition")==id)
		valueStr=Ta.weather.weatherCode;
	else if(GetPropID("m_nHighestTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nHighestT);
		valueStr = sText;
	}
	else if(GetPropID("m_nLowestTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nLowestT);
		valueStr = sText;
	}
	else if(GetPropID("m_nAverageTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nAverageT);
		valueStr = sText;
	}
	else if(GetPropID("m_fMaxWindVelocity")==id)
	{
		sText.Printf("%g",Ta.weather.m_fMaxWindV);
		valueStr = sText;
	}
	else if(GetPropID("m_nMaxWindVelocityTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nMaxWindT);
		valueStr = sText;
	}
	else if(GetPropID("m_fMaxIceThick")==id)
	{
		sText.Printf("%g",Ta.weather.m_fMaxIceThick);
		valueStr = sText;
	}
	else if(GetPropID("m_nMaxIceTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nMaxIceT);
		valueStr = sText;
	}
	else if(GetPropID("m_fMaxIceWindVelocity")==id)
	{
		sText.Printf("%g",Ta.weather.m_fMaxIceWindV);
		valueStr = sText;
	}
	else if(GetPropID("m_nSetupTemperature")==id)
	{
		sText.Printf("%d",Ta.weather.m_nSetupT);
		valueStr = sText;
	}
	else if(GetPropID("m_fSetupWindVelocity")==id)
	{
		sText.Printf("%g",Ta.weather.m_fSetupWindV);
		valueStr = sText;
	}
	else if(GetPropID("Ta.m_fIceDensity")==id)
	{
		sText.Printf("%g",Ta.m_fIceDensity);//kg
		valueStr = sText;
	}
	else if(GetPropID("Ta.m_cStructureType")==id)
	{
		if(Ta.m_cStructureType==0)
			sText.Copy("0.单主体结构");
		else
			sText.Copy("1.复合结构");
		valueStr = sText;
	}
#endif
	else if(GetPropID("g_sysPara.fRotateVelocity")==id)
	{
		sText.Printf("%g",g_sysPara.fRotateVelocity);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_bLineDepthTest")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_bLineDepthTest)
			sText.Copy("Enable");
		else
			sText.Copy("Disable");
#else
		if(g_sysPara.m_bLineDepthTest)
			sText.Copy("开启");
		else
			sText.Copy("关闭");
#endif
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_bDragOpenWndSel")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_bDragOpenWndSel)
			sText.Copy("Enable");
		else
			sText.Copy("Disable");
#else
		if(g_sysPara.m_bDragOpenWndSel)
			sText.Copy("开启");
		else
			sText.Copy("关闭");
#endif
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_bDblClkStartRotateOper")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_bDblClkStartRotateOper)
			sText.Copy("Enable");
		else
			sText.Copy("Disable");
#else
		if(g_sysPara.m_bDblClkStartRotateOper)
			sText.Copy("开启");
		else
			sText.Copy("关闭");
#endif
		valueStr = sText;
	}
	else if(GetPropID("Ta.m_fTowerHeight")==id)
	{
		if(Ta.m_fTowerHeight==0)
		{
			SCOPE_STRU scope;
			Ta.GetTaSizeScope(scope);
			Ta.m_fTowerHeight=scope.depth()/1000;	//mm->m
		}
		sText.Printf("%g",Ta.m_fTowerHeight);
		valueStr = sText;
	}
	else if(GetPropID("m_dwSelectPartsType")==id)
		return "";
	else if(GetPropID("CanSelectNode")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_NODE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectLineAngle")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_LINEANGLE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectLineTube")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_LINETUBE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectPlate")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_PLATE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectParamPlate")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_PARAMPLATE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectBolt")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_BOLT))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectLineSlot")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_LINESLOT))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectSphere")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_SPHERE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectLineFlat")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_LINEFLAT))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectArcAngle")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_ARCANGLE))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectArcFlat")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_ARCFLAT))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectArcSlot")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_ARCSLOT))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CanSelectStdPart")==id)
	{
		if(g_sysPara.m_dwSelectPartsType & GetSingleWord(SELECTINDEX_STDPART))
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("lineDisplay")==id)
		return "";
	else if(GetPropID("CanSelectEdgeLine")==id)
	{
		if(g_sysPara.m_bCanSelectEdgeLine)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	//格式刷属性选择
	else if(GetPropID("g_sysPara.brush.bMaterial")==id)
	{
		if(g_sysPara.brush.bMaterial)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bLayer")==id)
	{
		if(g_sysPara.brush.bLayer)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bGuige")==id)
	{
		if(g_sysPara.brush.bGuige)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bSeg")==id)
	{
		if(g_sysPara.brush.bSeg)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bCfgword")==id)
	{
		if(g_sysPara.brush.bCfgword)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bDisplayHole")==id)
	{
		if(g_sysPara.brush.bDisplayHole)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bVirtualPart")==id)
	{
		if(g_sysPara.brush.bVirtualPart)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.brush.bColour")==id)
	{
		if(g_sysPara.brush.bColour)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	
	else if(GetPropID("g_sysPara.display.bNodePnt")==id)
	{
		if(g_sysPara.display.bNodePnt)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bPoleLine")==id)
	{
		if(g_sysPara.display.bPoleLine)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CLDSLinePart::bDisplayLineVecMark")==id)
	{
		if(CLDSLinePart::bDisplayLineVecMark)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bPartSketch")==id)
	{
		if(g_sysPara.display.bPartSketch)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bLoadArrow")==id)
	{
		if(g_sysPara.display.bLoadArrow)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.nLoadArrowScale")==id)
	{
		valueStr.Format("%d",g_sysPara.display.nLoadArrowScale);
	}
	else if(GetPropID("g_sysPara.display.bWirePoint")==id)
	{
		if(g_sysPara.display.bWirePoint)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bDimNodeLabel")==id)
	{
		if(g_sysPara.display.bDimNodeLabel)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bDimRodDesignInfo")==id)
	{
		if(g_sysPara.display.bDimRodDesignInfo)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.nNodeLabelTextHeight")==id)
	{
		valueStr.Format("%d",g_sysPara.display.nNodeLabelTextHeight);
	}
	else if(GetPropID("g_sysPara.display.nRodDesignInfoTextHeight")==id)
	{
		valueStr.Format("%d",g_sysPara.display.nRodDesignInfoTextHeight);
	}
	else if(GetPropID("g_sysPara.display.xPointStyle.cbPointSize")==id)
	{
		valueStr.Format("%d",g_sysPara.display.xPointStyle.cbPointSize);
	}
	else if(GetPropID("g_sysPara.display.xPointStyle.ciPointStyle")==id)
	{
		if(g_sysPara.display.xPointStyle.ciPointStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="0.Square Point";
		else if(g_sysPara.display.xPointStyle.ciPointStyle==1)
			valueStr="1.Circular Point";
#else
			valueStr="0.方点";
		else if(g_sysPara.display.xPointStyle.ciPointStyle==1)
			valueStr="1.圆点";
#endif
	}
	else if(GetPropID("g_sysPara.m_fMaxScaleScr2User")==id)
	{
		valueStr.Format("%s",(char*)CXhChar16(g_sysPara.m_fMaxScaleScr2User));
	}
	else if(GetPropID("CLDSNode::m_bDisplayBeforeArcLift")==id)
	{
		if(CLDSNode::m_bDisplayBeforeArcLift)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Before Lift Arc";
		else
			valueStr = "After Lift Arc";
#else 
			valueStr = "预拱前";
		else
			valueStr = "预拱后";
#endif
	}
	else if(GetPropID("solidDisplay")==id)
		return "";
	else if(GetPropID("g_sysPara.display.bDispVirtualPart")==id)
	{
		if(g_sysPara.display.bDispVirtualPart)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidJg")==id)
	{
		if(g_sysPara.display.bSolidJg)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidTube")==id)
	{
		if(g_sysPara.display.bSolidTube)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidLjb")==id)
	{
		if(g_sysPara.display.bSolidLjb)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidJoint")==id)
	{
		if(g_sysPara.display.bSolidJoint)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidLs")==id)
	{
		if(g_sysPara.display.bSolidLs)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidSphere")==id)
	{
		if(g_sysPara.display.bSolidSphere)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidSlot")==id)
	{
		if(g_sysPara.display.bSolidSlot)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidFlat")==id)
	{
		if(g_sysPara.display.bSolidFlat)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidArcAngle")==id)
	{
		if(g_sysPara.display.bSolidArcAngle)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidArcFlat")==id)
	{
		if(g_sysPara.display.bSolidArcFlat)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidArcSlot")==id)
	{
		if(g_sysPara.display.bSolidArcSlot)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidStdPart")==id)
	{
		if(g_sysPara.display.bSolidStdPart)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.bSolidFoundation")==id)
	{
		if(g_sysPara.display.bSolidFoundation)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
#ifdef __COMMON_PART_INC_// !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetPropID("g_sysPara.display.bSolidTransectFace")==id)
	{
		if(CLDSLineTube::bDisplaySolidTransectFace)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CLDSLineAngle::m_bDisplaySolidAxisLine")==id)
	{
		if(CLDSLineAngle::m_bDisplaySolidAxisLine)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("CLDSGeneralPlate::m_ciDisplayVertexType")==id)
	{
		if(CLDSGeneralPlate::m_ciDisplayVertexType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "0.No";
		else if(CLDSGeneralPlate::m_ciDisplayVertexType==1)
			valueStr = "1.All";
		else
			valueStr = "2.Vertex";
#else 
			valueStr = "0.不显示轮廓点";
		else if(CLDSGeneralPlate::m_ciDisplayVertexType==1)
			valueStr = "1.显示带标签轮廓点";
		else
			valueStr = "2.只显示轮廓点";
#endif
	}
#endif
	else if(GetPropID("g_sysPara.display.bSolidLine")==id)
	{
		if(g_sysPara.display.bSolidLine)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("g_sysPara.display.nSmoothness")==id)
	{
		sText.Printf("%d",g_sysPara.display.nSmoothness);
		valueStr = sText;
	}
	else
		bContinueJustify=true;
	if(!bContinueJustify)	//if-else判断太多编译通不过去，所以只能在中间断开
		return valueStr;
	bContinueJustify=false;	//重新启动检查
	if(GetPropID("partColor")==id)
		return "";
	else if(GetPropID("partName")==id)
		return "";
	else if(GetPropID("partColorName")==id)
		return "";
	else if(GetPropID("defaultColor")==id)
		return "";
	else if(GetPropID("ColorScheme")==id)
		return "";
	//else if(GetPropID("env.mcs")==id)
	//	return "";
	else if(GetPropID("env.mcs")==id)
	{
		if(env.m_bDisplayWorldUcs)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Visible";
		else
			valueStr = "Invisible";
#else 
			valueStr = "显示";
		else
			valueStr = "隐藏";
#endif
	}
	else if(GetPropID("env.m_bDisplayWorldUcs")==id)
	{
		if(env.m_bDisplayWorldUcs)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Yes";
		else
			valueStr = "No";
#else 
			valueStr = "是";
		else
			valueStr = "否";
#endif
	}
	else if(GetPropID("env.mcs.axis_x")==id)
		return "";
	else if(GetPropID("env.mcs.axis_x.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.x);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_x.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.y);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_x.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.z);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_y")==id)
		return "";
	else if(GetPropID("env.mcs.axis_y.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.x);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_y.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.y);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_y.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.z);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_z")==id)
		return "";
	else if(GetPropID("env.mcs.axis_z.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.x);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_z.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.y);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("env.mcs.axis_z.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.z);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_nWorkUcsType")==id)
	{
		//if(m_nWorkUcsType==0)
#ifdef AFX_TARG_ENU_ENGLISH
		valueStr = "自定义坐标系";
#else 
			valueStr = "自定义坐标系";
#endif
		//else if(m_nWorkUcsType==1)
		//	valueStr = "构件装配坐标系";
		//else if(m_nWorkUcsType==2)
		//	valueStr = "模型坐标系";
	}
	else if(GetPropID("work_ucs.m_eDatumPlane")==id)
	{
		CParaCS::PARAMETRIC_INFO param=console.des_wcs.GetParamInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		if(param.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X Plane");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z Plane");
#else 
		if(param.m_eDatumPlane==CParaCS::DP_XY)
			strcpy(sText,"0.X-Y面");
		else if(param.m_eDatumPlane==CParaCS::DP_YX)
			strcpy(sText,"1.Y-X面");
		else if(param.m_eDatumPlane==CParaCS::DP_YZ)
			strcpy(sText,"2.Y-Z面");
		else if(param.m_eDatumPlane==CParaCS::DP_ZY)
			strcpy(sText,"3.Z-Y面");
		else if(param.m_eDatumPlane==CParaCS::DP_ZX)
			strcpy(sText,"4.Z-X面");
		else if(param.m_eDatumPlane==CParaCS::DP_XZ)
			strcpy(sText,"5.X-Z面");
#endif
		valueStr=sText;
	}
	else if(GetPropID("work_ucs.origin")==id)
	{
		sText.ConvertFromPoint(console.m_curACS.origin,3);
		valueStr=sText;
	}
	else if(GetPropID("work_ucs.origin.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.x);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.origin.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.y);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.origin.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.z);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_x")==id)
	{
		sText.Copy(console.des_wcs.GetAxisDescription('X'));
		if(sText.GetLength()<2)
			sText.ConvertFromPoint(console.m_curACS.axis_x);
		valueStr=sText;
	}
	else if(GetPropID("work_ucs.axis_x.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.x);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_x.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.y);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_x.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_x.z);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_y")==id)
	{
		sText.Copy(console.des_wcs.GetAxisDescription('Y'));
		if(sText.GetLength()<2)
			sText.ConvertFromPoint(console.m_curACS.axis_y);
		valueStr=sText;
	}
	else if(GetPropID("work_ucs.axis_y.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.x);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_y.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.y);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_y.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_y.z);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_z")==id)
	{
		sText.Copy(console.des_wcs.GetAxisDescription('Z'));
		if(sText.GetLength()<2)
			sText.ConvertFromPoint(console.m_curACS.axis_z);
		valueStr=sText;
	}
	else if(GetPropID("work_ucs.axis_z.x")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.x);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_z.y")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.y);
		valueStr = sText;
	}
	else if(GetPropID("work_ucs.axis_z.z")==id)
	{
		sText.Printf("%g",env.mcs.axis_z.z);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_bDispPartACS")==id)
	{
		if(g_sysPara.m_bDispPartACS)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Visible";
		else
			valueStr = "Invisible";
#else 
			valueStr = "显示";
		else
			valueStr = "隐藏";
#endif
	}
	else if(GetPropID("g_sysPara.m_bDispWorkUcs")==id)
	{
		if(g_sysPara.m_bDispWorkUcs)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Visible";
		else
			valueStr = "Invisible";
#else 
			valueStr = "显示";
		else
			valueStr = "隐藏";
#endif
	}
	else if(GetPropID("g_sysPara.m_bDispBlockLCS")==id)
	{
		if(g_sysPara.m_bDispBlockLCS)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr = "Visible";
		else
			valueStr = "Invisible";
#else 
			valueStr = "显示";
		else
			valueStr = "隐藏";
#endif
	}
	else
		bContinueJustify=TRUE;
	if(!bContinueJustify)	//if-else判断太多编译通不过去，所以只能在中间断开
		return valueStr;
	bContinueJustify=false;	//重新启动检查
	if(GetPropID("designInfo")==id)
		return "";
	else if(GetPropID("g_sysPara.HuoQuDist")==id)
		valueStr.Format("%ld",g_sysPara.HuoQuDist);
	else if(GetPropID("g_sysPara.JgEndDist")==id)
		valueStr.Format("%ld",g_sysPara.JgEndDist);
	else if(GetPropID("g_sysPara.JointPlankDist")==id)
		valueStr.Format("%ld",g_sysPara.JointPlankDist);
	else if(GetPropID("g_sysPara.TubeFlDist")==id)
		valueStr.Format("%ld",g_sysPara.TubeFlDist);
	else if(GetPropID("g_sysPara.m_bModifyCoupledFLTubeOddment")==id)
	{
		if(g_sysPara.m_bModifyCoupledFLTubeOddment)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("CLDSLineTube::ciFlTubeOddment")==id)
	{
		if(CLDSLineTube::ciFlTubeOddment==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="0.To Tube End Plane";
		else
			valueStr="1.To Flange End Plane";
#else 
			valueStr="0.至钢管端面";
		else
			valueStr="1.至法兰端面";
#endif
	}
	else if(GetPropID("g_sysPara.VertexDist")==id)
		valueStr.Format("%ld",g_sysPara.VertexDist);
	else if(GetPropID("g_sysPara.CollideDist")==id)
		valueStr.Format("%ld",g_sysPara.CollideDist);
	else if(GetPropID("g_sysPara.fStressDiffuseAngle")==id)
	{
		sText.Printf("%g",g_sysPara.fStressDiffuseAngle);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.fInsertPlateEdgeSpace")==id)
	{
		sText.Printf("%g",g_sysPara.fInsertPlateEdgeSpace);
		SimplifiedNumString(sText);
		valueStr=sText;
	}
	else if(GetPropID("g_sysPara.m_bDelBoltsResideOnPlate")==id)
	{
		if(g_sysPara.m_bDelBoltsResideOnPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.ciLinkAngleDlgFocusCtrlMode")==id)
	{
		if(g_sysPara.ciLiveFocusCtrlInLinkAngleDlg==1)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Size control prefer";
		else
			valueStr="Label control prefer";
#else
			valueStr="规格框优先";
		else
			valueStr="编号框优先";
#endif
	}
	else if(GetPropID("g_sysPara.FootNailDist")==id)
		valueStr.Format("%ld",g_sysPara.FootNailDist);
	else if(GetPropID("g_sysPara.fNailRepScope")==id)
	{
		sText.Printf("%g",g_sysPara.fNailRepScope);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.fTubeWeldRoadMinD")==id)
	{
		sText.Printf("%g",g_sysPara.fTubeWeldRoadMinD);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_fKaiHeJiaoThreshold")==id)
	{	//用注册表中保存的数据更新开合角度数阈值 wht 11-06-13
		g_sysPara.GetPublicSysParaFromReg("Settings","KaiHeJiaoThreshold");
		valueStr.Format("%f",g_sysPara.m_fKaiHeJiaoThreshold);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("g_sysPara.m_fKaiHeJiaoMinDeita")==id)
	{	//用注册表中保存的数据更新开合角外侧最小间隙
		g_sysPara.GetPublicSysParaFromReg("Settings","KaiHeJiaoMinDeita");
		valueStr.Format("%f",g_sysPara.m_fKaiHeJiaoMinDeita);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("g_sysPara.m_nGroupJgMachiningGap")==id)
	{
		g_sysPara.GetPublicSysParaFromReg("Settings","GroupJgMachiningGap");
		valueStr.Format("%d",g_sysPara.m_nGroupJgMachiningGap);
	}
	else if(GetPropID("g_sysPara.m_nMatCharPosType")==id)
	{
		g_sysPara.GetPublicSysParaFromReg("Settings","MatCharPosType");
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_nMatCharPosType==2)
			valueStr="2.After Label";
		else if(g_sysPara.m_nMatCharPosType==1)
			valueStr="1.Before Label";
		else 
			valueStr="0.None";
#else 
		if(g_sysPara.m_nMatCharPosType==2)
			valueStr="2.构件编号后";
		else if(g_sysPara.m_nMatCharPosType==1)
			valueStr="1.构件编号前";
		else 
			valueStr="0.不需要";
#endif
	}
	else if(GetPropID("g_sysPara.m_cMatCharSeparator")==id)
	{
		g_sysPara.GetPublicSysParaFromReg("Settings","MatSeparator");
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_cMatCharSeparator==0)
			valueStr="None";
		else if(g_sysPara.m_cMatCharSeparator==' ')
			valueStr="Space";
		else 
			valueStr.Format("%c",g_sysPara.m_cMatCharSeparator);
#else 
		if(g_sysPara.m_cMatCharSeparator==0)
			valueStr="无";
		else if(g_sysPara.m_cMatCharSeparator==' ')
			valueStr="空格";
		else 
			valueStr.Format("%c",g_sysPara.m_cMatCharSeparator);
#endif
	}
	else if(GetPropID("g_sysPara.m_fMinCutAngleLen")==id)
	{
		sText.Printf("%g",g_sysPara.m_fMinCutAngleLen);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.m_fMaxIgnoreCutAngleLen")==id)
	{
		sText.Printf("%g",g_sysPara.m_fMaxIgnoreCutAngleLen);
		SimplifiedNumString(sText);
		valueStr=sText;
	}
	else if (GetPropID("CLDSBolt::BOLTL0_CALMODE")==id)
	{
		if (CLDSBolt::BOLTL0_CALMODE==0)
			valueStr="0.累加通厚";
		else if (CLDSBolt::BOLTL0_CALMODE==1)
			valueStr="1.计入缺失垫板间隙";
		else if (CLDSBolt::BOLTL0_CALMODE==2)
			valueStr="2.螺栓自身属性";
	}
	else if (GetPropID("CLDSBolt::BOLTL0_PREFER_MODE")==id)
	{
		if (CLDSBolt::BOLTL0_PREFER_MODE==1)
			valueStr="1.计入缺失垫板间隙";
		else //if (CLDSBolt::BOLTL0_CALMODE==0)
			valueStr="0.累加通厚";
	}
	else if(GetPropID("MaxWasherN")==id)
		valueStr.Format("%d",g_sysPara.BoltPadThick.cnMaxWasherN);
	else if(GetPropID("g_sysPara.BoltPadThick.m_nM12")==id)
		valueStr=g_sysPara.BoltPadThick.ToThickStr(12);
	else if(GetPropID("g_sysPara.BoltPadThick.m_nM16")==id)
		valueStr=g_sysPara.BoltPadThick.ToThickStr(16);
	else if(GetPropID("g_sysPara.BoltPadThick.m_nM20")==id)
		valueStr=g_sysPara.BoltPadThick.ToThickStr(20);
	else if(GetPropID("g_sysPara.BoltPadThick.m_nM24")==id)
		valueStr=g_sysPara.BoltPadThick.ToThickStr(24);
	else if(GetPropID("g_sysPara.m_nPlankEdgeAngleThreshold")==id)
		valueStr.Format("%ld",g_sysPara.m_nPlankEdgeAngleThreshold);
	else if(GetPropID("m_iProfileAlgorithm")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(CLDSPlate::m_siProfileAlgor==1)
			valueStr="1.Improved Algorithm";
		else
			valueStr="0.Traditional Algorithm";
#else
		if(CLDSPlate::m_siProfileAlgor==1)
			valueStr="1.改进算法";
		else
			valueStr="0.传统算法";
#endif
	}
	else if(GetPropID("g_sysPara.nAutoAdjustTubePos")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.nAutoAdjustTubePos==1)
			valueStr = "Board Offset";
		else if(g_sysPara.nAutoAdjustTubePos==2)
			valueStr = "Tube Offset";
		else 
			valueStr = "Not Enable";
#else 
		if(g_sysPara.nAutoAdjustTubePos==1)
			valueStr = "插板偏移";
		else if(g_sysPara.nAutoAdjustTubePos==2)
			valueStr = "钢管偏移";
		else 
			valueStr = "不启用";
#endif
	}
	else if(GetPropID("g_sysPara.m_bDisplayConnectDlg")==id)
	{
		if(g_sysPara.m_bDisplayConnectDlg)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.b1BoltDesignPrompt")==id)
	{
		if(g_sysPara.b1BoltDesignPrompt)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if (GetPropID("CLDSBolt::BOLTL0_CALMODE") == id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
	if (CLDSBolt::BOLTL0_CALMODE == CLDSBolt::L0CAL_BY_SUMM_THICK)
		valueStr = "0.Sum Part Thick";
	else if (CLDSBolt::BOLTL0_CALMODE == CLDSBolt::L0CAL_INC_GAP_THICK)
		valueStr = "1.Include Part Gap";
	else //if(CLDSBolt::BOLTL0_CALMODE==CLDSBolt::L0CAL_BY_BOLTSELF)
		valueStr = "2.Bolt Self Property";
#else 
	if (CLDSBolt::BOLTL0_CALMODE == CLDSBolt::L0CAL_BY_SUMM_THICK)
		valueStr = "0.累加通厚";
	else if(CLDSBolt::BOLTL0_CALMODE==CLDSBolt::L0CAL_INC_GAP_THICK)
		valueStr = "1.考虑垫板间隙";
	else //if(CLDSBolt::BOLTL0_CALMODE==CLDSBolt::L0CAL_BY_BOLTSELF)
		valueStr = "2.螺栓自身属性";
#endif
	}
	else if (GetPropID("CLDSBolt::BOLTL0_PREFER_MODE") == id)
	{
	if (CLDSBolt::BOLTL0_PREFER_MODE == CLDSBolt::L0CAL_BY_SUMM_THICK)
#ifdef AFX_TARG_ENU_ENGLISH
		valueStr = "0.Sum Part Thick";
	else
		valueStr = "1.Include Part Gap";
#else 
		valueStr = "0.累加通厚";
	else
		valueStr = "1.考虑垫板间隙";
#endif
	}
	else if(GetPropID("g_sysPara.m_b45CutAngleNeedCoefR")==id)
	{
		if(g_sysPara.m_b45CutAngleNeedCoefR)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.m_fCutAngleCoefR")==id)
	{
		CXhChar16 scaleStr;
		CExpression::ToExpression(g_sysPara.m_fCutAngleCoefR,3,scaleStr,8);
		//scaleStr.ResizeLength(8,' ',true);
		valueStr.Format("%s",(char*)scaleStr);
	}
	else if(GetPropID("g_sysPara.m_bAnglePreferSimplePosMode")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.m_bAnglePreferSimplePosMode)
			valueStr="Simple";
		else
			valueStr="Parametric";
#else
		if(g_sysPara.m_bAnglePreferSimplePosMode)
			valueStr="简单定位";
		else
			valueStr="参数化定位";
#endif
	}
	else if(GetPropID("g_sysPara.display.cHoleWallMode")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(g_sysPara.display.cHoleWallMode==1)
			valueStr="1.Virtual bolt hole";
		else if(g_sysPara.display.cHoleWallMode==2)
			valueStr="2.All holes";
		else
			valueStr="0.None";
#else
		if(g_sysPara.display.cHoleWallMode==1)
			valueStr="1.仅虚拟螺栓孔壁";
		else if(g_sysPara.display.cHoleWallMode==2)
			valueStr="2.全部孔壁";
		else
			valueStr="0.不显示";
#endif
	}
	else if(GetPropID("CLDSPart::UPDATE_PARTNO_AND_SEG")==id)
	{
		if(CLDSPart::UPDATE_PARTNO_AND_SEG)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.m_bEnablePrompt")==id)
	{
		if(g_sysPara.m_bEnablePrompt)
#ifdef AFX_TARG_ENU_ENGLISH
		valueStr="Yes";
	else
		valueStr="No";
#else 
		valueStr="是";
	else
		valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bOnlyDesignCurView")==id)
	{
		if(g_sysPara.intelligentConnect.m_bOnlyDesignCurView)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bDesignCommonConnect")==id)
	{
		if(g_sysPara.intelligentConnect.m_bDesignCommonConnect)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bDesignBendConnect")==id)
	{
		if(g_sysPara.intelligentConnect.m_bDesignBendConnect)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bDesignFL")==id)
	{
		if(g_sysPara.intelligentConnect.m_bDesignFL)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bDesignAngleJoint")==id)
	{
		if(g_sysPara.intelligentConnect.m_bDesignAngleJoint)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
	else if(GetPropID("g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate")==id)
	{
		if(g_sysPara.intelligentConnect.m_bDesignTubeCoverPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr="Yes";
		else
			valueStr="No";
#else 
			valueStr="是";
		else
			valueStr="否";
#endif
	}
#ifdef __PART_DESIGN_INC_// defined(__TSA_)||defined(__LDS_)
	else if(GetPropID("g_sysPara.bCalStdFoundationForce")==id)
	{
		if(g_sysPara.bCalStdFoundationForce)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.bUpdateRodDesignForce")==id)
	{
		if(g_sysPara.bUpdateRodDesignForce)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.iShowDeformedMapPlan")==id)
	{
		if(g_sysPara.iShowDeformedMapPlan==0)
			valueStr="0.位移变形";
		else if(g_sysPara.iShowDeformedMapPlan==1)
			valueStr="1.承载+长细比系数";
		else if(g_sysPara.iShowDeformedMapPlan==2)
			valueStr="2.承载系数";
		else
			valueStr="3.内力比";
	}
	/*else if(GetPropID("g_sysPara.bCreateAnalFile")==id)
	{
		if(g_sysPara.bCreateAnalFile)
			valueStr="是";
		else
			valueStr="否";
	}*/
	else if(GetPropID("g_sysPara.bRecNodeOffset")==id)
	{
		if(g_sysPara.bRecNodeOffset)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.bCreateAnsysCmdFile")==id)
	{
		if(g_sysPara.bCreateAnsysCmdFile)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.bCreateDetailRptFile")==id)
	{
		if(g_sysPara.bCreateDetailRptFile)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.iDetailDocType")==id)
	{
		if(g_sysPara.iDetailDocType==0)
			valueStr="0.文本文档";
		else
			valueStr="1.EXCEL表格";
	}
	else if(GetPropID("g_sysPara.iLanguage")==id)
	{
		if(g_sysPara.iLanguage==0)
			valueStr="0.中文";
		else
			valueStr="1.英文";
	}
	else if(GetPropID("g_sysPara.bMergeRptFileMirObj")==id)
	{
		if(g_sysPara.bMergeRptFileMirObj)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("g_sysPara.nM") == id)
	{
		sprintf(sText,"%d",g_sysPara.nM);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.nSigemaM") == id)
	{
		sprintf(sText,"%d",g_sysPara.nSigemaM);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.nPrecentSigemaM") == id)
	{
		sprintf(sText,"%d",g_sysPara.nPrecentSigemaM);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.iEfficacyCeofType") == id)
	{
		if(g_sysPara.iEfficacyCeofType==0)
			valueStr="0.承载+长细比系数";
		else
			valueStr="1.承载系数";
	}
	else if(GetPropID("g_sysPara.iLegShiftType")==id)
	{
		if(g_sysPara.iLegShiftType==0)
			valueStr="0.不轮换";
		else if(g_sysPara.iLegShiftType==1)
			valueStr="1.等高腿轮换";
		else
			valueStr="2.高低腿轮换";
	}
	else if(GetPropID("g_sysPara.reactionExportType")==id)
	{
		if(g_sysPara.reactionExportType==0)
			valueStr="0.全部输出";
		else if(g_sysPara.reactionExportType==1)
			valueStr="1.高低腿级";
		else
			valueStr="2.工况级";
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_bAuxPoleAnalysisInc")==id)
	{
		if(g_sysPara.m_bAuxPoleAnalysisInc)
			valueStr = "是";
		else
			valueStr = "否";
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_iResolverStyle")==id)
	{
		if(g_sysPara.AnalysisPara.m_iResolverStyle==0)
			valueStr = "0.高斯法";
		else if(g_sysPara.AnalysisPara.m_iResolverStyle==1)
			valueStr = "1.波前法";
		else
			valueStr = "2.电力塔架";
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_iNonlinearStyle")==id)
	{
		if(g_sysPara.AnalysisPara.m_iNonlinearStyle==0)
			valueStr = "0.线性分析";
		else
			valueStr = "1.非线性分析";
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_nLoadStepCounts") == id)
		valueStr.Format("%d",g_sysPara.AnalysisPara.m_nLoadStepCounts);
	else if(GetPropID("g_sysPara.AnalysisPara.m_nIterationCounts") == id)
		valueStr.Format("%d",g_sysPara.AnalysisPara.m_nIterationCounts);
	else if(GetPropID("g_sysPara.AnalysisPara.m_fConvergeLim") == id)
	{
		sText.Printf("%g",g_sysPara.AnalysisPara.m_fConvergeLim);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_nSubSpaceIterCounts") == id)
	{	//子空间迭代法最多迭代次数
		sprintf(sText,"%d",g_sysPara.AnalysisPara.m_nSubSpaceIterCounts);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_fFreqsTolerance") == id)
	{		//频率千分之一误差率
		sText.Printf("%g",g_sysPara.AnalysisPara.m_fFreqsTolerance);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef") == id)
	{
		sText.Printf("%g",g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("Ta.m_fVibrationT1") == id)
	{
		sText.Printf("%g",Ta.m_fVibrationT1);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("Ta.m_cLandscape")==id)
	{
		if(Ta.m_cLandscape=='A')
			valueStr="A:近海或沙漠平坦地区";
		else if(Ta.m_cLandscape=='B')
			valueStr="B:内陆建筑物稀疏地区";
		else if(Ta.m_cLandscape=='C')
			valueStr="C:有密集建筑群市区";
		else if(Ta.m_cLandscape=='D')
			valueStr="D:有密集建筑群和高层建筑市区";
	}
	else if(GetPropID("Ta.m_siSafetyGrade")==id)
	{
		if(Ta.m_siSafetyGrade==0)
			valueStr="一级:特别重要的杆塔";
		else if(Ta.m_siSafetyGrade==1)
			valueStr="二级:一般的各类杆塔";
		else if(Ta.m_siSafetyGrade==2)
			valueStr="三级:临时使用的杆塔";
		else
			valueStr="用户指定";
	}
	else if(GetPropID("Ta.m_fSafetyCoef")==id)
	{
		sText.Printf("%g",Ta.SafetyCoef());
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_iLoadExecuteCode") == id)
	{
		if(Ta.m_iLoadExecuteCode== 0)
			valueStr = "110~750kV输电线路设计规定";
		else if(Ta.m_iLoadExecuteCode == 1)
			valueStr = "高耸结构设计规定 GB50135-2006";
		else if(Ta.m_iLoadExecuteCode == 2)
			valueStr = "建筑结构荷载规范 GB50009-2001";
		else if(Ta.m_iLoadExecuteCode == 4)
			valueStr = "架空输电线路荷载规范 DL/T5551-2018";
		else
			valueStr = "用户指定";
	}
	else if(GetPropID("m_fLimitBearCoef")==id)
	{
		sText.Printf("%g",Ta.m_fLimitBearCoef);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_fGamaGForDesign")==id)
	{
		sText.Printf("%g",Ta.m_fGamaGForDesign);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_fGamaGForFoundation")==id)
	{
		sText.Printf("%g",Ta.m_fGamaGForFoundation);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_fGamaQForTowerWindLoad")==id)
	{
		sText.Printf("%g",Ta.m_fGamaQForTowerWindLoad);
		SimplifiedNumString(sText);
		valueStr = sText;
	}
	else if(GetPropID("m_iMemberSizeCheckCode")==id)
	{
		valueStr=Ta.DesignCode()->CodeName();
		/*if(Ta.m_iMemberSizeCheckCode== 0)
			valueStr = "110~750kV输电线路设计规定";
		else if(Ta.m_iMemberSizeCheckCode == 1)
			valueStr = "高耸结构设计规定 GB50135-2006";
		else if(Ta.m_iMemberSizeCheckCode == 2)
			valueStr = "美国 ASCE 10-97";*/
		//else if(Ta.m_iMemberSizeCheckCode==3)
		//	valueStr = "用户指定";
	}
	else if(GetPropID("m_iSlendernessLimCode")==id)
	{
		if(Ta.m_iSlendernessLimCode== 0)
			valueStr = "110~750kV输电线路设计规定";
		else if(Ta.m_iSlendernessLimCode == 1)
			valueStr = "高耸结构设计规定 GB50135-2006";
		else if(Ta.m_iSlendernessLimCode == 2)
			valueStr = "美国 ASCE 10-97";
		else if(Ta.m_iSlendernessLimCode==3)
			valueStr = "用户指定";
	}
	else if(GetPropID("Ta.SelMatPara.m_nMainPoleSlendernessLimit") == id)
		valueStr.Format("%d",Ta.SelMatPara.m_nMainPoleSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nXiePoleSlendernessLimit") == id)
		valueStr.Format("%d",Ta.SelMatPara.m_nXiePoleSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nAuxPoleSlendernessLimit") == id)
		valueStr.Format("%ld",Ta.SelMatPara.m_nAuxPoleSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nTensivePoleSlendernessLimit") == id)
		valueStr.Format("%ld",Ta.SelMatPara.m_nTensivePoleSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nMainTubeSlendernessLimit") == id)
		valueStr.Format("%d",Ta.SelMatPara.m_nMainTubeSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nXieTubeSlendernessLimit") == id)
		valueStr.Format("%d",Ta.SelMatPara.m_nXieTubeSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nAuxTubeSlendernessLimit") == id)
		valueStr.Format("%ld",Ta.SelMatPara.m_nAuxTubeSlendernessLimit);
	else if(GetPropID("Ta.SelMatPara.m_nTensiveTubeSlendernessLimit") == id)
		valueStr.Format("%ld",Ta.SelMatPara.m_nTensiveTubeSlendernessLimit);
	/*else if(GetPropID("boltGrade")==id)
	{	//应设置一变量保存SelBoltFilter对应值? 
		CString sItem;
		pPropList->GetItemPropValue(CLDSApp::GetPropID("SelBoltFilter"), sItem);
		if(sItem.Compare("20≤肢宽＜ 50(mm)时")==0)
			_snprintf(valueStr,4,"%s",Ta.SelMatPara.m_csM20_50LsGrade);
		else if(sItem.Compare("50≤肢宽＜ 63(mm)时")==0)
			_snprintf(valueStr,4,"%s",Ta.SelMatPara.m_csM50_63LsGrade);
		else if(sItem.Compare("63≤肢宽＜160(mm)时")==0)
			_snprintf(valueStr,4,"%s",Ta.SelMatPara.m_csM63_160LsGrade);
		else if(sItem.Compare("160≤肢宽≤200(mm)时")==0)
			_snprintf(valueStr,4,"%s",Ta.SelMatPara.m_csM160_200LsGrade);
	}*/
	else if(GetPropID("Ta.SelMatPara.m_bPreferSingleAngle") == id)
	{
		if(Ta.SelMatPara.m_bPreferSingleAngle)
			valueStr="是";
		else
			valueStr="否";
	}
	else if(GetPropID("Ta.SelMatPara.m_iSizeOrderRule") == id)
	{
		if(Ta.SelMatPara.m_iSizeOrderRule==0)
			valueStr="0.截面积排序";
		else if(Ta.SelMatPara.m_iSizeOrderRule==1)
			valueStr="1.回转半径排序";
		else
			valueStr="2.宽度厚度排序";
	}
	else if(GetPropID("Ta.SelMatPara.m_nConvergeLim") == id)
		valueStr.Format("%d",Ta.SelMatPara.m_nConvergeLim);
	else if(GetPropID("Ta.SelMatPara.m_nMaxSelMatCount") == id)
		valueStr.Format("%ld",Ta.SelMatPara.m_nMaxSelMatCount);
	else if(GetPropID("Ta.SelMatPara.m_csMinAngleSize") == id)
		valueStr = Ta.SelMatPara.m_csMinAngleSize;
	else if(GetPropID("Ta.SelMatPara.m_csMinForceAngleSize") == id)
		valueStr = Ta.SelMatPara.m_csMinForceAngleSize;
	else if(GetPropID("Ta.SelMatPara.m_csMinMainAngleSize") == id)
		valueStr = Ta.SelMatPara.m_csMinMainAngleSize;
	else if(GetPropID("EarthQuake.grade") == id)
		valueStr.Format("%d级",Ta.EarthQuake.grade);
	else if(GetPropID("EarthQuake.AlfaMax") == id)
	{
		valueStr.Format("%f",Ta.EarthQuake.alfa_max);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("EarthQuake.Tg") == id)
	{
		valueStr.Format("%f",Ta.EarthQuake.Tg);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("EarthQuake.zeta") == id)
	{
		valueStr.Format("%f",Ta.EarthQuake.zeta);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("EarthQuake.gamaRE") == id)
	{
		valueStr.Format("%f",Ta.EarthQuake.gamaRE);
		SimplifiedNumString(valueStr);
	}
	else if(GetPropID("EarthQuake.nFreqsRank") == id)
		valueStr.Format("%d",Ta.EarthQuake.nFreqsRank);
#endif
	else
	{
		for (DESIGN_PARAM_ITEM* pParam=Ta.hashParams.GetFirst();pParam;pParam=Ta.hashParams.GetNext())
		{
			if(GetPropID((char*)pParam->sLabel)==id)
			{
				DESIGN_PARAM_ITEM* pDestParam=Ta.hashParams.GetValue(KEY4C((char*)pParam->sLabel));
				valueStr="0";
				//TODO:为什么只处理浮点类型
				if(pDestParam&&pDestParam->dataType==DESIGN_PARAM_ITEM::TYPE_DOUBLE)
					valueStr.Format("%f",pDestParam->value.fValue);
				return valueStr;
			}
		}
		return "";
	}
	return valueStr;
}
int CLDSApp::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CString str=GetPropValueStr(id);
	if(valueStr)
		StrCopy(valueStr,str,nMaxStrBufLen);
	return str.GetLength();
}
void GetSysPath(char* App_Path,const char *src_path/*=NULL*/,char *file_name/*=NULL*/,char *extension/*=NULL*/)
{
	char drive[4];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];

	if(src_path)
		_splitpath(src_path,drive,dir,fname,ext);
	else
		_splitpath(__argv[0],drive,dir,fname,ext);
	if(App_Path)
	{
		strcpy(App_Path,drive);
		strcat(App_Path,dir);
	}
	if(file_name)
		strcpy(file_name,fname);
	if(extension)
		strcpy(extension,ext);
}

const char* CLDSApp::get_WorkPath()
{
	if(*work_path==0)
	{
		CXhChar200 file_name;
		CString ss=GetLDSDoc()->GetPathName();
		if(ss.GetLength()==0)
			return execute_path;
		GetSysPath(work_path,ss,file_name);
	}
	return work_path;
}
const char* CLDSApp::get_ExecPath()
{
	return execute_path;
}
void CLDSApp::GetWorkPathAuto()
{
	CXhChar200 file_name;
	CString ss=GetLDSDoc()->GetPathName();
	GetSysPath(work_path,ss,file_name);
	_chdir(work_path);
	file_name.Append("_worktemp");
	_mkdir(file_name);
	_chdir(file_name);
}

BOOL CLDSApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinAppEx::PreTranslateMessage(pMsg);
}

CLDSDoc* CLDSApp::GetLDSDoc()
{
	POSITION pos,docpos;
	pos=AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
	for(CDocTemplate *pDocTemp=AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
		pDocTemp;pDocTemp=AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos))
	{
		docpos=pDocTemp->GetFirstDocPosition();
		for(CDocument *pDoc=pDocTemp->GetNextDoc(docpos);pDoc;pDoc=pDocTemp->GetNextDoc(docpos))
		{
			if(pDoc->IsKindOf(RUNTIME_CLASS(CLDSDoc)))
				return (CLDSDoc*)pDoc;
		}
	}
	return NULL;
}
UINT ListenTubeProc(LPVOID pParam)
{
	HANDLE hTubeServerSend=*((HANDLE*)pParam);
	LDS_MSGHEAD msg_head;
	DWORD dwBytes;
	while(ReadFile(hTubeServerSend,&msg_head,9,&dwBytes,NULL))
	{
		if(msg_head.command_id==NOTIFY_MSG)
		{
			CBuffer buffer;
			char *content=new char[msg_head.msg_length+1];
			ReadFile(hTubeServerSend,content,msg_head.msg_length,&dwBytes,NULL);
			buffer.Write(content,msg_head.msg_length);
			buffer.SeekToBegin();
			int size;
			buffer.ReadInteger(&size);
			buffer.Read(content,size);
			content[size]='\0';
			MSG_STRU *pMsg=theApp.m_arrMsg.append();
			strncpy(pMsg->user,content,50);
			buffer.ReadInteger(&size);
			buffer.Read(content,size);
			content[size]='\0';
			strncpy(pMsg->msg,content,1000);
			if(theApp.GetMainWnd()&&theApp.GetMainWnd()->GetSafeHwnd())
				((CMainFrame*)theApp.GetMainWnd())->StartInfoPrompt();
			delete []content;
		}
		else if(msg_head.command_id==NOTIFY_DATA_MODIFIED)
		{
			DWORD update_word=0;
			ReadFile(hTubeServerSend,&update_word,4,&dwBytes,NULL);
			theApp.m_dwUpdateWord|=update_word;
			if(theApp.GetMainWnd()&&theApp.GetMainWnd()->GetSafeHwnd())
			((CMainFrame*)theApp.GetMainWnd())->StartInfoPrompt();
		}
		else if(msg_head.command_id==NOTIFY_PERM_MODIFIED)
		{
			theApp.m_bNeedUpdatePerm=TRUE;
			if(theApp.GetMainWnd()&&theApp.GetMainWnd()->GetSafeHwnd())
				((CMainFrame*)theApp.GetMainWnd())->StartInfoPrompt();
		}
	}
	return 0;
}
BOOL CLDSApp::LogonServer()
{
	CReConnServerDlg logdlg;
	while(1)
	{
		if(logdlg.DoModal()==IDOK)
		{
			HANDLE PipeHandle;
			char server[MAX_PATH],user_name[MAX_PATH];
			m_sWorkgroupServer=logdlg.m_sServerName;
			m_sUserName=logdlg.m_sUserName;
			sprintf(server,"%s",m_sWorkgroupServer);
			sprintf(user_name,"%s",m_sUserName);
#ifdef __TMA_
			if(SERVR_SELF_MODE)
				sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tma\\CorePipe");
			else
				sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tma\\CorePipe",server);
#elif defined(__LMA_)
			if(SERVR_SELF_MODE)
				sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Lma\\CorePipe");
			else
				sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Lma\\CorePipe",server);
#elif defined(__TSA_)
			if(SERVR_SELF_MODE)
				sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tsa\\CorePipe");
			else
				sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tsa\\CorePipe",server);
#elif defined(__TDA_)
			if(SERVR_SELF_MODE)
				sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tda\\CorePipe");
			else
				sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tda\\CorePipe",server);
#else
			if(SERVR_SELF_MODE)
				sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Lds\\CorePipe");
			else
				sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Lds\\CorePipe",server);
#endif

			if(WaitNamedPipe(PIPE_CORE_NAME,NMPWAIT_USE_DEFAULT_WAIT)==0)
			{
				DWORD dw = GetLastError();
				CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
				ss.Format("The server login is failure #1,0X%X！",dw);
#else 
				ss.Format("登录服务器失败#1,0X%X！",dw);
#endif
				AfxMessageBox(ss);
				continue;
			}
			if((PipeHandle=CreateFile(PIPE_CORE_NAME,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
			{
				DWORD dw = GetLastError();
				CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
				ss.Format("The server login is failure #1.1,0X%X！",dw);
#else 
				ss.Format("登录服务器失败#1.1,0X%X！",dw);
#endif
				AfxMessageBox(ss);
				continue;
			}
#ifdef __TMA_
			if(SERVR_SELF_MODE)
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\.\\Pipe\\Tma\\%s_ListenPipe",m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\.\\Pipe\\Tma\\%s_SendPipe",m_sUserName);
			}
			else
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\%s\\Pipe\\Tma\\%s_ListenPipe",server,m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\%s\\Pipe\\Tma\\%s_SendPipe",server,m_sUserName);
			}
#elif defined(__LMA_)
			if(SERVR_SELF_MODE)
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\.\\Pipe\\Lma\\%s_ListenPipe",m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\.\\Pipe\\Lma\\%s_SendPipe",m_sUserName);
			}
			else
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\%s\\Pipe\\Lma\\%s_ListenPipe",server,m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\%s\\Pipe\\Lma\\%s_SendPipe",server,m_sUserName);
			}
#elif defined(__TSA_)
			if(SERVR_SELF_MODE)
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\.\\Pipe\\Tsa\\%s_ListenPipe",m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\.\\Pipe\\Tsa\\%s_SendPipe",m_sUserName);
			}
			else
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\%s\\Pipe\\Tsa\\%s_ListenPipe",server,m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\%s\\Pipe\\Tsa\\%s_SendPipe",server,m_sUserName);
			}
#elif defined(__TDA_)
			if(SERVR_SELF_MODE)
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\.\\Pipe\\Tda\\%s_ListenPipe",m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\.\\Pipe\\Tda\\%s_SendPipe",m_sUserName);
			}
			else
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\%s\\Pipe\\Tda\\%s_ListenPipe",server,m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\%s\\Pipe\\Tda\\%s_SendPipe",server,m_sUserName);
			}
#else
			if(SERVR_SELF_MODE)
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\.\\Pipe\\Lds\\%s_ListenPipe",m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\.\\Pipe\\Lds\\%s_SendPipe",m_sUserName);
			}
			else
			{
				sprintf(PIPE_SERVER_LISTEN,"\\\\%s\\Pipe\\Lds\\%s_ListenPipe",server,m_sUserName);
				sprintf(PIPE_SERVER_SEND,"\\\\%s\\Pipe\\Lds\\%s_SendPipe",server,m_sUserName);
			}
#endif
			DWORD BytesWritten,BytesRead;
			LDS_MSGHEAD msg_head;
			msg_head.command_id=REGISTER_LDS_USER;
			msg_head.msg_length=m_sUserName.GetLength();
			msg_head.src_code=0;
			WriteFile(PipeHandle,&msg_head,9,&BytesWritten,NULL);
			if(msg_head.msg_length>0)
				WriteFile(PipeHandle,user_name,msg_head.msg_length,&BytesWritten,NULL);
			ReadFile(PipeHandle,&m_nUserId,4,&BytesRead,NULL);
			CloseHandle(PipeHandle);
			//获得协同中心端的版本号
			if(WaitNamedPipe(PIPE_CORE_NAME,NMPWAIT_USE_DEFAULT_WAIT)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The server login is failure #2！");
#else 
				AfxMessageBox("登录服务器失败#2！");
#endif
				continue;
			}
			if((PipeHandle=CreateFile(PIPE_CORE_NAME,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Server connection id failure #2！!");
#else 
				AfxMessageBox("连接服务器失败#2！!");
#endif
				continue;
			}
			msg_head.command_id=ASK_PRODUCT_VERSION;
			msg_head.msg_length=0;
			msg_head.src_code=0;
			WriteFile(PipeHandle,&msg_head,9,&BytesWritten,NULL);
			char memory_pool[20];
			CString server_version,fileformat_version;
			ReadFile(PipeHandle,memory_pool,20,&BytesRead,NULL);
			CloseHandle(PipeHandle);

			server_version=memory_pool;
			GetFileVersion(fileformat_version);
			//GetProductVersion(product_version);
			//XHVERSION client(product_version);
			//if(client.Compare(server_version,3)!=0)
			if(compareVersion(fileformat_version,server_version)!=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The client-side doesn't match with version number of coordination center，registration is failure！");
#else 
				AfxMessageBox("客户端与协同中心版本号不匹配，注册失败！");
#endif
				return FALSE;
			}

			if(WaitNamedPipe(PIPE_SERVER_LISTEN,NMPWAIT_USE_DEFAULT_WAIT)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("User registration is failed！");
#else 
				AfxMessageBox("用户注册失败！");
#endif
				continue;
			}
			if((m_hPipeServerListen=CreateFile(PIPE_SERVER_LISTEN,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("User registration is failed！");
#else 
				AfxMessageBox("用户注册失败！");
#endif
				continue;
			}
			if(WaitNamedPipe(PIPE_SERVER_SEND,NMPWAIT_USE_DEFAULT_WAIT)==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("User registration is failed！");
#else 
				AfxMessageBox("用户注册失败！");
#endif
				continue;
			}
			if((m_hPipeServerSend=CreateFile(PIPE_SERVER_SEND,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("User registration is failed！");
#else 
				AfxMessageBox("用户注册失败！");
#endif
				continue;
			}
			CWinThread *pThread=AfxBeginThread(ListenTubeProc,&m_hPipeServerSend,THREAD_PRIORITY_NORMAL);
			break;
		}
		else
			return FALSE;
	};
	return TRUE;
}
BOOL CLDSApp::LogonTMSServer()
{
/*
#ifdef __CONNECT_REMOTE_SERVER_
	CReConnServerDlg logdlg;
	logdlg.m_bInputPassword=TRUE;
	logdlg.m_sServerName=theApp.GetProfileString("Server","ServerName");
	logdlg.m_sUserName=theApp.GetProfileString("Server","UserName");
	if(logdlg.DoModal()==IDOK)
	{
		WriteProfileString("Server","ServerName",logdlg.m_sServerName);
		WriteProfileString("Server","UserName",logdlg.m_sUserName);
		if(logdlg.m_sServerName.GetLength()>0)
			TMS.SetServerUrl(logdlg.m_sServerName.Trim());
		return TMS.LoginUser(logdlg.m_sUserName,logdlg.m_sPassword);
	}
	else
#endif
*/
		return FALSE;
}

int CLDSApp::ExitInstance() 
{
	if(m_hWireLayoutModule!=NULL)
		FreeLibrary(m_hWireLayoutModule);
	if(m_hBomExport!=NULL)
		FreeLibrary(m_hBomExport);
	if(m_bCooperativeWork)
	{
		LDS_MSGHEAD msg_head;
		msg_head.command_id=LOGOUT_LDS_USER;
		msg_head.msg_length=0;
		msg_head.src_code=(BYTE)m_nUserId;
		HANDLE PipeHandle;
		DWORD BytesWritten;
		char server[MAX_PATH];
		sprintf(server,"%s",m_sWorkgroupServer);
#ifdef __TMA_
		if(SERVR_SELF_MODE)
			sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tma\\CorePipe");
		else
			sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tma\\CorePipe",server);
#elif defined(__LMA_)
		if(SERVR_SELF_MODE)
			sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Lma\\CorePipe");
		else
			sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Lma\\CorePipe",server);
#elif defined(__TSA_)
		if(SERVR_SELF_MODE)
			sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tsa\\CorePipe");
		else
			sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tsa\\CorePipe",server);
#elif defined(__TDA_)
		if(SERVR_SELF_MODE)
			sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Tda\\CorePipe");
		else
			sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Tda\\CorePipe",server);
#elif defined(__LDS_)
		if(SERVR_SELF_MODE)
			sprintf(PIPE_CORE_NAME,"\\\\.\\Pipe\\Lds\\CorePipe");
		else
			sprintf(PIPE_CORE_NAME,"\\\\%s\\Pipe\\Lds\\CorePipe",server);
#endif
		
		if(WaitNamedPipe(PIPE_CORE_NAME,NMPWAIT_USE_DEFAULT_WAIT)==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The server login is failure ！");
#else 
			AfxMessageBox("登录服务器失败！");
#endif
		if((PipeHandle=CreateFile(PIPE_CORE_NAME,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL))==INVALID_HANDLE_VALUE)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("用户注销失败！");
#else 
			AfxMessageBox("用户注销失败！");
#endif
		WriteFile(PipeHandle,&msg_head,9,&BytesWritten,NULL);
		CloseHandle(PipeHandle);
		if(m_hPipeServerListen)
			CloseHandle(m_hPipeServerListen);
		if(m_hPipeServerSend)
			CloseHandle(m_hPipeServerSend);
	}
#ifndef _LEGACY_LICENSE
	ExitCurrentDogSession();
#elif defined(_NET_DOG)
	ExitNetLicEnv(m_wDogModule);
#endif
	return CWinAppEx::ExitInstance();
}

BOOL CLDSApp::IsAnalysisWorkEnv()
{
	if(m_eEnvType==FEMELEM_ENV||m_eEnvType==FEMROD_ENV)
		return TRUE;
	else
		return FALSE;
}
void CLDSApp::GetAppVersion(char *file_version,char *product_version)
{
	DWORD dwLen=GetFileVersionInfoSize(__argv[0],0);
	WORD file_ver[4],product_ver[4];
	BYTE _arrVerBytes[2048];
	BYTE_ARRAY xarrVerBytes(dwLen,false,_arrVerBytes,2048);
	if(GetFileVersionInfo(__argv[0],0,dwLen,xarrVerBytes))
	{
		VS_FIXEDFILEINFO *info;
		UINT uLen;
		VerQueryValue(xarrVerBytes,"\\",(void**)&info,&uLen);
		memcpy(file_ver,&info->dwFileVersionMS,4);
		memcpy(&file_ver[2],&info->dwFileVersionLS,4);
		memcpy(product_ver,&info->dwProductVersionMS,4);
		memcpy(&product_ver[2],&info->dwProductVersionLS,4);
		if(file_version)
			sprintf(file_version,"%d.%d.%d.%d",file_ver[1],file_ver[0],file_ver[3],file_ver[2]);
		if(product_version)
			sprintf(product_version,"%d.%d.%d.%d",product_ver[1],product_ver[0],product_ver[3],product_ver[2]);
	}
}
void CLDSApp::GetFileVersion(CString &file_version)
{
	DWORD dwLen=GetFileVersionInfoSize(__argv[0],0);
	WORD file_ver[4];
	BYTE _arrVerBytes[2048];
	BYTE_ARRAY xarrVerBytes(dwLen,false,_arrVerBytes,2048);
	if (GetFileVersionInfo(__argv[0],0,dwLen,xarrVerBytes))
	{
		VS_FIXEDFILEINFO *info;
		UINT uLen;
		VerQueryValue(xarrVerBytes,"\\",(void**)&info,&uLen);
		memcpy(file_ver,&info->dwFileVersionMS,4);
		memcpy(&file_ver[2],&info->dwFileVersionLS,4);
		file_version.Format("%d.%d.%d.%d",file_ver[1],file_ver[0],file_ver[3],file_ver[2]);
	}
}
void CLDSApp::GetProductVersion(CString &product_version)
{
	DWORD dwLen=GetFileVersionInfoSize(__argv[0],0);
	WORD product_ver[4];
	BYTE _arrVerBytes[2048];
	BYTE_ARRAY xarrVerBytes(dwLen,false,_arrVerBytes,2048);
	if(GetFileVersionInfo(__argv[0],0,dwLen,xarrVerBytes))
	{
		VS_FIXEDFILEINFO *info;
		UINT uLen;
		VerQueryValue(xarrVerBytes,"\\",(void**)&info,&uLen);
		memcpy(product_ver,&info->dwProductVersionMS,4);
		memcpy(&product_ver[2],&info->dwProductVersionLS,4);
		product_version.Format("%d.%d.%d.%d",product_ver[1],product_ver[0],product_ver[3],product_ver[2]);
	}
}
void CLDSApp::GetProductVersion(char byteVer[4])
{
	DWORD dwLen=GetFileVersionInfoSize(__argv[0],0);
	WORD product_ver[4];
	BYTE _arrVerBytes[2048];
	BYTE_ARRAY xarrVerBytes(dwLen,false,_arrVerBytes,2048);
	if(GetFileVersionInfo(__argv[0],0,dwLen,xarrVerBytes))
	{
		VS_FIXEDFILEINFO *info;
		UINT uLen;
		VerQueryValue(xarrVerBytes,"\\",(void**)&info,&uLen);
		memcpy(product_ver,&info->dwProductVersionMS,4);
		memcpy(&product_ver[2],&info->dwProductVersionLS,4);
	}
	byteVer[0]=(char)product_ver[2];
	byteVer[1]=(char)product_ver[3];
	byteVer[2]=(char)product_ver[0];
	byteVer[3]=(char)product_ver[1];
}

static BOOL CreateVersionInfoProcess( HANDLE hClientPipeRead, HANDLE hClientPipeWrite )
{
	//step1:获取荷载计算程序路径
	TCHAR cmd_str[MAX_PATH];
	GetSysPath(cmd_str);
	strcat(cmd_str,"Version.exe -child");

	STARTUPINFO startInfo;
	memset( &startInfo, 0 , sizeof( STARTUPINFO ) );

	startInfo.cb= sizeof( STARTUPINFO );
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	startInfo.hStdError= 0;//hPipeWrite;
	startInfo.hStdInput= hClientPipeRead;
	startInfo.hStdOutput= hClientPipeWrite;

	PROCESS_INFORMATION processInfo;
	memset( &processInfo, 0, sizeof(PROCESS_INFORMATION) );

	BOOL b=CreateProcess( NULL,cmd_str,
		NULL,NULL, TRUE,CREATE_NEW_CONSOLE, NULL, NULL,&startInfo,&processInfo );
	DWORD er=GetLastError();
	/*if( b )
	{
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
	}*/

	return b;

}

static BOOL WriteToClient(HANDLE hPipeWrite,bool bOnlyOnHasNewNotes)
{
	if( hPipeWrite == INVALID_HANDLE_VALUE )
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The handle of obtained tube is invalid\n");
#else 
		AfxMessageBox("获取管道句柄无效\n");
#endif
		return FALSE;
	}

	CBuffer buffer;
	buffer.WriteInteger(DogSerialNo());
	int doc_type=1;
	DWORD version;
	theApp.GetProductVersion((char*)(&version));
#ifdef __TMA_
	doc_type=1;
#elif defined(__LMA_)
	doc_type=2;
#elif defined(__TSA_)
	doc_type=3;
#elif defined(__LDS_)
	doc_type=4;
#elif defined(__TDA_)
	doc_type=5;
#endif
	buffer.WriteInteger(doc_type);
	buffer.WriteInteger(version);
	buffer.WriteBooleanThin(bOnlyOnHasNewNotes);
	//从匿名管道中读取数据，显示，并返回给服务器
	return buffer.WriteToPipe(hPipeWrite,1024);
}

void CLDSApp::GetVersionUpdateNotes(bool bOnlyOnHasNewNotes/*=true*/)
{
	//step1:创建第一个管道: 用于服务器端向客户端发送内容
	SECURITY_ATTRIBUTES attrib;
	attrib.nLength = sizeof( SECURITY_ATTRIBUTES );
	attrib.bInheritHandle= true;
	attrib.lpSecurityDescriptor = NULL;
	HANDLE hPipeClientRead=NULL, hPipeSrcWrite=NULL;

	if( !CreatePipe( &hPipeClientRead, &hPipeSrcWrite, &attrib, 0 ) )
	{
		//printf("创建匿名管道失败!GetLastError= %d\n", GetLastError() );
		return ;//FALSE;
	}

	//因为此时两个句柄均可继承，故可复制一个不可继承的写句柄并将原来的写句柄关掉
	HANDLE hPipeSrcWriteDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcWrite, GetCurrentProcess(), &hPipeSrcWriteDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("复制句柄失败,GetLastError=%d\n", GetLastError() );
		return ;//FALSE;
	}
	CloseHandle( hPipeSrcWrite );

	//step2:创建第二个管道，用于客户端向服务器端发送内容
	HANDLE hPipeClientWrite=NULL, hPipeSrcRead=NULL;
	if( !CreatePipe( &hPipeSrcRead, &hPipeClientWrite, &attrib, 0) )
	{
		//printf("创建第二个匿名管道失败,GetLastError=%d\n", GetLastError() );
		return ;//FALSE;
	}

	//复制不可继承的读句柄，并把原有的读句柄关掉
	HANDLE hPipeSrcReadDup=NULL;
	if( !DuplicateHandle( GetCurrentProcess(), hPipeSrcRead, GetCurrentProcess(), &hPipeSrcReadDup, 0, false, DUPLICATE_SAME_ACCESS ) )
	{
		//printf("复制第二个句柄失败,GetLastError=%d\n", GetLastError() );
		return ;//FALSE;
	}
	CloseHandle( hPipeSrcRead );

	//step3:创建子进程,
	if( !CreateVersionInfoProcess( hPipeClientRead, hPipeClientWrite ) )
	{
		//printf("创建子进程失败\n" );
		return ;//FALSE;
	}
	WriteToClient(hPipeSrcWriteDup,bOnlyOnHasNewNotes);
}

void CLDSApp::ProcessInfoPackage()
{
	CProcessMsgPackDlg pack_dlg;
	pack_dlg.DoModal();
}

void CLDSApp::SendMsgToOther(CString &receiver, CString &msg)
{
	LDS_MSGHEAD msg_head;
	DWORD dwBytes;
	CBuffer buffer;
	char *str_arr=new char[max(receiver.GetLength(),msg.GetLength())+1];
	msg_head.command_id=ASK_DELIVER_MSG;
	msg_head.msg_length=receiver.GetLength()+msg.GetLength()+8;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	buffer.Write(&msg_head,9);
	buffer.WriteInteger(receiver.GetLength());
	sprintf(str_arr,"%s",receiver);
	buffer.Write(str_arr,receiver.GetLength());
	buffer.WriteInteger(msg.GetLength());
	sprintf(str_arr,"%s",msg);
	buffer.Write(str_arr,msg.GetLength());
	WriteFile(theApp.m_hPipeServerListen,buffer.GetBufferPtr(),buffer.GetLength(),&dwBytes,NULL);
	delete []str_arr;
}

DWORD CLDSApp::GetPermDword()
{
	if(m_nUserId==0)
		return PERM_TOWER_ADMIN;
	else
		return GetSingleWord(m_nUserId);
}

BOOL CLDSApp::IsHasModifyPerm(DWORD dwPerm)
{
	if(dwPerm&GetPermDword())
		return TRUE;
	else
		return FALSE;
}

void CLDSApp::GetUserInfo(BRIEF_USER user_arr[])
{
	//向服务器发送指令提取用户信息
	LDS_MSGHEAD msg_head;
	DWORD dwBytes,dwRead;
	CBuffer buffer;
	BYTE memory_pool[PACK_SIZE];
	msg_head.command_id=ASK_USER_INFO;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)m_nUserId;
	WriteFile(m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	while(ReadFile(m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
	{
		buffer.Write(memory_pool,PACK_SIZE-1);
		if(memory_pool[PACK_SIZE-1]==0)
			break;	//数据接收结束
	}
	buffer.SeekToBegin();
	for(int i=0;i<31;i++)
	{
		buffer.ReadInteger(&user_arr[i].m_bInUse);
		buffer.ReadInteger(&user_arr[i].m_nUserId);
		buffer.Read(user_arr[i].m_sUserName,50);
		user_arr[i].m_sUserName[50]='\0';
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	HICON m_hLogoIcon;
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX_LDS};
	CString	m_sVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	m_hLogoIcon=0;
	//{{AFX_DATA_INIT(CAboutDlg)
	m_sVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_S_VERSION, m_sVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CLDSApp::OnUpdateAppAbout(CCmdUI* pCmdUI)
{
#ifdef __LDS_
	pCmdUI->SetText("关于 LDS(&A)...");
#endif
}
void CLDSApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	GetProductVersion(aboutDlg.m_sVersion);
	CString serialStr;
#ifndef _LEGACY_LICENSE
#ifdef AFX_TARG_ENU_ENGLISH
	serialStr.Format(" Authorization Key:%d",DogSerialNo());
#else 
	serialStr.Format("通用版 授权号:%d",DogSerialNo());
#endif
#elif defined(_NET_DOG)
	serialStr.Format("网络版 授权号:%d",DogSerialNo());
#else
	serialStr.Format("单机版 授权号:%d",DogSerialNo());
#endif
#ifdef __LMA_
	aboutDlg.m_sVersion="LMA "+aboutDlg.m_sVersion+serialStr;
#elif defined(__LDS_)
	aboutDlg.m_sVersion="LDS "+aboutDlg.m_sVersion+serialStr;
#elif defined(__TMA_)
	aboutDlg.m_sVersion="TMA "+aboutDlg.m_sVersion+serialStr;
#elif defined(__TDA_)
	aboutDlg.m_sVersion="TDA "+aboutDlg.m_sVersion+serialStr;
#else
	aboutDlg.m_sVersion="TSA "+aboutDlg.m_sVersion+serialStr;
#endif
	aboutDlg.m_hLogoIcon=this->LoadIcon(m_nMainMenuID);
	aboutDlg.DoModal();
}

void CLDSApp::GetFileName(char* file_path,bool hasExtension/*=false*/)
{
	CXhChar200 file_name;
	CString ss=GetLDSDoc()->GetPathName();
	sprintf(file_path,"%s",ss);
	CXhChar16 extension;
	GetSysPath(file_path,ss,file_name,extension);
	if(hasExtension)
		file_name.Append(extension);
	strcat(file_path,file_name);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __LMA_
	SetWindowText("About LMA");
#elif defined(__TMA_)
	SetWindowText("About TMA");
#elif defined(__LDS_)
	SetWindowText("About LDS");
#elif defined(__TDA_)
	SetWindowText("About TDA");
#else
	SetWindowText("About TSA");
#endif
#else
#ifdef __LMA_
	SetWindowText("关于LMA");
#elif defined(__TMA_)
	CXhChar50 sClientName=GetClientNameById(CLDSPart::CustomerSerial);
	if(sClientName.GetLength()>0)
		SetWindowText(CXhChar100("关于TMA(%s定制版)",(char*)sClientName));
	else if(CLDSPart::CustomerSerial==0)
		SetWindowText("关于TMA");
	else
		SetWindowText(CXhChar50("关于TMA(定制版%d)",CLDSPart::CustomerSerial));
#elif defined(__LDS_)
	SetWindowText("关于LDS");
#elif defined(__TDA_)
	SetWindowText("关于 TDA");
#else
	SetWindowText("关于 TSA");
#endif
#endif
	CWnd *pWnd=GetDlgItem(IDC_S_COPYRIGHT);
	if (m_hLogoIcon>0)
	{
		CStatic * pLogoPanel=(CStatic*)GetDlgItem(IDC_PIC_SKETCH);
		pLogoPanel->SetIcon(m_hLogoIcon);
	}
	CXhChar200 szCopyright="北京信狐天诚软件科技有限公司版权所有 (C) 2008-2019";
	if(pWnd!=NULL&&gxLocalizer.szLicenseCopyright.Length>0)
		pWnd->SetWindowText(gxLocalizer.szLicenseCopyright);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// CLDSApp 自定义加载/保存方法

void CLDSApp::PreLoadState()
{
	/*BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);*/
}

void CLDSApp::LoadCustomState()
{
}

void CLDSApp::SaveCustomState()
{
}

BOOL CLDSApp::SaveState(LPCTSTR lpszSectionName/*=NULL*/, CFrameImpl* pFrameImpl/*=NULL*/)
{
	CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	if(pView&&pView->m_curAppType==TA_PART_EDITOR)
		return TRUE;
	BOOL bRetCode = CWinAppEx::SaveState(lpszSectionName,pFrameImpl);
	if(bRetCode)
	{	//删除注册表中菜单相关内容 wht 14-11-26
		HKEY hParentKey= (lpszSectionName!=NULL)? GetSectionKey(lpszSectionName):GetSectionKey(m_strRegSection);
		if(hParentKey)
		{
			CString strMenuBarSection;
			strMenuBarSection.Format("BasePane-%d0",m_nMainMenuID);
			DelRegTree(hParentKey,strMenuBarSection);
			strMenuBarSection.Format("Pane-%d0",m_nMainMenuID);
			DelRegTree(hParentKey,strMenuBarSection);
			RegCloseKey(hParentKey);
		}
	}
	return bRetCode;
}

void ExportPropListHelpInfo(CBuffer& buffer)
{
	CXhChar100 m_defaultClsUrl;
	//单角钢
	buffer.WriteInteger(CLS_LINEANGLE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Single Angle");
#else 
	buffer.WriteString("单角钢");
#endif
	buffer.WriteString(CLDSLineAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM* pPropItem=CLDSLineAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//组合角钢
	buffer.WriteInteger(CLS_GROUPLINEANGLE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Group Angle");
#else 
	buffer.WriteString("组合角钢");
#endif
	buffer.WriteString(CLDSGroupLineAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSGroupLineAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM* pPropItem=CLDSGroupLineAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSGroupLineAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSGroupLineAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSDbObject
	buffer.WriteInteger(CLS_DBOBJECT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("General data object");
#else 
	buffer.WriteString("通用数据对象");
#endif
	buffer.WriteString(CLDSDbObject::HELPLINK_URL);
	buffer.WriteInteger(CLDSDbObject::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSDbObject::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSDbObject::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSDbObject::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//构件
	buffer.WriteInteger(CLS_PART);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("General Part");
#else 
	buffer.WriteString("通用构件");
#endif
	buffer.WriteString(CLDSPart::HELPLINK_URL);
	buffer.WriteInteger(CLDSPart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//球
	buffer.WriteInteger(CLS_SPHERE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Sphere");
#else 
	buffer.WriteString("球");
#endif
	buffer.WriteString(CLDSSphere::HELPLINK_URL);
	buffer.WriteInteger(CLDSSphere::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSSphere::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSSphere::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSSphere::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//螺栓
	buffer.WriteInteger(CLS_BOLT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Bolt");
#else 
	buffer.WriteString("螺栓");
#endif
	buffer.WriteString(CLDSBolt::HELPLINK_URL);
	buffer.WriteInteger(CLDSBolt::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSBolt::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSBolt::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSBolt::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//节点
	buffer.WriteInteger(CLS_NODE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Node");
#else 
	buffer.WriteString("节点");
#endif
	buffer.WriteString(CLDSNode::HELPLINK_URL);
	buffer.WriteInteger(CLDSNode::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSNode::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSNode::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSNode::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//线形杆件
	buffer.WriteInteger(CLS_LINEPART);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Line Rod");
#else 
	buffer.WriteString("直线杆件");
#endif
	buffer.WriteString(CLDSLinePart::HELPLINK_URL);
	buffer.WriteInteger(CLDSLinePart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLinePart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLinePart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLinePart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSArcPart
	buffer.WriteInteger(CLS_ARCPART);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Ring Rod");
#else 
	buffer.WriteString("环形杆件");
#endif
	buffer.WriteString(CLDSArcPart::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcPart::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcPart::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcPart::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcPart::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//环形角钢
	buffer.WriteInteger(CLS_ARCANGLE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Ring Angle");
#else 
	buffer.WriteString("环形角钢");
#endif
	buffer.WriteString(CLDSArcAngle::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcAngle::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcAngle::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcAngle::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcAngle::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//环形扁钢
	buffer.WriteInteger(CLS_ARCFLAT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Ring Flate Angle");
#else 
	buffer.WriteString("环形扁钢");
#endif
	buffer.WriteString(CLDSArcFlat::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcFlat::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcFlat::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcFlat::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcFlat::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//环形槽钢
	buffer.WriteInteger(CLS_ARCSLOT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Ring U-steel");
#else 
	buffer.WriteString("环形槽钢");
#endif
	buffer.WriteString(CLDSArcSlot::HELPLINK_URL);
	buffer.WriteInteger(CLDSArcSlot::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSArcSlot::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSArcSlot::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSArcSlot::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//参照板
	buffer.WriteInteger(CLS_PARAMPLATE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Parameter Plate");
#else 
	buffer.WriteString("参照化板");
#endif
	buffer.WriteString(CLDSParamPlate::HELPLINK_URL);
	buffer.WriteInteger(CLDSParamPlate::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSParamPlate::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSParamPlate::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSParamPlate::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//面板
	buffer.WriteInteger(CLS_PLATE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Plate");
#else 
	buffer.WriteString("钢板");
#endif
	buffer.WriteString(CLDSPlate::HELPLINK_URL);
	buffer.WriteInteger(CLDSPlate::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPlate::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPlate::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPlate::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//钢管
	buffer.WriteInteger(CLS_LINETUBE);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Tube");
#else 
	buffer.WriteString("钢管");
#endif
	buffer.WriteString(CLDSLineTube::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineTube::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineTube::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineTube::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineTube::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//槽钢
	buffer.WriteInteger(CLS_LINESLOT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("U-steel");
#else 
	buffer.WriteString("槽钢");
#endif
	buffer.WriteString(CLDSLineSlot::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineSlot::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineSlot::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineSlot::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineSlot::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//扁钢
	buffer.WriteInteger(CLS_LINEFLAT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Flate Anlge");
#else 
	buffer.WriteString("扁钢");
#endif
	buffer.WriteString(CLDSLineFlat::HELPLINK_URL);
	buffer.WriteInteger(CLDSLineFlat::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSLineFlat::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSLineFlat::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSLineFlat::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSVector
	buffer.WriteInteger(0x010000);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Parameter Direction");
#else 
	buffer.WriteString("参数化方向");
#endif
	buffer.WriteString(CLDSVector::HELPLINK_URL);
	buffer.WriteInteger(CLDSVector::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSVector::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSVector::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSVector::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSPoint
	buffer.WriteInteger(CLS_GEPOINT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Parameter Position");
#else 
	buffer.WriteString("参数化点位");
#endif
	buffer.WriteString(CLDSPoint::HELPLINK_URL);
	buffer.WriteInteger(CLDSPoint::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CLDSPoint::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSPoint::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSPoint::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CElemInfo
	buffer.WriteInteger(CLS_FEMELEMENT);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Finite Elment");
#else 
	buffer.WriteString("有限单元");
#endif
	buffer.WriteString(CElemInfo::HELPLINK_URL);
	buffer.WriteInteger(CElemInfo::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CElemInfo::propHashtable.GetFirst();pPropItem;
		pPropItem=CElemInfo::propHashtable.GetNext())
	{
		buffer.WriteString(CElemInfo::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CBlockModel
	buffer.WriteInteger(CLS_BLOCK);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Assemble Block");
#else 
	buffer.WriteString("装配部件");
#endif
	buffer.WriteString(CBlockModel::HELPLINK_URL);
	buffer.WriteInteger(CBlockModel::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem =CBlockModel::propHashtable.GetFirst();pPropItem;
		pPropItem=CBlockModel::propHashtable.GetNext())
	{
		buffer.WriteString(CBlockModel::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CBlockReference
	buffer.WriteInteger(CLS_BLOCKREF);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Assemble Object");
#else 
	buffer.WriteString("装配对象");
#endif
	buffer.WriteString(CBlockReference::HELPLINK_URL);
	buffer.WriteInteger(CBlockReference::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=CBlockReference::propHashtable.GetFirst();pPropItem;
		pPropItem=CBlockReference::propHashtable.GetNext())
	{
		buffer.WriteString(CBlockReference::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//PROFILE_VERTEX
	buffer.WriteInteger(CLS_PROFILEVERTEX);	//0x010003
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Plate Outline Vertex");
#else 
	buffer.WriteString("钢板外形顶点");
#endif
	buffer.WriteString(PROFILE_VERTEX::HELPLINK_URL);
	buffer.WriteInteger(PROFILE_VERTEX::propHashtable.GetNodeNum());
	for(PROPLIST_ITEM *pPropItem=PROFILE_VERTEX::propHashtable.GetFirst();pPropItem;
		pPropItem=PROFILE_VERTEX::propHashtable.GetNext())
	{
		buffer.WriteString(PROFILE_VERTEX::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSApp
	buffer.WriteInteger(0x010001);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("System Parameter");
#else 
	buffer.WriteString("系统参数");
#endif
	buffer.WriteString(CLDSApp::HELPLINK_URL);
	buffer.WriteInteger(CLDSApp::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	buffer.WriteInteger(0x010007);	//绘图模块系统参数
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("CAD System Parameter");
#else 
	buffer.WriteString("绘图模块系统参数");
#endif
	buffer.WriteString(CSystemSettingDlg::HELPLINK_URL);
	buffer.WriteInteger(CSystemSettingDlg::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CSystemSettingDlg::propHashtable.GetFirst();pPropItem;
		pPropItem=CSystemSettingDlg::propHashtable.GetNext())
	{
		buffer.WriteString(CSystemSettingDlg::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//CLDSView
	buffer.WriteInteger(0x010002);
#ifdef AFX_TARG_ENU_ENGLISH
	buffer.WriteString("Work View");
#else 
	buffer.WriteString("工作视图");
#endif
	buffer.WriteString(CLDSView::HELPLINK_URL);
	buffer.WriteInteger(CLDSView::propHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSView::propHashtable.GetFirst();pPropItem;
		pPropItem=CLDSView::propHashtable.GetNext())
	{
		buffer.WriteString(CLDSView::propHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//菜单（含主菜单、主环境右键菜单、构件编辑环境右键菜单及各工作区右键菜单）
	buffer.WriteInteger(0x010003);
	buffer.WriteString("菜单");
	buffer.WriteString(CLDSView::HELPLINK_URL);
	buffer.WriteInteger(CLDSApp::propMenuHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propMenuHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propMenuHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propMenuHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//对话框(含窗口)
	buffer.WriteInteger(0x010004);
	buffer.WriteString("主环境对话框");
	buffer.WriteString(CLDSView::HELPLINK_URL);
	buffer.WriteInteger(CLDSApp::propDialogHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propDialogHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propDialogHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propDialogHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//对话框(含窗口)
	buffer.WriteInteger(0x010005);
	buffer.WriteString("绘图环境对话框");
	buffer.WriteString(CLDSView::HELPLINK_URL);
	buffer.WriteInteger(CLDSApp::propCadDialogHashtable.GetNodeNum());
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propCadDialogHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propCadDialogHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propCadDialogHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);
		buffer.WriteString(pPropItem->Url);
	}
	//含子级分组的帮助
	buffer.WriteInteger(0x010006);
	buffer.WriteInteger(CLDSApp::propSubGroupHelpHashtable.GetNodeNum());//nGroup分组数
	for (PROPLIST_ITEM *pPropItem=CLDSApp::propSubGroupHelpHashtable.GetFirst();pPropItem;
		pPropItem=CLDSApp::propSubGroupHelpHashtable.GetNext())
	{
		buffer.WriteString(CLDSApp::propSubGroupHelpHashtable.GetCursorKey());
		buffer.WriteString(pPropItem->name);	//分组名称
		CHashStrList<PROPLIST_ITEM>* pHelpHashtable=CLDSApp::GetSubGroupHelpPropHashtable(pPropItem->id);
		buffer.WriteInteger(pPropItem->id);
		buffer.WriteString(pPropItem->Url);
		buffer.WriteInteger(pHelpHashtable->GetNodeNum());
		for (PROPLIST_ITEM *pSubPropItem=pHelpHashtable->GetFirst();pSubPropItem;pSubPropItem=pHelpHashtable->GetNext())
		{
			buffer.WriteInteger(pSubPropItem->id);
			buffer.WriteString(pHelpHashtable->GetCursorKey());
			buffer.WriteString(pSubPropItem->name);
			buffer.WriteString(pSubPropItem->Url);
		}
	}
	//buffer.WriteInteger(0x010007);	//绘图模块系统参数
}
void ExportPropListMapFile()
{
	FILE* fp=fopen("c:\\lds.hmp","wb");
	if(fp==NULL)
		return;
	CBuffer buffer;
	ExportPropListHelpInfo(buffer);
	long version=1;
	long buf_size=buffer.GetLength();
	fwrite(&version,4,1,fp);
	fwrite(&buf_size,4,1,fp);	//后续缓存长度
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
void ImportPropListHelpInfo(CBuffer& buffer)
{
	//读入文件信息
	//更新PROPLIST_ITEM::url
	long cls_id;
	CXhChar100 cls_name ,propKeyStr,propName,url,propUrl,defaultUrl,*pDefaultUrl=NULL;
	int nItem;
	try{
		do{
			buffer.ReadInteger(&cls_id);
			CHashStrList<PROPLIST_ITEM>* pPropHashList=NULL;
			CHashStrList<PROPLIST_ITEM> defaultHashList;
			if(cls_id==CLS_LINEANGLE)
			{
				pDefaultUrl=&CLDSLineAngle::HELPLINK_URL;
				pPropHashList=&CLDSLineAngle::propHashtable;
			}
			else if(cls_id==CLS_DBOBJECT)
			{
				pDefaultUrl=&CLDSDbObject::HELPLINK_URL;
				pPropHashList=&CLDSDbObject::propHashtable;
			}
			// 下边这一个有错
			else if(cls_id==CLS_PART)
			{
				pDefaultUrl=&CLDSPart::HELPLINK_URL;
				pPropHashList=&CLDSPart::propHashtable;
			}
			else if(cls_id==CLS_SPHERE)
			{
				pDefaultUrl =&CLDSSphere::HELPLINK_URL;
				pPropHashList=&CLDSSphere::propHashtable;
			}
			else if(cls_id==CLS_BOLT)
			{
				pDefaultUrl=&CLDSBolt::HELPLINK_URL;
				pPropHashList=&CLDSBolt::propHashtable;
			}
			else if(cls_id==CLS_NODE)
			{
				pDefaultUrl=&CLDSNode::HELPLINK_URL;
				pPropHashList=&CLDSNode::propHashtable;
			}
			else if(cls_id==CLS_GEPOINT)
			{
				pDefaultUrl=&CLDSPoint::HELPLINK_URL;
				pPropHashList=&CLDSPoint::propHashtable;
			}
			else if(cls_id==0x010000)
			{
				pPropHashList=&CLDSVector::propHashtable;
				pDefaultUrl =&CLDSVector::HELPLINK_URL;
			}
			else if(cls_id==CLS_GROUPLINEANGLE)
			{
				pPropHashList=&CLDSGroupLineAngle::propHashtable;
				pDefaultUrl =&CLDSGroupLineAngle::HELPLINK_URL;
			}
			else if(cls_id==CLS_FEMELEMENT)
			{
				pPropHashList=&CElemInfo::propHashtable;
				pDefaultUrl =&CElemInfo::HELPLINK_URL;
			}
			else if(cls_id==CLS_LINEPART)
			{
				pPropHashList=&CLDSLinePart::propHashtable;
				pDefaultUrl=&CLDSLinePart::HELPLINK_URL;
			}
			else if(cls_id==CLS_ARCPART)
			{
				pPropHashList=&CLDSArcPart::propHashtable;
				pDefaultUrl=&CLDSArcPart::HELPLINK_URL;
			}
			else if(cls_id==CLS_ARCANGLE)
			{
				pPropHashList=&CLDSArcAngle::propHashtable;
				pDefaultUrl =&CLDSArcAngle::HELPLINK_URL;
			}
			else if(cls_id==CLS_ARCFLAT)
			{
				pPropHashList=&CLDSArcFlat::propHashtable;
				pDefaultUrl=&CLDSArcFlat::HELPLINK_URL;
			}
			else if(cls_id==CLS_ARCSLOT)
			{
				pPropHashList=&CLDSArcSlot::propHashtable;
				pDefaultUrl = &CLDSArcSlot::HELPLINK_URL;
			}
			else if(cls_id==CLS_PARAMPLATE)
			{
				pPropHashList=&CLDSParamPlate::propHashtable;
				pDefaultUrl =&CLDSParamPlate::HELPLINK_URL;
			}
			else if(cls_id==CLS_PLATE)
			{
				pDefaultUrl=&CLDSPlate::HELPLINK_URL;
				pPropHashList=&CLDSPlate::propHashtable;
			}
			else if(cls_id==CLS_LINETUBE)
			{
				pPropHashList=&CLDSLineTube::propHashtable;
				pDefaultUrl=&CLDSLineTube::HELPLINK_URL;
			}
			else if(cls_id==CLS_LINESLOT)
			{
				pPropHashList=&CLDSLineSlot::propHashtable;
				pDefaultUrl=&CLDSLineSlot::HELPLINK_URL;
			}
			else if(cls_id==CLS_LINEFLAT)
			{
				pPropHashList=&CLDSLineFlat::propHashtable;
				pDefaultUrl=&CLDSLineFlat::HELPLINK_URL;
			}
			else if(cls_id==CLS_BLOCK)
			{
				pPropHashList=&CBlockModel::propHashtable;
				pDefaultUrl =&CBlockModel::HELPLINK_URL;
			}
			else if(cls_id==CLS_BLOCKREF)
			{
				pPropHashList=&CBlockReference::propHashtable;
				pDefaultUrl =&CBlockReference::HELPLINK_URL;
			}
			else if(cls_id==0x010001)
			{
				pPropHashList=&CLDSApp::propHashtable;
				pDefaultUrl =&CLDSApp::HELPLINK_URL;
			}
			else if(cls_id==0x010007)
			{	//绘图模块系统参数
				pPropHashList=&CSystemSettingDlg::propHashtable;
				pDefaultUrl=&CSystemSettingDlg::HELPLINK_URL;
			}
			else if(cls_id==0x010002)
			{
				pPropHashList=&CLDSView::propHashtable;
				pDefaultUrl =&CLDSView::HELPLINK_URL;
			}
			else if(cls_id==CLS_PROFILEVERTEX)
			{
				pPropHashList=&PROFILE_VERTEX::propHashtable;
				pDefaultUrl =&PROFILE_VERTEX::HELPLINK_URL;
			}
			else if(cls_id==0x010003)
			{
				pPropHashList=&CLDSApp::propMenuHashtable;
				pDefaultUrl =NULL;
			}
			else if(cls_id==0x010004)
			{	//主环境对话框
				pPropHashList=&CLDSApp::propDialogHashtable;
				pDefaultUrl =NULL;
			}
			else if(cls_id==0x010005)
			{	//绘图环境对话框
				pPropHashList=&CLDSApp::propCadDialogHashtable;
				pDefaultUrl =NULL;
			}
			else if(cls_id==0x010006)
			{	//含子级分组的帮助
				int nGroup=0;
				buffer.ReadInteger(&nGroup);//nGroup分组数
				for(int i=0;i<nGroup;i++)
				{
					CXhChar100 sGroupName;
					buffer.ReadString(cls_name,101);
					buffer.ReadString(sGroupName,101);	//分组名称
					long idGroup=0,nItemCount=0;
					buffer.ReadInteger(&idGroup);
					buffer.ReadString(defaultUrl);
					CHashStrList<PROPLIST_ITEM>* pHelpHashtable=CLDSApp::GetSubGroupHelpPropHashtable(idGroup);
					buffer.ReadInteger(&nItemCount);
					for(int j=0;j<nItemCount;j++)
					{
						long id=0;
						buffer.ReadInteger(&id);
						buffer.ReadString(propKeyStr,101);
						buffer.ReadString(propName,101);
						buffer.ReadString(url,101);
						PROPLIST_ITEM *pItem=pHelpHashtable->GetValue(propKeyStr);
						if(pItem)
							pItem->Url.Copy(url);
					}
				}
				continue;
			}
			else
			{
				pPropHashList=&defaultHashList;
				pDefaultUrl=NULL;
			}
			buffer.ReadString(cls_name,101);
			if(pDefaultUrl)
				buffer.ReadString(*pDefaultUrl,101);
			else
				buffer.ReadString(defaultUrl,101);
			buffer.ReadInteger(&nItem);
			for (int i =0;i<nItem;i++)
			{
				buffer.ReadString(propKeyStr,101);
				buffer.ReadString(propName,101);
				buffer.ReadString(url,101);
				PROPLIST_ITEM *pItem=pPropHashList->GetValue(propKeyStr);
				if(pItem)
					pItem->Url.Copy(url);
				//else
				//	logerr.Log("")
			}
		}while(buffer.GetRemnantSize()>0);
	}
	catch(char* error)
	{
		AfxMessageBox(error);
	}
}
void ImportPropHelpListMapFile(char* hmp_file)
{
	FILE* fp=fopen(hmp_file,"rb");
	if(fp==NULL)
		return;
	long version=1;
	long buf_size=0;
	fread(&version,4,1,fp);
	fread(&buf_size,4,1,fp);	//后续缓存长度
	CBuffer buffer;
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	buffer.SeekToBegin();
	ImportPropListHelpInfo(buffer);
}

BOOL CLDSApp::OnIdle(LONG lCount)
{
	if(intelli.Remind())
		intelli.Learn();

	return CWinAppEx::OnIdle(lCount);
}
