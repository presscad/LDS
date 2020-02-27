//<LOCALE_TRANSLATE/>
// MapLds.cpp : Initialization functions
#include "StdAfx.h"
#include "StdArx.h"
#include "resource.h"
#include <afxdllx.h>
#include "MenuFunc.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "database.h"
#include "RxTools.h"
#include "dbsymtb.h"
#include "aced.h"
#include "dbents.h"
#include "GSMH.h"
#include "NetDogAPI.h"
#include "ProcBarDlg.h"
#include "LibraryQuery.h"
#include "Console.h"
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"
#include "SystemSettingDlg.h"
#include "BPSModel.h"
#include "ClientId.h"
#include "MsgBox.h"

int g_nCurMapSegNo;		//当前分段图的分段号
int g_nCurBodyNo;		//当前总图的本体号
double g_fCurMapScale = 1.0;	//绘图塔块中的当前绘图比例
HMODULE m_hWireLayoutModule=NULL;
char g_sHelpFilePath[MAX_PATH];	//帮助文件路径
// NOTE: DO NOT edit the following lines.
//{{AFX_ARX_MSG
bool InitApplication();
void UnloadApplication();
//}}AFX_ARX_MSG

// NOTE: DO NOT edit the following lines.
//{{AFX_ARX_ADDIN_FUNCS
//}}AFX_ARX_ADDIN_FUNCS

////////////////////////////////////////////////////////////////////////////
//
// Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(MapTdaDLL);

// Now you can use the CAcModuleRecourceOverride class in
// your application to switch to the correct resource instance.
// Please see the ObjectARX Documentation for more details

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
#include "acui.h"
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// Extension DLL one time initialization
		MapTdaDLL.AttachInstance(hInstance);
		InitAcUiDLL();
	} else if (dwReason == DLL_PROCESS_DETACH) {
		// Terminate the library before destructors are called
		MapTdaDLL.DetachInstance();
	}
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// ObjectARX EntryPoint
extern "C" AcRx::AppRetCode 
#ifdef _MAPTMA_ZRX
zcrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
#else
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
#endif
{
	switch (msg) {
	case AcRx::kInitAppMsg:
		// Comment out the following line if your
		// application should be locked into memory
		acrxDynamicLinker->unlockApplication(pkt);
		acrxDynamicLinker->registerAppMDIAware(pkt);
		if (!InitApplication())
		{
			UnloadApplication();
			return AcRx::kRetError;
		}
		break;
	case AcRx::kUnloadAppMsg:
		UnloadApplication();
		break;
	case AcRx::kLoadDwgMsg:
#ifndef __BP_ONLY_
		InitDrawingEnv();
#endif
		break;
	}
	return AcRx::kRetOK;
}


// Init this application. Register your
// commands, reactors...
//from summap.cpp
extern int g_nCurMapSegNo;
//CTower Ta;
//from config.cpp
void SysSetImportDefault(BOOL bPrompt);
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
		pProcDlg->SetTitle("progress");
#else
		pProcDlg->SetTitle("进度");
#endif
	pProcDlg->Refresh(percent);
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
			if(cls_id==0x010007)
			{	//绘图模块系统参数
				pPropHashList=&CSystemSettingDlg::propHashtable;
				pDefaultUrl=&CSystemSettingDlg::HELPLINK_URL;
			}
			/*else if(cls_id==0x010005)
			{	//绘图环境对话框
				pPropHashList=&CLDSApp::propCadDialogHashtable;
				pDefaultUrl =NULL;
			}*/
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
					CHashStrList<PROPLIST_ITEM>* pHelpHashtable=&defaultHashList;
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

void ImportPropHelpListMapFile(const char* hmp_file)
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

WORD m_wDogModule=0;
CXhChar200 g_sLdsExecutePath;
CXhChar200 g_sLicFile;
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
#include ".\LdsDrwLocale.h"
void InitCustomizeConfiguration(UINT uiCustomizeSerial)
{
	//uiCustomizeSerial=CLIENT_QingDao_DongFang;	//TODO:测试东方组焊图临时代码
	gxDrwLocalizer.InitCustomerSerial(uiCustomizeSerial);
}

