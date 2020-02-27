// ParseTowerFile.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ParseTowerFile.h"
#include "Tower.h"
#include "LibraryQuery.h"
#include "CryptBuffer.h"

CTower Ta;
CXhChar50 g_sDocTypeName="";
int g_nDocType=0;
WORD g_wDogModule = 0;
void InitLicense()
{
	HKEY hKey;
	char lic_file[MAX_PATH];
	ULONG retCode=-1;
	DWORD dwDataType,dwLength=MAX_PATH;
	DWORD version[2]={0,20130801};
	BYTE* pByteVer=(BYTE*)version;
	char sSubKey[MAX_PATH]="";
	BYTE cProductType=0;
#ifdef __TMA_FILE_
	pByteVer[0]=4;
	pByteVer[1]=1;
	pByteVer[2]=0;
	pByteVer[3]=5;
	strcpy(sSubKey,"Software\\Xerofox\\TMA\\Settings");
	cProductType=PRODUCT_TMA;
#elif defined(__LMA_FILE_)
	pByteVer[0]=1;
	pByteVer[1]=1;
	pByteVer[2]=0;
	pByteVer[3]=5;
	strcpy(sSubKey,"Software\\Xerofox\\LMA\\Settings");
	cProductType=PRODUCT_LMA;
#elif defined(__TSA_FILE_)
	pByteVer[0]=1;
	pByteVer[1]=8;
	pByteVer[2]=1;
	pByteVer[3]=2;
	strcpy(sSubKey,"Software\\Xerofox\\TSA\\Settings");
	cProductType=PRODUCT_TSA;
#elif defined(__TDA_FILE_)
	pByteVer[0]=1;
	pByteVer[1]=1;
	pByteVer[2]=0;
	pByteVer[3]=5;
	strcpy(sSubKey,"Software\\Xerofox\\TDA\\Settings");
	cProductType=PRODUCT_TDA;
#else
	pByteVer[0]=1;
	pByteVer[1]=1;
	pByteVer[2]=1;
	pByteVer[3]=2;
	strcpy(sSubKey,"Software\\Xerofox\\LDS\\Settings");
	cProductType=PRODUCT_LDS;
#endif
	RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey);
	if(RegQueryValueEx(hKey,_T("lic_file"),NULL,&dwDataType,(BYTE*)&lic_file[0],&dwLength)== ERROR_SUCCESS)
	{
#ifndef _LEGACY_LICENSE
		retCode=ImportLicFile(lic_file,cProductType,version);
#else
#ifdef _NET_DOG
		g_wDogModule=NetDogModule(lic_file);
		if(!InitNetLicEnv(g_wDogModule))
		{
			CString ss;
			ss.Format("%d号模块",g_wDogModule);
			AfxMessageBox("0#网络狗"+ss+"初始化失败!");
			return;
		}
#endif
		retCode=ImportLicFile(lic_file,version);
#endif
	}
	if(retCode!=0)
	{
		if(retCode==-1)
			AfxMessageBox("首次使用，还未指定过证书文件！");
		else if(retCode==1)
			AfxMessageBox("1#无法打开证书文件");
		else if(retCode==2)
			AfxMessageBox("2#证书文件遭到破坏");
		else if(retCode==3)
			AfxMessageBox("3#证书与加密狗不匹配");
		else if(retCode==4)
			AfxMessageBox("4#授权证书的加密版本不对");
		else if(retCode==5)
			AfxMessageBox("5#证书与加密狗产品授权版本不匹配");
		else if(retCode==6)
			AfxMessageBox("6#超出版本使用授权范围");
#ifndef _LEGACY_LICENSE
		ExitCurrentDogSession();
#elif defined(_NET_DOG)
		ExitNetLicEnv(m_wDogModule);
#endif
	}
}

