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
void GetSysPath(char* App_Path,const char *src_path=NULL,char *file_name=NULL,char *extension=NULL);//���ϵͳ����·����App_PathӦ�ں����⿪�ٺ��ڴ�
void DisplayProcess(int percent,const char *sTitle=NULL);
void DisplayPrompt(char *sTitle=NULL);
BOOL IsCanceledPromptDlg();
#endif
enum WORKENV_TYPE{
	LINE_ENV,		//���߽�ģ����
	SOLID_ENV,		//ʵ�彨ģ����
	PARTEDITOR_ENV,	//�����༭����
	FEMROD_ENV,		//��ѧ��������
	FEMELEM_ENV,	//��ԪУ����ƻ���
	DRAWING2D_ENV,	//��άƽ��ͼ����
};
enum WORKUI_TYPE
{
	DEFAULT_MODELING_UI,	//Ĭ�Ͻ�ģUI
	PRECISE_MODELING_UI,	//��ϸ��ģUI
	RAPID_MODELING_UI,		//���ٽ�ģUI
	DRAWING_MODELING_UI,	//��ģ��ͼUI
	ELEMENT_CAL_UI,			//��ѧ����UI
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
	BOOL m_bNetModelWork;			//��������ʽ����
#endif
public:
	char work_path[MAX_PATH];
	char execute_path[MAX_PATH];//��ȡִ���ļ�������Ŀ¼
	char m_sLicFile[MAX_PATH];
	UINT m_nMainMenuID;
	HINSTANCE m_hResHandle;
	HANDLE m_hPipeServerListen,m_hPipeServerSend;
	DWORD m_dwUpdateWord;			//�������û������б�
	UINT m_uAnimateTimer;
	double m_fMaxAmplitude;			//������ֵ
	int m_nSampleTimerInterval;		//����ģ��������ʾʱȡ�������ms��
	int m_nSimulateVibraModePeriod;	//����ģ��������ʾʱ������ֵ��ms��
	long m_nUserId;					//�û���ʶ��
	bool m_bStartAsFemResolver;		//��Ϊ��̨����Ԫ���������������
	BOOL m_bCooperativeWork;		//������Эͬ�ķ�ʽ����
	WORKENV_TYPE m_eEnvType;		//��ǰ������������
	WORKENV_TYPE m_ePrevEnvType;	//��һ������������
	WORKUI_TYPE m_eUiType;			//��ǰ����UI
	WORKUI_TYPE m_ePreUiType;		//��һ����UI
	BOOL m_bAnalysisWorkEnv;		//����ѧ������ʽ����
	BOOL m_bAnalysisWorkEnvOnly;	//������ѧ������ʽ����
	BOOL m_bDrawingWorkEnvOnly;		//���Ի�ͼģʽ����
	BOOL m_bManuRevisionMode;		//��������У��ģʽ
	CString m_sWorkgroupServer;		//�����������������
	CString m_sUserName;			//�û���¼�����������ʱ���û���
	CString m_strRegSection_LXJS;	//��ѧ��������Զ����ص�ע���λ��
	CString m_strRegSection_KSJM;	//���ٽ�ģ�����Զ����ص�ע���λ��
	CString m_strRegSection_JMHT;	//��ģ��ͼ�����Զ����ص�ע���λ��
	CString m_strRegSection_JMFY;	//��ģ���������Զ����ص�ע���λ��
	ATOM_LIST<MSG_STRU> m_arrMsg;	//�����û���������Ϣ
	BOOL m_bLoadLastFile;
	DWORD m_version[2];				//�汾��Ϣ
	const char* get_WorkPath();
	const char* get_ExecPath();
	__declspec( property(get=get_WorkPath)) const char* szWorkPath;
	__declspec( property(get=get_ExecPath)) const char* szExecPath;
	struct ENV_CONFIG
	{
		//BOOL m_bDisplayCoordAxis;	//��ʾ����ϵ
		BOOL m_bDisplayLineVecMark;	//��ʾ�˼���ʼ����
		BOOL m_bDisplayWorldUcs;	//����������ʾ��׼(����)����ϵ(wcs)
		CString m_sUserName;		//�û���
		CString m_sShortUserName;	//�û�����д
		CString m_sAcadPath;		//AcadĿ¼
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
		CString m_sCInsertPlateLibPath;	//���Ͳ����ļ�·��
		CString m_sUInsertPlateLibPath;	//U�Ͳ����ļ�·��
		CString m_sXInsertPlateLibPath;	//ʮ���Ͳ����ļ�·��
		CString m_sStdPartFlDLibPath;	//�Ժ����������ļ�·��
		CString m_sStdPartFlPLibPath;	//ƽ�����������ļ�·��
		CString m_sStdPartFlGLibPath;   //���Է��������ļ�·��
		CString m_sStdPartFlRlibPath;   //���Է��������ļ�·��
		CString m_sFlCoupleLevelPath;	//�����ԽӼ������ļ�·��
		CString m_sWorkDir;				//�������Ŀ¼
		CString m_sJgNcDrvFile;
		CString m_sPlankNcDrvFile;
		CString m_sPartNoPrefix;
		int m_iNcGroupType;			//
		char layer[4];				//ϵͳ��ǰ�Ĳ���ͼ��
		SEGI  iSeg;					//ϵͳ��ǰ�Ĳ����κ�
		UCS_STRU mcs;				//ģ������ϵ
	}env;
	BOOL m_bNeedUpdatePerm;	//��Ҫ����Ȩ����Ϣ
public:
	void GetFileName(char* file_path,bool hasExtension=false);	//��ȡ��ǰ�ļ���(������չ��),��Ҫ��������ͬ���ĸ����ļ�
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
	//�����Զ���ȡ��ǰ�ļ������������Ŀ¼���������ڳ���򿪺��ڴ�һ�����ļ�ʱ����
	void GetWorkPathAuto();
	//���Թ���
	static long MAX_USER_PROPID;					//�û���̬�������С����IDֵ
	static long MIN_IDLE_PROPID;					//�û�����Сʹ�õĿ�������IDֵ
	DECLARE_PROP_FUNC(CLDSApp);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	CString GetPropValueStr(long id);		//��������ID�õ�����ֵ
	static void InitSyssettingPropHashtable();
	static long InitDesignParamsToSyssettingPropHashtable(long iInitId);
	//�˵������������
	static CHashStrList<PROPLIST_ITEM>propMenuHashtable;	//�����ַ�����ϣ��
	static CHashSet<PROPLIST_ITEM*>hashMenuHelpItemById;
	static void InitMenuHelpHashtable();
	static PROPLIST_ITEM* AddMenuHelpItem(char* keyStr,PROPLIST_ITEM item);
	//�Ի�������������
	static CHashStrList<PROPLIST_ITEM>propDialogHashtable;	//�����ַ�����ϣ��
	static CHashSet<PROPLIST_ITEM*>hashDialogHelpItemById;
	static void InitDialogHelpHashtable();
	static PROPLIST_ITEM* AddDialogHelpItem(char* keyStr,PROPLIST_ITEM item);
	//��ͼģ��Ի�������������
	static CHashStrList<PROPLIST_ITEM>propCadDialogHashtable;	//�����ַ�����ϣ��
	static CHashSet<PROPLIST_ITEM*>hashCadDialogHelpItemById;
	static void InitCadDialogHelpHashtable();
	static PROPLIST_ITEM* AddCadDialogHelpItem(char* keyStr,PROPLIST_ITEM item);
	//���Ӽ���������������
	static CHashStrList<PROPLIST_ITEM>propSubGroupHelpHashtable;	//�����ַ�����ϣ��
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