#ifdef __PNC_FUNC__
#include "PNCCmd.h"
void InitPNCCmd ()
{	
#ifdef _ARX_2007
	// 智能提取板信息
	acedRegCmds->addCommand(L"PNC-MENU",           // Group name
		L"SmartExtractPlate",             // Global function name
		L"SmartExtractPlate",             // Local function name
		ACRX_CMD_MODAL,   // Type
		&SmartExtractPlate);            // Function pointer
	//改正钢板信息
	acedRegCmds->addCommand(L"PNC-MENU",           // Group name
		L"ReviseThePlate",        // Global function name
		L"ReviseThePlate",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&ReviseThePlate);            // Function pointer
	//编辑钢板信息
	acedRegCmds->addCommand(L"PNC-MENU",           // Group name
		L"SendPartEdit",        // Global function name
		L"SendPartEdit",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&SendPartEditor);            // Function pointer
	//钢板布局
	acedRegCmds->addCommand(L"PNC-MENU",         // Group name 
		L"LayoutPlates",          // Global function name
		L"LayoutPlates",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LayoutPlates);
	//提取钢板并布局
	acedRegCmds->addCommand(L"PNC-MENU",           // Group name
		L"ExtractAndLayoutPlates",        // Global function name
		L"ExtractAndLayoutPlates",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&ExtractAndLayoutPlates);   // Function pointer
	//系统设置
	/*acedRegCmds->addCommand( L"PNC-MENU",         // Group name 
		L"PNCEnvGeneralSet",          // Global function name
		L"PNCEnvGeneralSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PNCEnvGeneralSet);*/
#else
	// 智能提取板信息
	acedRegCmds->addCommand( "PNC-MENU",           // Group name
		"SmartExtractPlate",        // Global function name
		"SmartExtractPlate",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&SmartExtractPlate);            // Function pointer
	//修正特定钢板信息
	acedRegCmds->addCommand( "PNC-MENU",           // Group name
		"ReviseThePlate",        // Global function name
		"ReviseThePlate",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&ReviseThePlate);            // Function pointer
	//编辑钢板信息
	acedRegCmds->addCommand( "PNC-MENU",           // Group name
		"SendPartEdit",        // Global function name
		"SendPartEdit",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&SendPartEditor);            // Function pointer
	//钢板布局
	acedRegCmds->addCommand( "PNC-MENU",         // Group name 
		"LayoutPlates",          // Global function name
		"LayoutPlates",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LayoutPlates);
	//提取钢板并布局
	acedRegCmds->addCommand( "PNC-MENU",           // Group name
		"ExtractAndLayoutPlates",        // Global function name
		"ExtractAndLayoutPlates",        // Local function name
		ACRX_CMD_MODAL,   // Type
		&ExtractAndLayoutPlates);   // Function pointer
	//系统设置
	/*acedRegCmds->addCommand( "PNC-MENU",         // Group name 
		"PNCEnvGeneralSet",          // Global function name
		"PNCEnvGeneralSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PNCEnvGeneralSet);	*/
#endif
}
#endif
bool VerifyDllVersionCompatible()
{
	DWORD dwXeroCadMajorVersion=XeroCadDllGetVersion()&0xffffff00;
	//DWORD dwDrawSolidMajorVersion=CDrawSolidFactory::DrawSolidDllGetVersion()&0xffffff00;
	if(dwXeroCadMajorVersion!=0x01000200)
	{
		AfxMessageBox("XeroCad.dll库版本与当前主程序版本不匹配，存在兼容性问题!");
		return false;
	}
	//if(dwDrawSolidMajorVersion!=0x01000000)
	//{
	//	AfxMessageBox("DrawSolid.dll库版本与当前主程序版本不匹配，存在兼容性问题!");
	//	return false;
	//}
	return true;
}
const static BYTE LIC_MODE_CHOICE	= 0;	//可选
const static BYTE LIC_MODE_CLIENT	= 1;	//客户端模式
const static BYTE LIC_MODE_SERVER	= 2;	//服务器模式
const static BYTE LIC_SOFT_LICENSE	= 3;	//软件虚拟锁模式
#include "ModuleRegApp.h"
class ARXMODULE_APP : public MODULEREG_APP{
public:
	ARXMODULE_APP(){
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
		m_szAppName="TMA-EN";
#else
		m_szAppName="TMA";
#endif
#elif defined(__LMA_FILE_)
		m_szAppName="LMA";
#elif defined(__TSA_FILE_)
		m_szAppName="TSA";
#elif defined(__TDA_FILE_)
		m_szAppName="TDA";
#elif defined(__LDS_FILE_)
		m_szAppName="LDS";
#endif
		InitRegistryKey("Xerofox",m_szAppName);
	}
}theApp;
struct LICSERV_MSGBUF
{	//消息头
	long msg_length;//消息长度
	long command_id;//命令标识
	BYTE src_code;	//源节点编号
	BYTE *lpBuffer;			//消息体
public:
	static const long APPLY_FOR_LICENSE = 1;	//申请终端使用授权
	static const long RETURN_LICENSE	= 2;	//返还终端使用授权
	static const long LICENSES_MODIFIED	= 3;	//合法终端用户信息发生变化
};
int InitLicenseModeByLicFile(const char* lic_file)
{
	BYTE m_cLicenseMode=LIC_MODE_CHOICE;
	CString sLicFile;
	if(lic_file==NULL)
		sLicFile=theApp.GetProfileString("Settings","lic_file2");
	else
		sLicFile=lic_file;
	FILE* fp=fopen(sLicFile,"rb");
	if(fp==NULL)
		return m_cLicenseMode;
	DWORD dwLicVersion=0;
	fread(&dwLicVersion,4,1,fp);
	if(dwLicVersion>=1000004)
	{
		BYTE ciDogType=0;
		fread(&ciDogType,1,1,fp);
		if(ciDogType==3&&dwLicVersion>=1000005)
			m_cLicenseMode=LIC_SOFT_LICENSE;
		else
		{
			fseek(fp,2,SEEK_CUR);	//2Byte wModule
			BYTE ciLicenseType=0;
			fread(&ciLicenseType,1,1,fp);
			if(ciLicenseType==1)
				m_cLicenseMode=LIC_MODE_SERVER;
			else if(ciLicenseType==2)	//网络版动态子证书文件
				m_cLicenseMode=LIC_MODE_CLIENT;
		}
	}
	fclose(fp);
	return m_cLicenseMode;
}
#include "TempFile.h"
#include "Communication.h"
int RenewLanLicense(const char* lic_file)	//续借局域网授权
{
	if(InitLicenseModeByLicFile(lic_file)!=LIC_MODE_CLIENT)
		return -1;
	CWaitCursor waitCursor;
	DWORD dwIpAddress=theApp.GetProfileInt("Settings","master_host_ip",0);
	CString masterLicenseHostName=theApp.GetProfileString("Settings","master_host_name","");
	u_short siPortNumber=theApp.GetProfileInt("Settings","master_host_port_number",0);
	CXhChar200 srvname("%s",masterLicenseHostName),sHostIpAddress;
	BYTE* ipbytes=(BYTE*)&dwIpAddress;
	if(srvname.GetLength()==0&&dwIpAddress!=0)
		srvname.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	sHostIpAddress.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	COMPUTER computer;
	CXhChar200 corePipeName("\\\\%s\\Pipe\\XhLicServ\\CorePipe",(char*)srvname);
	CXhChar200 readPipeName("\\\\%s\\Pipe\\XhLicServ\\%s_ListenPipe",(char*)srvname,computer.sComputerName);
	CCommunicationObject commCore(sHostIpAddress,siPortNumber,corePipeName,false);
	CXhChar200 sError=commCore.GetErrorStr();
	if(sError.GetLength()>0)
	{
		//sError.Append(CXhChar100(". 核心通信管道{%s}连接失败!",(char*)corePipeName));
		//AfxMessageBox(sError);
		return -2;
	}
	//CXhChar16 szSubKey="product";
	CXhChar200 errfile=lic_file;//("%s%s.err",theApp.execute_path,(char*)szSubKey);
	char* pchSpliter=SearchChar(errfile,'.',true);
	if(pchSpliter)
		strcpy(pchSpliter,".err");
	else
		errfile.Append(".err");
	CTempFileBuffer error(errfile);
	error.EnableAutoDelete(false);
	//m_pErrLogFile=&error;
	LICSERV_MSGBUF cmdmsg;
	cmdmsg.command_id=LICSERV_MSGBUF::APPLY_FOR_LICENSE;
	cmdmsg.src_code=0;
	CBuffer buffer;
	BYTE identity[32],details[512]={0};
	computer.Generate128Identity(identity,NULL,details,512);
	//COMPUTER::Verify128Identity(identity);
	WORD wDetailsLen=0;
	for(WORD i=0;i<512;i+=16)
	{
		if(((__int64*)(details+i))[0]==0&&((__int64*)(details+i))[1]==0)
			break;
	}
	wDetailsLen=i;
	CXhChar16 ipstr("%d.%d.%d.%d",computer.computer_ip[0],computer.computer_ip[1],computer.computer_ip[2],computer.computer_ip[3]);
	//ReadFile(hCorePipe,msg_contents,msg.msg_length-32,&nBytesRead,NULL);
	//char status,cApplyProductType=0;
	DWORD nBytesRead,nBytesWritten;
	CXhChar50 hostname,hostipstr;
	buffer.WriteString(ipstr,17);
	buffer.WriteString(computer.sComputerName,51);
	buffer.WriteWord(wDetailsLen);
	buffer.Write(details,wDetailsLen);
	buffer.WriteByte(PRODUCT_TYPE);
	buffer.WriteDword((DWORD)time(NULL));
	buffer.Write(identity,32);
	cmdmsg.msg_length=buffer.GetLength();
	error.WriteInteger(buffer.GetLength());
	error.Write(buffer.GetBufferPtr(),buffer.GetLength());
	if(!commCore.Write(&cmdmsg,9,&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	if(!commCore.Write(buffer.GetBufferPtr(),buffer.GetLength(),&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	char state,status=-100;
	DWORD dwRecvBytesLen=0;
	CCommunicationObject *pCommRead=&commCore;
	CCommunicationObject commRead("",0,readPipeName,false);
	if(commCore.IsPipeConnect())
		pCommRead=&commRead;
	if(!(state=pCommRead->Read(&status,1,&nBytesRead))||status<=0)
	{
		if(!pCommRead->IsValidConnection())
		{
			sError=pCommRead->GetErrorStr();
			sError.Append(". 实例通信管道连接失败!");
			AfxMessageBox(sError);
		}
		error.WriteByte(status);
		if(status==0)
			AfxMessageBox("子证书生成失败，请查看授权主服务器下c:\\service.log日志以排除故障");
		else if(status==-1)
			AfxMessageBox("当前主机与授权主服务器时钟存在过大偏差，请校准时钟后再申请授权");
		else if(status==-2)
			AfxMessageBox("主服务器缺少对相应产品的主授权服务文件");
		else if(status==-3)
			AfxMessageBox("主服务器对相应产品的主授权服务子授权数量已满");
		return -2;
	}
	else
		error.WriteByte(status);
	if(!pCommRead->Read(&dwRecvBytesLen,4,&nBytesRead))
	{
		error.WriteInteger(0);
		return -2;
	}
	else
		error.WriteInteger(nBytesRead);
	BYTE_ARRAY licbuf_bytes(dwRecvBytesLen);
	if(!pCommRead->Read(licbuf_bytes,dwRecvBytesLen,&nBytesRead))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	//根据解析数据生成本地证书的注册表部分及子证书文件部分
	//1.写入724个注册表初始播放的种子
	if(!theApp.WriteProfileBinary("",_T("Obfuscator"),(BYTE*)licbuf_bytes,724))
	{
		error.WriteBooleanThin(false);
		error.Write(licbuf_bytes,724);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	CXhChar200 licfile=lic_file;//("%s%s.lic",theApp.execute_path,(char*)szSubKey);
	error.WriteString(licfile);
	FILE* fp=fopen(licfile,"wb");
	if(fp==NULL)
		error.WriteBooleanThin(false);
	else
	{
		error.WriteBooleanThin(true);
		fwrite(licbuf_bytes+724,dwRecvBytesLen-724,1,fp);
		fclose(fp);
	}
	WIN32_FIND_DATA FindFileData;
	if(FindFirstFileA(licfile,&FindFileData)==NULL)
		AfxMessageBox(CXhChar200("'%s'证书文件未找到",(char*)licfile));

	//重新导入证书
	DWORD version[2];
	BYTE* byteVer=(BYTE*)version;
	version[1]=20160808;	//版本发布日期
	BYTE cProductType=PRODUCT_TMA;
#ifdef __TMA_FILE_
#ifdef _LOCAL_ENCRYPT
	cProductType=PRODUCT_CTMA;
#else
	cProductType=PRODUCT_TMA;
#endif
	byteVer[0]=5;
	byteVer[1]=1;
	byteVer[2]=3;
	byteVer[3]=0;
#elif defined(__LMA_FILE_)
	//sprintf(lic_file,"%sLMA.lic",APP_PATH);
	byteVer[0]=2;
	byteVer[1]=0;
	byteVer[2]=6;
	byteVer[3]=0;
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_FILE_)
	//sprintf(lic_file,"%sTSA.lic",APP_PATH);
	byteVer[0]=1;
	byteVer[1]=8;
	byteVer[2]=1;
	byteVer[3]=2;
	cProductType=PRODUCT_TSA;
#elif defined(__LDS_FILE_)
	//sprintf(lic_file,"%sLDS.lic",APP_PATH);
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=6;
	byteVer[3]=0;
	cProductType=PRODUCT_LDS;
#elif defined(__TDA_FILE_)
	//sprintf(lic_file,"%sTDA.lic",APP_PATH);
	byteVer[0]=1;
	byteVer[1]=3;
	byteVer[2]=6;
	byteVer[3]=0;
	cProductType=PRODUCT_TDA;
#else
	AfxMessageBox(CXhChar50("目前暂不支持的产品授权#%d",PRODUCT_TYPE));
#endif
	int retCode=ImportLicFile(licfile,cProductType,version);
	if(retCode==0&&(GetLicVersion()<1000004||!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_BASIC)))
		retCode=9;	//通过局域网续借授权失败
	if(retCode==0)
		error.EnableAutoDelete(true);
	else
		ExitCurrentDogSession();
	return retCode;
}

bool InitApplication()
{
	if(!VerifyDllVersionCompatible())
		return false;
	HKEY hKey;
	char lic_file[MAX_PATH] = {0};	//初始化字符串，否则提示未指定证书时提示字符串乱码 wht 19-07-08
	ULONG retCode=-2;
	DWORD dwDataType,dwLength=MAX_PATH;
	DWORD version[2]={0,20180130};
	BYTE* pByteVer=(BYTE*)version;
	HWND hWnd = adsw_acadMainWnd();
	char sSubKey[MAX_PATH]="";
	BYTE cProductType=0;
#ifdef __TMA_FILE_
#ifdef AFX_TARG_ENU_ENGLISH
	::SetWindowText(hWnd,"The tower manufacturing assistant");
#elif defined(__BP_ONLY_)
	::SetWindowText(hWnd,"工艺卡图像提取");
#else
	::SetWindowText(hWnd,"铁塔制造助手");
#endif
	pByteVer[0]=5;
	pByteVer[1]=1;
	pByteVer[2]=4;
	pByteVer[3]=0;
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sSubKey,"Software\\Xerofox\\TMA-EN\\Settings");
#else
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
#endif
	cProductType=PRODUCT_TMA;
#elif defined(__LMA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	::SetWindowText(hWnd,"The Gegou tower manufacturing assistant");
#else
	::SetWindowText(hWnd,"格构铁塔制造助手");
#endif
	pByteVer[0]=2;
	pByteVer[1]=0;
	pByteVer[2]=6;
	pByteVer[3]=0;
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	::SetWindowText(hWnd,"The tower internal force analysis");
#else
	::SetWindowText(hWnd,"铁塔内力分析");
#endif
	pByteVer[0]=1;
	pByteVer[1]=8;
	pByteVer[2]=1;
	pByteVer[3]=2;
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
	cProductType=PRODUCT_TSA;
#elif defined(__TDA_FILE_)
#ifdef AFX_TARG_ENU_ENGLISH
	::SetWindowText(hWnd,"The tower drawing assistant");
#else
	::SetWindowText(hWnd,"铁塔绘图助手");
#endif
	pByteVer[0]=1;
	pByteVer[1]=3;
	pByteVer[2]=6;
	pByteVer[3]=0;
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
	cProductType=PRODUCT_TDA;
#else
#ifdef AFX_TARG_ENU_ENGLISH
	::SetWindowText(hWnd,"Gegou tower design and drawing");
#else
	::SetWindowText(hWnd,"格构铁塔设计与绘图");
#endif
	pByteVer[0]=1;
	pByteVer[1]=3;
	pByteVer[2]=6;
	pByteVer[3]=0;
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
	cProductType=PRODUCT_LDS;
#endif
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	bool successed=false;
	if(RegQueryValueEx(hKey,_T("lic_file2"),NULL,&dwDataType,(BYTE*)&lic_file[0],&dwLength)== ERROR_SUCCESS)
		successed=true;	//优先读取当前V5版本证书序号, 以免不同版本间证书冲突
	else //if(!successed)
		successed=(RegQueryValueEx(hKey,_T("lic_file"),NULL,&dwDataType,(BYTE*)&lic_file[0],&dwLength)== ERROR_SUCCESS);
	if(successed)
	{
//#ifdef _NET_DOG
//		m_wDogModule=NetDogModule(lic_file);
//		if(!InitNetLicEnv(m_wDogModule))
//		{
//			CString ss;
//			ss.Format("%d号模块",m_wDogModule);
//			AfxMessageBox("0#网络狗"+ss+"初始化失败!");
//			exit(0);
//		}
//#endif
		//查找是否存在指定加密锁号的文件 wjh-2016.9.20
		char* separator=SearchChar(lic_file,'.',true);
		strcpy(separator,".key");
		DetectSpecifiedHaspKeyFile(lic_file);
		strcpy(separator,".lic");
		retCode=ImportLicFile(lic_file,cProductType,version);
	}
	if(retCode==9)
	{	//retCode==9时尝试重新申请局域网子授权 wht 19-02-25
		int nSubRetCode=RenewLanLicense(lic_file);
		if(nSubRetCode>=0)
			retCode=nSubRetCode;
		else if(nSubRetCode<0)
			retCode=9;
	}
	if(retCode!=0)
	{
		CXhChar500 sError("未知证书读取错误");
		if(retCode==-2)
#ifdef AFX_TARG_ENU_ENGLISH
			sError.Copy("The first time to use, haven't specified a certificate file!");
		else if(retCode==-1)
			sError.Copy("Dog initialize failed");
		else if(retCode==1)
			sError.Copy("1#Unable to open the certificate file");
		else if(retCode==2)
			sError.Copy("2#The certificate file was damaged");
		else if(retCode==3)
			sError.Copy("3#Certificate and encryption dog do not match");
		else if(retCode==4)
			sError.Copy("4#The encrypted version of the authorized certificate is wrong");
		else if(retCode==5)
			sError.Copy("5#The authorized versions of certificate and encryption dog do not match");
		else if(retCode==6)
			sError.Copy("6#Beyond the use scope of authorized version");
		else if(retCode==7)
			sError.Copy("7# Beyond the scope of free authorize version !");
		else if(retCode==8)
			sError.Copy("8# The serial number of license file does'nt match the hard lock!");
		else if(retCode==9)
			sError.Copy("9# Tryout expired!");
#else
			sError.Copy("首次使用，还未指定过证书文件！");
		else if(retCode==-1)
			sError.Copy("加密锁初始化失败！");
		else if(retCode==1)
			sError.Copy("1#无法打开证书文件！");
		else if(retCode==2)
			sError.Copy("2#证书文件遭到破坏！");
		else if(retCode==3)
			sError.Printf("3#证书与加密狗不匹配-%d！",DogSerialNo());
		else if(retCode==4)
			sError.Copy("4#授权证书的加密版本不对！");
		else if(retCode==5)
			sError.Copy("5#证书与加密狗产品授权版本不匹配！");
		else if(retCode==6)
			sError.Copy("6#超出版本使用授权范围！");
		else if(retCode==7)
			sError.Copy("7#超出免费版本升级授权范围！");
		else if(retCode==8)
			sError.Copy("8#证书序号与当前加密狗不一致！");
		else if(retCode==9)
			sError.Copy("9#授权过期，请续借授权！");
		else if(retCode==10)
			sError.Copy("10#程序缺少相应执行权限，请以管理员权限运行程序！");
		else if (retCode == 11)
			sError.Copy("11#授权异常，请使用管理员权限重新申请证书");
		else
			sError.Printf("错误码：%d#", retCode);
#endif
		sError.Append(lic_file);
		//
		strcpy(BTN_ID_YES_TEXT,"打开文件夹");
		strcpy(BTN_ID_NO_TEXT,"退出CAD");
		strcpy(BTN_ID_CANCEL_TEXT,"关闭");
		int nRetCode=MsgBox(adsw_acadMainWnd(),sError,"错误提示",MB_YESNOCANCEL|MB_ICONERROR);
		if(nRetCode==IDYES)
		{
			CXhChar500 sPath(lic_file);
			sPath.Replace("TMA.lic","");
			sPath.Replace("LMA.lic","");
			sPath.Replace("TDA.lic","");
			sPath.Replace("TSA.lic","");
			sPath.Replace("LDS.lic","");
			ShellExecute(NULL,"open",NULL,NULL,sPath,SW_SHOW);
			return false;
		}
		else if(nRetCode==IDNO)
			return false;
		else
		{	//卸载arx，不退出cad
			//kLoadDwgMsg中不能调用sendStringToExecute和acedCommand,acDocManager->curDocument()==NULL,无法执行 wht 18-12-25
			/*
#ifdef _ARX_2007
			ads_queueexpr(L"(command\"arx\" \"u\" \"MapTma.arx\")");
#else
			ads_queueexpr("(command\"arx\" \"u\" \"MapTma.arx\")");
#endif
			*/
			return false;
		}
	}
#if defined(_MAPTMA_ZRX)||defined(_MAPTMA_GRX)
	//DWORD dwFuncFlag=GetProductFuncFlag();
	//if(!PRODUCT_FUNC::IsHasZrxModule())
	if(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_ZRX_MODULE))
	{
#ifdef _MAPTMA_ZRX
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Without authorization for Zhongwang CAD drawing modular!");
#else
		AfxMessageBox("未取得中望CAD绘图模块授权！");
#endif
#elif defined(_MAPTMA_GRX)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Without authorization for Haochen CAD drawing modular!");
#else
		AfxMessageBox("未取得浩辰CAD绘图模块授权！");
#endif
#endif
		return false;
	}
#endif
	g_sLicFile.Copy(lic_file);
	g_sLdsExecutePath.Copy(lic_file);
	char* finalSlash=SearchChar(g_sLdsExecutePath,'\\',true);
	if(finalSlash)
		*(finalSlash+1)=0;
	char* separator=SearchChar(lic_file,'.',true);
	//读取本地化特征功能配置文件
	strcpy(separator,CXhChar16("%d.fac",DogSerialNo()));
	int nFacFileRetcode=ImportLocalFeatureAccessControlFile(lic_file);
	//logerr.Log("license fac '%s' imported code %d!",lic_file,retcode);
	if(nFacFileRetcode!=-1&&nFacFileRetcode<0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar100("FAC file init fail!%d# %s",nFacFileRetcode,lic_file));
#else
		AfxMessageBox(CXhChar100("FAC文件初始化失败!%d# %s",nFacFileRetcode,lic_file));
#endif
	}
	//导入帮助映射文件
	char hmp_file[MAX_PATH], APP_PATH[MAX_PATH];
	GetAppPath(APP_PATH);
	sprintf(hmp_file, "%sDocs\\LDS.hmp", APP_PATH);
	CSystemSettingDlg::InitPropHashtable();
	ImportPropHelpListMapFile(hmp_file);
#ifdef __TMA_FILE_
	sprintf(g_sHelpFilePath, "%sDocs\\TMA.chm", APP_PATH);
#else
	sprintf(g_sHelpFilePath, "%sDocs\\LDS.chm", APP_PATH);
#endif
	// NOTE: DO NOT edit the following lines.
	//{{AFX_ARX_INIT
	//}}AFX_ARX_INIT

#ifndef __BP_ONLY_
	Ta.m_iCurSeg = SEGI(1);
	char path[MAX_PATH];
	console.ActivateModel(&Ta);
	CLDSDbObject::_console = &console;
	CLDSPart::library = &globalLibrary;
	CDesignThickPara::m_pTower = &Ta;
	CLDSPoint::m_pTower = &Ta;
	CLDSVector::m_pTower = &Ta;
	Ta.DisplayProcess = DisplayProcess;
	SysSetImportDefault(FALSE);	//调用系统配置文件
	//GetSysPath(sys_path);
	GetJgGuiGeDBFilePath(path);
	if (strlen(path) != 0)
		JgGuiGeSerialize(path, FALSE);
	//GetSlotDBFilePath(path);
	//if(strlen(path)!=0)
	//	SlotGuiGeSerialize(path,FALSE);
	GetTubeGuiGeDBFilePath(path);
	if (strlen(path) != 0)
		TubeGuiGeSerialize(path, FALSE);
#endif
	//初始化企业定制配置信息(必须置于因为企业定制化的优先级高于系统配置，故须置于SysSetImportDefault之后） wjh-2019.5.11
	InitCustomizeConfiguration(ValidateLocalizeFeature(FEATURE::LOCALIZE_CUSTOMIZE_CLIENT));
	if(nFacFileRetcode>0)
	{
		CString szTitle;
		CXhChar200 sOldTitle;
		::GetWindowText(hWnd,sOldTitle,200);
		szTitle=sOldTitle;
		CXhChar50 sClientName=gxDrwLocalizer.szCustomerName;
		if(sClientName.GetLength()>0)
		{
			sClientName.Append('#');
			szTitle.Insert(0,sClientName);
			::SetWindowText(hWnd,szTitle);
		}
	}
	strcpy(separator,".plb");
	InitPartLibrary(lic_file);
#ifdef __PART_DESIGN_INC_
	CXhChar200 lib_path;
	lib_path.Printf("%sWireLayoutEvolver.dll",(char*)g_sLdsExecutePath);
	m_hWireLayoutModule=LoadLibrary(lib_path);
	if(m_hWireLayoutModule!=NULL)
	{
		CTower::WriteWireLayoutWeather =(DefWriteWeatherFunc)GetProcAddress(m_hWireLayoutModule,"WriteWeather");
		CTower::ReadWireLayoutData =(DefReadWireLayoutDataFunc)GetProcAddress(m_hWireLayoutModule,"ReadWireLayoutData");
	}
#endif
	//GetFlatGuiGeDBFilePath(path);
	//if(strlen(path)!=0)
	//	FlatGuiGeSerialize(path,FALSE);

	//LoadDefaultSysPara();
	char sGroupName[100]="TMA-MENU";
#ifdef __BP_ONLY_
	strcpy(sGroupName,"BPS-MENU");
#endif
/*
#ifdef __TMA_FILE_
	strcpy(sGroupName,"TMA_MENU");
#elif __LMA_FILE_
	strcpy(sGroupName,"LMA_MENU");
#elif __LDS_FILE_
	strcpy(sGroupName,"LDS_MENU");
#elif __TSA_FILE_
	strcpy(sGroupName,"TSA_MENU");
#endif
*/
//-------命令设置--------
#ifdef _ARX_2007
#if defined(__BP_ONLY_)||defined(__CARD_IMG_VIEWER_)
	if(ValidateLocalizeFeature(FEATURE::BATCH_INTELLI_PRINT_PROCESSCARD))
	{
	#ifdef __BP_ONLY_
		// 智能提取角钢
		acedRegCmds->addCommand(L"TMA-MENU",           // Group name
			L"BP",             // Global function name
			L"BP",             // Local function name
			ACRX_CMD_MODAL,   // Type
			&BatchPrintPartProcessCards);            // Function pointer
	#elif defined(__CARD_IMG_VIEWER_)
		// 清空模型重新提取工艺卡
		acedRegCmds->addCommand(_bstr_t(sGroupName),           // Group name
			L"RP",             // Global function name
			L"RP",             // Local function name
			ACRX_CMD_MODAL,   // Type
			&RetrievedProcessCards);            // Function pointer
		// 以追加方式提取工艺卡
		acedRegCmds->addCommand(_bstr_t(sGroupName),           // Group name
			L"AP",             // Global function name
			L"AP",             // Local function name
			ACRX_CMD_MODAL,   // Type
			&AppendOrUpdateProcessCards);            // Function pointer
	#endif
	}
#else
	//---------VV--数  据--VV-----------
	//导入LTMA文件
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"ImportLma",          // Global function name
		L"ImportLma",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&ImportTma );          // Function pointer
	//关闭LTMA文件
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"CloseLma",          // Global function name
		L"CloseLma",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CloseTma );          // Function pointer
	//文件属性
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaFileProp",          // Global function name
		L"LmaFileProp",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FileProp );          // Function pointer
	//---------VV--总  图--VV-----------
	//选择模型号
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSelModuleNo",          // Global function name
		L"LmaSelModuleNo",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SelModuleNo );          // Function pointer
	//正面
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSumFrontFace",          // Global function name
		L"LmaSumFrontFace",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumFrontFace );          // Function pointer
	//侧面
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSumSideFace",          // Global function name
		L"LmaSumSideFace",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumSideFace);          // Function pointer
	//接腿正面
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaFrontMutiLeg",          // Global function name
		L"LmaFrontMutiLeg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FrontMutiLeg);          // Function pointer
	//接腿侧面
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSideMutiLeg",          // Global function name
		L"LmaSideMutiLeg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SideMutiLeg);          // Function pointer