typedef void (*PROCESS_CALLBACK_FUNC)(int percent,const char *sTitle);
PROCESS_CALLBACK_FUNC g_pProcessFunc=NULL;
double g_fInternalCoef = 0.3,g_fExternalCoef=1;
int g_nInternalStartPer = 0,g_nExternalStartPer=0;
void DisplayProcess(int percent,const char *sTitle)
{
	if(g_pProcessFunc)
	{
		int nProcess=g_nExternalStartPer+(int)((g_nInternalStartPer+(int)(percent*g_fInternalCoef))*g_fExternalCoef);
		g_pProcessFunc(nProcess,sTitle);
	}
}
bool OpenTaFileFromBuffer(CBuffer &buffer)
{
	Ta.Empty();
	if(g_pProcessFunc)
		Ta.DisplayProcess=DisplayProcess;
	else 
		Ta.DisplayProcess=NULL;
	CLDSPart::library=&globalLibrary;	//构件库
	CDesignThickPara::m_pTower=&Ta;		
	CLDSPoint::m_pTower=&Ta;
	CLDSVector::m_pTower=&Ta;

	buffer.ReadString(g_sDocTypeName);
#ifdef __LMA_FILE_
	if(!g_sDocTypeName.EqualNoCase("格构铁塔制造系统")&&!g_sDocTypeName.EqualNoCase("格构铁塔制造助手"))
#elif defined(__TMA_FILE_)
	if(!g_sDocTypeName.EqualNoCase("铁塔制造助手"))
#elif defined(__TSA_FILE_)
	if(!g_sDocTypeName.EqualNoCase("铁塔应力设计")&&!g_sDocTypeName.EqualNoCase("铁塔内力分析设计"))
#elif defined(__TDA_FILE_)
	if(!g_sDocTypeName.EqualNoCase("铁塔绘图助手"))
#else
	if(!g_sDocTypeName.EqualNoCase("格构铁塔设计与绘图系统"))
#endif
	{
		//AfxMessageBox("非预期的文件格式");
		;//return;
	}
	if(g_sDocTypeName.EqualNoCase("铁塔制造助手"))
		g_nDocType=1;
	else if(g_sDocTypeName.EqualNoCase("格构铁塔制造系统")||g_sDocTypeName.EqualNoCase("格构铁塔制造助手"))
		g_nDocType=2;
	else if(g_sDocTypeName.EqualNoCase("铁塔应力设计")||g_sDocTypeName.EqualNoCase("铁塔内力分析设计"))
		g_nDocType=3;
	else if(g_sDocTypeName.EqualNoCase("铁塔绘图助手"))
		g_nDocType=5;
	else //defined(__LDS_FILE_)||defined(__LDS_)
		g_nDocType=4;
	CXhChar50 version;
	DWORD cursor_pipeline_no;
	buffer.ReadString(version);
	sprintf(Ta.version,"%s",(char*)version);
#if defined(__TMA_FILE_)
	if(compareVersion(Ta.version,"4.2.1.0")>0||compareVersion(Ta.version,"4.0.0.19")<0)
#elif defined(__LMA_FILE_)
	if(compareVersion(Ta.version,"1.2.1.0")>0||compareVersion(Ta.version,"1.0.0.73")<0)
#elif defined(__TSA_FILE_)
	if(compareVersion(Ta.version,"2.0.1.0")>0)
#elif defined(__TDA_FILE_)
	if(compareVersion(Ta.version,"1.2.1.0")>0)
#else
	if(compareVersion(Ta.version,"1.2.1.0")>0)
#endif
	{
		//AfxMessageBox("此文件版本号太高或太低,不能直接读取此文件");
		;//return;
	}
	try
	{
		Ta.user_pipeline_no=buffer.ReadDword();
		cursor_pipeline_no=buffer.ReadDword();
		DWORD buffer_len=buffer.ReadDword();
		CBuffer towerBuffer;
		towerBuffer.Write(buffer.GetCursorBuffer(),buffer_len);
		bool bEncryptByAES=false;
		if(g_nDocType==1&&compareVersion(Ta.version,"4.1.3.0")>=0)
			bEncryptByAES=true;
		if(g_nDocType==2&&compareVersion(Ta.version,"1.1.3.0")>=0)
			bEncryptByAES=true;
		if(g_nDocType==3&&compareVersion(Ta.version,"2.1.0.0")>=0)
			bEncryptByAES=true;
		if((g_nDocType==4||g_nDocType==5)&&compareVersion(Ta.version,"1.3.0.0")>=0)
			bEncryptByAES=true;
		DecryptBuffer(towerBuffer,bEncryptByAES);
		Ta.FromBuffer(towerBuffer,g_nDocType);
		//bom.Empty();
		//更新板上螺栓孔的的射线号
		CSuperSmartPtr<CLDSPart>pSuperPart;
		for(CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			memset(pBolt->relaRayArr,0,sizeof(CLDSBolt::RELA_RAYPART)*4);
		for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
			pSuperPart->ResetSonBoltRayNo();	//重新设置螺栓射线号,否则无法正确设置螺栓引用射线号进而不能正确标注螺栓特殊间距 wht 11-07-28
		for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
		{
			if(pSuperPart->GetClassTypeId()==CLS_PLATE)
				pSuperPart.PlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pSuperPart);
			else if(pSuperPart->GetClassTypeId()==CLS_PARAMPLATE)
				pSuperPart.ParamPlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pSuperPart);
		}
		return true;
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return false;
}

