// LDS.h : main header file for the LDS application
//

#if !defined(AFX_LDS_H__4A736184_DDB5_4FD2_B961_6BD72ABD873D__INCLUDED_)
#define AFX_LDS_H__4A736184_DDB5_4FD2_B961_6BD72ABD873D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "resource.h"       // main symbols
#include "f_ent_list.h"
#include "MsgDef.h"
#include "I_DrawSolid.h"
#include "Tower.h"
#include "DataCompare.h"
/////////////////////////////////////////////////////////////////////////////
// CLDSApp:
// See LDS.cpp for the implementation of this class
//

DWORD GetPermWord(long nUserId);
class CLDSDoc; 
typedef struct tagMSG_STRU
{
	char user[51];
	char msg[1024];
}MSG_STRU;

extern char APP_PATH[MAX_PATH];
#if defined(__TMA_)||defined(__LMA_)||defined(__STATION_)||defined(__TSA_)||defined(__LDS_)||defined(__TDA_)
void GetSysPath(char* App_Path,const char *src_path=NULL,char *file_name=NULL,char *extension=NULL);//获得系统运行路径，App_Path应在函数外开辟好内存
void DisplayProcess(int percent,const char *sTitle=NULL);
void DisplayPrompt(char *sTitle=NULL);
BOOL IsCanceledPromptDlg();
#endif
enum WORKENV_TYPE{
	LINE_ENV,		//单线建模环境
	SOLID_ENV,		//实体建模环境
	PARTEDITOR_ENV,	//构件编辑环境
	FEMROD_ENV,		//力学分析环境
	FEMELEM_ENV,	//单元校审设计环境
	DRAWING2D_ENV,	//二维平面图环境
};
enum WORKUI_TYPE
{
	DEFAULT_MODELING_UI,	//默认建模UI
	PRECISE_MODELING_UI,	//精细建模UI
	RAPID_MODELING_UI,		//快速建模UI
	DRAWING_MODELING_UI,	//建模绘图UI
	ELEMENT_CAL_UI,			//力学计算UI
};
//from LoadLibrary("BomExport.dll")
typedef DWORD (*DefGetSupportBOMType)();
typedef DWORD (*DefGetSupportDataBufferVersion)();
typedef int(*DefCreateExcelBomFile)(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag);
typedef int(*DefRecogniseReport)(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag);
typedef int (*DefGetExcelFormat)(int* colIndexArr,int *startRowIndex);
typedef int (*DefGetExcelFormatEx)(int* colIndexArr,int *startRowIndex,char* titleStr);
class CLDSApp : public CWinAppEx
{
	WORD m_wDogModule;
	HMODULE m_hWireLayoutModule;
	HMODULE m_hBomExport;
public:
	static DefGetSupportBOMType GetSupportBOMType;
	static DefGetSupportDataBufferVersion GetSupportDataBufferVersion;
	static DefCreateExcelBomFile CreateExcelBomFile;
	static DefRecogniseReport RecogniseReport;
	static DefGetExcelFormat GetBomExcelFormat;
	static DefGetExcelFormatEx GetBomExcelFormatEx;
#ifdef __CONNECT_REMOTE_SERVER_
	BOOL m_bNetModelWork;			//以连网方式工作
#endif
public:
	char work_path[MAX_PATH];
	char execute_path[MAX_PATH];//获取执行文件的启动目录
	char m_sLicFile[MAX_PATH];
	UINT m_nMainMenuID;
	HINSTANCE m_hResHandle;
	HANDLE m_hPipeServerListen,m_hPipeServerSend;
	DWORD m_dwUpdateWord;			//待更新用户数据列表
	UINT m_uAnimateTimer;
	double m_fMaxAmplitude;			//最大振幅值
	int m_nSampleTimerInterval;		//动画模拟振型显示时取样间隔（ms）
	int m_nSimulateVibraModePeriod;	//动画模拟振型显示时的周期值（ms）
	long m_nUserId;					//用户标识号
	bool m_bStartAsFemResolver;		//作为后台有限元计算程序启动运行
	BOOL m_bCooperativeWork;		//以网络协同的方式工作
	WORKENV_TYPE m_eEnvType;		//当前工作环境类型
	WORKENV_TYPE m_ePrevEnvType;	//上一工作环境类型
	WORKUI_TYPE m_eUiType;			//当前操作UI
	WORKUI_TYPE m_ePreUiType;		//上一操作UI
	BOOL m_bAnalysisWorkEnv;		//以力学分析方式工作
	BOOL m_bAnalysisWorkEnvOnly;	//仅以力学分析方式工作
	BOOL m_bDrawingWorkEnvOnly;		//仅以绘图模式工作
	BOOL m_bManuRevisionMode;		//生产放样校审模式
	CString m_sWorkgroupServer;		//工作组服务器主机名
	CString m_sUserName;			//用户登录工作组服务器时的用户名
	CString m_strRegSection_LXJS;	//力学计算界面自动加载的注册表位置
	CString m_strRegSection_KSJM;	//快速建模界面自动加载的注册表位置
	CString m_strRegSection_JMHT;	//建模绘图界面自动加载的注册表位置
	CString m_strRegSection_JMFY;	//建模放样界面自动加载的注册表位置
	ATOM_LIST<MSG_STRU> m_arrMsg;	//其它用户发来的信息
	BOOL m_bLoadLastFile;
	DWORD m_version[2];				//版本信息
	const char* get_WorkPath();
	const char* get_ExecPath();
	__declspec( property(get=get_WorkPath)) const char* szWorkPath;
	__declspec( property(get=get_ExecPath)) const char* szExecPath;
	struct ENV_CONFIG
	{
		//BOOL m_bDisplayCoordAxis;	//显示坐标系
		BOOL m_bDisplayLineVecMark;	//显示杆件起始方向
		BOOL m_bDisplayWorldUcs;	//工作区中显示基准(世界)坐标系(wcs)
		CString m_sUserName;		//用户名
		CString m_sShortUserName;	//用户名缩写
		CString m_sAcadPath;		//Acad目录
		CString m_sLsGuiGeTblPath;
		CString m_sJgGuiGeTblPath;
		CString m_sFlatGuiGeTblPath;
		CString m_sSlotGuiGeTblPath;
		CString m_sJgJointParaTblPath;
		CString m_sJgZhunJuTblPath;
		CString m_sTubeGuiGeTblPath;
		CString m_sHWGuiGeTblPath;
		CString m_sHMGuiGeTblPath;
		CString m_sHNGuiGeTblPath;
		CString m_sHPGuiGeTblPath;
		CString m_sLsSpaceTblPath;
		CString m_sCInsertPlateLibPath;	//槽型插板库文件路径
		CString m_sUInsertPlateLibPath;	//U型插板库文件路径
		CString m_sXInsertPlateLibPath;	//十字型插板库文件路径
		CString m_sStdPartFlDLibPath;	//对焊法兰规格库文件路径
		CString m_sStdPartFlPLibPath;	//平焊法兰规格库文件路径
		CString m_sStdPartFlGLibPath;   //刚性法兰规格库文件路径
		CString m_sStdPartFlRlibPath;   //柔性法兰规格库文件路径
		CString m_sFlCoupleLevelPath;	//法兰对接级别定义文件路径
		CString m_sWorkDir;				//工作输出目录
		CString m_sJgNcDrvFile;
		CString m_sPlankNcDrvFile;
		CString m_sPartNoPrefix;
		int m_iNcGroupType;			//
		char layer[4];				//系统当前的操作图层
		SEGI  iSeg;					//系统当前的操作段号
		UCS_STRU mcs;				//模型坐标系
	}env;
	BOOL m_bNeedUpdatePerm;	//需要更新权限信息
public:
	void GetFileName(char* file_path,bool hasExtension=false);	//获取当前文件名(不含扩展名),主要用于生成同名的辅助文件
	CLDSApp();
	void GetUserInfo(BRIEF_USER user_arr[32]);
	BOOL m_bDisplayAllPart;
	BOOL IsHasModifyPerm(DWORD dwPerm);
	DWORD GetPermDword();
	void SendMsgToOther(CString &receiver,CString &msg);
	void ProcessInfoPackage();
	BOOL IsAnalysisWorkEnv();
	void GetAppVersion(char *file_version,char *product_version);
	void GetFileVersion(CString &file_version);
	void GetProductVersion(CString &product_version);
	void GetProductVersion(char byteVer[4]);
	void GetVersionUpdateNotes(bool bOnlyOnHasNewNotes=true);
	void InitTowerModel();
	BOOL LogonServer();
	BOOL LogonTMSServer();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CLDSDoc* GetLDSDoc();
	//用于自动获取当前文件计算结果的输出目录，必须是在程序打开后在打开一个新文件时调用
	void GetWorkPathAuto();
	//属性管理
	static long MAX_USER_PROPID;					//用户动态定义的最小属性ID值
	static long MIN_IDLE_PROPID;					//用户可最小使用的空闲属性ID值
	DECLARE_PROP_FUNC(CLDSApp);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	CString GetPropValueStr(long id);		//根据属性ID得到属性值
	static void InitSyssettingPropHashtable();
	static long InitDesignParamsToSyssettingPropHashtable(long iInitId);
	//菜单帮助项导出管理
	static CHashStrList<PROPLIST_ITEM>propMenuHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashMenuHelpItemById;
	static void InitMenuHelpHashtable();
	static PROPLIST_ITEM* AddMenuHelpItem(char* keyStr,PROPLIST_ITEM item);
	//对话框帮助项导出管理
	static CHashStrList<PROPLIST_ITEM>propDialogHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashDialogHelpItemById;
	static void InitDialogHelpHashtable();
	static PROPLIST_ITEM* AddDialogHelpItem(char* keyStr,PROPLIST_ITEM item);
	//绘图模块对话框帮助项导出管理
	static CHashStrList<PROPLIST_ITEM>propCadDialogHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashCadDialogHelpItemById;
	static void InitCadDialogHelpHashtable();
	static PROPLIST_ITEM* AddCadDialogHelpItem(char* keyStr,PROPLIST_ITEM item);
	//含子级分组帮助项导出管理
	static CHashStrList<PROPLIST_ITEM>propSubGroupHelpHashtable;	//属性字符串哈希表
	static CHashSet<PROPLIST_ITEM*>hashSubGroupHlpHelpItemById;
	static void InitSubGroupHelpHashtable();
	static PROPLIST_ITEM* AddSubGroupHelpItem(char* keyStr,PROPLIST_ITEM item);
	static CHashStrList<PROPLIST_ITEM>* GetSubGroupHelpPropHashtable(long idSubGroup);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDSApp)
	public:
	BOOL ImportLicense();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CLDSApp)
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	virtual BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
	afx_msg void OnUpdateAppAbout(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};
extern CLDSApp theApp;
extern CTowerArchiveLibrary g_orgPartLib;
PROPLIST_ITEM* AddUniversalPropItem(char* propKey,PROPLIST_ITEM& item,CHashStrList<PROPLIST_ITEM>* pPropHashtable,CHashSet<PROPLIST_ITEM*>* pHashPropById);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LDS_H__4A736184_DDB5_4FD2_B961_6BD72ABD873D__INCLUDED_)