#ifdef __RAPID_ROUGH_3DMODEL_
	//博超要求三维单线
	acedRegCmds->addCommand(L"TMA-MENU",         // Group name 
		L"BC3DLine",          // Global function name
		L"BC3DLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&Draw3DLineBC);          // Function pointer
	//国网要求三维单线
	acedRegCmds->addCommand(L"TMA-MENU",         // Group name 
		L"GW3DLine",          // Global function name
		L"GW3DLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&Draw3dLineGW);          // Function pointer
	//国网要求三维实体
	acedRegCmds->addCommand(L"TMA-MENU",         // Group name 
		L"GWSolid",          // Global function name
		L"GWSolid",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSolidGW);          // Function pointer
	//批量导出塔例三维实体文件
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"EF",
		L"EF",
		ACRX_CMD_MODAL,
		&Export3DFiles);
	//导出三维实体文件
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"tid",
		L"tid",
		ACRX_CMD_MODAL,
		&TidToDwg);
#endif
#ifdef __TSA_FILE_
	bool bSupportDwgSolidExport=false;
#elif defined(__RAPID_ROUGH_3DMODEL_)
	bool bSupportDwgSolidExport=true;
#else	//TMA,LMA,TDA
	bool bSupportDwgSolidExport=false;
	if( GetLicVersion()>=1000003&&(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_DWGSOLID_EXPORT)||ValidateLocalizeFeature(FEATURE::EXPORT_3DSOLID_DWGMODEL)!=0))
		bSupportDwgSolidExport=true;