//打开文件
bool OpenTaFile(CString sFilePath)
{
	CBuffer buffer;
	FILE *fp=fopen(sFilePath,"rb");
	if(fp==NULL)
	{
		AfxMessageBox("打开文件失败！");
		return FALSE;
	}
	fseek(fp,0,SEEK_END);
	long len=ftell(fp);
	buffer.Write(NULL,len);
	fseek(fp,0,SEEK_SET);
	fread(buffer.GetBufferPtr(),1,len,fp);
	fclose(fp);
	buffer.SeekToBegin();
	return OpenTaFileFromBuffer(buffer);
}
class CLog2FileLife{
private:
	ILog2File *m_pOldLogFile;
public:
	CLog2FileLife(ILog2File *pLogFile){
		m_pOldLogFile=CLDSObject::log2file;
		CLDSObject::log2file=pLogFile;
	}
	~CLog2FileLife(){
		CLDSObject::log2file=m_pOldLogFile;
	}
};
//获取TID缓存
CBuffer g_tidBuffer(50000000);	//默认内存递增梯度大小为50M，及小了因频繁分配内存会导致很慢
int PrepareTidBuffer(const char *sFilePath,void (*DisplayProcessFunc)(int percent,const char *sTitle),
					 int startProcess/*=0*/,double processCoef/*=1*/,void *pLogFile/*=NULL*/)
{	
	g_pProcessFunc=DisplayProcessFunc;
	g_nExternalStartPer=startProcess;
	g_fExternalCoef=processCoef;
	//1.打开文件
	g_nInternalStartPer=0;
	g_fInternalCoef=0.3;
	CLog2FileLife log2FileLife((ILog2File*)pLogFile);
	if(!OpenTaFile(sFilePath))
		return 0;
	//2.生成Tid缓存
	g_nInternalStartPer=30;
	g_fInternalCoef=0.7;
	g_tidBuffer.ClearContents();
	Ta.ToTowerSolidDataExchangeBuffer(g_tidBuffer);
	DisplayProcess(100,NULL);
	return g_tidBuffer.GetLength();
}
int PrepareTidBufferFromTowerBuffer(char *buf,int buf_len,void (*DisplayProcessFunc)(int percent,const char *sTitle),
									int startProcess/*=0*/,double processCoef/*=1*/,void *pLogFile/*=NULL*/)
{	
	g_pProcessFunc=DisplayProcessFunc;
	g_nExternalStartPer=startProcess;
	g_fExternalCoef=processCoef;
	CLog2FileLife((ILog2File*)pLogFile);
	//1.打开文件
	g_nInternalStartPer=0;
	g_fInternalCoef=0.3;
	CBuffer buffer(buf_len);
	buffer.Write(buf,buf_len);
	buffer.SeekToBegin();
	if(!OpenTaFileFromBuffer(buffer))
		return 0;
	//2.生成Tid缓存
	g_nInternalStartPer=30;
	g_fInternalCoef=0.7;
	g_tidBuffer.ClearContents();
	Ta.ToTowerSolidDataExchangeBuffer(g_tidBuffer);
	DisplayProcess(100,NULL);
	return g_tidBuffer.GetLength();
}
bool ReadTidBuffer(char *tid_buffer)
{
	memcpy(tid_buffer,g_tidBuffer.GetBufferPtr(),g_tidBuffer.GetLength());
	return true;
}