#endif
	if(bSupportDwgSolidExport)
	{
		//显示实体
		acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
			L"LmaDrawSolid",          // Global function name
			L"LmaDrawSolid",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&DrawSolid );          // Function pointer
		//显示单线
		acedRegCmds->addCommand(L"TMA-MENU",         // Group name 
			L"Draw3DLine",          // Global function name
			L"Draw3DLine",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&Draw3DLine);          // Function pointer
	}
	//标注分段号
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimSegI",          // Global function name
		L"LmaDimSegI",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSegI);          // Function pointer
	//法兰明细表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaFLPartBOM",          // Global function name
		L"LmaFLPartBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FLPartBOM);          // Function pointer
	//材料汇总表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSumTaMat",          // Global function name
		L"LmaSumTaMat",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumTaMat );          // Function pointer
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//设置材料汇总表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSumTaMatCfg",          // Global function name
		L"LmaSumTaMatCfg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumTaMatCfg );          // Function pointer
#endif
	//配置信息表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaConfigInfoBOM",          // Global function name
		L"LmaConfigInfoBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&ConfigInfoBOM );          // Function pointer
	//角钢准距表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaJgZhunJuTable",          // Global function name
		L"LmaJgZhunJuTable",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&JgZhunJuTable );          // Function pointer
	//---------VV--结构图--VV-----------
	//选择分段号
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSelSegI",    // Global function name
		L"LmaSelSegI",    // Local function name
		ACRX_CMD_MODAL,			// Type
		&SelSegI);    // Function pointer
	//单线图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSegLine",          // Global function name
		L"LmaSegLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegLine);          // Function pointer
	//组焊装配图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPartGroupSetupStru",          // Global function name
		L"LmaPartGroupSetupStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupSetupDrawing);          // Function pointer
	//组焊图构件明细表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPartGroupBOM",          // Global function name
		L"LmaPartGroupBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupBOM);          // Function pointer
	//分段结构图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSegStru",          // Global function name
		L"LmaSegStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegStru);          // Function pointer
	// 分段构件明细表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSegPartBOM",          // Global function name
		L"LmaSegPartBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegPartBOM);          // Function pointer
	// 分段螺栓脚钉垫圈明细表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaBoltPadBOM",          // Global function name
		L"LmaBoltPadBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BoltPadBOM);          // Function pointer
	// 绘制底脚板的俯视平面图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDrawFootPlanform",          // Global function name
		L"LmaDrawFootPlanform",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawFootPlanform);          // Function pointer
	//---------VV-插入图框-VV-----------
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaInsertMapFrm",          // Global function name
		L"LmaInsertMapFrm",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&InsertMapFrm);          // Function pointer
	//插入钢印区
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"MK",
		L"MK",
		ACRX_CMD_MODAL,
		&InsertMKRect);
#ifdef __SMART_DRAW_INC_
	//插入接头包板俯视图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"SV",
		L"SV",
		ACRX_CMD_MODAL,
		&InsertWrapperSideView);
#endif
	//检测件号
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"CP",
		L"CP",
		ACRX_CMD_MODAL,
		&CheckDwgFilePartNo);
	//对直线消隐处理
	/*acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"MT",
		L"MT",
		ACRX_CMD_MODAL,
		&MoveTop);*/
	//---------VV--标  注--VV-----------
	//空间尺寸标注
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimSpaceSize",          // Global function name
		L"LmaDimSpaceSize",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSpaceSize);          // Function pointer
	//图面尺寸标注
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimMapSize",          // Global function name
		L"LmaDimMapSize",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimMapSize);          // Function pointer
#ifndef __TSA_FILE_
	//脚钉间距标注
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimFootNailSpace",          // Global function name
		L"LmaDimFootNailSpace",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimFootNailSpace);          // Function pointer
#endif
	//垫圈规格及数量标注
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimBoltPad",          // Global function name
		L"LmaDimBoltPad",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimBoltPad);          // Function pointer
	//标注螺栓信息
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimLsInfo",          // Global function name
		L"LmaDimLsInfo",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimLsInfo);          // Function pointer
	//标注正负头
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimOddment",          // Global function name
		L"LmaDimOddment",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimOddment);          // Function pointer
	//标注构件明细
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimPartSpec",          // Global function name
		L"LmaDimPartSpec",          // Local function name
		ACRX_CMD_MODAL|ACRX_CMD_USEPICKSET	,      // Type
		&DimPartSpec);          // Function pointer
	//相似形标注设置
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimSimPolyCfg",          // Global function name
		L"LmaDimSimPolyCfg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSimPolyCfg);          // Function pointer
	//相似形标注
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDimSimPoly",          // Global function name
		L"LmaDimSimPoly",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSimPoly);          // Function pointer
	//---------VV--构件图--VV-----------
	//批量生成典型角钢工艺卡
	acedRegCmds->addCommand( L"TMA-MENU",       // Group name 
		L"LmaPartDrawingClassicAngle",          // Global function name
		L"LmaPartDrawingClassicAngle",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartDrawingClassicAngle);          // Function pointer
	//逐个生成零件图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPartDrawingOneByOne",          // Global function name
		L"LmaPartDrawingOneByOne",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartDrawingOneByOne);          // Function pointer
	//批量生成零件图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaBatchPartDrawing",          // Global function name
		L"LmaBatchPartDrawing",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BatchPartDrawing);          // Function pointer
	//特殊工艺统计卡
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaSpecialProcessStatCard",       // Global function name
		L"LmaSpecialProcessStatCard",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&SpecialProcessStatCard);          // Function pointer
	//标准组焊件工艺卡
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaStdPartGroupProcessCard",       // Global function name
		L"LmaStdPartGroupProcessCard",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&StdPartGroupProcessCard);          // Function pointer
	//工装辅助钢板
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaSetupAidedPlate",       // Global function name
		L"LmaSetupAidedPlate",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&SetupAidedPlate);          // Function pointer
	//读取库文件生成标准构件零件图
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaStdPartDrawing",       // Global function name
		L"LmaStdPartDrawing",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&StdPartDrawingByReadLibFile);          // Function pointer
	//制弯角钢拓印图
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaPolyJgDrawing",       // Global function name
		L"LmaPolyJgDrawing",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&PolyJgBendDrawing);          // Function pointer
	//---------VV--工  具--VV-----------
	//定义工艺卡
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"DefTable",          // Global function name
		L"DefTable",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DefTable);          // Function pointer
#if defined(__LDS_FILE_)//||defined(__TMA_FILE_)||defined(__LMA_FILE_)||defined(__TDA_FILE_)||
	if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
	{	//提取构件明细表
		acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
			L"BomRecog",          // Global function name
			L"BomRecog",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&RecogizePartBomToTxt);          // Function pointer
		acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
			L"RG",
			L"RG",
			ACRX_CMD_MODAL,
			&RecogizePartBomToBom);
	}
#endif
	//计算两钢管夹角
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaCalTwoTubeAngle",          // Global function name
		L"LmaCalTwoTubeAngle",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CalTwoTubeAngle);          // Function pointer
	//截断指定直线
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaBreakSpecLine",          // Global function name
		L"LmaBreakSpecLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BreakSpecLine);          // Function pointer
	// 绘制焊缝
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDrawSpecialWeld",          // Global function name
		L"LmaDrawSpecialWeld",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSpecialWeld);          // Function pointer
	// 绘制钢管破断线
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDrawSpecTubeBreakLine",          // Global function name
		L"LmaDrawSpecTubeBreakLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSpecTubeBreakLine);          // Function pointer
	//绘制剖视图符
	acedRegCmds->addCommand( L"TMA-MENU",  // Group name 
		L"LmaDrawCutawayViewSymbol",       // Global function name
		L"LmaDrawCutawayViewSymbol",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&DrawSpecCutawayViewSymbol);          // Function pointer
	//极坐标转换
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPolarCsys",          // Global function name
		L"LmaPolarCsys",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PolarCsys);          // Function pointer
	//批量打印
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaBatchPlotPartDrawing",          // Global function name
		L"LmaBatchPlotPartDrawing",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BatchPlotPartDrawing);          // Function pointer
	
	//加载MapTma.arx
	acedRegCmds->addCommand( L"ShiftMapTMAArx",         // Group name 
		L"LoadTMAMapArx",          // Global function name
		L"LoadTMAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadTMAMapArx);          // Function pointer
	//加载MapLma.arx
	acedRegCmds->addCommand( L"ShiftMapLMAArx",         // Group name 
		L"LoadLMAMapArx",          // Global function name
		L"LoadLMAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadLMAMapArx);          // Function pointer
	//加载MapLDS.arx
	acedRegCmds->addCommand( L"ShiftMapLDSArx",         // Group name 
		L"LoadLDSMapArx",          // Global function name
		L"LoadLDSMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadLDSMapArx);          // Function pointer
	//加载MapTsa.arx
	acedRegCmds->addCommand( L"ShiftMapTSAArx",         // Group name 
		L"LoadTSAMapArx",          // Global function name
		L"LoadTSAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadTSAMapArx);          // Function pointer
#ifdef __SMART_DRAW_INC_
	//智能绘图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"SD",          // Global function name
		L"SD",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SmartDraw);          // Function pointer
#endif
	//---------VV--配  置--VV-----------
	//系统设置
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaEnvGeneralSet",          // Global function name
		L"LmaEnvGeneralSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&EnvGeneralSet);          // Function pointer
	//导入设置
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSysSetImport",          // Global function name
		L"LmaSysSetImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SysSetImport);          // Function pointer
	//导出设置
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaSysSetExport",          // Global function name
		L"LmaSysSetExport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SysSetExport);          // Function pointer
	//导入角钢规格库
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaJgGuiGeFileImport",          // Global function name
		L"LmaJgGuiGeFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&JgGuiGeFileImport);          // Function pointer
	//导入钢管规格库
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaTubeGuiGeFileImport",          // Global function name
		L"LmaTubeGuiGeFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&TubeGuiGeFileImport);          // Function pointer
	//导入标注构件库
	  //槽型插板
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaCInsertPlateLibImport",          // Global function name
		L"LmaCInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CInsertPlateLibImport);          // Function pointer
	  //U型插板
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaUInsertPlateLibImport",          // Global function name
		L"LmaUInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&UInsertPlateLibImport);          // Function pointer
	  //十字型插板
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaXInsertPlateLibImport",          // Global function name
		L"LmaXInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&XInsertPlateLibImport);          // Function pointer
	  //平焊法兰
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPFlLibImport",          // Global function name
		L"LmaPFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PFlLibImport);          // Function pointer
	  //对焊法兰
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaDFlLibImport",          // Global function name
		L"LmaDFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DFlLibImport);          // Function pointer
	//刚性法兰
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name
		L"LmaGFlLibImport",          // Global function name
		L"LmaGFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&GFlLibImport);          // Function pointer
	//柔性法兰
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaRFlLibImport",          // Global function name
		L"LmaRFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&RFlLibImport);          // Function pointer
	  //法兰对接级别定义文件
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaFlCoupleLevelFileImport",          // Global function name
		L"LmaFlCoupleLevelFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FlCoupleLevelFileImport);          // Function pointer

	//---------VV--菜单中未显示的命令--VV-----------
	//组焊图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaPartGroupStru",          // Global function name
		L"LmaPartGroupStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupStru);          // Function pointer
	//绘图比例
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"LmaEnvScaleSet",          // Global function name
		L"LmaEnvScaleSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&EnvScaleSet);          // Function pointer
	//转换工艺卡
	/*acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"PCD",          // Global function name
		L"PCD",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&TransformProcessCard);*/          // Function pointer
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	//展开面单线图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"SpreadFaceLine",          // Global function name
		L"SpreadFaceLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SpreadFaceLine);          // Function pointer
	//展开面结构图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"SpreadFaceStru",          // Global function name
		L"SpreadFaceStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SpreadFaceStru);          // Function pointer
	//构件规格表
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"TsaPartSpecTbl",          // Global function name
		L"TsaPartSpecTbl",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartSpecTbl);          // Function pointer
	//绘制荷载图
	acedRegCmds->addCommand( L"TMA-MENU",         // Group name 
		L"TsaDrawLoadGraph",          // Global function name
		L"TsaDrawLoadGraph",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawLoadGraph);          // Function pointer
#endif
#endif
#else
	//---------VV--数  据--VV-----------
#if defined(__BP_ONLY_)||defined(__CARD_IMG_VIEWER_)
	if(ValidateLocalizeFeature(FEATURE::BATCH_INTELLI_PRINT_PROCESSCARD))
	{
	#ifdef __BP_ONLY_
		// 智能提取角钢
		acedRegCmds->addCommand( sGroupName,           // Group name
			"BP",        // Global function name
			"BP",        // Local function name
			ACRX_CMD_MODAL,   // Type
			&BatchPrintPartProcessCards);            // Function pointer
	#elif defined(__CARD_IMG_VIEWER_)
		//TODO:临时开放给汇金通原型系统用 wjh-2017.12.21
		// 清空模型重新提取工艺卡
		acedRegCmds->addCommand(sGroupName,           // Group name
			"RP",             // Global function name
			"RP",             // Local function name
			ACRX_CMD_MODAL,   // Type
			&RetrievedProcessCards);            // Function pointer
		// 以追加方式提取工艺卡
		acedRegCmds->addCommand(sGroupName,           // Group name
			"AP",             // Global function name
			"AP",             // Local function name
			ACRX_CMD_MODAL,   // Type
			&AppendOrUpdateProcessCards);            // Function pointer*/
	#endif
	}
	//else
	//	AfxMessageBox("缺少智能批量打印的合法授权!");
#else
	//导入LTMA文件
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"ImportLma",          // Global function name
		"ImportLma",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&ImportTma );          // Function pointer
	//关闭LTMA文件
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"CloseLma",          // Global function name
		"CloseLma",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CloseTma );          // Function pointer
	//文件属性
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaFileProp",          // Global function name
		"LmaFileProp",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FileProp );          // Function pointer
	//---------VV--总  图--VV-----------
	//选择模型号
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSelModuleNo",          // Global function name
		"LmaSelModuleNo",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SelModuleNo );          // Function pointer
	//正面
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSum",          // Global function name
		"LmaSum",          // Local function name
		ACRX_CMD_MODAL,				// Type
		&DrawSumMapByAnalysisMode );// Function pointer
	//正面
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSumFrontFace",          // Global function name
		"LmaSumFrontFace",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumFrontFace );          // Function pointer
	//侧面
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSumSideFace",          // Global function name
		"LmaSumSideFace",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumSideFace);          // Function pointer
	//接腿正面
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaFrontMutiLeg",          // Global function name
		"LmaFrontMutiLeg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FrontMutiLeg);          // Function pointer
	//接腿侧面
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSideMutiLeg",          // Global function name
		"LmaSideMutiLeg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SideMutiLeg);          // Function pointer
#ifdef __RAPID_ROUGH_3DMODEL_
	//博超要求三维单线
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"BC3DLine",          // Global function name
		"BC3DLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&Draw3DLineBC);          // Function pointer
	//国网要求三维单线
	acedRegCmds->addCommand(sGroupName,         // Group name 
		"GW3DLine",          // Global function name
		"GW3DLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&Draw3dLineGW);          // Function pointer
	//国网要求三维实体
	acedRegCmds->addCommand(sGroupName,         // Group name 
		"GWSolid",          // Global function name
		"GWSolid",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSolidGW);          // Function pointer
	//批量导出塔例三维实体文件
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"EF",
		"EF",
		ACRX_CMD_MODAL,
		&Export3DFiles);
	//导出三维实体文件
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"tid",
		"tid",
		ACRX_CMD_MODAL,
		&TidToDwg);
#endif
#ifdef __TSA_FILE_
	bool bSupportDwgSolidExport=false;
#elif defined(__RAPID_ROUGH_3DMODEL_)
	bool bSupportDwgSolidExport=true;
#else	//TMA,LMA,TDA
	bool bSupportDwgSolidExport=false;
	if( GetLicVersion()>=1000003&&(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_DWGSOLID_EXPORT)||ValidateLocalizeFeature(FEATURE::EXPORT_3DSOLID_DWGMODEL)!=0))
		bSupportDwgSolidExport=true;
#endif
	if(bSupportDwgSolidExport)
	{	//显示实体
		acedRegCmds->addCommand( sGroupName,         // Group name 
			"LmaDrawSolid",          // Global function name
			"LmaDrawSolid",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&DrawSolid);          // Function pointer
		//显示三位单线
		acedRegCmds->addCommand(sGroupName,         // Group name 
			"Draw3DLine",          // Global function name
			"Draw3DLine",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&Draw3DLine);          // Function pointer
	}
	//标注分段号
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimSegI",          // Global function name
		"LmaDimSegI",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSegI);          // Function pointer
	//法兰明细表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaFLPartBOM",          // Global function name
		"LmaFLPartBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FLPartBOM);          // Function pointer
	//材料汇总表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSumTaMat",          // Global function name
		"LmaSumTaMat",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumTaMat );          // Function pointer
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//设置材料汇总表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSumTaMatCfg",          // Global function name
		"LmaSumTaMatCfg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SumTaMatCfg );          // Function pointer
#endif
	//配置信息表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaConfigInfoBOM",          // Global function name
		"LmaConfigInfoBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&ConfigInfoBOM );          // Function pointer
	//角钢准距表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaJgZhunJuTable",          // Global function name
		"LmaJgZhunJuTable",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&JgZhunJuTable );          // Function pointer
	//---------VV--结构图--VV-----------
	//选择分段号
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSelSegI",    // Global function name
		"LmaSelSegI",    // Local function name
		ACRX_CMD_MODAL,			// Type
		&SelSegI);    // Function pointer
	//单线图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSegLine",          // Global function name
		"LmaSegLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegLine);          // Function pointer
	//组焊装配图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPartGroupSetupStru",          // Global function name
		"LmaPartGroupSetupStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupSetupDrawing);          // Function pointer
	//组焊图构件明细表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPartGroupBOM",          // Global function name
		"LmaPartGroupBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupBOM);          // Function pointer
	//分段结构图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSegStru",          // Global function name
		"LmaSegStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegStru);          // Function pointer
	// 分段构件明细表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSegPartBOM",          // Global function name
		"LmaSegPartBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SegPartBOM);          // Function pointer
	// 分段螺栓脚钉垫圈明细表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaBoltPadBOM",          // Global function name
		"LmaBoltPadBOM",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BoltPadBOM);          // Function pointer
	// 绘制底脚板的俯视平面图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDrawFootPlanform",          // Global function name
		"LmaDrawFootPlanform",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawFootPlanform);          // Function pointer
	//---------VV-插入图框-VV-----------
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaInsertMapFrm",          // Global function name
		"LmaInsertMapFrm",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&InsertMapFrm);          // Function pointer
	//插入钢印区
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"MK",
		"MK",
		ACRX_CMD_MODAL,
		&InsertMKRect);
#ifdef __SMART_DRAW_INC_
	//插入接头包板俯视图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"SV",
		"SV",
		ACRX_CMD_MODAL,
		&InsertWrapperSideView);
#endif
	//检测件号
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"CP",
		"CP",
		ACRX_CMD_MODAL,
		&CheckDwgFilePartNo);
	//---------VV--标  注--VV-----------
	//空间尺寸标注
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimSpaceSize",          // Global function name
		"LmaDimSpaceSize",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSpaceSize);          // Function pointer
	//图面尺寸标注
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimMapSize",          // Global function name
		"LmaDimMapSize",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimMapSize);          // Function pointer
#ifndef __TSA_FILE_
	//脚钉间距标注
	acedRegCmds->addCommand( sGroupName,// Group name 
		"LmaDimFootNailSpace",          // Global function name
		"LmaDimFootNailSpace",          // Local function name
		ACRX_CMD_MODAL,					// Type
		&DimFootNailSpace);				// Function pointer
#endif
	//垫圈规格及数量标注
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimBoltPad",          // Global function name
		"LmaDimBoltPad",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimBoltPad);          // Function pointer
	//标注螺栓信息
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimLsInfo",          // Global function name
		"LmaDimLsInfo",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimLsInfo);          // Function pointer
	//标注正负头
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimOddment",          // Global function name
		"LmaDimOddment",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimOddment);          // Function pointer
	//标注构件明细
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimPartSpec",          // Global function name
		"LmaDimPartSpec",          // Local function name
		ACRX_CMD_MODAL|ACRX_CMD_USEPICKSET	,      // Type
		&DimPartSpec);          // Function pointer
	//相似形标注设置
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimSimPolyCfg",          // Global function name
		"LmaDimSimPolyCfg",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSimPolyCfg);          // Function pointer
	//相似形标注
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDimSimPoly",          // Global function name
		"LmaDimSimPoly",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DimSimPoly);          // Function pointer
	//---------VV--构件图--VV-----------
	//批量生成典型角钢工艺卡
	acedRegCmds->addCommand( sGroupName,       // Group name 
		"LmaPartDrawingClassicAngle",          // Global function name
		"LmaPartDrawingClassicAngle",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartDrawingClassicAngle);          // Function pointer
	//逐个生成零件图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPartDrawingOneByOne",          // Global function name
		"LmaPartDrawingOneByOne",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartDrawingOneByOne);          // Function pointer
	//批量生成零件图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaBatchPartDrawing",          // Global function name
		"LmaBatchPartDrawing",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BatchPartDrawing);          // Function pointer
	//特殊工艺统计卡
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaSpecialProcessStatCard",       // Global function name
		"LmaSpecialProcessStatCard",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&SpecialProcessStatCard);          // Function pointer
	//标准组焊件工艺卡
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaStdPartGroupProcessCard",       // Global function name
		"LmaStdPartGroupProcessCard",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&StdPartGroupProcessCard);          // Function pointer
	//工装辅助钢板
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaSetupAidedPlate",       // Global function name
		"LmaSetupAidedPlate",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&SetupAidedPlate);          // Function pointer
	//读取库文件生成标准构件零件图
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaStdPartDrawing",       // Global function name
		"LmaStdPartDrawing",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&StdPartDrawingByReadLibFile);          // Function pointer
	//制弯角钢拓印图
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaPolyJgDrawing",       // Global function name
		"LmaPolyJgDrawing",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&PolyJgBendDrawing);          // Function pointer
	//---------VV--工  具--VV-----------
	//定义工艺卡
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"DefTable",          // Global function name
		"DefTable",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DefTable);          // Function pointer
#if defined(__LDS_FILE_)//||defined(__TMA_FILE_)||defined(__LMA_FILE_)||defined(__TDA_FILE_)||
	if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
	{	//提取构件明细表
		acedRegCmds->addCommand( sGroupName,         // Group name 
			"BomRecog",          // Global function name
			"BomRecog",          // Local function name
			ACRX_CMD_MODAL,      // Type
			&RecogizePartBomToTxt);          // Function pointer
		acedRegCmds->addCommand( sGroupName,         // Group name 
			"RG",
			"RG",
			ACRX_CMD_MODAL,
			&RecogizePartBomToBom);
	}
#endif
	//计算两钢管夹角
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaCalTwoTubeAngle",          // Global function name
		"LmaCalTwoTubeAngle",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CalTwoTubeAngle);          // Function pointer
	//截断指定直线
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaBreakSpecLine",          // Global function name
		"LmaBreakSpecLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BreakSpecLine);          // Function pointer
	// 绘制焊缝
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDrawSpecialWeld",          // Global function name
		"LmaDrawSpecialWeld",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSpecialWeld);          // Function pointer
	// 绘制钢管破断线
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDrawSpecTubeBreakLine",          // Global function name
		"LmaDrawSpecTubeBreakLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawSpecTubeBreakLine);          // Function pointer
	//绘制剖视图符
	acedRegCmds->addCommand( sGroupName,  // Group name 
		"LmaDrawCutawayViewSymbol",       // Global function name
		"LmaDrawCutawayViewSymbol",       // Local function name
		ACRX_CMD_MODAL,			 // Type
		&DrawSpecCutawayViewSymbol);          // Function pointer
	//极坐标转换
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPolarCsys",          // Global function name
		"LmaPolarCsys",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PolarCsys);          // Function pointer
	//批量打印
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaBatchPlotPartDrawing",          // Global function name
		"LmaBatchPlotPartDrawing",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&BatchPlotPartDrawing);          // Function pointer
	
	//加载MapTma.arx
	acedRegCmds->addCommand( "ShiftMapTMAArx",         // Group name 
		"LoadTMAMapArx",          // Global function name
		"LoadTMAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadTMAMapArx);          // Function pointer
	//加载MapLma.arx
	acedRegCmds->addCommand( "ShiftMapLMAArx",         // Group name 
		"LoadLMAMapArx",          // Global function name
		"LoadLMAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadLMAMapArx);          // Function pointer
	//加载MapLDS.arx
	acedRegCmds->addCommand( "ShiftMapLDSArx",         // Group name 
		"LoadLDSMapArx",          // Global function name
		"LoadLDSMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadLDSMapArx);          // Function pointer
	//加载MapTsa.arx
	acedRegCmds->addCommand( "ShiftMapTSAArx",         // Group name 
		"LoadTSAMapArx",          // Global function name
		"LoadTSAMapArx",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&LoadTSAMapArx);          // Function pointer
#ifdef __SMART_DRAW_INC_
	//智能绘图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"SD",          // Global function name
		"SD",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SmartDraw);          // Function pointer
#endif
	//---------VV--配  置--VV-----------
	//系统设置
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaEnvGeneralSet",          // Global function name
		"LmaEnvGeneralSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&EnvGeneralSet);          // Function pointer
	//导入设置
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSysSetImport",          // Global function name
		"LmaSysSetImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SysSetImport);          // Function pointer
	//导出设置
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaSysSetExport",          // Global function name
		"LmaSysSetExport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SysSetExport);          // Function pointer
	//导入角钢规格库
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaJgGuiGeFileImport",          // Global function name
		"LmaJgGuiGeFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&JgGuiGeFileImport);          // Function pointer
	//导入钢管规格库
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaTubeGuiGeFileImport",          // Global function name
		"LmaTubeGuiGeFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&TubeGuiGeFileImport);          // Function pointer
	//导入标注构件库
	  //槽型插板
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaCInsertPlateLibImport",          // Global function name
		"LmaCInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&CInsertPlateLibImport);          // Function pointer
	  //U型插板
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaUInsertPlateLibImport",          // Global function name
		"LmaUInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&UInsertPlateLibImport);          // Function pointer
	  //十字型插板
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaXInsertPlateLibImport",          // Global function name
		"LmaXInsertPlateLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&XInsertPlateLibImport);          // Function pointer
	  //平焊法兰
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPFlLibImport",          // Global function name
		"LmaPFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PFlLibImport);          // Function pointer
	  //对焊法兰
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaDFlLibImport",          // Global function name
		"LmaDFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DFlLibImport);          // Function pointer
	//刚性法兰
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaGFlLibImport",          // Global function name
		"LmaGFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&GFlLibImport);          // Function pointer
	//柔性法兰
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaRFlLibImport",          // Global function name
		"LmaRFlLibImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&RFlLibImport);          // Function pointer
	  //法兰对接级别定义文件
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaFlCoupleLevelFileImport",          // Global function name
		"LmaFlCoupleLevelFileImport",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&FlCoupleLevelFileImport);          // Function pointer
	//---------VV--菜单中未显示的命令--VV-----------
	//组焊图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaPartGroupStru",          // Global function name
		"LmaPartGroupStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartGroupStru);          // Function pointer
	//绘图比例
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"LmaEnvScaleSet",          // Global function name
		"LmaEnvScaleSet",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&EnvScaleSet);          // Function pointer
	/*
	//转换工艺卡
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"PCD",          // Global function name
		"PCD",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&TransformProcessCard);       
	*/
	//展开面单线图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"SpreadFaceLine",          // Global function name
		"SpreadFaceLine",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SpreadFaceLine);          // Function pointer
	//展开面结构图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"SpreadFaceStru",          // Global function name
		"SpreadFaceStru",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&SpreadFaceStru);          // Function pointer
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	//构件规格表
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"TsaPartSpecTbl",          // Global function name
		"TsaPartSpecTbl",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&PartSpecTbl);          // Function pointer
	//绘制荷载图
	acedRegCmds->addCommand( sGroupName,         // Group name 
		"TsaDrawLoadGraph",          // Global function name
		"TsaDrawLoadGraph",          // Local function name
		ACRX_CMD_MODAL,      // Type
		&DrawLoadGraph);          // Function pointer
#endif
#endif
#endif
#ifdef _ARX_2007
	acedCommand(RTSTR,L"-OSnap",RTSTR,L"off",RTNONE);	//关闭自动捕捉功能，否则生成点线时位置不对
#else
	acedCommand(RTSTR,"-OSnap",RTSTR,"off",RTNONE);	//关闭自动捕捉功能，否则生成点线时位置不对
#endif
#ifdef __PNC_FUNC__
	InitPNCCmd();
#endif
	return true;
}

// Unload this application. Unregister all objects
// registered in InitApplication.
void CloseTaFile(BOOL bPrompt,BOOL bClearTa);
void UnloadApplication()
{
	ExitCurrentDogSession();
	if(m_hWireLayoutModule!=NULL)
		FreeLibrary(m_hWireLayoutModule);

	CloseTaFile(FALSE,TRUE);
	// NOTE: DO NOT edit the following lines.
	//{{AFX_ARX_EXIT
	//}}AFX_ARX_EXIT
	// TODO: clean up your application
	char sGroupName[100]="TMA-MENU";
#ifdef __BP_ONLY_
	strcpy(sGroupName,"BPS-MENU");
#endif

/*
#ifdef __TMA_FILE_
	strcpy(sGroupName,"TMA_MENU");
#elif __LMA_FILE_
	strcpy(sGroupName,"LMA_MENU");
#elif __LDS_FILE_
	strcpy(sGroupName,"LDS_MENU");
#elif __TSA_FILE_
	strcpy(sGroupName,"TSA_MENU");
#endif
*/
#ifdef _ARX_2007
	acedRegCmds->removeGroup((ACHAR*)_bstr_t(sGroupName));
#else
	acedRegCmds->removeGroup(sGroupName);
#endif
}
