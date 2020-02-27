// LDSDoc.cpp : implementation of the CLDSDoc class
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "MainFrm.h"
#include "Tower.h"
#include "BodyCtrlPtDlg.h"
#include "NewFileDlg.h"
#include "FilePropertyDlg.h"
#include "dlg.h"
#include "GSMH.h"
#include "NetDogAPI.h"
#include "LDSView.h"
#include "env_def.h"
#include "MsgDef.h"
#include "UpdateUserDataDlg.h"
#include "excel9.h"
#include "ExcelExportOptionDlg.h"
#include "ExcelTblManager.h"
#include "Query.h"
#include "MD5.h"
#include "ScrTools.h"
#include "CryptBuffer.h"
#include "InputAnValDlg.h"
#include "GlobalFunc.h"
#include "ExportExcelOptionDlg.h"
#include "TubeSpecialProcessDlg.h"
#include "PartTablePreviewDlg.h"
#include "XhCharString.h"
#include "Variant.h"
#include "ExcelOper.h"
#include "TtaDataFile.h"
#include ".\Log\XhLog.h"
#include "OutputInfo.h"
#include "SortFunc.h"
#include "Expression.h"
#include "PreProcessor.h"
#include "PostProcessReport.h"
#include "RevisionOperDlg.h"
#include "TowerBOM.h"
#include "ArrayList.h"
#include "dlg.h"
#include "LicFuncDef.h"
#include "OriginalData.h"
#include "LocalFeatureDef.h"
#include "LibraryQuery.h"
#include "InputAnValDlg.h"
#include "ClientId.h"
#include "LdsPortalLocale.h"
#ifdef __CONNECT_REMOTE_SERVER_
#include "TMS.h"
#endif
#if defined(__PART_DESIGN_INC_)||defined(__RAPID_ROUGH_3DMODEL_)
#include "I3DS.h"
#include "ISTL.h"
#include "ModDataFile.h"
#include "ElectricTower.h"
#include "InputGimInfoDlg.h"
#endif
#if defined(__TMA_)||defined(__LMA_)
#include "DataCompare.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NONE_ENCRYPT_STRUTOWER_FILE
/////////////////////////////////////////////////////////////////////////////
// CLDSDoc

IMPLEMENT_DYNCREATE(CLDSDoc, CDocument)

BEGIN_MESSAGE_MAP(CLDSDoc, CDocument)
	//{{AFX_MSG_MAP(CLDSDoc)
	ON_COMMAND(ID_FILE_PROPERTY, OnFileProperty)
	ON_COMMAND(ID_RELOAD_FILE, OnReloadFile)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_IN_NEW_WINDOW, &CLDSDoc::OnFileOpenInNewWindow)
	ON_COMMAND(ID_CHECK_MODEL_BY_CMDDRW, OnCheckModelByCmdDrawFile)
	ON_COMMAND(ID_UPDATE_DATA_FILE, OnUpdateDataFile)
	ON_COMMAND(ID_EXPORT_EXCEL_FILE, OnExportExcelFile)
	ON_COMMAND(ID_EXPORT_SPEC_PART_EXCEL_FILE, OnExportSpecPartExcelFile)
	ON_COMMAND(ID_EXPORT_TXT_PART_DATA, OnExportTxtPartData)
	ON_COMMAND(ID_EXPORT_TXT_LS_DATA, OnExportTxtLsData)
	ON_COMMAND(ID_EXPORT_TXT_PART_DATA2, OnExportTxtPartData2)
	ON_COMMAND(ID_EXPORT_TXT_PART_DATA3, OnExportTxtPartData3)
	ON_COMMAND(ID_EXPORT_TXT_PART_DATA4, OnExportTxtPartData4)
	ON_COMMAND(ID_EXPORT_JG_EXCEL_FILE, OnExportJgExcelFile)
	ON_COMMAND(ID_EXPORT_TUBE_SPECIAL_PROCESS_TABLE, OnExportTubeSpecialProcessTable)
	ON_COMMAND(ID_EXPORT_EXCEL_FILE2, OnExportExcelFile2)
	ON_COMMAND(ID_EXPORT_EXCEL_FILE3, OnExportExcelFile3)
	ON_COMMAND(ID_EXPORT_SWAP_INFO_FILE, OnExportSwapInfoFile)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_READ_TTA_FILE, &CLDSDoc::OnReadTtaFile)
	ON_COMMAND(ID_TOWER_GIM_PROPERTY, &CLDSDoc::OnTowerGimProperty)
	//ON_COMMAND(ID_ADD_BRACE_ELEM_DATA_FILE, &CLDSDoc::OnAddBraceElemDataFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLDSDoc construction/destruction
//添加一行数据到Excel
static void AddRowToExcelSheet(_Worksheet &excel_sheet,int iRow,CStringArray &str_arr,double *col_arr=NULL,BOOL bWrapText=FALSE)
{
	Range excel_range;
	LPDISPATCH pRange;
	for(int i=0;i<str_arr.GetSize();i++)
	{
		int nLen=i/26;
		int nSubLen=i%26;
		if(nLen>26)
			return;
		char cell[50]="";
		if(nLen>0)
			_snprintf(cell,49,"%c%c%d",'A'+nLen-1,'A'+nSubLen,iRow);
		else
			_snprintf(cell,49,"%c%d",'A'+nSubLen,iRow);
		//
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetValue(COleVariant(str_arr[i]));	//,VT_BSTR
		if(col_arr)
			excel_range.SetColumnWidth(COleVariant(col_arr[i]));
		if(bWrapText)
			excel_range.SetWrapText(COleVariant((BYTE)1));
		excel_range.DetachDispatch();
		pRange->Release();
		
	}
}
//读取一行数据从Excel
static void ReadRowFromExcelSheet(_Worksheet &excel_sheet,int iRow,int nColCount,CStringArray &str_arr)
{
	Range excel_range;
	LPDISPATCH pRange;
	for(int i=0;i<nColCount;i++)
	{
		int nLen=i/26;
		int nSubLen=i%26;
		if(nLen>26)
			return;
		char cell[50]="";
		if(nLen>0)
			_snprintf(cell,49,"%c%c%d",'A'+nLen-1,'A'+nSubLen,iRow);
		else
			_snprintf(cell,49,"%c%d",'A'+nSubLen,iRow);
		//
		pRange=excel_sheet.GetRange(COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		COleVariant cell_value=excel_range.GetValue();
		excel_range.DetachDispatch();
		pRange->Release();
		if(cell_value.vt==VT_EMPTY)
			str_arr.Add("");
		else
			str_arr.Add(cell_value.bstrVal);
	}
}
CLDSDoc::CLDSDoc()
{
	m_nTimer=0;
	m_bStartAutoSaveThread=false;
	m_iMaxBakSerial=0;
	theApp.GetFileVersion(m_sFileVersion);
	theApp.GetProductVersion(m_sProductVersion);
#if defined(__TMA_FILE_)||defined(__TMA_)
	m_sDocType = "铁塔制造助手";
#elif defined(__LMA_FILE_)||defined(__LMA_)
	m_sDocType = "格构铁塔制造助手";
#elif defined(__TSA_FILE_)||defined(__TSA_)
	m_sDocType = "铁塔内力分析设计";	//铁塔内力分析设计软件
#else //defined(__LDS_FILE_)||defined(__LDS_)
	m_sDocType = "格构铁塔设计与绘图系统";	//格构铁塔绘图及放样软件
#endif
	m_idServerFile=0;
	Ta.user_pipeline_no = DogSerialNo();
}

CLDSDoc::~CLDSDoc()
{
}

BOOL CLDSDoc::OnNewDocument()
{
	Ta.m_uiOriginalDogKey=DogSerialNo();
	Ta.m_uiLastSaveDogKey=DogSerialNo();
	if(Ta.user_pipeline_no==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Because can't find the encryption dog, so can not create a new file correctly!");
#else 
		AfxMessageBox("找不到加密狗,无法正确创建新文件!");
#endif
		return FALSE;
	}
	if (!CDocument::OnNewDocument())
		return FALSE;
	UpdateAllViews(NULL);

#ifdef AFX_TARG_ENU_ENGLISH
	CLDSDoc::SetTitle("No Title");
#else 
	CLDSDoc::SetTitle("无标题");
#endif

	return TRUE;
}

void CALLBACK EXPORT AutoSaveProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(nIDEvent==theApp.GetLDSDoc()->m_nTimer)
		theApp.GetLDSDoc()->AutoSaveBakFile();
}

/////////////////////////////////////////////////////////////////////////////
// CLDSDoc serialization
BOOL LoadTsaFile(CArchive &ar);
#ifndef __TSA_FILE_
void CLDSDoc::ImportTsaFile(LPCTSTR lpszPathName) 
{
	/*CFileDialog dlg(TRUE,"tsa","TSA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔内力分析设计文件(*.tsa)|*.tsa|所有文件(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;*/
	CFile *pFile = new CFile(lpszPathName, CFile::modeRead);//modeReadWrite | CFile::shareDenyWrite);
	CArchive *pDoc  = new CArchive(pFile,CArchive::load);
	//
	CString sDocTypeName,sFileVersion;
	Ta.Empty();
	Ta.InitTower();
	BeginWaitCursor();
	try{
		*pDoc >> sDocTypeName;
#ifdef AFX_TARG_ENU_ENGLISH
		if(sDocTypeName.Compare("Tower Stress Design") !=0&&sDocTypeName.Compare("Tower Internal Stress Design") !=0)
		{
			SetTitle("No Title");
			throw "This is the unexpected file type,so the file open failed!";
		}
#else 
		if(sDocTypeName.Compare("铁塔应力设计") !=0&&sDocTypeName.Compare("铁塔内力分析设计") !=0)
		{
			SetTitle("无标题");
			throw "非预期的文件格式，文件打开失败!";
		}
#endif
		
		*pDoc >> sFileVersion;
		_snprintf(Ta.version,19,"%s",sFileVersion);
		if(compareVersion(Ta.version,"1.7.0.0")<0)
			LoadTsaFile(*pDoc);
		else if(compareVersion(Ta.version,"1.8.1.2")>0)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The file's version number is too high or too low,so it can't read directly";
#else 
			throw "此文件版本号太高或太低,不能直接读取此文件";
#endif
		else
		{
			DWORD cursor_pipeline_no=0;
			*pDoc>>Ta.user_pipeline_no;
			*pDoc>>cursor_pipeline_no;
			DWORD buffer_len;
			*pDoc>>buffer_len;
			BYTE *pBuf = new BYTE[buffer_len];
			pDoc->Read(pBuf,buffer_len);
			CBuffer buffer(buffer_len);
			buffer.Write(pBuf,buffer_len);
			delete []pBuf;
			DecryptBuffer(buffer);
			Ta.FromBuffer(buffer,3);
		}
	}
	catch(char *sError)
	{
		AfxMessageBox(sError);
	}
	EndWaitCursor();
	pDoc->Close();
	delete pDoc;
	delete pFile;
}
#endif
/* 用于修订过往错误数据文件用
void SaveAsFile(CBuffer& buffer)
{
	char* sFileVersion="4.0.1.11";
	int part_n_arr[7]={2991,2992,2993,2994,2995,2996,2997};
	int pos_arr[7]={2378775,2379105,2379449,2380822,2382211,2382235,2383624};
	for(int i=0;i<7;i++)
	{
		char filename[20];
		sprintf(filename,"c:\\rev%d.tma",i);
		char doctype[50]="铁塔制造助手";
		FILE* fp=fopen(filename,"wb");
		BYTE verlen=8;
		verlen=12;
		fwrite(&verlen,1,1,fp);
		fwrite(doctype,12,1,fp);
		verlen=8;
		fwrite(&verlen,1,1,fp);
		fwrite(sFileVersion,8,1,fp);
		fwrite(&Ta.user_pipeline_no,4,1,fp);
		fwrite(&Ta.user_pipeline_no,4,1,fp);
		CBuffer copy(buffer.GetLength());
		copy.Write(buffer.GetBufferPtr(),buffer.GetLength());
		copy.SeekPosition(250564);
		copy.WriteInteger(part_n_arr[i]);
		copy.SeekPosition(pos_arr[i]);
		copy.WriteInteger(0);
		copy.WriteInteger(0);
		EncryptBuffer(copy);
		DWORD buf_len=copy.GetLength();
		fwrite(&buf_len,4,1,fp);
		fwrite(copy.GetBufferPtr(),buf_len,1,fp);
		fclose(fp);
	}
}*/
void CLDSDoc::Serialize(CArchive& ar)
{
	DWORD cursor_pipeline_no=DogSerialNo();
	BeginWaitCursor();
	CLogErrorLife loglife;
	try{
		char file_version[20],product_version[20];
		theApp.GetAppVersion(file_version,product_version);
		if(ar.IsStoring())
		{
			CString version;
			int doc_type=PRODUCT_TYPE;
			bool bSpecifiedVersion=false;
			if(g_sysPara.iFileSaveDocType>0&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
			{
				doc_type=g_sysPara.iFileSaveDocType;
				g_sysPara.iFileSaveDocType=0;	//为避免错误存储为其余文件格式，每次执行完另其其余格式后立即恢复iFileSaveDocType=0;
				bSpecifiedVersion=true;
				if(doc_type==1)
					strcpy(file_version,"5.1.7.0");
				else if(doc_type==2)
					strcpy(file_version,"2.0.8.0");
				else if(doc_type==5)
					strcpy(file_version,"1.3.8.0");
				else
					bSpecifiedVersion=false;
			}
			if(doc_type==1)
				ar<<CString("铁塔制造助手");
			else if(doc_type==2)
				ar<<CString("格构铁塔制造助手");
			else if(doc_type==3)
				ar<<CString("铁塔内力分析设计");
			else if(doc_type==5)
				ar<<CString("铁塔绘图助手");
			else if(doc_type==4)
				ar<<CString("格构铁塔设计与绘图系统");
			//ar <<m_sDocType;	//直接用字符串代替此变量，以便减少黑客攻击导致数据破坏的可能性　wjh 2011.12.2
#ifdef __TMA_
			DWORD dwSaveFileVersion=ValidateLocalizeFeature(FEATURE::SAVEAS_FILE_VERSION);
#else
			DWORD dwSaveFileVersion=0;
#endif
			if(g_sysPara.uFileSaveVersion>0&&(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT)||dwSaveFileVersion>0))
			{
				ToStringVersion(g_sysPara.uFileSaveVersion,file_version);
				g_sysPara.uFileSaveVersion=0;	//为避免错误存储为旧版本，每次执行完旧版存储立即恢复uFileSaveVersion=0;
			}
			else if(!bSpecifiedVersion)
				strcpy(Ta.version,file_version);
			char bEncryptByAES=false;
			if(doc_type==1)
			{
				if(compareVersion(file_version,"5.1.7.0")>=0)
					bEncryptByAES=2;	//带指纹的加密模式
				else if(compareVersion(file_version,"4.1.3.0")>=0)
					bEncryptByAES=true;
			}
			else if(doc_type==2)
			{
				if(compareVersion(file_version,"2.0.8.0")>=0)
					bEncryptByAES=2;	//LMA
				else if(compareVersion(file_version,"1.1.3.0")>=0)
					bEncryptByAES=true;	//LMA
			}
			else if(doc_type==3&&compareVersion(file_version,"2.1.0.0")>=0)
				bEncryptByAES=true;	//TSA
			else if(doc_type==4||doc_type==5)
			{
				if(compareVersion(file_version,"1.3.8.0")>=0)
					bEncryptByAES=2;	//LMA
				else if(compareVersion(file_version,"1.3.0.0")>=0)
					bEncryptByAES=true;	//LDS or TDA
			}
			CString sFileVersion=file_version;
			ar << sFileVersion;
			ar <<Ta.user_pipeline_no;
			ar <<cursor_pipeline_no;
			//通过EncryptFileEx验算得到的dwSafeBuffSize保证加密转换时，内存块长度不变
			//否则超大文件存储时有可能导致文件内存文件节节不对齐错位现像 wjh-2019.8.13
			DWORD SAFE_BUFFSIZE=0xa00000;	//SAFE_BUFFSIZE必须是16的整数倍
			CBuffer buffer(SAFE_BUFFSIZE);//10000000);
			//个别情况存在内存碎片过多，写入时内存分配失败进而导致系统异常文档写入失败的情况 wjh-2019.6.21
			//因此增加了写入时的额外辅助文件
			CXhChar500 szSaveBuffFile("%s__overflowsavebuf.tmplds",(char*)theApp.szWorkPath);
			FILE* fpOverflowBuffFile=fopen(szSaveBuffFile,"w+b");
			CSelfCloseFile closefile(fpOverflowBuffFile);
			UINT uiRWBuffSize= AfxGetApp()->GetProfileInt("Settings","FileRW_BUFFSIZE",50);
			UINT FileRW_BUFFSIZE=0x100000*uiRWBuffSize;	//默认50M缓存空间
			buffer.InitOverflowBuffFile(fpOverflowBuffFile,FileRW_BUFFSIZE);	//超出40M采用溢出缓存文件
			Ta.ToBuffer(buffer,DogSerialNo(),file_version,doc_type);
			UINT uiOverflowBuffBytes=buffer.DetachOverflowBuffFile();	//分离溢出缓存文件
			if (!EncryptBuffer(buffer, bEncryptByAES))
				throw "授权证书异常，重新申请授权后再试!";
			DWORD dwFileLen = buffer.GetLength()+uiOverflowBuffBytes;
			ar.Write(&dwFileLen,sizeof(DWORD));
			ar.Write(buffer.GetBufferPtr(),buffer.Length);
			if (uiOverflowBuffBytes>0)
			{
				const UINT READBLOCKSIZE=0x10000;	//64K
				char _READBLOCK[READBLOCKSIZE];
				UINT dwSwapBytes=READBLOCKSIZE;
				fseek(fpOverflowBuffFile,0,SEEK_SET);
				for (UINT uiAddr=0;uiAddr<uiOverflowBuffBytes;uiAddr+=READBLOCKSIZE)
				{
					if (uiAddr+READBLOCKSIZE>uiOverflowBuffBytes)
						dwSwapBytes=uiOverflowBuffBytes-uiAddr;
					fread(_READBLOCK,dwSwapBytes,1,fpOverflowBuffFile);
					ar.Write(_READBLOCK,dwSwapBytes);
				}
			}
			closefile.EarlyCloseFile();
			::DeleteFile(szSaveBuffFile);
#if defined(__PART_DESIGN_INC_)//||defined(__SMART_DRAW_INC_)
			//需要进行版本日志记录时应生成最新的模型修改日志数据
			if(g_sysPara.m_bSaveLogVersion)
			{
				CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
				CTower logTower;
				if(pLogModel->GetLength()>0)
				{
					logTower.FromBuffer(*pLogModel,0,true);
					Ta.CompareVersion(&logTower,&logger);
					CAttachBuffer* pLoggerBuffer=Ta.AttachBuffer.GetValue(CTower::LOGGER);
					logger.ToBuffer(*pLoggerBuffer);
				}
				CLogViewerDlg *pLogViewerDlg=((CMainFrame*)AfxGetMainWnd())->GetLogViewerPage();
				if (pLogViewerDlg)
					pLogViewerDlg->RefreshLogVersionList();
			}
			//写入挂点信息模型
			if(gxWirePointModel.m_hashWirePoints.Count>0)
			{
				CBuffer* pUniWireModel=Ta.AttachBuffer.GetValue(CTower::UNI_WIREPOINT_MODEL);
				if(pUniWireModel)
				{
					pUniWireModel->ClearContents();
					gxWirePointModel.ToBuffer(*pUniWireModel);
				}
			}
#endif
#ifdef __TMA_
			if(PRODUCT_FUNC::IsHasInternalTest())
			{	//记录铁塔数据模型校审项
				CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
				pCheckBuffer->ClearContents();
				xDataCheckModel.ToBuffer(*pCheckBuffer);
			}
#endif
			CBuffer attachBuffer(1000000);
			Ta.ToAttachBuffer(attachBuffer);
			ar<<attachBuffer.GetLength();	//附加缓存整体长度
			ar.Write(attachBuffer.GetBufferPtr(),attachBuffer.GetLength());
			Ta.SetDocModifiedFlag(FALSE);
			ResetAutoSaveTimer();
		}
		else// if(ar.IsLoading())
		{
			CString sDocTypeName,sFileVersion;
			theApp.GetFileVersion(m_sFileVersion);
			ar >> sDocTypeName;
#if   defined(__TMA_FILE_)
			if(sDocTypeName.Compare("铁塔制造助手") !=0)
#elif defined(__LMA_FILE_)
			if(sDocTypeName.Compare("铁塔制造助手") !=0&&sDocTypeName.Compare("格构铁塔制造系统") !=0&&sDocTypeName.Compare("格构铁塔制造助手") !=0)
#elif defined(__TSA_FILE_)
			if(sDocTypeName.Compare("铁塔应力设计") !=0&&sDocTypeName.Compare("铁塔内力分析设计") !=0)
#elif defined(__TDA_FILE_)
			if( sDocTypeName.Compare("铁塔绘图助手") !=0&&sDocTypeName.Compare("铁塔制造助手")!=0&&
				sDocTypeName.Compare("格构铁塔制造系统") !=0&&sDocTypeName.Compare("格构铁塔制造助手") !=0)
#else //defined(__LDS_FILE_)||defined(__LDS_)
			//theApp.m_bAnalysisWorkEnv=TRUE;
			if(theApp.m_eEnvType==SOLID_ENV)
				theApp.m_eEnvType=LINE_ENV;	//防止当前为显示实体状态时打开文件后，无法显示单线问题 wjh-2013.4.6
			g_pSolidSet->SetDisplayType(DISP_LINE);	//受力分析模式下显示单线
			if( sDocTypeName.Compare("铁塔制造助手") !=0&&
				sDocTypeName.Compare("铁塔绘图助手") !=0&&
				sDocTypeName.Compare("格构铁塔制造系统") !=0&&sDocTypeName.Compare("格构铁塔制造助手") !=0&&
				sDocTypeName.Compare("格构铁塔设计与绘图系统") !=0)
#endif
			{
#ifdef AFX_TARG_ENU_ENGLISH
				SetTitle("No Title");//throw CArchiveException(CArchiveException::badClass);
				throw "This is the unexpected file type,so the file open failed!";
#else 
				SetTitle("无标题");//throw CArchiveException(CArchiveException::badClass);
				throw "非预期的文件格式，文件打开失败!";
#endif
			}
			int doc_type=0;
			if(sDocTypeName.Compare("铁塔制造助手") ==0)
				doc_type=1;
			else if(sDocTypeName.Compare("格构铁塔制造系统") ==0||sDocTypeName.Compare("格构铁塔制造助手") ==0)
				doc_type=2;
			else if(sDocTypeName.Compare("铁塔应力设计") ==0||sDocTypeName.Compare("铁塔内力分析设计") ==0)
				doc_type=3;
			else if(sDocTypeName.Compare("铁塔绘图助手") ==0)
				doc_type=5;
			else //defined(__LDS_FILE_)||defined(__LDS_)
				doc_type=4;
			
			ar >> sFileVersion;
			CXhChar16 productfile_version("%s",m_sFileVersion);
			_snprintf(Ta.version,19,"%s",sFileVersion);
#if defined(__TMA_FILE_)
			if(compareVersion(Ta.version,productfile_version)>0)
#elif defined(__LMA_FILE_)
			if(doc_type==2&&compareVersion(Ta.version,productfile_version)>0||
			  (doc_type==1&&compareVersion(Ta.version,"5.2.2.0")>0))
#elif defined(__TDA_FILE_)
			if(doc_type==5&&compareVersion(Ta.version,productfile_version)>0||
			  (doc_type==2&&compareVersion(Ta.version,"2.1.2.0")>0)||
			  (doc_type==1&&compareVersion(Ta.version,"5.2.2.0")>0))
#elif defined(__LDS_FILE_)
			if(doc_type==4&&compareVersion(Ta.version,productfile_version)>0||
			  (doc_type==5&&compareVersion(Ta.version,"1.3.10.0")>0)||
			  (doc_type==2&&compareVersion(Ta.version,"2.1.2.0")>0)||
			  (doc_type==1&&compareVersion(Ta.version,"5.2.2.0")>0))
#else
			if(compareVersion(Ta.version,productfile_version)>0)
#endif
#ifdef AFX_TARG_ENU_ENGLISH
				throw "The file's version number is too high or too low,so it can't read directly";
#else 
				throw "此文件版本号太高,不能直接读取此文件";
#endif
			DWORD buffer_len;
#if defined(__TSA_FILE_)
			if(compareVersion(Ta.version,"1.7.0.0")<0)
				LoadTsaFile(ar);
			else
			{
#endif
				ar>>Ta.user_pipeline_no;
				ar>>cursor_pipeline_no;
				ar>>buffer_len;
				BYTE_ARRAY buffpool(buffer_len);
				ar.Read(buffpool,buffer_len);
				CBuffer buffer(buffpool,buffer_len);
				if((Ta.user_pipeline_no=DogSerialNo())==0)
#ifdef AFX_TARG_ENU_ENGLISH
					throw "Can't find the encryption dog,so unable to open the data file!";
#else 
					throw "找不到加密狗,无法打开数据文件!";
#endif

#if defined(__TMA_FILE_)
				if(compareVersion(Ta.version,"4.0.0.19")<0)
#elif defined(__LMA_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#elif defined(__TDA_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#elif defined(__LDS_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#endif
				{
					CXhChar500 version_convert_exefile(theApp.execute_path);
					if(doc_type==1)
						version_convert_exefile.Append("TMAVersion.exe");
					else //if(doc_type==2)
						version_convert_exefile.Append("LMAVersion.exe");
#ifdef AFX_TARG_ENU_ENGLISH
					PutMessage(theApp.m_pMainWnd,"It is converting the old file version, please be patient!");
#else 
					PutMessage(theApp.m_pMainWnd,"正在进行旧格式文件版本转换，请耐心等待!");
#endif
					if(!VersionOldLTMAFile(theApp.m_sLicFile,version_convert_exefile,buffer))
					{
						PutMessage(theApp.m_pMainWnd,"");
#ifdef AFX_TARG_ENU_ENGLISH
						throw "L/TMA old file converts failure!";
#else 
						throw "L/TMA旧文件转换失败!";
#endif
					}
					PutMessage(theApp.m_pMainWnd,"");
				}
				char bEncryptByAES=false;
				if(doc_type==1)
				{
					if(compareVersion(Ta.version,"5.1.7.0")>=0)
						bEncryptByAES=2;	//带指纹的加密模式
					else if(compareVersion(Ta.version,"4.1.3.0")>=0)
						bEncryptByAES=true;
				}
				else if(doc_type==2)
				{
					if(compareVersion(Ta.version,"2.0.8.0")>=0)
						bEncryptByAES=2;	//LMA
					else if(compareVersion(Ta.version,"1.1.3.0")>=0)
						bEncryptByAES=true;	//LMA
				}
				else if(doc_type==3&&compareVersion(Ta.version,"2.1.0.0")>=0)
					bEncryptByAES=true;	//TSA
				else if(doc_type==4||doc_type==5)
				{
					if(compareVersion(Ta.version,"1.3.8.0")>=0)
						bEncryptByAES=2;	//LMA
					else if(compareVersion(Ta.version,"1.3.0.0")>=0)
						bEncryptByAES=true;	//LDS or TDA
				}
				DecryptBuffer(buffer,bEncryptByAES,cursor_pipeline_no);
				if (!Ta.FromBuffer(buffer,doc_type))
				{
					DisplayProcess(100);
					Ta.m_uiOriginalDogKey=cursor_pipeline_no;//文件最初创建时的加密锁号
					Ta.m_uiLastSaveDogKey=cursor_pipeline_no;//文件最近一次保存使用的加密锁号
				}
				CLinkJGDlg::InitPreferAngleMat(&Ta);
				//如果有附加缓存区
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
				DWORD dwCurPos=(DWORD)ar.GetFile()->GetPosition();
				DWORD dwFileLen=(DWORD)ar.GetFile()->GetLength();
				if(dwFileLen>dwCurPos&&compareVersion(Ta.version,"1.2.0.3")<0)
				{	//有附加缓存区时读取智能绘图缓存区数据
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
					CAttachBuffer* pDrawBuff=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
					CString bufname;
					ar>>bufname;
					DWORD buf_len=0;
					ar>>buf_len;	//BOM_CONFIG+SMART_DRAW两个缓存总长
					ar>>buf_len;	//BOMCONFIG 缓存长度
					pBomBuff->SetBlockSize(buf_len);
					char* buf=new char[buf_len];
					ar.Read(buf,buf_len);
					pBomBuff->Write(buf,buf_len);
					delete []buf;
					ar>>buf_len;	//SMART_DRAW 缓存长度
					pDrawBuff->SetBlockSize(buf_len);
					buf=new char[buf_len];
					ar.Read(buf,buf_len);
					pDrawBuff->Write(buf,buf_len);
					delete []buf;
					//ar.Write(m_attachBuffer.GetBufferPtr(),m_attachBuffer.GetLength());
				}
				else
#endif

			if((doc_type==5&&compareVersion(Ta.version,"1.2.0.3")>=0)||
			   (doc_type==4&&compareVersion(Ta.version,"1.2.0.3")>=0)||
			   (doc_type==3&&compareVersion(Ta.version,"2.0.0.3")>=0)||
			   (doc_type==2&&compareVersion(Ta.version,"1.2.0.3")>=0)||
			   (doc_type==1&&compareVersion(Ta.version,"4.2.0.3")>0))
			{
				DWORD buf_len=0;
				DWORD dwRead=ar.Read(&buf_len,4);
				if(dwRead==4&&buf_len>0)
				{
					CHAR_ARRAY buf(buf_len);
					CBuffer attachBuffer(buf,buf_len);
					ar.Read(buf,buf_len);
					Ta.FromAttachBuffer(attachBuffer);
				}
			}
#if defined(__PART_DESIGN_INC_)//||defined(__SMART_DRAW_INC_)
				//记录原始铁塔数据模型以便比对记录改动日志
				CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
				pLogModel->ClearContents();
				pLogModel->SetBlockSize(buffer.GetLength());
				Ta.ToBuffer(*pLogModel,DogSerialNo(),file_version);
				CBuffer* pLoggerBuf=Ta.AttachBuffer.GetValue(CTower::LOGGER);
				if(pLoggerBuf&&pLoggerBuf->Length>0)
					logger.FromBuffer(*pLoggerBuf);
				else
					logger.EmptyLogVersion();
				//挂点模型缓存
				CBuffer* pUniWireModel=Ta.AttachBuffer.GetValue(CTower::UNI_WIREPOINT_MODEL);
				if(pUniWireModel&&pUniWireModel->Length>0)
					gxWirePointModel.FromBuffer(*pUniWireModel);
				else
					gxWirePointModel.m_bGimWireModelInherited=false;
				if(doc_type==4&&!gxWirePointModel.m_bGimWireModelInherited)
					Ta.RetrieveWireModelFromNodes(&gxWirePointModel);
#endif

#if defined(__TMA_)||defined(__LMA_)
				if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE) != 0 ||
					VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
				{
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_COMPARE_MODEL);
					if(pBomBuff)
					{
						pBomBuff->SeekToBegin();
						globalDataCmpModel.ReadFromDrawAndBomBuffer(pBomBuff);
						globalDataCmpModel.ComparePartData();	//从缓存加载数据后应重新对比数据 wht 18-11-14
					}
					else
						globalDataCmpModel.Empty();
				}
				if(PRODUCT_FUNC::IsHasInternalTest())
				{	//记录铁塔数据模型校审项
					CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
					if(pCheckBuffer&&pCheckBuffer->GetLength()>0)
					{
						pCheckBuffer->SeekToBegin();
						xDataCheckModel.FromBuffer(*pCheckBuffer);
					}
				}
#endif
#if defined(__TSA_FILE_)||defined(__TSA_)
			}
#endif	
			Ta.SetDocModifiedFlag(FALSE);
			SetModifiedFlag(FALSE);
			/*int hits=0,hitsOfAngles=0,hitsOfPlates=0,hitsOfNodes=Ta.Node.GetNodeNum();
			for (CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
			{
				if (pPart->IsBolt())
					hits++;
				if (pPart->IsAngle())
					hitsOfAngles++;
				if (pPart->IsPlate())
					hitsOfPlates++;
			}
			int boltalloc=hits*1612;*/
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100,NULL);
	}
	EndWaitCursor();
}
/////////////////////////////////////////////////////////////////////////////
// CLDSDoc diagnostics

#ifdef _DEBUG
void CLDSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLDSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLDSDoc commands
#ifdef __PART_DESIGN_INC_
//from TdfFile.cpp
void ExportTowerDesignTxtFile(LPCTSTR lpszPathName);
void ImportTdfFile(LPCTSTR lpszPathName) ;

void CLDSDoc::ExportCommandDrawingFile(LPCTSTR lpszPathName) 
{
	int i;
	CBuffer buffer;
	buffer.WriteString("General Line Drawing");
	buffer.WriteString("1.0.0.1");
	buffer.WriteInteger(DogSerialNo());	//狗号
	buffer.WriteString(Ta.m_sPrjName);
	buffer.WriteString(Ta.m_sTaType);
	//准距表
	buffer.WriteWord(zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.WriteWord(zhunju_table[i].wide);	//肢宽
		buffer.WriteWord(zhunju_table[i].g);	//标准准距g
		buffer.WriteWord(zhunju_table[i].g1);	//准距g1
		buffer.WriteWord(zhunju_table[i].g2);	//准距g2
		buffer.WriteWord(zhunju_table[i].g3);	//准距g3
	}
	//螺栓间距表
	buffer.WriteInteger(lsspace_N);
	for(i=0;i<lsspace_N;i++)
	{
		buffer.WriteInteger(LSSPACE_TABLE[i].M_Diameter);
		buffer.WriteInteger(LSSPACE_TABLE[i].SingleRowSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].doubleRowSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].EndSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].PlateEdgeSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].RollingEdgeSpace);
		buffer.WriteInteger(LSSPACE_TABLE[i].CutAngleSpace);
	}
	//螺栓规格系列信息
	buffer.WriteInteger(CLsLibrary::GetFamilyNum());
	for(CLsFamily* pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
	{
		buffer.WriteString(pFamily->name);
		buffer.WriteInteger(pFamily->GetCount());
		buffer.WriteString(pFamily->descr);
		buffer.WriteInteger(pFamily->m_bAntiLoose);
		buffer.WriteInteger(pFamily->m_bDualCap?2:1);//pFamily->m_nLsCap);
		buffer.WriteInteger(pFamily->m_bFootNail);
		buffer.WriteInteger(pFamily->m_bBurglarproof);
		for(int j=0;j<pFamily->GetCount();j++)
		{
			LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.WriteDouble(pXingHao->grade);			//螺栓级别
			buffer.WriteInteger(pXingHao->d);			//螺栓直径
			buffer.WriteString(pXingHao->guige);			//螺栓规格
			buffer.WriteInteger(pXingHao->valid_len);	//有效长
			buffer.WriteInteger(pXingHao->no_thread_len);//无扣长
			buffer.WriteInteger(pXingHao->down_lim);		//螺栓通厚下限
			buffer.WriteInteger(pXingHao->up_lim);		//螺栓通厚上限
			buffer.WriteInteger(pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
			buffer.WriteDouble(pXingHao->weight);		//螺栓理论重量
		}
	}
	//材料库
	buffer.WriteInteger(CSteelMatLibrary::GetCount());
	for(i=0;i<CSteelMatLibrary::GetCount();i++)
	{
		STEELMAT mat=CSteelMatLibrary::RecordAt(i);
		buffer.WriteString(mat.mark);
		buffer.WriteByte(mat.cBriefMark);
		buffer.WriteDouble(mat.E);
		buffer.WriteDouble(mat.price_coef);
		buffer.Write(mat.t_lim,sizeof(int)*3);
		buffer.Write(mat.intensity,sizeof(double)*16);
	}
	//构件库
#if defined(__TSA_FILE_)||defined(__LDS_FILE_)
	buffer.WriteInteger(jgguige_N);
	for(i=0;i<jgguige_N;i++)
	{
		buffer.WriteDouble(jgguige_table[i].wing_wide);
		buffer.WriteByte(jgguige_table[i].cType);
		buffer.WriteDouble(jgguige_table[i].wing_thick);
		buffer.WriteDouble(jgguige_table[i].r);
		buffer.WriteDouble(jgguige_table[i].area);
		buffer.WriteDouble(jgguige_table[i].theroy_weight);
		buffer.WriteDouble(jgguige_table[i].Ix);
		buffer.WriteDouble(jgguige_table[i].Rx);
		buffer.WriteDouble(jgguige_table[i].Wx);
		buffer.WriteDouble(jgguige_table[i].Iy0);
		buffer.WriteDouble(jgguige_table[i].Ry0);
		buffer.WriteDouble(jgguige_table[i].Wy0);
	}
#endif
#if !defined(__TMA_FILE_)
	buffer.WriteInteger(tubeguige_N);
	for(i=0;i<tubeguige_N;i++)
	{
		buffer.WriteDouble(tubeguige_table[i].D);
		buffer.WriteDouble(tubeguige_table[i].thick);
		buffer.WriteDouble(tubeguige_table[i].area);
		buffer.WriteDouble(tubeguige_table[i].theroy_weight);
		buffer.WriteDouble(tubeguige_table[i].I);
		buffer.WriteDouble(tubeguige_table[i].r);
		buffer.WriteDouble(tubeguige_table[i].W);
	}
#endif
	//哈希表管理信息部分
	int objsN=0;
	buffer.LogPosition();
	buffer.WriteInteger(objsN);
	CLDSPoint *pPoint=NULL;
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		buffer.WriteInteger(CLS_GEPOINT);
		objsN++;
	}
	CLDSLine *pLine=NULL;
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		buffer.WriteInteger(CLS_GELINE);
		objsN++;
	}
	CLDSPlane *pPlane=NULL;
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		buffer.WriteInteger(CLS_GEPLANE);
		objsN++;
	}

	CLDSNode *pNode=NULL;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		buffer.WriteInteger(CLS_NODE);
		objsN++;
	}
	CLDSLinePart *pRod=NULL;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		buffer.WriteInteger(pRod->handle);
		buffer.WriteInteger(pRod->GetClassTypeId());
		objsN++;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(objsN);
	buffer.RecallPosition();
	buffer.WriteInteger(Ta.Point.GetNodeNum());
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		buffer.WriteInteger(pPoint->handle);
		pPoint->ToBuffer(buffer);
	}
	buffer.WriteInteger(Ta.Line.GetNodeNum());
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		buffer.WriteInteger(pLine->handle);
		pLine->ToBuffer(buffer);
	}
	buffer.WriteInteger(Ta.Plane.GetNodeNum());
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		buffer.WriteInteger(pPlane->handle);
		pPlane->ToBuffer(buffer);
	}
	//呼高信息
	BUFFERPOP stack(&buffer,Ta.Module.GetNodeNum());
	buffer.WriteInteger(Ta.Module.GetNodeNum());
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		buffer.WriteInteger(pModule->handle);
		pModule->ToCdfBuffer(buffer);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of module is wrong!");
#else 
		CLDSObject::Log2File()->Log("模型记录数出错!");
#endif
	buffer.WriteInteger(Ta.m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.WriteWord(Ta.m_arrActiveQuadLegNo[i]);
	//视图集合
	stack.Initialize(&buffer,Ta.View.GetNodeNum());
	buffer.WriteInteger(Ta.View.GetNodeNum());
	for(CDisplayView* pView=Ta.View.GetFirst();pView;pView=Ta.View.GetNext())
	{
		if(pView->m_iViewType==2)
			continue;	//组合构件附加视图，该类视图不保存，显示时临时生成
		buffer.WriteInteger(pView->handle);
		pView->ToCdfBuffer(buffer);
		stack.Increment();
	}
	stack.VerifyAndRestore();
	buffer.WriteInteger(CLDSObject::_console->GetActiveView()->handle);

	//节点信息
	// ------------ 1. vvvvvvvv 节点 --------------
	buffer.WriteInteger(Ta.Node.GetNodeNum());
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		pNode->ToCdfBuffer(buffer);
	}
	// ------------ 2. vvvvvvvv 构件 --------------
	stack.Initialize(&buffer,Ta.Parts.GetNodeNum());
	buffer.WriteInteger(Ta.Parts.GetNodeNum());
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		buffer.WriteInteger(pRod->handle);
		pRod->ToCdfBuffer(buffer);
		stack.Increment();
	}
	stack.VerifyAndRestore();
		//   10   XYZ-对称     5.100     0.300    -4.900    自由    自由    自由    自由    自由    自由  1,2
	//杆件信息
		//规格，材质，连接螺栓数，单双肢连接
	// ------------ 3. vvvvvvvv 钢管法兰设计信息 --------------
	buffer.WriteInteger(Ta.hashConnectFl.GetNodeNum());
	for(CConnectFL *pFL=Ta.hashConnectFl.GetFirst();pFL;pFL=Ta.hashConnectFl.GetNext())
	{
		buffer.WriteDouble(pFL->m_fThickTube);			//主管壁厚(mm)
		buffer.WriteDouble(pFL->m_fInnerRadius);			//基准法兰内环半径(mm)
		buffer.WriteDouble(pFL->m_fCoupleInnerRadius);	//配对法兰内环半径(mm)
		buffer.WriteString(pFL->bolt_grade);
		buffer.WriteInteger(pFL->bolt_d);			//螺栓直径（规格）
		buffer.WriteInteger(pFL->bolt_n);			//螺栓组中的螺栓数量
		buffer.WriteDouble(pFL->N);	//N 轴向力(N)
		buffer.WriteDouble(pFL->M);	//M弯矩(N*m)
		buffer.WriteDouble(pFL->m_fBoltRadius);	//螺栓圆半径(mm)
		buffer.WriteInteger(pFL->m_nThickFL);		//法兰盘厚度(mm)
		buffer.WriteInteger(pFL->m_nWidthFL);		//法兰环形带宽(mm)
		buffer.WriteInteger(pFL->m_nThickRib);		//肋板厚度(mm),厚度为0表示无加劲肋
		buffer.WriteInteger(pFL->m_nHeightRib);		//肋板高度(mm)
		buffer.WriteInteger(pFL->m_nRibS);			//肋板焊缝切角高(宽)度(mm)
		buffer.WriteInteger(pFL->m_nCoupleRibS);	//配对肋板焊缝切角高(宽)度(mm)
		buffer.WriteInteger(pFL->m_nCoupleThickFL);	//法兰盘厚度(mm)
		buffer.WriteInteger(pFL->m_nCoupleHeightRib);//配对肋板高度(mm)
		buffer.WriteByte(pFL->cMaterial);
		buffer.WriteString(pFL->m_sPartNo);     //构件编号
	}
#ifdef __COMPLEX_PART_INC_
	//保存部件及部件引用 wht 12-09-10
	buffer.WriteInteger(Ta.Block.GetNodeNum());
	for (CBlockModel *pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		buffer.WriteInteger(pBlock->handle);
		pBlock->ToBuffer(buffer);
	}
	buffer.WriteInteger(Ta.BlockRef.GetNodeNum());
	for (CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		buffer.WriteInteger(pBlockRef->handle);
		pBlockRef->ToBuffer(buffer);
	}
#endif
	//保存参数坐标系	wht 13-01-06
	buffer.WriteInteger(Ta.Acs.GetNodeNum());
	for (CParaCS *pCS=Ta.Acs.GetFirst();pCS;pCS=Ta.Acs.GetNext())
	{
		buffer.WriteInteger(pCS->id);
		pCS->ToBuffer(buffer);
	}
	//GetMinAvailableHandle();
	FILE* fp=fopen(lpszPathName,"wb");
	if(fp==NULL)
		return;
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
#endif
#ifndef __SMART_DRAW_INC_
void CLDSDoc::OnCheckModelByCmdDrawFile(){;}
#else
void CLDSDoc::OnCheckModelByCmdDrawFile()
{
	CFileDialog dlg(TRUE,"cdf","杆塔",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"司令图模型(*.cdf)|*.cdf|所有文件(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	CheckModelByCommandDrawFile(dlg.GetPathName());
}
#include "CommandDrawingModel.h"
void CLDSDoc::CheckModelByCommandDrawFile(LPCTSTR lpszPathName) 
{
	FILE* fp=fopen(lpszPathName,"rb");
	if(fp==NULL)
		return;
	fseek(fp,0,SEEK_END);
	long length=ftell(fp);
	CHAR_ARRAY pool(length);
	fseek(fp,0,SEEK_SET);
	fread((char*)pool,1,length,fp);
	fclose(fp);

	CBuffer buffer(pool,pool.Size());
	UINT i;
	CXhChar50 str50;
	buffer.ReadString(str50,51);
	if(!str50.Equal("General Line Drawing"))
	{
		AfxMessageBox("错误的司令图格式");
		return;
	}
	buffer.ReadString(str50,51);//version,"1.0");
	long liFileVersion=FromStringVersion(str50);
	int serial=0;
	buffer.ReadInteger(&serial);	//狗号
	buffer.ReadString(str50,51);	//Ta.m_sPrjName
	buffer.ReadString(str50,51);	//Ta.m_sTaType
	//准距表
	WORD wCount=0;
	buffer.ReadWord(&wCount);
	DYN_ARRAY<ANGLE_G_REC> garr(wCount);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&garr[i].wide);	//肢宽
		buffer.ReadWord(&garr[i].g);	//标准准距g
		buffer.ReadWord(&garr[i].g1);	//准距g1
		buffer.ReadWord(&garr[i].g2);	//准距g2
		buffer.ReadWord(&garr[i].g3);	//准距g3
		//TODO:与zhunju_arr进行比对
	}
	//螺栓间距表
	UINT uiCount=0,uiCount2=0; 
	buffer.ReadInteger(&uiCount);
	buffer.Offset(uiCount*28);
	/*for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&LSSPACE_TABLE[i].M_Diameter);
		buffer.ReadInteger(&LSSPACE_TABLE[i].SingleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].doubleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].EndSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].PlateEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].RollingEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].CutAngleSpace);
	}*/
	//螺栓规格系列信息
	//int nSeries=0,record_n;
	//CLsLibrary::ClearLibrary();
	CXhChar16 name;
	buffer.ReadInteger(&uiCount);//nSeries);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadString(name,17);
		buffer.ReadInteger(&uiCount2);
		buffer.ReadString(str50,51);
		buffer.Offset(16);
		/*CLsFamily* pFamily=CLsLibrary::AppendFamily();
		buffer.ReadString(pFamily->name,17);
		buffer.ReadInteger(&record_n);
		buffer.ReadString(pFamily->descr,51);
		buffer.ReadInteger(&pFamily->m_nDisplayWasher);
		buffer.ReadInteger(&pFamily->m_nLsCap);
		buffer.ReadInteger(&pFamily->m_bFootNail);
		buffer.ReadInteger(&pFamily->m_bBurglarproof);
		pFamily->CreateFamily(record_n);*/
		for(UINT j=0;j<uiCount2;j++)
		{
			buffer.Offset(12);
			buffer.ReadString(name,17);
			buffer.Offset(28);
			/*LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
			buffer.ReadInteger(&pXingHao->d);			//螺栓直径
			buffer.ReadString(pXingHao->guige);			//螺栓规格
			buffer.ReadInteger(&pXingHao->valid_len);	//有效长
			buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
			buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
			buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
			buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);*/		//螺栓理论重量
		}
	}
	//材料库
	//nSeries=0;
	buffer.ReadInteger(&uiCount);
	//CSteelMatLibrary::SetCount(nSeries);
	for(i=0;i<uiCount;i++)
	{
		//STEELMAT mat;
		buffer.ReadString(name,17);//mat.mark);
		buffer.Offset(17+12+128);
		/*buffer.ReadByte(&mat.cBriefMark);
		buffer.ReadDouble(&mat.E);
		buffer.ReadDouble(&mat.price_coef);
		buffer.Read(mat.t_lim,sizeof(int)*3);
		buffer.Read(mat.intensity,sizeof(double)*16);
		CSteelMatLibrary::SetRecordAt(i,mat);*/
	}
	//构件库
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.Offset(89);
		/*buffer.ReadDouble(&jgguige_table[i].wing_wide);
		buffer.ReadByte(&jgguige_table[i].cType);
		buffer.ReadDouble(&jgguige_table[i].wing_thick);
		buffer.ReadDouble(&jgguige_table[i].r);
		buffer.ReadDouble(&jgguige_table[i].area);
		buffer.ReadDouble(&jgguige_table[i].theroy_weight);
		buffer.ReadDouble(&jgguige_table[i].Ix);
		buffer.ReadDouble(&jgguige_table[i].Rx);
		buffer.ReadDouble(&jgguige_table[i].Wx);
		buffer.ReadDouble(&jgguige_table[i].Iy0);
		buffer.ReadDouble(&jgguige_table[i].Ry0);
		buffer.ReadDouble(&jgguige_table[i].Wy0);*/
	}
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.Offset(56);
		/*buffer.ReadDouble(&tubeguige_table[i].D);
		buffer.ReadDouble(&tubeguige_table[i].thick);
		buffer.ReadDouble(&tubeguige_table[i].area);
		buffer.ReadDouble(&tubeguige_table[i].theroy_weight);
		buffer.ReadDouble(&tubeguige_table[i].I);
		buffer.ReadDouble(&tubeguige_table[i].r);
		buffer.ReadDouble(&tubeguige_table[i].W);*/
	}
	//哈希表管理信息部分
	UINT h,cls_typeid;
	//CLDSPoint *pPoint;
	//CLDSLine *pLine;
	//CLDSPlane *pPlane;
	//CDisplayView *pDispView;
	CCmdDrawNode *pNode;
	CCmdDrawRod *pRod;
	buffer.ReadInteger(&uiCount);
	CCommandDrawingModel model;
	//Ta.hashObjs.CreateHashTable(n);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		CLDSObject *pObj=NULL;
 		/*if(cls_typeid==CLS_GEPOINT)
		{
			pPoint=Ta.Point.append(FALSE);
			pPoint->handle=h;
			Ta.Point.UpdateHashtable(h,pPoint);
			Ta.hashObjs.SetValueAt(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			pLine=Ta.Line.append(FALSE);
			pLine->handle=h;
			Ta.Line.UpdateHashtable(h,pLine);
			Ta.hashObjs.SetValueAt(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			pPlane=Ta.Plane.append(FALSE);
			pPlane->handle=h;
			Ta.Plane.UpdateHashtable(h,pPlane);
			Ta.hashObjs.SetValueAt(h,pPlane);
		}*/
		/*else if(cls_typeid==CLS_DISPLAYVIEW)
		{
			pDispView=View.append(FALSE);
			pDispView->handle=h;
			hashObjs.SetValueAt(h,pDispView);
		}*/
		//else 
			if(cls_typeid==CLS_NODE)
		{
			pNode=model.hashNodes.Add(h);
		}
		else if(cls_typeid==CLS_LINEANGLE||cls_typeid==CLS_GROUPLINEANGLE||
			cls_typeid==CLS_LINETUBE||cls_typeid==CLS_LINEPART)	//CLDSPart类型工件
		{
			pRod=model.hashRods.Add(h);
			pRod->idClsType=cls_typeid;
		}
	}
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPoint point;
		//if(Ta.hashObjs.GetValueAt(h,(CLDSObject*&)pPoint))
			point.FromBuffer(buffer);//,nVersion,doc_type);
		//else
	}
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		CLDSLine line;
		//if(Ta.hashObjs.GetValueAt(h,(CLDSObject*&)pLine))
			line.FromBuffer(buffer);
	}
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		CLDSPlane plane;
		//if(Ta.hashObjs.GetValueAt(h,(CLDSObject*&)pPlane))
			plane.FromBuffer(buffer);
	}
	//呼高信息
	buffer.ReadInteger(&uiCount);
	//Ta.Module.Empty();
	for(i=0;i<uiCount;i++)
	{
		CLDSModule module;//=Ta.Module.append(FALSE);
		buffer.ReadInteger(&module.handle);
		module.FromCdfBuffer(buffer);//,nVersion,doc_type);
	}
	buffer.Offset(12);
	//buffer.ReadInteger(&Ta.m_hActiveModule);
	//for(i=0;i<4;i++)
	//	buffer.ReadWord(&Ta.m_arrActiveQuadLegNo[i]);
	//视图集合
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		CDisplayView view;//=Ta.View.append(FALSE);
		buffer.ReadInteger(&view.handle);
		view.FromCdfBuffer(buffer);
	}
	long hActiveView=0;
	buffer.ReadInteger(&hActiveView);

	//节点信息
	// ------------ 1. vvvvvvvv 节点 --------------
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0)
		//	hashObjs.GetValueAt(h,(CLDSObject*&)pNode);
		pNode=model.hashNodes.GetValue(h);
		if(pNode!=NULL)
		{
			CLDSNode node;
			node.FromCdfBuffer(buffer,liFileVersion);//,nVersion,doc_type);
			pNode->SetBelongModel(&model);
			pNode->RetrieveCdfInfo(&node);
		}
	}
	// ------------ 2. vvvvvvvv 构件 --------------
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0&&hashObjs.GetValueAt(h,(CLDSObject*&)pPart))
		pRod=model.hashRods.GetValue(h);
		if(pRod!=NULL)
		{
			pRod->SetBelongModel(&model);
			if(pRod->idClsType==CLS_LINEANGLE||pRod->idClsType==CLS_GROUPLINEANGLE)
			{
				CLDSLineAngle angle;
				angle.SetBelongModel(&Ta);
				angle.FromCdfBuffer(buffer,liFileVersion,true);
				pRod->RetrieveCdfInfo(&angle);
			}
			else if(pRod->idClsType==CLS_LINETUBE)
			{
				CLDSLineTube tube;
				tube.SetBelongModel(&Ta);
				tube.FromCdfBuffer(buffer,liFileVersion,true);
				pRod->RetrieveCdfInfo(&tube);
			}
			else
			{
				CLDSLinePart rod;
				rod.SetBelongModel(&Ta);
				rod.FromCdfBuffer(buffer,0,true);//,nVersion,doc_type);
				pRod->RetrieveCdfInfo(&rod);
			}
		}
	}
		//   10   XYZ-对称     5.100     0.300    -4.900    自由    自由    自由    自由    自由    自由  1,2
	//杆件信息
		//规格，材质，连接螺栓数，单双肢连接
	// ------------ 3. vvvvvvvv 钢管法兰设计信息 --------------
	/*buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//主管壁厚(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//基准法兰内环半径(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//配对法兰内环半径(mm)
		buffer.ReadString(pFL->bolt_grade);
		buffer.ReadInteger(&pFL->bolt_d);			//螺栓直径（规格）
		buffer.ReadInteger(&pFL->bolt_n);			//螺栓组中的螺栓数量
		buffer.ReadDouble(&pFL->N);	//N 轴向力(N)
		buffer.ReadDouble(&pFL->M);	//M弯矩(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//螺栓圆半径(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//法兰环形带宽(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//肋板厚度(mm),厚度为0表示无加劲肋
		buffer.ReadInteger(&pFL->m_nHeightRib);		//肋板高度(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//配对肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//配对肋板高度(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //构件编号
	}
#ifdef __COMPLEX_PART_INC_
	//保存部件及部件引用 wht 12-09-10
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CBlockModel *pBlock=Ta.Block.append(FALSE);
		pBlock->handle=h;
		pBlock->FromBuffer(buffer);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CBlockReference *pBlockRef=Ta.BlockRef.append(FALSE);
		pBlockRef->handle=h;
		pBlockRef->FromBuffer(buffer);
	}
#endif
	//保存参数坐标系	wht 13-01-06
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CParaCS *pCS=Ta.Acs.Add(h);
		pCS->FromBuffer(buffer);
	}	
	Ta.GetMinAvailableHandle();*/
	//提取附加的手动映射节点集合
	CHashList<long> hashPointIById;	//键值为三模型中节点的句柄，数据为司令图模型中的节点编号
	CHashList<long> hashIdByPointI;
	CXhChar200 matchfilename("%s.match",lpszPathName);
	FILE* fpmatch=fopen(matchfilename,"rt");
	if(fpmatch!=NULL)
	{
		CXhChar100 linetext;
		while(!feof(fpmatch))
		{
			fgets(linetext,100,fpmatch);
			char* pIdStr=NULL,*pPointIStr=NULL;
			if((pIdStr=strtok(linetext,"="))!=NULL)
				pPointIStr=strtok(NULL,"=");
			if(pIdStr&&pPointIStr)
			{
				long hid=HexStrToLong(pIdStr);
				//long pointI=atoi(pPointIStr);
				//hashPointIById.SetValue(hid,pointI);
				char* pKey=strtok(pPointIStr,",");
				bool bFirstChance=true;
				while(pKey)
				{
					long pointI=atoi(pKey);
					if(bFirstChance)
						hashPointIById.SetValue(hid,pointI);
					hashIdByPointI.SetValue(pointI,hid);
					bFirstChance=false;
					pKey=strtok(NULL,",");
				}
			}
		}
		fclose(fpmatch);
	}
	matchfilename.Append('2');
	CString segIncStr="*";
	Input("输入待校审的构件段号范围:",&segIncStr,'s');
	model.CheckModel(&Ta,&hashPointIById,matchfilename,segIncStr);
}
void CLDSDoc::ImportCommandDrawingFile(LPCTSTR lpszPathName) 
{
	FILE* fp=fopen(lpszPathName,"rb");
	if(fp==NULL)
		return;
	fseek(fp,0,SEEK_END);
	long length=ftell(fp);
	CHAR_ARRAY pool(length);
	fseek(fp,0,SEEK_SET);
	fread((char*)pool,1,length,fp);
	fclose(fp);
	CBuffer buffer(pool,pool.Size());
	int i;
	CXhChar50 str50;
	buffer.ReadString(str50,51);
	if(!str50.Equal("General Line Drawing"))
	{
		AfxMessageBox("错误的司令图格式");
		return;
	}
	buffer.ReadString(str50,51);//version,"1.0");
	int serial=0;
	buffer.ReadInteger(&serial);	//狗号
	buffer.ReadString(Ta.m_sPrjName,51);
	buffer.ReadString(Ta.m_sTaType,51);
	//准距表
	buffer.ReadWord(&zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&zhunju_table[i].wide);	//肢宽
		buffer.ReadWord(&zhunju_table[i].g);	//标准准距g
		buffer.ReadWord(&zhunju_table[i].g1);	//准距g1
		buffer.ReadWord(&zhunju_table[i].g2);	//准距g2
		buffer.ReadWord(&zhunju_table[i].g3);	//准距g3
	}
	//螺栓间距表
	buffer.ReadInteger(&lsspace_N);
	for(i=0;i<lsspace_N;i++)
	{
		buffer.ReadInteger(&LSSPACE_TABLE[i].M_Diameter);
		buffer.ReadInteger(&LSSPACE_TABLE[i].SingleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].doubleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].EndSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].PlateEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].RollingEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].CutAngleSpace);
	}
	//螺栓规格系列信息
	int nSeries=0,record_n=0,intVal=0;
	CLsLibrary::ClearLibrary();
	buffer.ReadInteger(&nSeries);
	for(i=0;i<nSeries;i++)
	{
		CLsFamily* pFamily=CLsLibrary::AppendFamily();
		buffer.ReadString(pFamily->name,17);
		buffer.ReadInteger(&record_n);
		buffer.ReadString(pFamily->descr,51);
		buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=intVal>0;
		buffer.ReadInteger(&intVal);pFamily->m_bDualCap=intVal>=2?true:false;
		buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal!=FALSE);
		buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal!=FALSE);
		pFamily->CreateFamily(record_n);
		for(int j=0;j<record_n;j++)
		{
			LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
			buffer.ReadInteger(&pXingHao->d);			//螺栓直径
			buffer.ReadString(pXingHao->guige);			//螺栓规格
			buffer.ReadInteger(&pXingHao->valid_len);	//有效长
			buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
			buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
			buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
			buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);		//螺栓理论重量
		}
	}
	//材料库
	nSeries=0;
	buffer.ReadInteger(&nSeries);
	CSteelMatLibrary::SetCount(nSeries);
	for(i=0;i<nSeries;i++)
	{
		STEELMAT mat;
		buffer.ReadString(mat.mark);
		buffer.ReadByte(&mat.cBriefMark);
		buffer.ReadDouble(&mat.E);
		buffer.ReadDouble(&mat.price_coef);
		buffer.Read(mat.t_lim,sizeof(int)*3);
		buffer.Read(mat.intensity,sizeof(double)*16);
		CSteelMatLibrary::SetRecordAt(i,mat);
	}
	//构件库
	AngleLibrary()->Count=buffer.ReadInteger();
	for(i=0;i<jgguige_N;i++)
	{
		buffer.ReadDouble(&jgguige_table[i].wing_wide);
		buffer.ReadByte(&jgguige_table[i].cType);
		buffer.ReadDouble(&jgguige_table[i].wing_thick);
		buffer.ReadDouble(&jgguige_table[i].r);
		buffer.ReadDouble(&jgguige_table[i].area);
		buffer.ReadDouble(&jgguige_table[i].theroy_weight);
		buffer.ReadDouble(&jgguige_table[i].Ix);
		buffer.ReadDouble(&jgguige_table[i].Rx);
		buffer.ReadDouble(&jgguige_table[i].Wx);
		buffer.ReadDouble(&jgguige_table[i].Iy0);
		buffer.ReadDouble(&jgguige_table[i].Ry0);
		buffer.ReadDouble(&jgguige_table[i].Wy0);
	}
	buffer.ReadInteger(&tubeguige_N);
	for(i=0;i<tubeguige_N;i++)
	{
		buffer.ReadDouble(&tubeguige_table[i].D);
		buffer.ReadDouble(&tubeguige_table[i].thick);
		buffer.ReadDouble(&tubeguige_table[i].area);
		buffer.ReadDouble(&tubeguige_table[i].theroy_weight);
		buffer.ReadDouble(&tubeguige_table[i].I);
		buffer.ReadDouble(&tubeguige_table[i].r);
		buffer.ReadDouble(&tubeguige_table[i].W);
	}
	//哈希表管理信息部分
	long n,h,cls_typeid;
	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	//CDisplayView *pDispView;
	CLDSNode *pNode;
	CLDSPart *pPart;
	buffer.ReadInteger(&n);
	Ta.hashObjs.SetHashTableGrowSize(n);//CreateHashTable(n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		CLDSObject *pObj=NULL;
 		if(cls_typeid==CLS_GEPOINT)
		{
			pPoint=Ta.Point.append(FALSE);
			pPoint->handle=h;
			Ta.Point.UpdateHashtable(h,pPoint);
			Ta.hashObjs.SetValue(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			pLine=Ta.Line.append(FALSE);
			pLine->handle=h;
			Ta.Line.UpdateHashtable(h,pLine);
			Ta.hashObjs.SetValue(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			pPlane=Ta.Plane.append(FALSE);
			pPlane->handle=h;
			Ta.Plane.UpdateHashtable(h,pPlane);
			Ta.hashObjs.SetValue(h,pPlane);
		}
		/*else if(cls_typeid==CLS_DISPLAYVIEW)
		{
			pDispView=View.append(FALSE);
			pDispView->handle=h;
			hashObjs.SetValueAt(h,pDispView);
		}*/
		else if(cls_typeid==CLS_NODE)
		{
			pNode=Ta.Node.append(FALSE);
			pNode->handle=h;
			Ta.Node.UpdateHashtable(h,pNode);
			Ta.hashObjs.SetValue(h,pNode);
		}
		else	//CLDSPart类型工件
		{
			pPart=Ta.Parts.append(cls_typeid,FALSE);
			if(pPart)
			{
				pPart->handle=h;
				Ta.Parts.UpdateHashtable(h,pPart);
				Ta.hashObjs.SetValue(h,pPart);
			}
		}
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if((pPoint=(CLDSPoint*)Ta.hashObjs.GetValue(h))!=NULL)
			pPoint->FromBuffer(buffer);//,nVersion,doc_type);
		//else
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if((pLine=(CLDSLine*)Ta.hashObjs.GetValue(h))!=NULL)
			pLine->FromBuffer(buffer);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		if((pPlane=(CLDSPlane*)Ta.hashObjs.GetValue(h))!=NULL)
			pPlane->FromBuffer(buffer);
	}
	//呼高信息
	buffer.ReadInteger(&n);
	Ta.Module.Empty();
	for(i=0;i<n;i++)
	{
		CLDSModule *pModule=Ta.Module.append(FALSE);
		buffer.ReadInteger(&pModule->handle);
		pModule->FromCdfBuffer(buffer);//,nVersion,doc_type);
	}
	buffer.ReadInteger(&Ta.m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.ReadWord(&Ta.m_arrActiveQuadLegNo[i]);
	//视图集合
	buffer.ReadInteger(&n);
	Ta.View.Empty();
	for(i=0;i<n;i++)
	{
		CDisplayView* pView=Ta.View.append(FALSE);
		buffer.ReadInteger(&pView->handle);
		pView->FromCdfBuffer(buffer);
	}
	long hActiveView=0;
	buffer.ReadInteger(&hActiveView);

	//节点信息
	// ------------ 1. vvvvvvvv 节点 --------------
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0)
		//	hashObjs.GetValueAt(h,(CLDSObject*&)pNode);
		if((pNode=Ta.Node.FromHandle(h))!=NULL)
			pNode->FromCdfBuffer(buffer);//,nVersion,doc_type);
		//else
		//	int c=3;
	}
	// ------------ 2. vvvvvvvv 构件 --------------
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0&&hashObjs.GetValueAt(h,(CLDSObject*&)pPart))
		if((pPart=Ta.Parts.FromHandle(h))!=NULL)
			((CLDSLinePart*)pPart)->FromCdfBuffer(buffer);//,nVersion,doc_type);
	}
		//   10   XYZ-对称     5.100     0.300    -4.900    自由    自由    自由    自由    自由    自由  1,2
	//杆件信息
		//规格，材质，连接螺栓数，单双肢连接
	// ------------ 3. vvvvvvvv 钢管法兰设计信息 --------------
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//主管壁厚(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//基准法兰内环半径(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//配对法兰内环半径(mm)
		buffer.ReadString(pFL->bolt_grade);
		buffer.ReadInteger(&pFL->bolt_d);			//螺栓直径（规格）
		buffer.ReadInteger(&pFL->bolt_n);			//螺栓组中的螺栓数量
		buffer.ReadDouble(&pFL->N);	//N 轴向力(N)
		buffer.ReadDouble(&pFL->M);	//M弯矩(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//螺栓圆半径(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//法兰环形带宽(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//肋板厚度(mm),厚度为0表示无加劲肋
		buffer.ReadInteger(&pFL->m_nHeightRib);		//肋板高度(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//配对肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//配对肋板高度(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //构件编号
	}
#ifdef __COMPLEX_PART_INC_
	//保存部件及部件引用 wht 12-09-10
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CBlockModel *pBlock=Ta.Block.append(FALSE);
		pBlock->handle=h;
		pBlock->FromBuffer(buffer);
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CBlockReference *pBlockRef=Ta.BlockRef.append(FALSE);
		pBlockRef->handle=h;
		pBlockRef->FromBuffer(buffer);
	}
#endif
	//保存参数坐标系	wht 13-01-06
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CParaCS *pCS=Ta.Acs.Add(h);
		pCS->FromBuffer(buffer);
	}	
	Ta.GetMinAvailableHandle();
}
#endif
#ifdef __RAPID_ROUGH_3DMODEL_
static void Export3dsOrStlFile(LPCTSTR lpszPathName,BYTE ciMode3DS0_STL1=0,bool blBinaryStl=false)
{
	CLDSModule* pWorkingHeight = Ta.GetActiveModule();
	if (pWorkingHeight == NULL)
	{
		AfxMessageBox("模型当前激活呼高获取失败");
		return;
	}
	CXhChar16 szFormatType = ciMode3DS0_STL1 == 0 ? "生成3DS模型文件" : "生成STL模型文件";
	CXhChar16 szPromptTitle = ciMode3DS0_STL1 == 0 ? "生成3DS模型文件" : "生成STL模型文件";
	pWorkingHeight->GetModuleScopeZ();
	//初始化GIM坐标系
	UCS_STRU stdcs;
	LoadDefaultUCS(&stdcs);
	UCS_STRU draw_ucs;
	draw_ucs.origin.Set(0, 0, pWorkingHeight->LowestZ());
	draw_ucs.axis_x.Set(1, 0, 0);
	draw_ucs.axis_y.Set(0, -1, 0);
	draw_ucs.axis_z.Set(0, 0, -1);
	//解析三维实体的基本三角面信息
	int serial = 1, liCountOfParts = Ta.Parts.GetNodeNum();
	I3DSData* p3dsFile = ciMode3DS0_STL1==0?C3DSFactory::Create3DSInstance():NULL;
	IStlData* pStlFile = ciMode3DS0_STL1 == 1 ? CStlFactory::CreateStl() : NULL;
	DisplayProcess(0, szPromptTitle);
	static const int UNIT_3DS_INCH	= 0;
	static const int UNIT_3DS_M		= 1;
	static const int UNIT_3DS_MM	= 2; //mm
	int UNIT_3DS_TYPE = UNIT_3DS_M;
	static char _localSolidBufPool[0x200000];	//零件实体缓存2M
	for (CLDSPart* pPart = Ta.EnumPartFirst();pPart;pPart = Ta.EnumPartNext(), serial++)
	{
		DisplayProcess(ftoi(100 * serial / liCountOfParts), szPromptTitle);
		if (!pWorkingHeight->IsSonPart(pPart) || pPart->m_bVirtualPart)
			continue;
		if (pPart->pSolidBody == NULL)
			pPart->Create3dSolidModel();
		if (pPart->pSolidBody)
		{
			CSolidBody solidBody(_localSolidBufPool, 0x200000, true);
			solidBody.CopyBuffer(pPart->pSolidBody->BufferPtr(), pPart->pSolidBody->BufferLength(),false);
			solidBody.TransToACS(draw_ucs);
			if(p3dsFile)
				p3dsFile->AddSolidPart(&solidBody, serial, CXhChar16("Part%d", serial), UNIT_3DS_TYPE);
			if(pStlFile)
				pStlFile->AddSolidBody(solidBody.BufferPtr(), solidBody.BufferLength());
		}
 	}
	DisplayProcess(0, "生成装配部件的三维模型信息....");
	for (CBlockReference *pBlockRef = Ta.BlockRef.GetFirst();pBlockRef;pBlockRef = Ta.BlockRef.GetNext())
	{
		CBlockModel *pBlock = Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if (pBlock == NULL)
			continue;
		UCS_STRU acs = pBlockRef->GetACS();
		UCS_STRU cs = pBlock->TransToACS(acs);
		PARTSET partSet;
		if (pBlock->IsEmbeded())
		{	//嵌入式部件
			for (CLDSPart *pPart = Ta.EnumPartFirst();pPart;pPart = Ta.EnumPartNext())
			{
				if (pPart->m_hBlock == pBlock->handle)
					partSet.append(pPart);
			}
		}
		else
		{	//非嵌入式部件
			for (CLDSPart* pPart = pBlock->Parts.GetFirst();pPart;pPart = pBlock->Parts.GetNext())
				partSet.append(pPart);
		}
		//
		for (pPart = partSet.GetFirst();pPart;pPart = partSet.GetNext(), serial++)
		{
			if (pPart->pSolidBody == NULL)
				pPart->Create3dSolidModel(FALSE);
			if (pPart->pSolidBody)
			{
				CSolidBody solidBody(_localSolidBufPool, 0x100000, true);
				solidBody.CopyBuffer(pPart->pSolidBody->BufferPtr(), pPart->pSolidBody->BufferLength(), false);
				solidBody.TransACS(stdcs, cs);	//
				solidBody.TransToACS(draw_ucs);	//
				if(p3dsFile)
					p3dsFile->AddSolidPart(&solidBody, serial, CXhChar16("Part%d", serial), UNIT_3DS_TYPE);
				if(pStlFile)
					pStlFile->AddSolidBody(solidBody.BufferPtr(), solidBody.BufferLength());
			}
		}
	}
	DisplayProcess(100, szPromptTitle);
	if (p3dsFile)
	{
		p3dsFile->Creat3DSFile(lpszPathName);
		C3DSFactory::Destroy(p3dsFile->GetSerial());
	}
	if (pStlFile)
	{
		pStlFile->SaveFile(lpszPathName, blBinaryStl?1:0);
		CStlFactory::Destroy(pStlFile->GetSerial());
	}
	//MessageBox("3DS文件生成功");
}
void CLDSDoc::Export3dsFile(LPCTSTR lpszPathName)
{
	Export3dsOrStlFile(lpszPathName, 0);
}
void CLDSDoc::ExportStlFile(LPCTSTR lpszPathName)
{
	Export3dsOrStlFile(lpszPathName, 1);
}
//#define TEMP_HENAN_LOG
void CLDSDoc::ExportGimFile(LPCTSTR lpszPathName)
{
#ifndef __STATION_	//暂时变电构架SDA软件不需要移交功能 wjh-2019.9.25
	CLogErrorLife life(&logerr);
	CWaitCursor waitCursor;
	//检测是否存在MOD文件，不存在则生成MOD文件
	char drive[4];
	char dir[MAX_PATH],fname[MAX_PATH],folder[MAX_PATH];
	_splitpath(lpszPathName,drive,dir,fname,NULL);
	strcpy(folder,drive);
	strcat(folder,dir);
	CXhChar500 mod_file("%s%s.mod",folder,fname);
	FILE* fp=fopen(mod_file,"rb");
#ifdef TEMP_HENAN_LOG
	logerr.Log("mod文件生成目录:%s", (char*)mod_file);
	logerr.Log("mod-fp=%8X", fp);
#endif
	if(fp==NULL)
	{	//生成MOD文件
		CModDataFile modfile;
		LOCALE_PROPITEM* pLocaleItem=gxPortalLocalizer.GetLocaleItem("ExportTurnLeftGimFile");
		if (pLocaleItem&&pLocaleItem->bVal&&
			AfxMessageBox("杆塔模型是否进行左转？", MB_YESNO) == IDYES)
			modfile.m_bTurnLeft = TRUE;
		modfile.ExportModFile(mod_file);
#ifdef TEMP_HENAN_LOG
		logerr.Log("独立ExportModFile");
#endif
	}
	else
		fclose(fp);
	//弹出对话框，录入GIM头信息和杆塔属性信息
	CGimDataFile gim_file;
	CInputGimInfoDlg dlg;
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step1");
#endif
	dlg.SetGimInfo(&gxTowerGimInfo);
	if(dlg.DoModal()!=IDOK)
		return;
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step2");
#endif
	gim_file.m_sName.Copy(fname);
	gim_file.m_sFolder.Copy(folder);
	gim_file.m_ciCodingType=g_sysPara.m_iModFileCodeType;
	gim_file.m_xGimHeadProp.Clone(&gxTowerGimInfo.m_xCurrGimFileHeader);
	gim_file.m_xTowerProp.Clone(&gxTowerGimInfo.m_xTowerGimPropFile);
#ifdef TEMP_HENAN_LOG
	logerr.Log("fname=%s", fname);
	logerr.Log("folder=%s", folder);
	logerr.Log("code=%d", g_sysPara.m_iModFileCodeType);
#endif
	gim_file.CreateGuidFile();
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step3");
#endif
	gim_file.PackGimFile();		//进行压缩打包
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step4");
#endif
	gim_file.UpdateGimFile();	//更新GIM头部信息
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step5");
#endif
#endif
}

void CLDSDoc::OnTowerGimProperty()
{
#ifndef __STATION_	//暂时变电构架SDA软件不需要移交功能 wjh-2019.9.25
	//弹出对话框，录入GIM头信息和杆塔属性信息
	CInputGimInfoDlg dlg;
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step1");
#endif
	dlg.SetGimInfo(&gxTowerGimInfo);
	if(dlg.DoModal()!=IDOK)
		return;
#endif
}
#else
void CLDSDoc::OnTowerGimProperty() {}
#endif
void CLDSDoc::OnFileProperty() 
{
	CFilePropertyDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		CLDSView* pWorkView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
		if(pWorkView&&pWorkView->selectObjs.GetNodeNum()==0)
			pWorkView->DisplaySysSettingProperty(TRUE);
	}
}

void CLDSDoc::DeleteContents() 
{
	try{
		Ta.Empty();
		Ta.InitTower();
		markInfoList.Empty();
		CLinkJGDlg::InitPreferAngleMat(NULL);
#if defined(__TMA_)||defined(__LMA_)
		globalDataCmpModel.Empty();
#endif
		//执行Ta.Empty()时已将所有的Block删除,此处应将当前激活模型切换为Ta,否则默认激活模型为Block时会出错 wht 12-10-29
		console.ActivateModel(&Ta);	
		console.InitTowerModel();
#if defined(__PART_DESIGN_INC_)
		gxWirePointModel.Empty();
		try{
			postreport.Empty();
			logger.EmptyLogVersion();
			//TODO:此条语句会在反复执行时导致死机异常 wjh-2019.5.28
			logger.schema.EmptySubMembers();
			CLogSchema::InitCurrentRootSchema(&logger.schema);
			if(theApp.m_pMainWnd)
			{
				CLogViewerDlg *pLogViewerDlg=((CMainFrame*)theApp.m_pMainWnd)->GetLogViewerPage();
				if (pLogViewerDlg)
					pLogViewerDlg->RefreshLogVersionList();
				COutputInfoDlg *pOutputInfoDlg=((CMainFrame*)theApp.m_pMainWnd)->GetOutputInfoPage();
				if (pOutputInfoDlg)
					pOutputInfoDlg->ClearOutputInfoList();
				CRevisionOperDlg *pRevisionDlg=((CMainFrame*)theApp.m_pMainWnd)->GetRevisionOperPage();
				if (pRevisionDlg)
					pRevisionDlg->UpdateRevision(postreport);
			}
		}
		catch(CXhChar50 sError)
		{
			AfxMessageBox(sError);
		}
#endif
		m_iMaxBakSerial=0;
		//清空显示缓存提高内存利用率
		if(g_pSolidDraw)
			g_pSolidDraw->EmptyDisplayBuffer();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	CDocument::DeleteContents();
}

BOOL CLDSDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if(!Ta.AssertValid())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Illegal data in the file, so it can't save!");
#else 
		AfxMessageBox("文件中有非法数据，不能存盘!");
#endif
		return FALSE;
	}
	CXhChar200 filepathname=lpszPathName;
#ifdef __LDS_FILE_
	//防止用户导入TTA数据或MYL数据后直接点保存时把原始数据文件冲掉 wjh-2018.5.5
	char* pszDotExt=SearchChar(filepathname,'.',true);
	if(pszDotExt==NULL)
		filepathname.Append(".lds");
	if (pszDotExt != NULL)
	{
		if (stricmp(pszDotExt, ".mod") == 0)
			return FALSE;	//打开的MOD文件不进行保存
		if (stricmp(pszDotExt, ".myl") == 0 || stricmp(pszDotExt, ".dat") == 0 || stricmp(pszDotExt, ".tans") == 0)
			strcpy(pszDotExt, ".lds");
	}
#endif
	return CDocument::OnSaveDocument(filepathname);
}
void CorrectFileError(CString sFilePath);
BOOL CLDSDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	theApp.work_path[0]=0;	//打开新文件时应清空之前的文件工作目录，否则会导致theApp.szWorkPath不更新 wjh-2019.9.22
	char drive[4];
	char dir[MAX_PATH];
	CXhChar50 file_name;
	CXhChar16 extension;
	_splitpath(lpszPathName,drive,dir,file_name,extension);
	SetPathName(lpszPathName);
	SetModifiedFlag();  // dirty during de-serialize
#ifdef __PART_DESIGN_INC_
	CRevisionOperDlg *pRevisionDlg=((CMainFrame*)AfxGetMainWnd())->GetRevisionOperPage();
	if(extension.EqualNoCase(".tsa"))
	{
		DeleteContents();
		ImportTsaFile(lpszPathName);
		if (pRevisionDlg)
			pRevisionDlg->UpdateRsltNames();
		return TRUE;
	}
	else if(extension.EqualNoCase(".tdf"))
	{
		DeleteContents();
		BeginWaitCursor();
		ImportTdfFile(lpszPathName);
		EndWaitCursor();
		if (pRevisionDlg)
			pRevisionDlg->UpdateRsltNames();
		return TRUE;
	}
#endif
#if defined(__PART_DESIGN_INC_)||defined(__RAPID_ROUGH_3DMODEL_)
	/*else if(extension.EqualNoCase(".ini"))
	{
		DeleteContents();
		TtaFileIn(lpszPathName);
		//if (pRevisionDlg)
		//	pRevisionDlg->UpdateRsltNames();
		return TRUE;
	}*/
#ifndef __STATION_	//暂时变电构架SDA软件不需要移交功能 wjh-2019.9.25
	else if(extension.EqualNoCase(".mod"))
	{
		DeleteContents();
		CModDataFile modfile;
		modfile.ImportModFile(lpszPathName);
		gxWirePointModel.m_bGimWireModelInherited=true;
		//if (pRevisionDlg)
		//	pRevisionDlg->UpdateRsltNames();
		return TRUE;
	}
	else if (extension.EqualNoCase(".gim"))
	{
		CGimDataFile gimfile;
		if(gimfile.ParseGimFileEx(lpszPathName))
		{
			DeleteContents();
			CModDataFile modfile;
			modfile.ImportModFile(gimfile.m_sModFile);
			gxWirePointModel.m_bGimWireModelInherited = true;
			gimfile.DeleteGuidFile();
			//记录GIM信息和杆塔属性信息
			gxTowerGimInfo.m_xCurrGimFileHeader.Clone(&gimfile.m_xGimHeadProp);
			gxTowerGimInfo.m_xTowerGimPropFile.Clone(&gimfile.m_xTowerProp);
			return TRUE;
		}
		return FALSE;
	}
#endif
#endif
#ifdef __SMART_DRAW_INC_
	else if(extension.EqualNoCase(".cdf"))
	{
		DeleteContents();
		BeginWaitCursor();
		ImportCommandDrawingFile(lpszPathName);
		EndWaitCursor();
		return TRUE;
	}
#endif
	//CorrectFileError(lpszPathName);
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
#ifdef __PART_DESIGN_INC_
	if (pRevisionDlg)
		pRevisionDlg->UpdateRsltNames();
#endif

	return TRUE;
}

void CLDSDoc::OnReadTtaFile()
{
#ifndef __STATION_	//暂时变电构架SDA软件不需要导入杆塔计算模型功能 wjh-2019.9.25
#if defined(__PART_DESIGN_INC_)||defined(__RAPID_ROUGH_3DMODEL_)
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CXhChar500 filter= "TTA数据文件(*.*)|*.*|"\
		"MYL数据文件(*.myl)|*.myl|"\
		"SmartTower数据文件(*.dat)|*.dat|"\
		"TANS数据文件(*.*)|*.*||";
	//"西北院数据文件(*.dat)|*.dat|"
	CXhChar50 xarrFileFormats[16], xarrFileFormatItems[32];
	UINT niFilterCount=DELIMITER_STR::ParseWordsFromStr(filter, "|", xarrFileFormatItems, 32);
	for (UINT i = 0;i < niFilterCount;i++)
	{
		if (i % 2 == 0)
			xarrFileFormats[i / 2] = xarrFileFormatItems[i];
	}
	CFileDialog dlg(TRUE,"",pDoc->GetPathName(),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if(dlg.DoModal()!=IDOK)
		return;
	DeleteContents();	//必须置于DoModal()之后，否则如果用户只要执行此命令就会删除文档 wjh-2019.11.29
	char ciFileFormatType = -1;
	CXhChar50 szCurrFormat = xarrFileFormats[dlg.m_ofn.nFilterIndex - 1];
	if (strstr(szCurrFormat, "TTA") != NULL)	//"二进制"
		ciFileFormatType = -1;// CTtaDataFile::FILETYPE_TTA;此时允许根据文件扩展名判断文件格式类型 wjh-2019.7.10
	else if (strstr(szCurrFormat, "TANS") != NULL)	//"二进制"
		ciFileFormatType = CTtaDataFile::FILETYPE_TANS;
	TtaFileIn(dlg.GetPathName(), ciFileFormatType);
	//if (pRevisionDlg)
	//	pRevisionDlg->UpdateRsltNames();
	//return TRUE;
	UpdateAllViews(NULL);
#endif
#endif
}

/*void CLDSDoc::OnAddBraceElemDataFile()
{
#ifdef __RAPID_ROUGH_3DMODEL_
	CXhChar500 filter("TTA辅助材计算及输出文件(*.*)|*.*||");
	CFileDialog dlg(TRUE,"tower","LDS",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,filter);

	DWORD nFileNumbers = 100;    //CFileDialog最大选择文件数量
	TCHAR *pBufOld = dlg.m_ofn.lpstrFile;   //保存旧的指针
	DWORD dwMaxOld = dlg.m_ofn.nMaxFile;    //保存旧的数量
	dlg.m_ofn.lpstrFile = new TCHAR[ nFileNumbers * MAX_PATH];
	ZeroMemory(dlg.m_ofn.lpstrFile,sizeof(TCHAR) * nFileNumbers * MAX_PATH);
	dlg.m_ofn.nMaxFile = nFileNumbers * MAX_PATH;    //此最大数,按MSDN解释为fileDlg.m_ofn.lpstrFile指向的缓冲的最大字符数
	if(dlg.DoModal()!=IDOK)
		return;
	CXhSimpleList<CXhChar200> listBraceFiles;
	POSITION pos=dlg.GetStartPosition();
	while(pos)
	{
		CString sPathName=dlg.GetNextPathName(pos);		//获取文件名 
		listBraceFiles.Append(CXhChar200(sPathName));
	}
	delete [](dlg.m_ofn.lpstrFile);        //清除堆内存
	dlg.m_ofn.lpstrFile = pBufOld;        //还原之前的指针,因为没有深入过CFileDialog源码分析,不清这个是否必须的,还是先还原上吧,
	dlg.m_ofn.nMaxFile = dwMaxOld;        //还原之前的最大数
	CUndoOperObject undo(&Ta,true);
	for(CXhChar200* pszFile=listBraceFiles.EnumObjectFirst();pszFile;pszFile=listBraceFiles.EnumObjectNext())
	{
		CTtaDataFile bracetta;
		bracetta.ReadBraceElemsDataFile(*pszFile);
	}
	//自动将力学简化线转换为实体杆件
	logerr.GlobalEnabled(false);
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->GetClassTypeId()!=pRod->size_idClassType&&pRod->size_idClassType!=0)			//转换构件类型
			pRod=(CLDSLinePart*)Ta.Parts.InstantiateCursorLinePart();
	}
	logerr.GlobalEnabled(true);
#endif
}*/

CView* CLDSDoc::GetView(const CRuntimeClass *pClass)
{
	CView *pView;
	POSITION position;
	position = GetFirstViewPosition();
	for(;;)
	{
		if(position==NULL)
		{
			pView = NULL;
			break;
		}
		pView = GetNextView(position);
		if(pView->IsKindOf(pClass))
			break;
	}
	return pView;
}


void CLDSDoc::AutoSaveBakFile()
{
	CXhChar200 ss("%s",GetPathName());
	char* separator=SearchChar(ss,'.',true);
	if(separator==NULL)
		return;
	*separator=0;
	if(g_sysPara.iAutoSaveType==0)
	{
		ss.Append(".bak");
		OnSaveDocument(ss);
	}
	else if(g_sysPara.iAutoSaveType==1)
	{
		CString filename="";
		CFileFind fileFind;
		do
		{
			filename="";
			m_iMaxBakSerial++;
			filename.Format("%s(%d).bak",(char*)ss,m_iMaxBakSerial);
		}while(fileFind.FindFile(filename));
		OnSaveDocument(filename);
	}
}

void CLDSDoc::ResetAutoSaveTimer()
{
	if(g_sysPara.nAutoSaveTime==0)
	{
		if(m_nTimer)
			AfxGetMainWnd()->KillTimer(m_nTimer);
		m_nTimer=0;
		return;
	}
	else if(m_nTimer!=2)
		return;
	if(m_nTimer)
	{
		AfxGetMainWnd()->KillTimer(m_nTimer);
		m_nTimer=AfxGetMainWnd()->SetTimer(2,g_sysPara.nAutoSaveTime,AutoSaveProc);
	}
	else
		m_nTimer=AfxGetMainWnd()->SetTimer(2,g_sysPara.nAutoSaveTime,AutoSaveProc);
	m_bStartAutoSaveThread=true;	//启用自动存盘功能
}
void CorrectFileError(CString sFilePath);
void CLDSDoc::OnFileOpen() 
{
#ifdef __TMA_FILE_
	CFileDialog dlg(TRUE,"tma","TMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Tower Manufacturing Assistant(*.tma)|*.tma|all files(*.*)|*.*||");
#else 
		"铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(TRUE,"lma","LMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"格构铁塔制造助手(*.lma)|*.lma|铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔绘图助手(*.tda)|*.tda|格构铁塔制造助手(*.lma)|*.lma|铁塔制造助手(*.tma)|*.tma|司令图模型(*.cdf)|*.cdf|所有文件(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CXhChar500 filter("铁塔设计绘图一体化软件(*.lds)|*.lds");
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("|铁塔内力分析设计文件(*.tsa)|*.tsa|原始设计信息文件(*.tdf)|*.tdf");
	//filter.Append("|TTA接口塔型配置文件(*.ini)|*.ini");
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		filter.Append("|国网移交几何模型文件(*.mod)|*.mod");
		filter.Append("|国网移交GIM文件(*.gim)|*.gim");
		filter.Append("|铁塔制造助手(*.tma)|*.tma|格构铁塔制造助手(*.lma)|*.lma|铁塔绘图助手(*.tda)|*.tda|司令图模型(*.cdf)|*.cdf");
	}
	filter.Append("|所有文件(*.*)|*.*||");
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
#elif defined(__TSA_FILE_)
	CFileDialog dlg(TRUE,"tsa","TSA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔内力分析设计文件(*.tsa)|*.tsa|原始设计信息文件(*.tdf)|*.tdf|所有文件(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	//CorrectFileError(dlg.GetPathName());
	OnOpenDocument(dlg.GetPathName());
	//如果直接从菜单中选择的最近打开文件项时，系统在调用完OnOpenDocument后，
	//会自动触发CView::OnInitialUpdate函数，但当前的OnFileOpen中不会触发此函数，因此必须
	//显示调用如下函数，更新视图
	UpdateAllViews(NULL);
}

void CLDSDoc::OnFileOpenInNewWindow()
{
#ifdef __TMA_FILE_
	CFileDialog dlg(TRUE,"tma","TMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Tower Manufacturing Assistant(*.tma)|*.tma|all files(*.*)|*.*||");
#else 
		"铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(TRUE,"lma","LMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"格构铁塔制造助手(*.lma)|*.lma|铁塔制造助手(*.tma)|*.tma|所有文件(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔绘图助手(*.tda)|*.tda|格构铁塔制造助手(*.lma)|*.lma|铁塔制造助手(*.tma)|*.tma|司令图模型(*.cdf)|*.cdf|所有文件(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CXhChar500 filter("铁塔设计绘图一体化软件(*.lds)|*.lds");
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("|铁塔内力分析设计文件(*.tsa)|*.tsa|原始设计信息文件(*.tdf)|*.tdf|TTA接口塔型配置文件(*.ini)|*.ini");
	if(!theApp.m_bAnalysisWorkEnvOnly)
		filter.Append("|铁塔制造助手(*.tma)|*.tma|格构铁塔制造助手(*.lma)|*.lma|铁塔绘图助手(*.tda)|*.tda|司令图模型(*.cdf)|*.cdf");
	filter.Append("|所有文件(*.*)|*.*||");
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
//#elif defined(__TSA_FILE_)
//	CFileDialog dlg(TRUE,"tsa","TSA",
//		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
//		"铁塔内力分析设计文件(*.tsa)|*.tsa|原始设计信息文件(*.tdf)|*.tdf|TTA接口塔型配置文件(*.ini)|*.ini|所有文件(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	CString szNewFilePathName=dlg.GetPathName();
	//如果当前已有打开文件，则重新启动一个exe进行文件操作
	CXhChar500 exe_path("%s%s.exe ",APP_PATH,theApp.m_pszExeName);
	exe_path.Append(szNewFilePathName,' ');
	WinExec(exe_path,SW_SHOW);
}
void CLDSDoc::OnFileSave() 
{
	CLDSView* pWorkView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pWorkView->SaveCursorView();
	if(theApp.m_bCooperativeWork)
	{	//以网络协同方式工作
		BeginWaitCursor();
		SaveDataFile();
		Ta.SetDocModifiedFlag(FALSE);
		SetModifiedFlag(FALSE);
		EndWaitCursor();
	}
	/* 直接存到网络数据服务中心上不实用 wjh-2017.6.22
#ifdef __CONNECT_REMOTE_SERVER_
	else if(theApp.m_bNetModelWork)
	{
		CServerTowerFile *pTowerFile=AgentServer.GetActiveTowerFile();
		if(pTowerFile==NULL)
		{
			AfxMessageBox("未激活任务塔型文件，无法上传数据！");
			return;
		}
		pTowerFile->m_dataBuff.ClearBuffer();
		SerializeTowerFileBuffer(pTowerFile->m_dataBuff,TRUE);
		//
		//CBuffer buffer;
		//pTowerFile->ToBuffer(buffer);
		//_ns1__SaveOrUpdateObject inputPara;
		//inputPara.clsName="TowerTypeFile";
		//inputPara.sessionId=TMS.m_idSession;
		//inputPara.byteArr=new xsd__base64Binary();
		//inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		//inputPara.byteArr->__size=buffer.GetLength();
		//_ns1__SaveOrUpdateObjectResponse response;
		if(!TMS.SaveOrUpdateObjects("TowerTypeFile",&pTowerFile))
		{
			AfxMessageBox("上传数据失败，请重试！");
			return;
		}
		//else 
		//	pTowerFile->ParseSaveOrUpdateServerRetStr(response.SaveOrUpdateObjectResult);
		//delete inputPara.byteArr;
	}
#endif
	*/
	else
	{
		CXhChar200 path_name("%s",GetPathName());
		if( Ta.Node.GetNodeNum()==0&&
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Current data model is empty，continue to store may be at risk "\
						  "(empty data file with the same name)",MB_YESNO)!=IDYES)
#else
			AfxMessageBox("当前数据模型为空，继续存储（可能清空已有同名数据文件）",MB_YESNO)!=IDYES)
#endif
			return;
		char* separator=SearchChar(path_name,'.',true);
		if(separator!=NULL&&stricmp(separator,".bak")!=0)
		{
			int product_type=PRODUCT_TYPE;
			if(PRODUCT_TYPE==1)
				strcpy(separator,".tma");
			else if(PRODUCT_TYPE==2)
				strcpy(separator,".lma");
			else if(PRODUCT_TYPE==3)
				strcpy(separator,".tsa");
			else if(PRODUCT_TYPE==4)
				strcpy(separator,".lds");
			else if(PRODUCT_TYPE==5)
				strcpy(separator,".tda");
			SetPathName(path_name,FALSE);
		}
		m_nTimer=2;	//启用备份文件存储
		CDocument::OnFileSave();
	}
}

//From Tower.cpp 保存/读取标准库 wht 11-05-23
extern void WriteStdPartLibToBuffer(CBuffer &buffer);	//保存标准库
extern void ReadStdPartLibFromBuffer(CBuffer &buffer,CTowerArchiveLibrary* pLoadToArchive,long version=0,long doc_type=0);	//读取标准库
void CLDSDoc::OnReloadFile() 
{
	BeginWaitCursor();
	//向服务器发送指令
	DWORD dwBytes;
	int retCode;
	LDS_MSGHEAD msg_head;
	CBuffer buffer(1000000);
	msg_head.command_id=ASK_DATA_CHECK_OUT;
	msg_head.msg_length=4;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	buffer.Write(&msg_head,9);
	buffer.WriteInteger(FALSE);		//不需更新权限信息
	buffer.WriteDword(0xffffffff);	//设定更新权限范围
	WriteFile(theApp.m_hPipeServerListen,buffer.GetBufferPtr(),buffer.GetLength(),&dwBytes,NULL);
	//从服务器接收数据
	Ta.Empty();
	BYTE memory_pool[PACK_SIZE];
	DWORD dwRead;
	ReadFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwRead,NULL);
	WriteFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwBytes,NULL);
	//接收协同中心反馈回来的数据资源空闲确认
	ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
	if(retCode==0)
	{//协同中心数据资源被占用
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"Collaborative center data resource temporarily occupied by other users, please be patient！");
#else 
		PutMessage(theApp.GetMainWnd(),"协同中心数据资源暂被其它用户占用，请耐心等待！");
#endif
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
		//PutMessage(theApp.GetMainWnd(),"协同中心数据资源已可用，正在准备接收数据，请耐心等待！");
	}
	//else
	//	PutMessage(theApp.GetMainWnd(),"协同中心数据资源可用，正在准备接收数据，请耐心等待！");
	DWORD nDataSize;
	BYTE verify[17]="",verify2[17]="";
	int i,j,iIteration=0;
	while(1)
	{
		ReadFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwRead,NULL);
		ReadFile(theApp.m_hPipeServerListen,&verify,16,&dwRead,NULL);
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center, please be patient！");
#else 
		PutMessage(theApp.GetMainWnd(),"正在从协同中心接收数据，请耐心等待！");
#endif
		buffer.ClearBuffer();
		while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
		{
			buffer.Write(memory_pool,PACK_SIZE-1);
			int percent=min(100,buffer.GetLength()*100/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center（Read %d%%）！",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"正在从协同中心接收数据（已读取%d%%）！",percent);
#endif
			if(memory_pool[PACK_SIZE-1]==0)
				break;	//数据接收结束
		}
		iIteration++;
		if(buffer.GetLength()<(int)nDataSize)
		{	//
			if(iIteration<2)
				retCode=0;
			else
				retCode=-1;
			WriteFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
			if(retCode==-1)
				return;	//程序出错，以后待查,可能是客户端此时出现异常中断数据传输导致-wjh 2009.5.4
		}
		else
		{
			MD5_CTX md5;
			md5.MD5Update((BYTE*)buffer.GetBufferPtr(),nDataSize);
			md5.MD5Final(verify2);
			if(strcmp((char*)verify,(char*)verify2)==0)
			{
				retCode=1;
				WriteFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
				break;
			}
			else
			{
				if(iIteration>=2)
					retCode=-1;
				else
					retCode=0;
				WriteFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
				if(retCode==-1)
				{
					PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Task load failure!");
#else 
					AfxMessageBox("任务加载失败!");
#endif
					return;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					PutMessage(theApp.GetMainWnd(),"The data transmits errors，so asking to resend, please be patient！");
#else 
					PutMessage(theApp.GetMainWnd(),"从协同中心接收到的数据传输出现错误，正在要求重发，请耐心等待！");
#endif
			}
		}
	}
	PutMessage(theApp.GetMainWnd(),"");
	buffer.SeekToBegin();
	//铁塔公用基本信息
	buffer.ReadString(Ta.version,20);
	buffer.ReadDword(&Ta.user_pipeline_no);
	buffer.ReadString(Ta.m_sPrjName,50);
	buffer.ReadString(Ta.m_sTaType,50);
	buffer.ReadString(Ta.m_sTaAlias,50);
	buffer.ReadString(Ta.m_sTaStampNo,50);
	buffer.ReadDouble(&Ta.m_fMaxXieJGLen);
	buffer.Read(Ta.cfgword.flag.word,24);
	buffer.ReadByte(&Ta.m_ciRollProcessType);
	buffer.ReadByte(&Ta.m_ciRollOffsetType);
	buffer.ReadByte(&Ta.m_ciDeformType);
	BYTE cnThickItemCount=0,cnAngleItemCount=0;
	buffer.ReadByte(&cnThickItemCount);	//厚度项分组数目
	buffer.ReadByte(&cnAngleItemCount);	//制弯角度项分组数目
	if(cnAngleItemCount<1||cnThickItemCount<1)
	{
		buffer.ReadDouble(&Ta.m_fDeformCoef);
		buffer.ReadDouble(&Ta.m_fHuoquR);
	}
	else
	{	//标题行为厚度项分组，标题列为制弯角度分组(°)
		DYN_ARRAY<double> thick_header(cnThickItemCount),angle_header(cnAngleItemCount);
		DYN_ARRAY<double> mapdata(cnThickItemCount*cnAngleItemCount);
		buffer.Read(thick_header,sizeof(double)*cnThickItemCount);
		buffer.Read(angle_header,sizeof(double)*cnAngleItemCount);
		//读入中性层系数插值表
		buffer.Read(mapdata,sizeof(double)*mapdata.Size());
		Ta.mapNeutroCoef.Init(cnAngleItemCount,cnThickItemCount,mapdata);
		Ta.mapNeutroCoef.InitHeader(thick_header,angle_header);
		//读入制弯线内圆弧插值表
		buffer.Read(mapdata,sizeof(double)*mapdata.Size());
		Ta.mapBendR.Init(cnAngleItemCount,cnThickItemCount,mapdata);
		Ta.mapBendR.InitHeader(thick_header,angle_header);
	}
	//buffer.ReadInteger(&CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	char sText[100]="";
	buffer.ReadString(sText,100);		
	Ta.m_sCompanyName=sText;		//设计单位
	Ta.weather.FromBuffer(buffer);	//气象条件
	buffer.ReadInteger(&Ta.volt_grade);//电压等级:330,500
	buffer.ReadByte(&Ta.m_cLandscape);	//塔位地形:'A','B','C'
	buffer.ReadDouble(&Ta.m_fTowerHeight);
	/*安全等级(决定结构重要性系数gama0)：
	0:一级特别重要的杆塔结构(gama0=1.1)；
	1:二级各级电压线路的各类杆塔(gama0=1.0)；
	2:三级临时使用的各类杆塔(gama0=0.9).*/
	buffer.ReadWord(&Ta.m_siSafetyGrade);
	buffer.ReadDouble(&Ta.m_fSafetyCoef);
	buffer.ReadDouble(&Ta.m_fIceDensity);	//杆塔裹冰重度kg/m3
#endif
	//铁塔准距表区
	buffer.ReadWord(&zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&zhunju_table[i].wide);	//肢宽
		buffer.ReadWord(&zhunju_table[i].g);	//标准准距g
		buffer.ReadWord(&zhunju_table[i].g1);	//准距g1
		buffer.ReadWord(&zhunju_table[i].g2);	//准距g2
		buffer.ReadWord(&zhunju_table[i].g3);	//准距g3
	}
	
    //螺栓间距表
	buffer.ReadInteger(&lsspace_N);
	for(i=0;i<lsspace_N;i++)
	{
		buffer.ReadInteger(&LSSPACE_TABLE[i].M_Diameter);
		buffer.ReadInteger(&LSSPACE_TABLE[i].SingleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].doubleRowSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].EndSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].PlateEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].RollingEdgeSpace);
		buffer.ReadInteger(&LSSPACE_TABLE[i].CutAngleSpace);
	}
	//读取螺栓零件库
	int nSeries=0,record_n=0,intVal=0;
	if(PRODUCT_TYPE!=3)
	{	
		WORD widType=0;
		buffer.ReadInteger(&nSeries);
		CLsLibrary::ClearLsSubPartLibrary();
		for(i=0;i<nSeries;i++)
		{
			buffer.ReadWord(&widType);
			CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::AppendLsSubPartFamily(widType);
			buffer.ReadString(pLsSubPartFamily->name,16);
		}
	}
	//螺栓规格系列表
	buffer.ReadInteger(&nSeries);
	CLsLibrary::ClearLibrary();
	for(i=0;i<nSeries;i++)
	{
		CLsFamily* pFamily=CLsLibrary::AppendFamily();
		buffer.ReadString(pFamily->name,16);
		buffer.ReadInteger(&record_n);
		buffer.ReadString(pFamily->descr,50);
		buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=intVal>0;
		buffer.ReadInteger(&intVal);pFamily->m_bDualCap=intVal>=2?true:false;
		buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal!=FALSE);
		buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal!=FALSE);
		buffer.ReadInteger(&pFamily->m_hMappingAntiTheftFamily);
		buffer.ReadInteger(&pFamily->m_hMappingAntiLooseFamily);
		pFamily->CreateFamily(record_n);
		for(int j=0;j<record_n;j++)
		{
			LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
			buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
			buffer.ReadInteger(&pXingHao->d);			//螺栓直径
			buffer.ReadString(pXingHao->guige,15);			//螺栓规格
			buffer.ReadInteger(&pXingHao->valid_len);	//有效长
			buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
			buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
			buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
			buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);		//螺栓理论重量
		}
	}
	//螺栓规格选择条件
	Ta.listSelBoltCond.Empty();
	int nRow;
	buffer.ReadInteger(&nRow);
	for(i=0;i<nRow;i++)
	{
		BOLT_SELCOND *pSelBolt=Ta.listSelBoltCond.append();
		buffer.ReadInteger(&pSelBolt->d);
		buffer.ReadString(pSelBolt->grade,5);
		buffer.ReadString(pSelBolt->matStr,8);
		buffer.ReadInteger(&pSelBolt->upWidthLim);
	}
	//材质库
	int steelmat_N;
	buffer.ReadInteger(&steelmat_N);
	CSteelMatLibrary::SetCount(steelmat_N);
	for(i=0;i<steelmat_N;i++)
	{
		STEELMAT mat;
		buffer.ReadString(mat.mark);
		buffer.ReadByte(&mat.cBriefMark);
		buffer.ReadByte(&mat.cPreferQualityLevel);
		buffer.ReadDouble(&mat.E);
		buffer.ReadDouble(&mat.price_coef);
		buffer.Read(mat.t_lim,sizeof(int)*3);
		buffer.Read(mat.intensity,sizeof(double)*16);
		CSteelMatLibrary::SetRecordAt(i,mat);
	}
#ifdef __PART_DESIGN_INC_
	AngleLibrary()->Count=buffer.ReadInteger();
	for(i=0;i<jgguige_N;i++)
	{
		buffer.ReadDouble(&jgguige_table[i].wing_wide);
		buffer.ReadByte(&jgguige_table[i].cType);
		buffer.ReadDouble(&jgguige_table[i].wing_thick);
		buffer.ReadDouble(&jgguige_table[i].r);
		buffer.ReadDouble(&jgguige_table[i].area);
		buffer.ReadDouble(&jgguige_table[i].theroy_weight);
		buffer.ReadDouble(&jgguige_table[i].Ix);
		buffer.ReadDouble(&jgguige_table[i].Rx);
		buffer.ReadDouble(&jgguige_table[i].Wx);
		buffer.ReadDouble(&jgguige_table[i].Iy0);
		buffer.ReadDouble(&jgguige_table[i].Ry0);
		buffer.ReadDouble(&jgguige_table[i].Wy0);
	}
	//抗震验算参数
	buffer.ReadByte(&Ta.EarthQuake.grade);			//地震烈度6,7,8,9
	buffer.ReadDouble(&Ta.EarthQuake.alfa_max);		//水平地震影响系数最大值
	buffer.ReadByte(&Ta.EarthQuake.cLandType);		//场地类别1,2,3,4
	buffer.ReadByte(&Ta.EarthQuake.cDesignGroup);	//设计地震分组1,2,3
	buffer.ReadDouble(&Ta.EarthQuake.Tg);			//特征周期,s
	buffer.ReadDouble(&Ta.EarthQuake.zeta);			//阻尼系数ζ=0.02s
	buffer.ReadDouble(&Ta.EarthQuake.gamaRE);		//承载力抗震调整系数,一般跨越塔取0.85，其余塔取0.8
	buffer.ReadInteger(&Ta.EarthQuake.nFreqsRank);
#endif
#if !defined(__TMA_FILE_)&&!defined(__TMA_)
	buffer.ReadInteger(&tubeguige_N);
	for(i=0;i<tubeguige_N;i++)
	{
		buffer.ReadDouble(&tubeguige_table[i].D);
		buffer.ReadDouble(&tubeguige_table[i].thick);
		buffer.ReadDouble(&tubeguige_table[i].area);
		buffer.ReadDouble(&tubeguige_table[i].theroy_weight);
		buffer.ReadDouble(&tubeguige_table[i].I);
		buffer.ReadDouble(&tubeguige_table[i].r);
		buffer.ReadDouble(&tubeguige_table[i].W);
	}
#endif
	//LMA或LDS支持保存钢管端连接标准库 wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LMA_)||defined(__LDS_FILE_)||defined(__LDS_)
	ReadStdPartLibFromBuffer(buffer,NULL);
#endif
	
	int h=0,nn=0,cls_typeid=0;
	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CLDSNode *pNode;
	CLDSPart *pPart;
	CBlockReference *pBlkRef;
	//基本模型区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		CLDSModule* pModule=Ta.Module.append(FALSE);
		buffer.ReadInteger(&pModule->handle);
		pModule->FromBuffer(buffer);
	}
	buffer.ReadInteger(&Ta.m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.ReadWord(&Ta.m_arrActiveQuadLegNo[i]);
	//哈希表管理信息部分
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		if(cls_typeid==CLS_GEPOINT)
		{
			pPoint=Ta.Point.append(FALSE);
			pPoint->handle=h;
		}
		else if(cls_typeid==CLS_GELINE)
		{
			pLine=Ta.Line.append(FALSE);
			pLine->handle=h;
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			pPlane=Ta.Plane.append(FALSE);
			pPlane->handle=h;
		}
		else if(cls_typeid==CLS_BLOCKREF)
		{
			pBlkRef=Ta.BlockRef.append(FALSE);
			pBlkRef->handle=h;
		}
		else if(cls_typeid==CLS_NODE)
		{
			pNode=Ta.Node.append(FALSE);
			pNode->handle=h;
		}
		else	//CLDSPart类型工件
		{
			pPart=Ta.Parts.append(cls_typeid,FALSE);
			pPart->handle=h;
		}
	}
	Ta.ReBuildObjsHashTable();	//建立全塔对象哈希表
	//显示视图数据
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CDisplayView* pDispView=Ta.View.append(FALSE);
		buffer.ReadInteger(&pDispView->handle);
		pDispView->FromBuffer(buffer);
	}
	buffer.ReadInteger(&h);
	console.SetActiveView(h);
	//圆弧预拱数据区
	CObjNoGroup *pObjNoGroup=Ta.NoManager.FromGroupID(MODEL_ARCLIFT_GROUP);
	pObjNoGroup->EmptyNo();
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CLDSArcLift* pLift=Ta.ArcLift.append(FALSE);
		buffer.ReadInteger(&pLift->handle);
		pLift->FromBuffer(buffer);
		pObjNoGroup->SetNoState(pLift->iNo);
	}
	//部件块数据
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CBlockModel* pBlock=Ta.Block.append(FALSE);
		buffer.ReadInteger(&pBlock->handle);
		pBlock->FromBuffer(buffer);
	}

	long buffer_len=buffer.GetLength();
	DisplayProcess(0);
	//基准点存储区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pPoint=(CLDSPoint*)Ta.hashObjs.GetValue(h);
		pPoint->FromBuffer(buffer,0);
	}
	//基准线存储区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pLine=(CLDSLine*)Ta.hashObjs.GetValue(h);
		pLine->FromBuffer(buffer);
	}
	//基准面存储区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pPlane=(CLDSPlane*)Ta.hashObjs.GetValue(h);
		pPlane->FromBuffer(buffer);
	}
	DisplayProcess(buffer.GetCursorPosition()*100/buffer_len);
	//节点存储区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pNode=(CLDSNode*)Ta.hashObjs.GetValue(h);
		pNode->FromBuffer(buffer);
		DisplayProcess(buffer.GetCursorPosition()*100/buffer_len);
	}
	//构件数据存储区
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		pPart=Ta.Parts.FromHandle(h,cls_typeid);
		if(pPart)
		{
			if(pPart->IsLinePart())
				((CLDSLinePart*)pPart)->FromPipeBuffer(buffer,0xffffffff);
			else
				pPart->FromBuffer(buffer);
		}
		DisplayProcess(buffer.GetCursorPosition()*100/buffer_len);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CSupplementPart *pSuppPart=Ta.SuppPart.append(FALSE);
		buffer.ReadInteger(&pSuppPart->handle);
		pSuppPart->FromBuffer(buffer,0,0);
	}
	if(DisplayProcess!=NULL)
		DisplayProcess(buffer.GetCursorPosition()*100/buffer_len,NULL);
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CLDSPartGroup *pPartGroup=Ta.PartGroup.append();
		pPartGroup->FromBuffer(buffer,0,0);
		if(DisplayProcess!=NULL)
			DisplayProcess(buffer.GetCursorPosition()*100/buffer_len,NULL);
	}
	
	for(CLDSParamPlate *pParamPlate=(CLDSParamPlate*)Ta.Parts.GetFirst(CLS_PARAMPLATE);pParamPlate;pParamPlate=(CLDSParamPlate*)Ta.Parts.GetNext(CLS_PARAMPLATE))
	{
		for(ATTACHPART_ITEM *pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
		{
			if(Ta.hashObjs.GetNodeNum()>0)//GetHashTableSize()>0)
				pPart=(CLDSPart*)Ta.hashObjs.GetValue(pItem->h);
			else
				pPart=Ta.Parts.FromHandle(pItem->h);
			if(pPart)
				strcpy(pItem->sPartNo,pPart->GetPartNo());
		}
	}
#endif
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	/*CWireLayout *pWireLayout=NULL;
	CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WIRELAYOUT_GROUP);
	int nWireLayout=1;
	pNoGroup->EmptyNo();
	Ta.groupWireLayout.Empty();
	buffer.ReadInteger(&nWireLayout);
	for(int iLayout=0;iLayout<nWireLayout;iLayout++)
	{
		pWireLayout=Ta.groupWireLayout.append();
		buffer.ReadInteger(&pWireLayout->iNo);
		pNoGroup->SetNoState(pWireLayout->iNo);
		buffer.ReadString(pWireLayout->name,49);

		//原始线条计算荷载
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			int iNo=0;
			buffer.ReadInteger(&iNo);
			CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.Add(iNo);
			pWireModel->iNo=iNo;
			pWireModel->FromBuffer(buffer);
		}
		//工况组合信息
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			long key=0;
			buffer.ReadInteger(&key);
			RAWWORKSTATUS_STRU* pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.Add(key);
			buffer.ReadString(pRawStatus->name);
			buffer.ReadString(pRawStatus->lim_str);	//新加
			buffer.ReadInteger(&pRawStatus->m_hWorkStatus);			//对应的CWorkStatus的句柄值
			buffer.ReadByte(&pRawStatus->m_cMainLoadType);			//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
			buffer.ReadBooleanThin(&pRawStatus->m_bCheckWorkCond);	//验算工况
			buffer.ReadBooleanThin(&pRawStatus->m_bMinVertSpace);	//最小垂直档距工况
			buffer.ReadDouble(&pRawStatus->ice_thick);				//基本裹冰厚度(mm)
			buffer.ReadInteger(&pRawStatus->WindLoad.azimuth);		//风荷载,风向角(°)
			buffer.ReadDouble(&pRawStatus->WindLoad.velocity);		//风荷载,风速(m/s)
		}
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			long key=0;
			buffer.ReadInteger(&key);
			LOADNODEROOM_STRU* pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.Add(key);
			buffer.ReadInteger(&key);
			pRoom->m_iRawWireLoadNo=key;
			buffer.ReadString(pRoom->helpString);
			//写入点号位对应的默认原始工况标识
			long j,k,n2,n3,key2;
			buffer.ReadInteger(&n2);
			for(j=0;j<n2;j++)
			{
				buffer.ReadInteger(&key);
				buffer.ReadInteger(&key2);
				pRoom->hashWorkEnv.SetValue(key,key2);
			}
			//写入当前点号位对应的点号信息
			buffer.ReadInteger(&n2);
			for(j=0;j<n2;j++)
			{
				buffer.ReadInteger(&key);
				WIRENODE_STRU *pRawNode=pRoom->hashNode.Add(key);
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefX);
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefY);
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefZ);
				buffer.ReadInteger(&pRawNode->hNode);
				buffer.ReadInteger(&n3);
				for(k=0;k<n3;k++)
				{
					buffer.ReadInteger(&key);
					buffer.ReadInteger(&key2);
					pRawNode->hashWorkEnv.SetValue(key,key2);
				}
			}
		}
	}
	*/
	//工作状况
	nn=0;
	buffer.ReadInteger(&nn);
	pObjNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
	pObjNoGroup->EmptyNo();
	for(i=0;i<nn;i++)
	{
		CWorkStatus *pStatus=Ta.WorkStatus.append();
		buffer.ReadInteger(&h);
		pStatus->handle=h;
		pStatus->FromBuffer(buffer,0);
		pObjNoGroup->SetNoState(pStatus->iNo);

	}
	//附加风荷载
	buffer.ReadInteger(&nn);
	pObjNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
	for(i=0;i<nn;i++)
	{
		CAttachWindLoad *pAttachWindLoad=Ta.AttachWindLoad.append();
		buffer.ReadInteger(&h);
		pAttachWindLoad->handle=h;
		pAttachWindLoad->FromBuffer(buffer);
		pObjNoGroup->SetNoState(pAttachWindLoad->iNo);
	}
	//风压段信息
	buffer.ReadInteger(&nn);
	pObjNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
	pObjNoGroup->EmptyNo();
	for(i=0;i<nn;i++)
	{
		CWindSegment *pWindSeg=Ta.WindLoadSeg.append();
		pWindSeg->FromBuffer(buffer);
		pObjNoGroup->SetNoState(pWindSeg->iNo);
	}
	//钢管法兰设计信息
	Ta.hashConnectFl.Empty();
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//主管壁厚(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//基准法兰内环半径(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//配对法兰内环半径(mm)
		buffer.ReadString(pFL->bolt_grade,8);		//螺栓级别
		buffer.ReadInteger(&pFL->bolt_d);			//螺栓直径（规格）
		buffer.ReadInteger(&pFL->bolt_n);			//螺栓组中的螺栓数量
		buffer.ReadDouble(&pFL->N);
		buffer.ReadDouble(&pFL->M);				//N 轴向力(N),M弯矩(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//螺栓圆半径(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//法兰环形带宽(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//肋板厚度(mm),厚度为0表示无加劲肋
		buffer.ReadInteger(&pFL->m_nHeightRib);		//肋板高度(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//配对肋板焊缝切角高(宽)度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//法兰盘厚度(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//配对肋板高度(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //构件编号
	}
#endif

	DisplayProcess(100);
	Ta.SetDocModifiedFlag(FALSE);
	SetModifiedFlag(FALSE);
	UpdateAllViews(NULL);
	EndWaitCursor();
	SetModifiedFlag(FALSE);
}
static void UpdatePermFromServer()
{
	DWORD dwRead;
	BYTE memory_pool[PACK_SIZE];
	CBuffer buffer;
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"It is reading update permission information from server！");
#else 
	PutMessage(theApp.GetMainWnd(),"正在从服务器读取更新权限信息！");
#endif
	buffer.ClearBuffer();
	//读取消息内容（提取数据的用户范围）
	while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
	{
		buffer.Write(memory_pool,PACK_SIZE-1);
		if(memory_pool[PACK_SIZE-1]==0)
			break;	//数据接收结束
	}
	buffer.SeekToBegin();
	int i,n;
	DWORD dwPermission,dwStartPermission,dwEndPermission;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long h;
		buffer.ReadInteger(&h);
		buffer.ReadDword(&dwPermission);
		buffer.ReadDword(&dwStartPermission);
		buffer.ReadDword(&dwEndPermission);
		CLDSObject *pObj=Ta.FromHandle(h);
		if(pObj)
		{
			pObj->dwPermission=dwPermission;
			if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
				pLinePart->dwStartPermission=dwStartPermission;
				pLinePart->dwEndPermission=dwEndPermission;
			}
		}
		else
		{
			CBlockModel *pBlock=Ta.Block.FromHandle(h);
			if(pBlock)
				pBlock->dwPermission=dwPermission;
		}
	}
	theApp.m_bNeedUpdatePerm=FALSE;
}
void CLDSDoc::OnUpdateDataFile() 
{
	int retCode;
	CUpdateUserDataDlg update_dlg;
	update_dlg.m_dwUpdateUser=theApp.m_dwUpdateWord;
	DWORD update_word=theApp.m_dwUpdateWord;

	//向服务器发送指令提取用户信息
	LDS_MSGHEAD msg_head;
	DWORD dwBytes,dwRead;
	CBuffer buffer(1000000);
	BYTE memory_pool[PACK_SIZE];
	theApp.GetUserInfo(update_dlg.user_arr);
	if(update_dlg.DoModal()==IDOK)
		update_word=update_dlg.m_dwUpdateUser;
	else
		return;

	//向服务器发送指令
	BeginWaitCursor();
	msg_head.command_id=ASK_DATA_CHECK_OUT;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	WriteFile(theApp.m_hPipeServerListen,&theApp.m_bNeedUpdatePerm,4,&dwBytes,NULL);
	WriteFile(theApp.m_hPipeServerListen,&update_word,4,&dwBytes,NULL);
	if(theApp.m_bNeedUpdatePerm)
	{	//需要更新权限信息
		UpdatePermFromServer();
	}
	int fileid;
	ReadFile(theApp.m_hPipeServerListen,&fileid,4,&dwRead,NULL);
	if(fileid!=m_idServerFile)
		fileid=0;
	WriteFile(theApp.m_hPipeServerListen,&fileid,4,&dwBytes,NULL);
	if(fileid==0)
	{
		PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Client and collaborative center operate different file, update failed！");	//
#else 
		AfxMessageBox("客户端与协同中心操作的是不同的文件，更新失败！");	//
#endif
		return;
	}
	//接收协同中心反馈回来的数据资源空闲确认
	ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
	if(retCode==0)
	{//协同中心数据资源被占用
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"Collaborative center data resource temporarily occupied by other users, please be patient！");
#else 
		PutMessage(theApp.GetMainWnd(),"协同中心数据资源暂被其它用户占用，请耐心等待！");
#endif
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
		//PutMessage(theApp.GetMainWnd(),"协同中心数据资源已可用，正在准备接收数据，请耐心等待！");
	}
	//else
	//	PutMessage(theApp.GetMainWnd(),"协同中心数据资源可用，正在准备接收数据，请耐心等待！");
	DWORD nDataSize;
	char verify[17]="",verify2[17]="";
	int iIteration=0;
	while(1)
	{
		ReadFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwRead,NULL);
		ReadFile(theApp.m_hPipeServerListen,verify,16,&dwRead,NULL);
		//从服务器接收更新数据
		buffer.ClearBuffer();
		int nRecSize=0;
		while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
		{
			buffer.Write(memory_pool,PACK_SIZE-1);
			nRecSize+=PACK_SIZE-1;
			int percent=min(100,buffer.GetLength()*100/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center（Read %d%%）！",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"正在从协同中心接收数据（已读取%d%%）！",percent);
#endif
			if(memory_pool[PACK_SIZE-1]==0)
				break;	//数据接收结束
		}
		MD5_CTX md5;
		md5.MD5Update((BYTE*)buffer.GetBufferPtr(),nDataSize);
		md5.MD5Final((BYTE*)verify2);
		if(strcmp(verify,verify2)==0)
		{
			retCode=1;
			WriteFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
			break;
		}
		else
		{
			retCode=0;
			WriteFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"The data transmits errors，so asking to resend, please be patient！");
#else 
			PutMessage(theApp.GetMainWnd(),"从协同中心接收到的数据传输出现错误，正在要求重发，请耐心等待！");
#endif
		}
		iIteration++;
		if(iIteration>=2)
		{
			PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data update failed!");
#else 
			AfxMessageBox("数据更新失败!");
#endif
			return;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"Data reads over, began to update the stored data！");
#else 
	PutMessage(theApp.GetMainWnd(),"数据读取结束，开始更新内存数据！");
#endif
	long i,nn;
	buffer.SeekToBegin();
	CObjNoGroup *pNoGroup=NULL;
	if(update_word&PERM_TOWER_ADMIN)	//管理员
	{	//基本信息区
		buffer.ReadString(Ta.version);		//保存文件版本号
		buffer.ReadDword(&Ta.user_pipeline_no);
		buffer.ReadString(Ta.m_sPrjName);	//保存项目名称
		buffer.ReadString(Ta.m_sTaType);	//保存塔型
		buffer.ReadString(Ta.m_sTaStampNo);//保存铁塔钢印号
		buffer.ReadString(Ta.m_sTaAlias);	//保存铁塔代号
		buffer.ReadDouble(&Ta.m_fMaxXieJGLen);	//保存最大斜材长度
		buffer.Read(Ta.cfgword.flag.word,24);	//保存总配材号
		buffer.ReadByte(&Ta.m_ciRollProcessType);
		buffer.ReadByte(&Ta.m_ciRollOffsetType);
		buffer.ReadByte(&Ta.m_ciDeformType);
		BYTE cnThickItemCount=0,cnAngleItemCount=0;
		buffer.ReadByte(&cnThickItemCount);	//厚度项分组数目
		buffer.ReadByte(&cnAngleItemCount);	//制弯角度项分组数目
		if(cnAngleItemCount<1||cnThickItemCount<1)
		{
			buffer.ReadDouble(&Ta.m_fDeformCoef);
			buffer.ReadDouble(&Ta.m_fHuoquR);
		}
		else
		{	//标题行为厚度项分组，标题列为制弯角度分组(°)
			DYN_ARRAY<double> thick_header(cnThickItemCount),angle_header(cnAngleItemCount);
			DYN_ARRAY<double> mapdata(cnThickItemCount*cnAngleItemCount);
			buffer.Read(thick_header,sizeof(double)*cnThickItemCount);
			buffer.Read(angle_header,sizeof(double)*cnAngleItemCount);
			//读入中性层系数插值表
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			Ta.mapNeutroCoef.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			Ta.mapNeutroCoef.InitHeader(thick_header,angle_header);
			//读入制弯线内圆弧插值表
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			Ta.mapBendR.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			Ta.mapBendR.InitHeader(thick_header,angle_header);
		}
		//buffer.ReadInteger(&CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		char sText[100]="";
		buffer.ReadString(sText);		
		Ta.m_sCompanyName=sText;		//设计单位
		//气象条件
		Ta.weather.FromBuffer(buffer);
		buffer.ReadInteger(&Ta.volt_grade);//电压等级:330,500
		buffer.ReadByte(&Ta.m_cLandscape);	//塔位地形:'A','B','C'
		buffer.ReadDouble(&Ta.m_fTowerHeight);
		/*安全等级(决定结构重要性系数gama0)：
		0:一级特别重要的杆塔结构(gama0=1.1)；
		1:二级各级电压线路的各类杆塔(gama0=1.0)；
		2:三级临时使用的各类杆塔(gama0=0.9).*/
		buffer.ReadWord(&Ta.m_siSafetyGrade);
		buffer.ReadDouble(&Ta.m_fSafetyCoef);
		buffer.ReadDouble(&Ta.m_fIceDensity);	//杆塔裹冰重度kg/m3
#endif
		//铁塔准距表区
		buffer.ReadWord(&zhunju_N);	//保存所用准距数目
		for(i=0;i<zhunju_N;i++)
		{
			buffer.ReadWord(&zhunju_table[i].wide);	//肢宽
			buffer.ReadWord(&zhunju_table[i].g);	//标准准距g
			buffer.ReadWord(&zhunju_table[i].g1);	//准距g1
			buffer.ReadWord(&zhunju_table[i].g2);	//准距g2
			buffer.ReadWord(&zhunju_table[i].g3);	//准距g3
		}
		//螺栓间距表
		buffer.ReadInteger(&lsspace_N);
		for(i=0;i<lsspace_N;i++)
		{
			buffer.ReadInteger(&LSSPACE_TABLE[i].M_Diameter);
			buffer.ReadInteger(&LSSPACE_TABLE[i].SingleRowSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].doubleRowSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].EndSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].PlateEdgeSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].RollingEdgeSpace);
			buffer.ReadInteger(&LSSPACE_TABLE[i].CutAngleSpace);
		}
		//读取螺栓零件库
		int nSeries=0,record_n=0,intVal=0;
		if(PRODUCT_TYPE!=3)
		{	
			WORD widType=0;
			buffer.ReadInteger(&nSeries);
			CLsLibrary::ClearLsSubPartLibrary();
			for(i=0;i<nSeries;i++)
			{
				buffer.ReadWord(&widType);
				CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::AppendLsSubPartFamily(widType);
				buffer.ReadString(pLsSubPartFamily->name,16);
			}
		}
		//螺栓规格系列信息
		buffer.ReadInteger(&nSeries);
		CLsLibrary::ClearLibrary();
		for(i=0;i<nSeries;i++)
		{
			CLsFamily* pFamily=CLsLibrary::AppendFamily();
			buffer.ReadString(pFamily->name,16);
			buffer.ReadInteger(&record_n);
			buffer.ReadString(pFamily->descr,50);
			buffer.ReadInteger(&intVal);pFamily->m_bAntiLoose=intVal>0;
			buffer.ReadInteger(&intVal);pFamily->m_bDualCap=intVal>=2?true:false;
			buffer.ReadInteger(&intVal);pFamily->m_bFootNail=(intVal!=FALSE);
			buffer.ReadInteger(&intVal);pFamily->m_bBurglarproof=(intVal!=FALSE);
			buffer.ReadInteger(&pFamily->m_hMappingAntiTheftFamily);
			buffer.ReadInteger(&pFamily->m_hMappingAntiLooseFamily);
			pFamily->CreateFamily(record_n);
			for(int j=0;j<record_n;j++)
			{
				LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
				buffer.ReadDouble(&pXingHao->grade);			//螺栓级别
				buffer.ReadInteger(&pXingHao->d);			//螺栓直径
				buffer.ReadString(pXingHao->guige);			//螺栓规格
				buffer.ReadInteger(&pXingHao->valid_len);	//有效长
				buffer.ReadInteger(&pXingHao->no_thread_len);//无扣长
				buffer.ReadInteger(&pXingHao->down_lim);		//螺栓通厚下限
				buffer.ReadInteger(&pXingHao->up_lim);		//螺栓通厚上限
				buffer.ReadInteger(&pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
				buffer.ReadDouble(&pXingHao->weight);		//螺栓理论重量
			}
		}
		//螺栓规格选择条件
		Ta.listSelBoltCond.Empty();
		int nRow;
		buffer.ReadInteger(&nRow);
		for(i=0;i<nRow;i++)
		{
			BOLT_SELCOND *pSelBolt=Ta.listSelBoltCond.append();
			buffer.ReadInteger(&pSelBolt->d);
			buffer.ReadString(pSelBolt->grade,5);
			buffer.ReadString(pSelBolt->matStr,8);
			buffer.ReadInteger(&pSelBolt->upWidthLim);
		}
		//材质库
		int steelmat_N;
		buffer.ReadInteger(&steelmat_N);
		CSteelMatLibrary::SetCount(steelmat_N);
		for(i=0;i<steelmat_N;i++)
		{
			STEELMAT mat;
			buffer.ReadString(mat.mark);
			buffer.ReadByte(&mat.cBriefMark);
			buffer.ReadByte(&mat.cPreferQualityLevel);
			buffer.ReadDouble(&mat.E);
			buffer.ReadDouble(&mat.price_coef);
			buffer.Read(mat.t_lim,sizeof(int)*3);
			buffer.Read(mat.intensity,sizeof(double)*16);
			CSteelMatLibrary::SetRecordAt(i,mat);
		}
#ifdef __PART_DESIGN_INC_
		AngleLibrary()->Count=buffer.ReadInteger();
		for(i=0;i<jgguige_N;i++)
		{
			buffer.ReadDouble(&jgguige_table[i].wing_wide);
			buffer.ReadByte(&jgguige_table[i].cType);
			buffer.ReadDouble(&jgguige_table[i].wing_thick);
			buffer.ReadDouble(&jgguige_table[i].r);
			buffer.ReadDouble(&jgguige_table[i].area);
			buffer.ReadDouble(&jgguige_table[i].theroy_weight);
			buffer.ReadDouble(&jgguige_table[i].Ix);
			buffer.ReadDouble(&jgguige_table[i].Rx);
			buffer.ReadDouble(&jgguige_table[i].Wx);
			buffer.ReadDouble(&jgguige_table[i].Iy0);
			buffer.ReadDouble(&jgguige_table[i].Ry0);
			buffer.ReadDouble(&jgguige_table[i].Wy0);
		}
		//抗震验算参数
		buffer.ReadByte(&Ta.EarthQuake.grade);			//地震烈度6,7,8,9
		buffer.ReadDouble(&Ta.EarthQuake.alfa_max);		//水平地震影响系数最大值
		buffer.ReadByte(&Ta.EarthQuake.cLandType);		//场地类别1,2,3,4
		buffer.ReadByte(&Ta.EarthQuake.cDesignGroup);	//设计地震分组1,2,3
		buffer.ReadDouble(&Ta.EarthQuake.Tg);			//特征周期,s
		buffer.ReadDouble(&Ta.EarthQuake.zeta);			//阻尼系数ζ=0.02s
		buffer.ReadDouble(&Ta.EarthQuake.gamaRE);		//承载力抗震调整系数,一般跨越塔取0.85，其余塔取0.8
		buffer.ReadInteger(&Ta.EarthQuake.nFreqsRank);
#endif
#ifndef __TMA_FILE_
		buffer.ReadInteger(&tubeguige_N);
		for(i=0;i<tubeguige_N;i++)
		{
			buffer.ReadDouble(&tubeguige_table[i].D);
			buffer.ReadDouble(&tubeguige_table[i].thick);
			buffer.ReadDouble(&tubeguige_table[i].area);
			buffer.ReadDouble(&tubeguige_table[i].theroy_weight);
			buffer.ReadDouble(&tubeguige_table[i].I);
			buffer.ReadDouble(&tubeguige_table[i].r);
			buffer.ReadDouble(&tubeguige_table[i].W);
		}
#endif
		//LMA或LDS支持保存钢管端连接标准库 wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LMA_)||defined(__LDS_FILE_)||defined(__LDS_)
		ReadStdPartLibFromBuffer(buffer,NULL);
#endif
		//基本模型区
		buffer.ReadInteger(&nn);
		Ta.Module.Empty();
		for(int jj=0;jj<nn;jj++)
		{
			CLDSModule *pModule=Ta.Module.append(FALSE);
			buffer.ReadInteger(&pModule->handle);
			pModule->FromBuffer(buffer);
		}
		buffer.ReadInteger(&Ta.m_hActiveModule);
		for(i=0;i<4;i++)
			buffer.ReadWord(&Ta.m_arrActiveQuadLegNo[i]);
	}
	CLDSObject *pObj=NULL;
	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CLDSNode *pNode;
	CLDSPart *pPart;
	CBlockReference *pBlkRef;
	long h,cls_typeid;
	//哈希表管理信息部分
	Ta.ReBuildObjsHashTable();	//建立全塔对象哈希表
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		if((pObj=Ta.hashObjs.GetValue(h))!=NULL)
		{
			if(cls_typeid==pObj->GetClassTypeId())
				continue;	//已存在此构件不必新添加
			else
			{
				//Ta.DispPartSet.DeleteNode(h);
				console.DeletePart(h,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Object' handle appear repeated, please contact the software provider！");
#else 
				AfxMessageBox("出现对象句柄重复，请与软件供应商联系！");
#endif
			}
		}
		if(cls_typeid==CLS_GEPOINT)
		{
			pPoint=Ta.Point.append(FALSE);
			pPoint->handle=h;
			Ta.hashObjs.SetValue(h,pPoint);
		}
		else if(cls_typeid==CLS_GELINE)
		{
			pLine=Ta.Line.append(FALSE);
			pLine->handle=h;
			Ta.hashObjs.SetValue(h,pLine);
		}
		else if(cls_typeid==CLS_GEPLANE)
		{
			pPlane=Ta.Plane.append(FALSE);
			pPlane->handle=h;
			Ta.hashObjs.SetValue(h,pPlane);
		}
		else if(cls_typeid==CLS_BLOCKREF)
		{
			pBlkRef=Ta.BlockRef.append(FALSE);
			pBlkRef->handle=h;
			Ta.hashObjs.SetValue(h,pBlkRef);
		}
		else if(cls_typeid==CLS_NODE)
		{
			pNode=Ta.Node.append(FALSE);
			pNode->handle=h;
			Ta.hashObjs.SetValue(h,pNode);
		}
		else	//CLDSPart类型工件
		{
			pPart=Ta.Parts.append(cls_typeid,FALSE);
			pPart->handle=h;
			Ta.hashObjs.SetValue(h,pPart);
		}
	}
	//显示视图数据
	buffer.ReadInteger(&nn);
	CDisplayView* pDispView;
	for(pDispView=Ta.GetFirstDisplayView();pDispView;pDispView=Ta.GetNextDisplayView())
		pDispView->feature=0;	//置删除标记
	long hView;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&hView);
		pDispView=Ta.View.FromHandle(hView);
		if(pDispView==NULL)
			pDispView=Ta.View.append(FALSE);
		pDispView->handle=hView;
		pDispView->FromBuffer(buffer);
		pDispView->feature=1;	//置保留标记
	}
	//协同中心当前视图，此处只是在重新打开文件时有用，更新数据情况下不修改用户的当前视图
	buffer.ReadInteger(&hView);
	for(pDispView=Ta.GetFirstDisplayView();pDispView;pDispView=Ta.GetNextDisplayView())
	{
		if(pDispView->feature==0&&pDispView->dwPermission&update_word)
		{
			if(pDispView==console.GetActiveView())
				console.SetActiveView(0);	//设为系统默认视图
			if(pDispView!=console.GetActiveView())	//必须确保删除视图为当前视图情况下不出错
				Ta.View.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
		}
	}
	Ta.View.Clean();
	//圆弧预拱数据区
	buffer.ReadInteger(&nn);
	CLDSArcLift* pLift;
	for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
		pLift->feature=0;
	for(i=0;i<nn;i++)
	{
		long hLift=0;
		buffer.ReadInteger(&hLift);
		if((pLift=Ta.ArcLift.FromHandle(hLift))==NULL)
		{
			pLift=Ta.ArcLift.append(FALSE);
			pLift->handle=hLift;
		}
		pLift->FromBuffer(buffer);
		pLift->feature=1;
	}
	for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
	{
		if(pLift->feature==0&&pLift->dwPermission&update_word)
			Ta.ArcLift.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
	}
	Ta.ArcLift.Clean();
	//部件块数据
	buffer.ReadInteger(&nn);
	CBlockModel* pBlock;
	for(pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
		pBlock->feature=0;	//置删除标记
	long hBlock;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&hBlock);
		pBlock=Ta.Block.FromHandle(hBlock);
		if(pBlock==NULL)
		{
			pBlock=Ta.Block.append(FALSE);//AppendDisplayView();
			pBlock->handle=hBlock;
		}
		pBlock->FromBuffer(buffer);
		pBlock->feature=1;	//置保留标记
	}
	for(pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		if(pBlock->feature==0&&pBlock->dwPermission&update_word)
			Ta.Block.DeleteCursor();
	}
	Ta.Block.Clean();

	//基准点存储区
	buffer.ReadInteger(&nn);
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
		pPoint->feature=0;	//置删除标记
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		pPoint=(CLDSPoint*)Ta.hashObjs.GetValue(h);
		pPoint->FromBuffer(buffer,0);
		pPoint->feature=1;	//置保留标记
	}
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		if(pPoint->feature==0&&pPoint->dwPermission&update_word)
			Ta.Point.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
	}
	Ta.Point.Clean();
	//基准线存储区
	buffer.ReadInteger(&nn);
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
		pLine->feature=0;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		pLine=(CLDSLine*)Ta.hashObjs.GetValue(h);
		pLine->FromBuffer(buffer);
		pLine->feature=1;
	}
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		if(pLine->feature==0&&pLine->dwPermission&update_word)
			Ta.Line.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
	}
	Ta.Line.Clean();
	//基准面存储区
	buffer.ReadInteger(&nn);
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
		pPlane->feature=0;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		pPlane=(CLDSPlane*)Ta.hashObjs.GetValue(h);
		pPlane->FromBuffer(buffer);
		pPlane->feature=1;
	}
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		if(pPlane->feature==0&&pPlane->dwPermission&update_word)
			Ta.Plane.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
	}
	Ta.Plane.Clean();
	buffer.ReadInteger(&nn);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		pNode->feature=0;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		pNode=(CLDSNode*)Ta.hashObjs.GetValue(h);
		pNode->FromBuffer(buffer);
		pNode->feature=1;
	}
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->feature==0&&pNode->dwPermission&update_word)
			Ta.Node.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
	}
	Ta.Node.Clean();
	//构件信息区
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"It is updating parts' data！");
#else 
	PutMessage(theApp.GetMainWnd(),"正在更新构件数据！");
#endif
	buffer.ReadInteger(&nn);
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		pPart->feature=0;	//置为0表示删除
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		pPart=(CLDSPart*)Ta.hashObjs.GetValue(h);
		pPart->feature=1;
		if(pPart->IsLinePart())
			((CLDSLinePart*)pPart)->FromPipeBuffer(buffer,update_word);
		else
			pPart->FromBuffer(buffer);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->feature==0&&pPart->dwPermission&update_word)
		{
			if(pPart->GetClassTypeId()==CLS_BOLT)
			{	//防止删除螺栓后还存在指向此螺栓的螺栓引用
				BOOL bPush=Ta.Parts.push_stack();
				for(CLDSPart *pContainPart=Ta.Parts.GetFirst();pContainPart;pContainPart=Ta.Parts.GetNext())
				{
					BOOL bDelete=FALSE;
					if(pContainPart->GetClassTypeId()==CLS_LINEANGLE||pContainPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						bDelete=((CLDSLineAngle*)pContainPart)->DeleteLsRef(pPart->handle);
					else if(pContainPart->GetClassTypeId()==CLS_PLATE)
						bDelete=((CLDSPlate*)pContainPart)->DeleteLsRef(pPart->handle);
					else if(pContainPart->GetClassTypeId()==CLS_PARAMPLATE)
						bDelete=((CLDSParamPlate*)pContainPart)->DeleteLsRef(pPart->handle);
					else if(pContainPart->GetClassTypeId()==CLS_LINESLOT)
						bDelete=((CLDSLineSlot*)pContainPart)->DeleteLsRef(pPart->handle);
					if(bDelete)
						pContainPart->SetModified();
				}
				if(bPush)
					Ta.Parts.pop_stack();
			}
			Ta.Parts.DeleteCursor(FALSE,FALSE);	//删除时其余协同伙伴的数据对象不能进行权限验证,否则显然通不过 wjh-2016.3.29
		}
	}
#endif
	if(update_word&PERM_TOWER_ADMIN)	//管理员
	{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			CSupplementPart *pSuppPart=Ta.SuppPart.append(FALSE);
			buffer.ReadInteger(&pSuppPart->handle);
			pSuppPart->FromBuffer(buffer,0,0);
		}
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			CLDSPartGroup *pPartGroup=Ta.PartGroup.append();
			pPartGroup->FromBuffer(buffer,0,0);
		}
#endif
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		//工作状况
		nn=0;
		buffer.ReadInteger(&nn);
		CObjNoGroup *pObjNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
		pObjNoGroup->EmptyNo();
		Ta.WorkStatus.Empty();
		for(i=0;i<nn;i++)
		{
			CWorkStatus *pStatus=Ta.WorkStatus.append();
			buffer.ReadInteger(&h);
			pStatus->handle=h;
			pStatus->FromBuffer(buffer,0);
			pObjNoGroup->SetNoState(pStatus->iNo);

		}
		//附加风荷载
		buffer.ReadInteger(&nn);
		pObjNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
		Ta.AttachWindLoad.Empty();
		for(i=0;i<nn;i++)
		{
			CAttachWindLoad *pAttachWindLoad=Ta.AttachWindLoad.append();
			buffer.ReadInteger(&h);
			pAttachWindLoad->handle=h;
			pAttachWindLoad->FromBuffer(buffer);
			pObjNoGroup->SetNoState(pAttachWindLoad->iNo);
		}
		//风压段信息
		buffer.ReadInteger(&nn);
		pObjNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
		pObjNoGroup->EmptyNo();
		Ta.WindLoadSeg.Empty();
		for(i=0;i<nn;i++)
		{
			CWindSegment *pWindSeg=Ta.WindLoadSeg.append();
			pWindSeg->FromBuffer(buffer);
			pObjNoGroup->SetNoState(pWindSeg->iNo);
		}
		//钢管法兰设计信息
		Ta.hashConnectFl.Empty();
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			CConnectFL *pFL=Ta.hashConnectFl.append();
			buffer.ReadDouble(&pFL->m_fThickTube);			//主管壁厚(mm)
			buffer.ReadDouble(&pFL->m_fInnerRadius);			//基准法兰内环半径(mm)
			buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//配对法兰内环半径(mm)
			buffer.ReadString(pFL->bolt_grade,8);		//螺栓级别
			buffer.ReadInteger(&pFL->bolt_d);			//螺栓直径（规格）
			buffer.ReadInteger(&pFL->bolt_n);			//螺栓组中的螺栓数量
			buffer.ReadDouble(&pFL->N);
			buffer.ReadDouble(&pFL->M);				//N 轴向力(N),M弯矩(N*m)
			buffer.ReadDouble(&pFL->m_fBoltRadius);	//螺栓圆半径(mm)
			buffer.ReadInteger(&pFL->m_nThickFL);		//法兰盘厚度(mm)
			buffer.ReadInteger(&pFL->m_nWidthFL);		//法兰环形带宽(mm)
			buffer.ReadInteger(&pFL->m_nThickRib);		//肋板厚度(mm),厚度为0表示无加劲肋
			buffer.ReadInteger(&pFL->m_nHeightRib);		//肋板高度(mm)
			buffer.ReadInteger(&pFL->m_nRibS);			//肋板焊缝切角高(宽)度(mm)
			buffer.ReadInteger(&pFL->m_nCoupleRibS);	//配对肋板焊缝切角高(宽)度(mm)
			buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//法兰盘厚度(mm)
			buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//配对肋板高度(mm)
			buffer.ReadByte(&pFL->cMaterial);
			buffer.ReadString(pFL->m_sPartNo);     //构件编号
		}
#endif
	}
	
	Ta.hashObjs.Empty();
	PutMessage(theApp.GetMainWnd(),"");

	theApp.m_dwUpdateWord-=theApp.m_dwUpdateWord&update_word;
	((CMainFrame*)theApp.GetMainWnd())->KillInfoPrompt();
	EndWaitCursor();
	UpdateAllViews(NULL);
}
BOOL CLDSDoc::SaveDataFile()
{
	DWORD update_word;
	BYTE memory_pool[PACK_SIZE];
	if(theApp.m_nUserId==0)	//管理员
		update_word=PERM_TOWER_ADMIN;
	else					//普通用户
		update_word=GetSingleWord(theApp.m_nUserId);

	//向服务器发送指令
	LDS_MSGHEAD msg_head;
	DWORD dwBytes;
	CBuffer buffer(1000000);
	msg_head.command_id=ASK_DATA_CHECK_IN;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	//发送消息头
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	//发送与服务器协调一致的文件ID
	WriteFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwBytes,NULL);
	int retCode;
	//接收协同中心返馈回来的文件ID一致确认
	if(!ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL))
		return FALSE;
	else if(retCode==0)	//文件ID不匹配，中断传送
		return FALSE;
	WriteFile(theApp.m_hPipeServerListen,&theApp.m_bNeedUpdatePerm,4,&dwBytes,NULL);
	if(theApp.m_bNeedUpdatePerm)
		UpdatePermFromServer();//需要更新权限信息
	//向服务器发送更新数据
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"Client data is packaged,It is ready to send to collabrative center, please be patient！");
#else 
	PutMessage(theApp.GetMainWnd(),"正在将客户端数据打包，准备向协同中心发送，请耐心等待！");
#endif
	buffer.ClearBuffer();
	if(theApp.m_nUserId==0)	//管理员
	{	//基本信息区
		buffer.WriteString(Ta.version);		//保存文件版本号
		buffer.WriteDword(Ta.user_pipeline_no);
		buffer.WriteString(Ta.m_sPrjName);	//保存项目名称
		buffer.WriteString(Ta.m_sTaType);	//保存塔型
		buffer.WriteString(Ta.m_sTaStampNo);//保存铁塔钢印号
		buffer.WriteString(Ta.m_sTaAlias);	//保存铁塔代号
		buffer.WriteDouble(Ta.m_fMaxXieJGLen);	//保存最大斜材长度
		buffer.Write(Ta.cfgword.flag.word,24);	//保存总配材号
		buffer.WriteByte(Ta.m_ciRollProcessType);
		buffer.WriteByte(Ta.m_ciRollOffsetType);
		buffer.WriteByte(Ta.m_ciDeformType);
		BYTE cnThickItemCount=(BYTE)Ta.mapNeutroCoef.ColCount;
		BYTE cnAngleItemCount=(BYTE)Ta.mapNeutroCoef.RowCount;
		buffer.WriteByte(cnThickItemCount);
		buffer.WriteByte(cnAngleItemCount);
		if(cnAngleItemCount<1||cnThickItemCount<1)
		{
			buffer.WriteDouble(Ta.m_fDeformCoef);
			buffer.WriteDouble(Ta.m_fHuoquR);
		}
		else
		{	//标题行为厚度项分组，标题列为制弯角度分组(°)
			buffer.Write(Ta.mapNeutroCoef.RowHeader(),sizeof(double)*cnThickItemCount);
			buffer.Write(Ta.mapNeutroCoef.ColHeader(),sizeof(double)*cnAngleItemCount);
			//写入中性层系数插值表
			buffer.Write(Ta.mapNeutroCoef.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
			//写入制弯线内圆弧插值表
			buffer.Write(Ta.mapBendR.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
		}
		//buffer.WriteInteger(CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		buffer.WriteString(Ta.m_sCompanyName.GetBuffer(100));	//设计单位
		Ta.weather.ToBuffer(buffer);		//气象条件
		buffer.WriteInteger(Ta.volt_grade);	//电压等级:330,500
		buffer.WriteByte(Ta.m_cLandscape);	//塔位地形:'A','B','C'
		buffer.WriteDouble(Ta.m_fTowerHeight);
		/*安全等级(决定结构重要性系数gama0)：
		0:一级特别重要的杆塔结构(gama0=1.1)；
		1:二级各级电压线路的各类杆塔(gama0=1.0)；
		2:三级临时使用的各类杆塔(gama0=0.9).*/
		buffer.WriteWord(Ta.m_siSafetyGrade);
		buffer.WriteDouble(Ta.m_fSafetyCoef);
		buffer.WriteDouble(Ta.m_fIceDensity);	//杆塔裹冰重度kg/m3
#endif
		//铁塔准距表区
		buffer.WriteWord(zhunju_N);	//保存所用准距数目
		for(WORD i=0;i<zhunju_N;i++)
		{
			buffer.WriteWord(zhunju_table[i].wide);	//肢宽
			buffer.WriteWord(zhunju_table[i].g);	//标准准距g
			buffer.WriteWord(zhunju_table[i].g1);	//准距g1
			buffer.WriteWord(zhunju_table[i].g2);	//准距g2
			buffer.WriteWord(zhunju_table[i].g3);	//准距g3
		}
		//螺栓间距表
		buffer.WriteInteger(lsspace_N);
		for(i=0;i<lsspace_N;i++)
		{
			buffer.WriteInteger(LSSPACE_TABLE[i].M_Diameter);
			buffer.WriteInteger(LSSPACE_TABLE[i].SingleRowSpace);
			buffer.WriteInteger(LSSPACE_TABLE[i].doubleRowSpace);
			buffer.WriteInteger(LSSPACE_TABLE[i].EndSpace);
			buffer.WriteInteger(LSSPACE_TABLE[i].PlateEdgeSpace);
			buffer.WriteInteger(LSSPACE_TABLE[i].RollingEdgeSpace);
			buffer.WriteInteger(LSSPACE_TABLE[i].CutAngleSpace);
		}
		if(PRODUCT_TYPE!=3)
		{	//写入螺栓零件库
			buffer.WriteInteger(CLsLibrary::GetLsSubPartFamilyNum());
			for(CLsSubPartFamily* pLsSubPart=CLsLibrary::GetFirstLsSubPartFamily();pLsSubPart;pLsSubPart=CLsLibrary::GetNextLsSubPartFamily())
			{
				buffer.WriteWord(pLsSubPart->m_widType);
				buffer.WriteString(pLsSubPart->name);
			}
		}
		//螺栓规格系列信息
		buffer.WriteInteger(CLsLibrary::GetFamilyNum());
		for(CLsFamily* pFamily=CLsLibrary::GetFirstFamily();pFamily;pFamily=CLsLibrary::GetNextFamily())
		{
			buffer.WriteString(pFamily->name);
			buffer.WriteInteger(pFamily->GetCount());
			buffer.WriteString(pFamily->descr);
			buffer.WriteInteger(pFamily->m_bAntiLoose);
			buffer.WriteInteger(pFamily->m_bDualCap?2:1);//pFamily->m_nLsCap);
			buffer.WriteInteger(pFamily->m_bFootNail);
			buffer.WriteInteger(pFamily->m_bBurglarproof);
			buffer.WriteInteger(pFamily->m_hMappingAntiTheftFamily);
			buffer.WriteInteger(pFamily->m_hMappingAntiLooseFamily);
			for(int j=0;j<pFamily->GetCount();j++)
			{
				LS_XING_HAO *pXingHao=pFamily->RecordAt(j);
				buffer.WriteDouble(pXingHao->grade);			//螺栓级别
				buffer.WriteInteger(pXingHao->d);			//螺栓直径
				buffer.WriteString(pXingHao->guige);			//螺栓规格
				buffer.WriteInteger(pXingHao->valid_len);	//有效长
				buffer.WriteInteger(pXingHao->no_thread_len);//无扣长
				buffer.WriteInteger(pXingHao->down_lim);		//螺栓通厚下限
				buffer.WriteInteger(pXingHao->up_lim);		//螺栓通厚上限
				buffer.WriteInteger(pXingHao->washer_thick);	//垫圈厚度pXingHao->pad_thick
				buffer.WriteDouble(pXingHao->weight);		//螺栓理论重量
			}
		}
		//螺栓规格选择条件
		buffer.WriteInteger(Ta.listSelBoltCond.GetNodeNum());
		for(BOLT_SELCOND *pSelBolt=Ta.listSelBoltCond.GetFirst();pSelBolt;pSelBolt=Ta.listSelBoltCond.GetNext())
		{
			buffer.WriteInteger(pSelBolt->d);
			buffer.WriteString(pSelBolt->grade);
			buffer.WriteString(pSelBolt->matStr);
			buffer.WriteInteger(pSelBolt->upWidthLim);
		}
		//材质库
		buffer.WriteInteger(CSteelMatLibrary::GetCount());
		for(i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			STEELMAT mat =CSteelMatLibrary::RecordAt(i);
			buffer.WriteString(mat.mark);
			buffer.WriteByte(mat.cBriefMark);
			buffer.WriteByte(mat.cPreferQualityLevel);
			buffer.WriteDouble(mat.E);
			buffer.WriteDouble(mat.price_coef);
			buffer.Write(mat.t_lim,sizeof(int)*3);
			buffer.Write(mat.intensity,sizeof(double)*16);
		}
#ifdef __PART_DESIGN_INC_
		buffer.WriteInteger(jgguige_N);
		for(i=0;i<jgguige_N;i++)
		{
			buffer.WriteDouble(jgguige_table[i].wing_wide);
			buffer.WriteByte(jgguige_table[i].cType);
			buffer.WriteDouble(jgguige_table[i].wing_thick);
			buffer.WriteDouble(jgguige_table[i].r);
			buffer.WriteDouble(jgguige_table[i].area);
			buffer.WriteDouble(jgguige_table[i].theroy_weight);
			buffer.WriteDouble(jgguige_table[i].Ix);
			buffer.WriteDouble(jgguige_table[i].Rx);
			buffer.WriteDouble(jgguige_table[i].Wx);
			buffer.WriteDouble(jgguige_table[i].Iy0);
			buffer.WriteDouble(jgguige_table[i].Ry0);
			buffer.WriteDouble(jgguige_table[i].Wy0);
		}
		//抗震验算参数
		buffer.WriteByte(Ta.EarthQuake.grade);			//地震烈度6,7,8,9
		buffer.WriteDouble(Ta.EarthQuake.alfa_max);		//水平地震影响系数最大值
		buffer.WriteByte(Ta.EarthQuake.cLandType);		//场地类别1,2,3,4
		buffer.WriteByte(Ta.EarthQuake.cDesignGroup);	//设计地震分组1,2,3
		buffer.WriteDouble(Ta.EarthQuake.Tg);			//特征周期,s
		buffer.WriteDouble(Ta.EarthQuake.zeta);			//阻尼系数ζ=0.02s
		buffer.WriteDouble(Ta.EarthQuake.gamaRE);		//承载力抗震调整系数,一般跨越塔取0.85，其余塔取0.8
		buffer.WriteInteger(Ta.EarthQuake.nFreqsRank);
#endif
#ifndef __TMA_FILE_
		buffer.WriteInteger(tubeguige_N);
		for(i=0;i<tubeguige_N;i++)
		{
			buffer.WriteDouble(tubeguige_table[i].D);
			buffer.WriteDouble(tubeguige_table[i].thick);
			buffer.WriteDouble(tubeguige_table[i].area);
			buffer.WriteDouble(tubeguige_table[i].theroy_weight);
			buffer.WriteDouble(tubeguige_table[i].I);
			buffer.WriteDouble(tubeguige_table[i].r);
			buffer.WriteDouble(tubeguige_table[i].W);
		}
#endif
		//LMA或LDS支持保存钢管端连接标准库 wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LMA_)||defined(__LDS_FILE_)||defined(__LDS_)
		WriteStdPartLibToBuffer(buffer);
#endif
		//基本模型区
		buffer.WriteInteger(Ta.Module.GetNodeNum());
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			buffer.WriteInteger(pModule->handle);
			pModule->ToBuffer(buffer);
		}
	}
	//哈希表管理信息部分
	OBJSET objset;
	CLDSPoint *pPoint;
	CLDSLine *pLine;
	CLDSPlane *pPlane;
	CLDSNode *pNode;
	CLDSPart *pPart;
	CBlockReference *pBlkRef;
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		if(pPoint->dwPermission&update_word)
			objset.append(pPoint);
	}
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		if(pLine->dwPermission&update_word)
			objset.append(pLine);
	}
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		if(pPlane->dwPermission&update_word)
			objset.append(pPlane);
	}
	for(pBlkRef=Ta.BlockRef.GetFirst();pBlkRef;pBlkRef=Ta.BlockRef.GetNext())
	{
		if(pBlkRef->dwPermission&update_word)
			objset.append(pBlkRef);
	}
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->dwPermission&update_word)
			objset.append(pNode);
	}
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		DWORD dwPermission=pPart->dwPermission;
		if(pPart->IsLinePart())
			dwPermission|=((CLDSLinePart*)pPart)->dwStartPermission|((CLDSLinePart*)pPart)->dwEndPermission;
		if(dwPermission&update_word)
			objset.append(pPart);
	}
	buffer.WriteInteger(objset.GetNodeNum());
	for(CLDSObject* pObj=objset.GetFirst();pObj;pObj=objset.GetNext())
	{
		buffer.WriteInteger(pObj->handle);
		buffer.WriteInteger(pObj->GetClassTypeId());
	}
	//视图存储
	buffer.LogPosition();
	buffer.WriteInteger(0);
	int nn=0;
	for(CDisplayView *pDispView=Ta.GetFirstDisplayView();pDispView;pDispView=Ta.GetNextDisplayView())
	{
		if(pDispView->dwPermission&update_word)
		{
			buffer.WriteInteger(pDispView->handle);
			pDispView->ToBuffer(buffer);
			nn++;
		}
	}
	buffer.RecallPosition();//召回标记点位置（即节点数存储位置）
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//返回到当前正确存储位置
#ifdef __COMMON_PART_INC_
	//预拱数据项
	BUFFERPOP stack(&buffer,Ta.ArcLift.GetNodeNum());
	buffer.WriteInteger(Ta.ArcLift.GetNodeNum());
	for(CLDSArcLift* pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
	{
		if(pLift->dwPermission&update_word)
		{
			buffer.WriteInteger(pLift->handle);
			pLift->ToBuffer(buffer);
			stack.Increment();
		}
	}
	stack.VerifyAndRestore();
#endif
	//部件块存储
	buffer.LogPosition();
	buffer.WriteInteger(0);
	nn=0;
	for(CBlockModel *pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		if(pBlock->dwPermission&update_word)
		{
			buffer.WriteInteger(pBlock->handle);
			pBlock->ToBuffer(buffer);
			nn++;
		}
	}
	buffer.RecallPosition();//召回标记点位置（即节点数存储位置）
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//返回到当前正确存储位置
	//基准点存储区
	buffer.LogPosition();
	buffer.WriteInteger(0);
	nn=0;
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		if(pPoint->dwPermission&update_word)
		{
			buffer.WriteInteger(pPoint->handle);
			pPoint->ToBuffer(buffer);
			nn++;
		}
	}
	buffer.RecallPosition();//召回标记点位置（即节点数存储位置）
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//返回到当前正确存储位置
	//基准线存储区
	buffer.LogPosition();
	buffer.WriteInteger(0);
	nn=0;
	for(pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		if(pLine->dwPermission&update_word)
		{
			buffer.WriteInteger(pLine->handle);
			pLine->ToBuffer(buffer);
			nn++;
		}
	}
	buffer.RecallPosition();//召回标记点位置（即节点数存储位置）
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//返回到当前正确存储位置
	//基准面存储区
	buffer.LogPosition();
	buffer.WriteInteger(0);
	nn=0;
	for(pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
	{
		if(pPlane->dwPermission&update_word)
		{
			buffer.WriteInteger(pPlane->handle);
			pPlane->ToBuffer(buffer);
			nn++;
		}
	}
	buffer.RecallPosition();//召回标记点位置（即基准面数存储位置）
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//返回到当前正确存储位置
	
	long old_pos=buffer.GetCursorPosition();
	buffer.WriteInteger(0);
	nn=0;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->dwPermission&update_word)
		{
			buffer.WriteInteger(pNode->handle);
			pNode->ToBuffer(buffer);
			nn++;
		}
	}
	long new_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(old_pos);
	buffer.WriteInteger(nn);
	buffer.SeekPosition(new_pos);
	//构件信息区
	nn=0;
	old_pos=buffer.GetCursorPosition();
	buffer.WriteInteger(0);
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		DWORD dwPermission=pPart->dwPermission;
		if(pPart->IsLinePart())
			dwPermission|=((CLDSLinePart*)pPart)->dwStartPermission|((CLDSLinePart*)pPart)->dwEndPermission;
		if((dwPermission&update_word)==0)
			continue;
		buffer.WriteInteger(pPart->handle);
		buffer.WriteInteger(pPart->GetClassTypeId());
		if(pPart->IsLinePart())
			((CLDSLinePart*)pPart)->ToPipeBuffer(buffer,update_word);
		else
			pPart->ToBuffer(buffer);
		nn++;
	}
	new_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(old_pos);
	buffer.WriteInteger(nn);
	buffer.SeekPosition(new_pos);

	MD5_CTX md5;
	BYTE verify[16];
	DWORD nDataSize=buffer.GetLength();
	md5.MD5Update((BYTE*)buffer.GetBufferPtr(),nDataSize);
	md5.MD5Final(verify);
	while(1)
	{
		//发送数据大小
		WriteFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwBytes,NULL);
		//发送数据报文MD5摘要
		WriteFile(theApp.m_hPipeServerListen,verify,16,&dwBytes,NULL);
		int iOrder=0;
		buffer.SeekToBegin();
		while(buffer.GetRemnantSize()>PACK_SIZE-1)
		{
			buffer.Read(memory_pool,PACK_SIZE-1);
			memory_pool[PACK_SIZE-1]=1;	//数据发送未结束
			WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL);
			int percent=min(100,100*iOrder*PACK_SIZE/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is sending date to collaborative center（Sent %d%%）！",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"正在向协同中心发送数据（已发送%d%%）！",percent);
#endif
			iOrder++;
		}
		buffer.Read(memory_pool,buffer.GetRemnantSize());
		memory_pool[PACK_SIZE-1]=0;		//数据发送已结束
		WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL);
		//接收协同中心发来的数据接收确认字（如果数据与报文MD5摘要不一致返回FALSE）
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
		if(retCode==1)
			break;
		if(retCode==-1)
		{
			PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Send failed!");
#else 
			AfxMessageBox("发送失败!");
#endif
			return FALSE;
		}
	}
	PutMessage(theApp.GetMainWnd(),"");
	return TRUE;
}

void CLDSDoc::UpdatePermList()
{
	//需要更新权限信息
	DWORD dwBytes;
	LDS_MSGHEAD msg_head;
	BYTE memory_pool[PACK_SIZE];
	msg_head.command_id=ASK_PERM_MODIFY_OUT;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	CBuffer buffer;
	//读取消息内容（提取数据的用户范围）
	while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL))
	{
		buffer.Write(memory_pool,PACK_SIZE-1);
		if(memory_pool[PACK_SIZE-1]==0)
			break;	//数据接收结束
	}
	buffer.SeekToBegin();
	int i,n;
	DWORD dwPermission,dwStartPermission,dwEndPermission;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long h;
		buffer.ReadInteger(&h);
		CLDSObject *pObj=Ta.FromHandle(h);
		if(pObj)
		{
			buffer.ReadDword(&dwPermission);
			buffer.ReadDword(&dwStartPermission);
			buffer.ReadDword(&dwEndPermission);

			pObj->dwPermission=dwPermission;
			if(pObj->IsDbObject()&&((CLDSDbObject*)pObj)->IsPart()&&((CLDSPart*)pObj)->IsLinePart())
			{
				CLDSLinePart *pLinePart=(CLDSLinePart*)pObj;
				pLinePart->dwStartPermission=dwStartPermission;
				pLinePart->dwEndPermission=dwEndPermission;
			}
		}
	}
}

BOOL CLDSDoc::SaveModified() 
{
	if(!theApp.m_bCooperativeWork)
		return CDocument::SaveModified();
	else if(IsModified())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		int ret=AfxMessageBox("Whether the modified data stored in the server？",MB_YESNOCANCEL|MB_ICONQUESTION);
#else 
		int ret=AfxMessageBox("是否将修改后的数据存储到服务器？",MB_YESNOCANCEL|MB_ICONQUESTION);
#endif
		if(ret!=IDCANCEL)
		{
			if(ret==IDYES)
				OnFileSave();
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return TRUE;
}
#include "LicFuncDef.h"
void CLDSDoc::OnExportSwapInfoFile()
{
	CXhChar200 fn("%s",GetPathName());
	if(fn.GetLength()==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Not open any files to export any data exchange information！");
#else 
		AfxMessageBox("未打开任何文件，无法导出任何数据交换信息！");
#endif
		return;
	}
	char* separator=SearchChar(fn,'.',true);
	if(separator!=NULL)
		*separator=0;
	//fn.Append(".bom");
	CXhChar500 filter;
	if(!theApp.m_bAnalysisWorkEnvOnly)
#ifdef AFX_TARG_ENU_ENGLISH
		filter.Copy("BOM Information(*.bom)|*.bom|Excel BOM Information(*.xls)|*.xls|");
#else 
		filter.Copy("统材信息(*.bom)|*.bom|Excel统材信息(*.xls)|*.xls|");
#endif
#ifdef __PART_DESIGN_INC_
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("原始设计信息文件(*.tdf)|*.tdf|");
	filter.Append("司令图模型(*.cdf)|*.cdf|");
#endif
#ifdef __RAPID_ROUGH_3DMODEL_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		filter.Append("铁塔模型信息展示文件(*.tid)|*.tid|");//|实体模型文本文件(*.txf)|*.txf|");
		filter.Append("3DS几何模型文件(*.3ds)|*.3ds|");
		filter.Append("STL几何模型ASCII文件(*.stl)|*.stl|");
		filter.Append("STL几何模型二进制文件(*.stl)|*.stl|");
#ifndef __STATION_	//暂时变电构架SDA软件不需要移交功能 wjh-2019.9.25
		filter.Append("国网移交几何模型文件(*.mod)|*.mod|");
		filter.Append("国网移交GIM文件(*.gim)|*.gim|");
#endif
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	filter.Append("all files(*.*)|*.*||");
#else 
	filter.Append("所有文件(*.*)|*.*||");
#endif
	CXhChar50 xarrFileFormats[16], xarrFileFormatItems[32];
	UINT niFilterCount=DELIMITER_STR::ParseWordsFromStr(filter, "|", xarrFileFormatItems, 32);
	for (UINT i = 0;i < niFilterCount;i++)
	{
		if (i % 2 == 0)
			xarrFileFormats[i / 2] = xarrFileFormatItems[i];
	}

	CFileDialog dlg(FALSE,"tid",fn,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if(dlg.DoModal()!=IDOK)
		return;
	CString ext=dlg.GetFileExt();
	if(ext.CompareNoCase("bom")==0)
	{
		ExportTxtSumMatData(dlg.GetPathName());
		return;
	}
	else if(ext.CompareNoCase("xls")==0)
	{
		ExportExcelSumMatData();//dlg.GetPathName());
		return;
	}
#ifdef __RAPID_ROUGH_3DMODEL_
	else if(ext.CompareNoCase("tid")==0)
	{
		CString filename=dlg.GetPathName();
		if(GetLicVersion()<1000003||!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
			AfxMessageBox("缺少输出铁塔模型信息展示文件的使用授权");
		else
			Ta.ExportTowerSolidDataExchangeFile(filename.GetBuffer());
	}
#ifndef __STATION_	//暂时变电构架SDA软件不需要移交功能 wjh-2019.9.25
	else if(ext.CompareNoCase("mod")==0)
	{
		CModDataFile modfile;
		LOCALE_PROPITEM* pLocaleItem = gxPortalLocalizer.GetLocaleItem("ExportTurnLeftGimFile");
		if (pLocaleItem&&pLocaleItem->bVal&&
			AfxMessageBox("杆塔模型是否进行左转？", MB_YESNO) == IDYES)
			modfile.m_bTurnLeft = TRUE;
		modfile.ExportModFile(dlg.GetPathName());
	}
	else if(ext.CompareNoCase("gim")==0)
	{
		ExportGimFile(dlg.GetPathName());
		return;
	}
#endif
	else if (ext.CompareNoCase("3ds") == 0)
	{
		Export3dsOrStlFile(dlg.GetPathName(), 0);
		return;
	}
	else if (ext.CompareNoCase("stl") == 0)
	{
		CXhChar50 szCurrFormat=xarrFileFormats[dlg.m_ofn.nFilterIndex - 1];
		if (strstr(szCurrFormat,"二进制")!=NULL)	//"二进制"
			Export3dsOrStlFile(dlg.GetPathName(), 1, true);
		else
			Export3dsOrStlFile(dlg.GetPathName(), 1, false);
		return;
	}
#endif
#ifdef __PART_DESIGN_INC_
	else if(ext.CompareNoCase("tdf")==0)
	{
		ExportTowerDesignTxtFile(dlg.GetPathName());
		return;
	}
	else if(ext.CompareNoCase("cdf")==0)
	{
		ExportCommandDrawingFile(dlg.GetPathName());
		return;
	}
#endif
}
struct RODRECORD{
	int startPointI,endPointI;
	char cMaterial;
	double m_fWeigthRaiseCoef;
	SIZESPEC size;
	double length,area,weight,efficWeight;
	RODRECORD()
	{
		memset(this,0,sizeof(RODRECORD));
		m_fWeigthRaiseCoef=1.0;
	}
};
static int LocalCompareRodRecord(const RODRECORD& rodrec1,const RODRECORD& rodrec2)
{
	if(rodrec1.startPointI<rodrec2.startPointI)
		return -1;
	else if(rodrec1.startPointI>rodrec2.startPointI)
		return  1;
	else if(rodrec1.endPointI<rodrec2.endPointI)
		return -1;
	else if(rodrec1.endPointI>rodrec2.endPointI)
		return  1;
	else
		return 0;
}
void CLDSDoc::OnExportSpecPartExcelFile() 
{
	long *id_arr,i,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
#ifdef __PART_DESIGN_INC_
	if(theApp.m_bAnalysisWorkEnv)
	{
		ARRAY_LIST<RODRECORD> rodarr(0,n);
		for(i=0;i<n;i++)
		{
			CLDSLinePart *pRod=Ta.FromRodHandle(id_arr[i]);
			if(pRod==NULL||pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			RODRECORD* pRodRec=rodarr.append();
			pRodRec->startPointI=min(pRod->pStart->point_i,pRod->pEnd->point_i);
			pRodRec->endPointI  =max(pRod->pStart->point_i,pRod->pEnd->point_i);
			pRodRec->length=DISTANCE(pRod->pStart->Position(false),pRod->pEnd->Position(false));
			pRodRec->size=pRod->GetSizeSpec();
			pRodRec->cMaterial=pRod->cMaterial;
			pRodRec->area=pRodRec->length*pRodRec->size.wide;
			pRodRec->weight=pRod->GetWeight(FALSE);
			pRodRec->m_fWeigthRaiseCoef=pRod->m_fWeightRaiseCoef;
			pRodRec->efficWeight=pRodRec->weight*pRod->m_fWeightRaiseCoef;
		}
		CQuickSort<RODRECORD>::QuickSort(rodarr.m_pData,rodarr.GetSize(),LocalCompareRodRecord);
		CXhChar500 szTmpStatFile = theApp.szExecPath;
		szTmpStatFile.Append("ldsstat.txt");
		CLogFile logfile(szTmpStatFile);
		CLogErrorLife life(&logfile);
		logfile.Log("                      杆件明细清单\n");
		logfile.Log("杆件标识     规格   长度(mm) 挡风面积(m2) 重量(kg) 增重系数 加权重量(kg)");
		logfile.Log("------------------------------------------------------------------------");
		RODRECORD sumstat;
		for(i=0;i<rodarr.GetSize();i++)
		{
			CXhChar16 spec("%3.0fx%2.0f%C",rodarr[i].size.wide,rodarr[i].size.thick,rodarr[i].cMaterial);
			if(rodarr[i].size.idClassType==CLS_LINEANGLE||rodarr[i].size.idClassType==CLS_GROUPLINEANGLE)
			{
				spec.InsertBefore(" L",0);
				if(rodarr[i].size.cSubClassType!='L')
					spec[0]=rodarr[i].size.cSubClassType;
			}
			else if(rodarr[i].size.idClassType==CLS_LINETUBE)
				spec.InsertBefore("Φ",0);//φ
			else if(rodarr[i].size.idClassType==CLS_LINESLOT)
				spec.InsertBefore(" [",0);
			else
				spec.InsertBefore(" U",0);
			rodarr[i].area=0.001*ftoi(0.001*rodarr[i].area);
			rodarr[i].weight=0.01*ftoi(100*rodarr[i].weight);
			logfile.Log("%4d-%4d %s %5.0f      %6.3f      %6.2f    %.2f     %6.2f",rodarr[i].startPointI,rodarr[i].endPointI,(char*)spec,
				rodarr[i].length,rodarr[i].area,rodarr[i].weight,rodarr[i].m_fWeigthRaiseCoef,rodarr[i].efficWeight);
			sumstat.area+=rodarr[i].area;
			sumstat.weight+=0.01*ftoi(100*rodarr[i].weight);
			sumstat.efficWeight+=0.01*ftoi(100*rodarr[i].efficWeight);
		}
		logfile.Log("\n");
		logfile.Log("合计  挡风面积:  %8.3f m2",sumstat.area);
		logfile.Log("        重  量: %8.2f kg     效应重量: %8.2f kg",sumstat.weight,sumstat.efficWeight);
		return;
	}
#endif
#ifdef __COMMON_PART_INC_
	CTowerType mat_tbl;
	mat_tbl.m_bStatLsBySegI=true;	//按段统计构件
	for(i=0;i<n;i++)
	{
		CLDSPart *pPart=Ta.Parts.FromHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		else if(pPart->m_bVirtualPart)	//隐含角钢不统计
			continue;
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//材料汇总时不统计组合角钢(单线)重量 wht 11-03-11
		Ta.Parts.push_stack();
		mat_tbl.SummarizeOnePart(pPart,&Ta);
		Ta.Parts.pop_stack();
	}
	static CExportExcelOptionDlg option_dlg;
	if(option_dlg.DoModal()!=IDOK)
		return;
	int nSheet=0;
	if(option_dlg.m_bBoltSumMatTbl)
		nSheet++;
	if(option_dlg.m_bLineAngleSumMatTbl)
		nSheet++;
	if(option_dlg.m_bLineTubeSumMatTbl)
		nSheet++;
	if(option_dlg.m_bLineSlotSumMatTbl)
		nSheet++;
	if(option_dlg.m_bLineFlatSumMatTbl)
		nSheet++;
	if(option_dlg.m_bPlateSumMatTbl)
		nSheet++;
	BeginWaitCursor();
	//输出到EXCEL文件

	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;
	
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}
	
	excel_app.SetSheetsInNewWorkbook(nSheet);	//新生成工作簿中默认工作表的数量√
	
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	
	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int iSheet=1;

	LPDISPATCH pWorksheet;
	if(option_dlg.m_bBoltSumMatTbl)
	{	//生成螺栓汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddLsSumMatTbl(mat_tbl,pWorksheet,2);//,TRUE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineAngleSumMatTbl)
	{	//生成角钢汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_ANGLE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineTubeSumMatTbl)
	{	//生成钢管汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_TUBE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineFlatSumMatTbl)
	{	//生成扁铁汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_FLAT);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineSlotSumMatTbl)
	{	//生成槽钢汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_SLOT);
		pWorksheet->Release();
	}
	if(option_dlg.m_bPlateSumMatTbl)
	{	//生成钢板汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_PLATE);
		pWorksheet->Release();
	}
	EndWaitCursor();
#endif
}
#if defined(__TSA_)||defined(__TSA_FILE_)
void CLDSDoc::OnExportExcelFile(){;}
void CLDSDoc::OnExportJgExcelFile(){;}
void CLDSDoc::OnExportTxtPartData(){;}
void CLDSDoc::OnExportTxtPartData2(){;}
void CLDSDoc::OnExportTxtPartData3(){;}
void CLDSDoc::OnExportTxtPartData4(){;}
void CLDSDoc::OnExportTxtLsData(){;}
void CLDSDoc::OnExportTubeSpecialProcessTable(){;}
void CLDSDoc::OnExportExcelFile2(){;}
void CLDSDoc::OnExportExcelFile3(){;}
void CLDSDoc::ExportExcelSumMatData(){;}
void CLDSDoc::ExportTxtSumMatData(LPCTSTR lpszPathName){;}
#else
void CLDSDoc::OnExportExcelFile() 
{
// 	CPartTablePreviewDlg preview_dlg;
// 	if(preview_dlg.DoModal()==IDOK)
// 		return;
	CLogErrorLife life;
	CTowerType mat_tbl;
	static CExcelExportOptionDlg option_dlg;
	char seg_str[500]="";
	Ta.GetSegStr(seg_str);
	option_dlg.m_sSegStr=seg_str;	//段号字符串
	if(option_dlg.DoModal()!=IDOK)
		return;
	int nSheet=0;
	if(option_dlg.m_bDataInterface)
		nSheet+=2;
	if(option_dlg.m_bOrgMatTbl)
		nSheet++;
	if(option_dlg.m_bPartTbl)
		nSheet++;
	if((option_dlg.m_bPartTbl||option_dlg.m_bOrgMatTbl)&&(option_dlg.m_iOrgmatTblStyle==5||option_dlg.m_iOrgmatTblStyle==8))
		nSheet++;	//输出开口组焊代号表
	if(option_dlg.m_bLsSumMatTbl)
		nSheet++;
	if(option_dlg.m_bWeldPartTbl)
		nSheet++;
	if(option_dlg.m_bPoleExcelFile)
		nSheet++;
	const int BOM_SHEET_PART		= 0x01;	//构件明细表
	const int BOM_SHEET_BOLT		= 0x02;	//螺栓明细表
	const int BOM_SHEET_WELDPART	= 0x04;	//组焊件明细表
	DWORD dwSupportType=CLDSApp::GetSupportBOMType!=NULL?CLDSApp::GetSupportBOMType():BOM_SHEET_PART;
	
	if(option_dlg.m_bCustomTbl)
	{
		CWaitCursor waitCursor;
		CTowerBOM towerBOM;
		CModelBOM bomModel;
		towerBOM.RunSummarize(&Ta,true);
		towerBOM.CloneToModelBOM(&bomModel);
		bomModel.m_sSegStr.Copy(option_dlg.m_sSegStr);
		CBuffer buffer;
		bomModel.ToBuffer(buffer,CLDSApp::GetSupportDataBufferVersion());
		
		CXhChar500 filePath("D:/EXCEL模板.xls");
		if (CLDSApp::RecogniseReport!=NULL&&PathFileExists(filePath))
			CLDSApp::RecogniseReport(buffer.GetBufferPtr(), buffer.GetLength(), GetPathName(), 0);
		else if (CLDSApp::CreateExcelBomFile != NULL && CLDSApp::GetSupportDataBufferVersion != NULL)
			CLDSApp::CreateExcelBomFile(buffer.GetBufferPtr(),buffer.GetLength(),GetPathName(),0);
		//判断是否有选中其他输出项，没有只输出定制表
		if (nSheet== 0)
			return;
	}
	BeginWaitCursor();
	//进行材料统计
	bool bIncLsSegI=false;
	if(option_dlg.m_iLsTblStyle==2)	//按段统计螺栓
		bIncLsSegI=true;
	mat_tbl.RunSummarize(&Ta,bIncLsSegI);
	int i=0;
	mat_tbl.SortPart(0);
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;

	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}

	excel_app.SetSheetsInNewWorkbook(nSheet);	//新生成工作簿中默认工作表的数量√

	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);

	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int iSheet=1;
	//填写第一张工件数据表单
	LPDISPATCH pWorksheet;
	if(option_dlg.m_bDataInterface)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
		AddWorkPartsSheet(mat_tbl,pWorksheet);
		pWorksheet->Release();
		pWorksheet = excel_sheets.GetItem(COleVariant((short) 2));
		AddManuModuleSheet(pWorksheet);
		pWorksheet->Release();
		iSheet+=2;
	}
	// 填写生成构件明细表表单
	if(option_dlg.m_bOrgMatTbl||option_dlg.m_bPartTbl)
	{
		for(int ii=0;ii<2;ii++)
		{
			if(!option_dlg.m_bOrgMatTbl&&!option_dlg.m_bPartTbl)
				break;		//不需要输出原始材料表或部件明细表
			else if(ii==0&&!option_dlg.m_bOrgMatTbl)
				continue;	//不需要输出原始材料表
			else if(ii==1&&!option_dlg.m_bPartTbl)
				break;	//不需要输出部件明细表
			pWorksheet = excel_sheets.GetItem(COleVariant((short) iSheet));
			if(option_dlg.m_iOrgmatTblStyle==1)
				AddZheJiangShengDaPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==2)
				AddShanXiJinNengPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==3)
				AddChengDuPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==4)
				AddChangShuPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==5)
				AddGuangZhouZengLiPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==6)
				AddHuZhouFeiJianPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==7)
				AddZheJiangYongDaPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else if(option_dlg.m_iOrgmatTblStyle==8)	//潍坊长安 wht 10-10-11
				AddWeiFangChangAnPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else //if(option_dlg.m_iOrgmatTblStyle==0)	//一般样式
				AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,ii==0, option_dlg.m_sSegStr, TA_OTHER, option_dlg.m_iPartsSortType);
			iSheet++;
			pWorksheet->Release();
		}
		if(option_dlg.m_iOrgmatTblStyle==5)
		{
			pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
			AddSlotAndWeldCodeTblSheet(pWorksheet,0);
			iSheet++;
		}
		else if(option_dlg.m_iOrgmatTblStyle==8)
		{	//潍坊长安 wht 10-10-11
			pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
			AddSlotAndWeldCodeTblSheet(pWorksheet,1);
			iSheet++;
		}
	}
//生成螺栓汇总表
	if(option_dlg.m_bLsSumMatTbl)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iLsTblStyle==2)	//按段统计螺栓
			AddBoltSumTblSheet(mat_tbl,pWorksheet);
		else
			AddLsSumMatTbl(mat_tbl,pWorksheet,option_dlg.m_iLsTblStyle);
		pWorksheet->Release();
	}
//生成焊件组合明细表
	if(option_dlg.m_bWeldPartTbl)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iWeldPartTblStyle==1)		//广州增立
			AddGuangZhouZengLiPartGroupSumMatTbl(mat_tbl,pWorksheet);
		else if(option_dlg.m_iWeldPartTblStyle==2)	//潍坊长安组焊明细表 wht 10-09-30
			AddWeiFangChangAnPartGroupSumMatTbl(mat_tbl,pWorksheet);
		else 
			AddPartGroupSumMatTbl(mat_tbl,pWorksheet);
		pWorksheet->Release();
	}
	/*VARIANT vt;
	vt.pdispVal = (LPDISPATCH)pWorksheet;
	vt.vt = VT_DISPATCH;
	excel_sheets.Add(COleVariant(DISP_E_PARAMNOTFOUND,VT_ERROR),COleVariant(DISP_E_PARAMNOTFOUND,VT_ERROR),COleVariant((short)1),COleVariant((short)1));*/
//生成样杆表
	if(option_dlg.m_bPoleExcelFile)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iPoleSortType==0)//按编号统计
			AddPoleTblSheet(mat_tbl,pWorksheet);
		else if(option_dlg.m_iPoleSortType==1)//按规格排序不区分段号区分材质
		{
			mat_tbl.SortPart(2);
			AddPoleTblSheet(mat_tbl,pWorksheet);
		}
		else if(option_dlg.m_iPoleSortType==2)//按规格排序不区分段号不区分材质
		{
			mat_tbl.SortPart(2,FALSE);
			AddPoleTblSheet(mat_tbl,pWorksheet);
		}
		pWorksheet->Release();
	}
	EndWaitCursor();
}

void CLDSDoc::ExportExcelSumMatData()
{
	CTowerType mat_tbl;
	int i=0;
	//进行材料统计
	BeginWaitCursor();
	mat_tbl.RunSummarize(&Ta,TRUE);
	mat_tbl.SortPart(0);
	mat_tbl.SortLs();
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry

	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}
	excel_app.SetSheetsInNewWorkbook(3);
	LPDISPATCH pWorkbooks;
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);

	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);

	LPDISPATCH pWorksheet;
	pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	AddManuPartSheet(mat_tbl,pWorksheet);
	pWorksheet->Release();
	pWorksheet = excel_sheets.GetItem(COleVariant((short) 2));
	AddBoltSheet(mat_tbl,pWorksheet);
	pWorksheet->Release();
	pWorksheet = excel_sheets.GetItem(COleVariant((short) 3));
	AddBodySheet(pWorksheet);
	pWorksheet->Release();

	EndWaitCursor();
	pWnd->CloseWindow();
}
void CLDSDoc::OnExportJgExcelFile() 
{
	CTowerType mat_tbl;
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	int i=0;
	mat_tbl.SortPart(0);
	mat_tbl.SortPart(1,TRUE,FALSE);
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;

	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}
	excel_app.SetSheetsInNewWorkbook(1);	//新生成工作簿中默认工作表的数量√
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);

	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int iSheet=1;

	LPDISPATCH pWorksheet;
	//生成样杆表
	pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	AddPoleTblSheet(mat_tbl,pWorksheet);
	pWorksheet->Release();
	EndWaitCursor();
}
//长春铁塔厂用
void CLDSDoc::OnExportTxtPartData() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"clb","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.clb)|*.clb||");
#else 
		"文本材料表数据文件(*.clb)|*.clb||");
#endif
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file！");
#else 
			AfxMessageBox("打不开指定文件！");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"Segment Number,Part Lable,Specification,Material Texture,Length/Area,Amount,Weld,Bend,Leg Cut,Cut Back,Cut Root,M16,M20,M24,Other Bolts,Open Merge Angle\n");
#else 
			fprintf(fp,"段号,构件编号,规格,材质,长度/面积,数量,焊接,火曲,切角,铲背,清根,M16,M20,M24,其它螺栓,开合角\n");
#endif
			for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
			{
				char mat[5];
				QuerySteelMatMark(pPartRec->cMaterial,mat);
				double part_para;
				if(pPartRec->part_type==STEELTYPE_ANGLE)
					part_para=pPartRec->length;
				else if(pPartRec->part_type==STEELTYPE_PLATE)
				{
					part_para=pPartRec->wide*pPartRec->length;
					if(CLDSPart::CustomerSerial==11)	//长春聚德龙
						sprintf(pPartRec->sSpec,"-%.fx%.fx%.f",pPartRec->thick,pPartRec->wide,pPartRec->length);
				}
				char kaihe[50]="";
				if(pPartRec->part_type==STEELTYPE_ANGLE)
				{	//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-23
					double fKaiHeAngle=pPartRec->wing_angle-90;
					fKaiHeAngle*=(fKaiHeAngle<0)?-1:1;
					if(CLDSPart::CustomerSerial==11)	//长春聚德龙
						fKaiHeAngle=pPartRec->wing_angle;
					if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f°",fKaiHeAngle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"开角%.1f°",fKaiHeAngle);
#endif
					else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f°",fKaiHeAngle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"合角%.1f°",fKaiHeAngle);
#endif
				}
				fprintf(fp,"%s, %s, %s, %s, %.1f, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d, %3d,%s\n",(char*)pPartRec->iSeg.ToString(),pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator),
					(char*)pPartRec->sSpec,mat,part_para,pPartRec->GetPartNum(),pPartRec->bWeldPart,pPartRec->bZhiWan,
					pPartRec->bCutAngle ,pPartRec->bCutBer,pPartRec->bCutRoot,pPartRec->nM16Ls,pPartRec->nM20Ls,pPartRec->nM24Ls,pPartRec->nMXLs,kaihe);
			}
			fclose(fp);
		}
	}
	EndWaitCursor();
}
//福建坚石用
void CLDSDoc::OnExportTxtPartData2() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"dat","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.dat)|*.dat||");
#else 
		"文本材料表数据文件(*.dat)|*.dat||");
#endif
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file！");
#else 
			AfxMessageBox("打不开指定文件！");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"Block Number,Material Name,Design Specification,Borrow Specification,Material Texture,Length,Amount,Screw One Heavy,Weld One Heavy,Single Hole,Bend,Cold Bend,Leg Cut,Cut Back,Cut Root,Uncoated,Notes\n");
#else 
			fprintf(fp,"部件编号,材料名称,设计规格,借用规格,材质,长度,数量,螺丝件重,焊接件重,单件孔数,火曲,冷曲,切角,铲背,清根,非涂,备注\n");
#endif
			for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
			{
				char mat[5];
				QuerySteelMatMark(pPartRec->cMaterial,mat);
				double part_para;
				char parttype[5]="";
				char sSpec[30]="";
				part_para=pPartRec->length;
				BOOL bHuoQu=FALSE,bLengQu=FALSE;
				if(pPartRec->part_type==STEELTYPE_ANGLE)
				{
					strcpy(parttype,"L");
					CLDSLineAngle::GetAngleSimpleSpec(sSpec,pPartRec->wide,pPartRec->y_wing_wide,pPartRec->thick);
					bHuoQu=pPartRec->bZhiWan;
				}
				else if(pPartRec->part_type==STEELTYPE_PLATE)
				{
					strcpy(parttype,"-");
					_snprintf(pPartRec->sSpec,MAX_TEMP_CHAR_50,"%.0fx%.0f",pPartRec->thick,pPartRec->wide);
					bLengQu=pPartRec->bZhiWan;
					strcpy(sSpec,pPartRec->sSpec);
				}
				char kaihe[50]="";
				if(pPartRec->part_type==STEELTYPE_ANGLE)
				{	//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-23
					if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f°",pPartRec->wing_angle-90);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"开角%.1f°",pPartRec->wing_angle-90);
#endif
					else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f°",90-pPartRec->wing_angle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"合角%.1f°",90-pPartRec->wing_angle);
#endif
				}
				double weldweigth=0,lspartweight=0;
				if(pPartRec->bWeldPart)
					weldweigth=pPartRec->fPieceMinWeight;
				else
					lspartweight=pPartRec->fPieceMinWeight;
				char cHuoQu=' ',cLengQu=' ',cCutAngle=' ',cCutBer=' ',cCutRoot=' ';
				if(bHuoQu)
					cHuoQu='1';
				if(bLengQu)
					cLengQu='1';
				if(pPartRec->bCutAngle)
					cCutAngle='1';
				if(pPartRec->bCutBer)
					cCutBer='1';
				if(pPartRec->bCutRoot)
					cCutRoot='1';
				fprintf(fp,"%s,%s,%s, ,%s,%.0f,%d,%.3f,%.3f,%d,%C,%C,%C,%C,%C,%C,%s,\n", //福建坚石要求在最后添加一个逗号 wht 11-06-01
				pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator),parttype,sSpec,mat,part_para,pPartRec->GetPartNum(),lspartweight,weldweigth,
				pPartRec->nM16Ls+pPartRec->nM20Ls+pPartRec->nM24Ls+pPartRec->nMXLs,
					cHuoQu,cLengQu,cCutAngle,cCutBer,cCutRoot,' ',pPartRec->sNotes);
			}
			fclose(fp);
		}
	}
	EndWaitCursor();
}

void CLDSDoc::OnExportTxtPartData3() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"txt","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.txt)|*.txt||");
#else 
		"文本材料表数据文件(*.txt)|*.txt||");
#endif
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		CString ext = dlg.GetFileExt();
		CString filename=dlg.GetPathName().Left(dlg.GetPathName().GetLength()-ext.GetLength()-1);
		ATOM_LIST<SEGI> segArr;
		for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
		{
			bool bFound=false;
			for(SEGI *pSegI=segArr.GetFirst();pSegI;pSegI=segArr.GetNext())
			{
				if((*pSegI).iSeg==pPartRec->iSeg.iSeg)
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
				segArr.append(pPartRec->iSeg);
		}
		for(SEGI *pSegI=segArr.GetFirst();pSegI;pSegI=segArr.GetNext())
		{
			char fn[200];
			_snprintf(fn,199,"%s-%s.%s",filename,(char*)pSegI->ToString(),ext);
			FILE *fp = fopen(fn,"wt");
			if(fp==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Can't open the specified file！");
#else 
				AfxMessageBox("打不开指定文件！");
#endif
			}
			else
			{
				//fprintf(fp,"部件编号,材料名称,设计规格,借用规格,材质,长度,数量,螺丝件重,焊接件重,单件孔数,火曲,冷曲,切角,铲背,清根,非涂,备注\n");
				for(pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
				{
					if(pPartRec->iSeg.iSeg!=(*pSegI).iSeg)
						continue;
					char mat[5],spec[MAX_TEMP_CHAR_50+1]="",spec2[MAX_TEMP_CHAR_50+1]="",spec3[MAX_TEMP_CHAR_50+1]="",spec4[MAX_TEMP_CHAR_50+1]="";
					QuerySteelMatMark(pPartRec->cMaterial,mat);
					double part_para;
					char parttype[5]="";
					part_para=pPartRec->length;
					BOOL bHuoQu=FALSE,bLengQu=FALSE;
					if(pPartRec->part_type==STEELTYPE_ANGLE)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(parttype,"Angle");
#else 
						strcpy(parttype,"角钢");
#endif
						_snprintf(spec,MAX_TEMP_CHAR_50,"%.0f	%.0f",pPartRec->wide,pPartRec->thick);
						_snprintf(spec2,MAX_TEMP_CHAR_50,"0",pPartRec->length);
						_snprintf(spec3,MAX_TEMP_CHAR_50,"%s",(char*)pPartRec->sSpec);
						_snprintf(spec4,MAX_TEMP_CHAR_50,"%.0f",pPartRec->length);
						bHuoQu=pPartRec->bZhiWan;
					}
					else if(pPartRec->part_type==STEELTYPE_PLATE)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						strcpy(parttype,"Plate");
#else 
						strcpy(parttype,"钢板");
#endif
						_snprintf(spec,MAX_TEMP_CHAR_50,"%.0f",pPartRec->thick);
						_snprintf(spec2,MAX_TEMP_CHAR_50,"%.0f	%.0f",pPartRec->wide,pPartRec->length);
						_snprintf(spec3,MAX_TEMP_CHAR_50,"-%.0f",pPartRec->thick);
						_snprintf(spec4,MAX_TEMP_CHAR_50,"%.0fx%.0f",pPartRec->wide,pPartRec->length);
						bHuoQu=pPartRec->bZhiWan;
					}
					char kaihe[MAX_TEMP_CHAR_50+1]="";
					if(pPartRec->part_type==STEELTYPE_ANGLE)
					{	//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-23
						if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f°",pPartRec->wing_angle-90);
#else 
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"开角%.1f°",pPartRec->wing_angle-90);
#endif
						else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f°",90-pPartRec->wing_angle);
#else 
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"合角%.1f°",90-pPartRec->wing_angle);
#endif
					}
					char cHuoQu=' ',cCutAngle=' ',cCutBer=' ',cCutRoot=' ',cWeldPart=' ',cKaiJiao=' ',cHeJiao=' ';
					if(bHuoQu)
						cHuoQu='V';
					if(bLengQu)
						cHuoQu='V';
					if(pPartRec->bCutAngle)
						cCutAngle='V';
					if(pPartRec->bCutBer)
						cCutBer='V';
					if(pPartRec->bCutRoot)
						cCutRoot='V';
					if(pPartRec->bWeldPart)
						cWeldPart = 'V';
					if(pPartRec->bKaiJiao)
						cKaiJiao='V';
					if(pPartRec->bHeJiao)
						cHeJiao='V';
#ifdef AFX_TARG_ENU_ENGLISH
					fprintf(fp,"%s	%s	Angle Tower	%s	%s	%s	%s	%s	%.0f	%d	%.2f	0	%d	0	%d	%d		%C	%C	%C	%C	%C	%s	0	0	%s		%s		%C	%C\n",
#else 
					fprintf(fp,"%s	%s	角钢塔	%s	%s	%s	%s	%s	%.0f	%d	%.2f	0	%d	0	%d	%d		%C	%C	%C	%C	%C	%s	0	0	%s		%s		%C	%C\n",
#endif
						Ta.m_sTaAlias,(char*)pPartRec->iSeg.ToString(),
						pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator),parttype, mat,spec,spec2,part_para,pPartRec->GetPartNum(),pPartRec->fPieceMinWeight,
						pPartRec->nM16Ls,pPartRec->nM20Ls,pPartRec->nM24Ls,//pPartRec->nMXLs,
						cCutAngle,cHuoQu,cCutBer,cCutRoot,cWeldPart,pPartRec->sNotes,spec3,spec4,cKaiJiao,cHeJiao);
				}
				fclose(fp);
			}
		}
	}
	EndWaitCursor();
}

//浙江绍兴用 wht 11-07-12
void CLDSDoc::OnExportTxtPartData4()
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"txt","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.txt)|*.txt||");
#else 
		"文本材料表数据文件(*.txt)|*.txt||");
#endif
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file！");
#else 
			AfxMessageBox("打不开指定文件！");
#endif
		}
		else
		{
			//fprintf(fp,"零件编号,材料(L/-),规格, ,材质(A3/16Mn),长度,单基数量,重量, 0.00,孔数,火曲,火曲,切角,铲背, , ,备注,塔型\n");
			for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
			{
				char sPartNo[16]=" ";	//编号(编号为空时输出一个空格)
				char sValue[16]=" ";
				sprintf(sValue,"%s",pPartRec->GetPartNo());
				if(strlen(sValue)>0&&pPartRec->iSeg.iSeg<10			//编号不为空且段号小于10
					&&sValue[0]>'0'&&sValue[0]<='9')	//编号第一个字符为1~9
				{
					if(g_sysPara.m_nMatCharPosType==1)
						sprintf(sPartNo,"%c0%s",pPartRec->cMaterial,sValue);				//编号前加0,便于从编号中得到段号信息
					else if(g_sysPara.m_nMatCharPosType==2)
						sprintf(sPartNo,"0%s%c",sValue,pPartRec->cMaterial);
					else
						sprintf(sPartNo,"0%s",sValue);
				}
				else 
					strcpy(sPartNo,pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator));	

				char sPartType[16]=" ";	//构件类型
				char sSpec[100]=" ";	//规格
				if(pPartRec->part_type==STEELTYPE_ANGLE)
				{
					strcpy(sPartType,"L");
					CLDSLineAngle::GetAngleSimpleSpec(sSpec,pPartRec->wide,pPartRec->y_wing_wide,pPartRec->thick);
				}
				else if(pPartRec->part_type==STEELTYPE_PLATE)
				{
					strcpy(sPartType,"-");
					sprintf(sSpec,"%.0fx%.0f",pPartRec->thick,pPartRec->wide);
				}
				else if(pPartRec->part_type==STEELTYPE_TUBE)
				{
					strcpy(sPartType,"Φ1");
					sprintf(sSpec,"%.0fx%.0f",pPartRec->wide,pPartRec->thick);
				}
				else if(pPartRec->part_type==STEELTYPE_SLOT)
					strcpy(sPartType,"[");
				//材质 A3->Q235(S) 16Mn->Q345(H)
				char sMat[10]=" ";
				if(pPartRec->cMaterial==CSteelMatLibrary::Q235BriefMark())//'S')
					strcpy(sMat,"A3");
				else if(pPartRec->cMaterial==CSteelMatLibrary::Q345BriefMark())//'H')
					strcpy(sMat,"16Mn");
				//火曲、切角、铲背
				char cHuoQu=' ',cCutAngle=' ',cCutBer=' ';
				if(pPartRec->bZhiWan)
					cHuoQu='1';
				if(pPartRec->bCutAngle)
					cCutAngle='1';
				if(pPartRec->bCutBer)
					cCutBer='1';
				//备注
				char sNotes[200]=" ";
				strcat(sNotes,pPartRec->sNotes);
				double length=pPartRec->length;
				if(CLDSPlate::m_iPlankWeightStatType==2&&pPartRec->part_type==STEELTYPE_PLATE)
					length=pPartRec->real_area/pPartRec->wide;	//浙江绍兴提出钢板长度应根据<净面积/宽度>计算所得 wjh-2014.4.09
				fprintf(fp,"%s,%s,%s, ,%s,%.0f,%d,%.3f, 0.00,%d,%C,%C,%C,%C, , ,%s,%s\n", 
					sPartNo,sPartType,sSpec,sMat,length,
					pPartRec->GetPartNum(),pPartRec->fPieceMinWeight,pPartRec->nMSumLs,
					cHuoQu,cHuoQu,cCutAngle,cCutBer,sNotes,Ta.m_sTaType);
			}
			fclose(fp);
		}
		char sCmdPara[500]="";
		sprintf(sCmdPara,"notepad.exe %s",dlg.GetPathName());
		WinExec(sCmdPara,SW_SHOW);
	}
	EndWaitCursor();
}

void CLDSDoc::OnExportTxtLsData() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"lsb","ls",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"The data file of bolt table(*.lsb)|*.lsb||");
#else 
		"文本螺栓表数据文件(*.lsb)|*.lsb||");
#endif
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta,TRUE);
	mat_tbl.SortLs();

	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file！");
#else 
			AfxMessageBox("打不开指定文件！");
#endif
		}
		else
		{
			for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
			{
				char name[MAX_TEMP_CHAR_50+1]="";
				if(pLsRec->typels0_pad1==1)
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(name,"Washer");
#else 
					strcpy(name,"垫圈");
#endif
				else if(pLsRec->bFootNail)
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(name,"Footnail");
#else 
					strcpy(name,"脚钉");
#endif
				else
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(name,"Bolt");
#else 
					strcpy(name,"螺栓");
#endif
				fprintf(fp,"%s, %s, M%dX%d, %s, %d~%d, %3d, %.1f\n",(char*)pLsRec->iSeg.ToString(),name,
					pLsRec->d,pLsRec->L,pLsRec->grade,pLsRec->L0_downlim,pLsRec->L0_uplim,pLsRec->GetPartNum(),
					pLsRec->sum_weight);
			}
			fclose(fp);
		}
	}
}
//材料汇总数据
void CLDSDoc::ExportTxtSumMatData(LPCTSTR lpszPathName) 
{
	CTowerType mat_tbl;
	BeginWaitCursor();
	mat_tbl.RunSummarize(&Ta,TRUE);
	mat_tbl.SortPart();
	mat_tbl.SortLs();

	FILE *fp = fopen(lpszPathName,"wt");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't open the specified file！");
#else 
		AfxMessageBox("打不开指定文件！");
#endif
	}
	else
	{
		fprintf(fp,"SECTION BODY\n");
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			char seg_str[500]="";
			Ta.GetSegStr(seg_str,pModule);
			fprintf(fp,"%s\t%s\n",pModule->description,seg_str);
		}
		fprintf(fp,"SECTION PART\n");
		for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
		{
			//编号(编号为空时输出一个空格)
			CXhChar16 sPartNo(pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator));
			if(strlen(sPartNo)<5)
				sPartNo.ResizeLength(5,' ',true);
			//材质 A3->Q235(S) 16Mn->Q345(H)
			char matStr[20]="";
			QuerySteelMatMark(pPartRec->cMaterial,matStr);
			//备注
			char sNotes[200]=" ";
			pPartRec->GetProcessNotes(sNotes);
			strcat(sNotes,pPartRec->sNotes);
			//法兰及遮水板规格按钢板规格显示	wht 12-05-18
			char sSpec[50]="";
			if(pPartRec->part_type==STEELTYPE_PLATE)
				sprintf(sSpec,"-%.f",pPartRec->thick);
			else 
				strcpy(sSpec,pPartRec->sSpec);
			//
			double fWeight=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)//根据设置输出重量 wht 11-06-01
				fWeight=pPartRec->fPieceMinWeight;
			else
				fWeight=pPartRec->fPieceNetWeight;
			fWeight=TruncateDouble(fWeight,2);
			double sumWeight=TruncateDouble(pPartRec->GetPartNum()*fWeight,1);
			fprintf(fp,"%s\t%s\t%s\t%s\t\t%.1f\t%d\t%.2f\t%.1f\t%s\n", 
				(char*)pPartRec->iSeg.ToString(),(char*)sPartNo,matStr,sSpec,pPartRec->length,
				pPartRec->GetPartNum(),fWeight,sumWeight,sNotes);
		}
		fprintf(fp,"SECTION BOLT\n");
		for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
		{
			int type;
			char sNote[30]="\0";
			if(pLsRec->typels0_pad1==1)
			{
				type=2;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sNote,"Washer");
#else 
				strcpy(sNote,"垫圈");
#endif
			}
			else if(pLsRec->bFootNail)
			{
				type=1;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sNote,"Footnail");
#else 
				strcpy(sNote,"脚钉");
#endif
			}
			else
			{
				type=0;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sNote,"Bolt");
#else 
				strcpy(sNote,"螺栓");
#endif
			}
			double sumWeight=TruncateDouble(pLsRec->sum_weight,1);
			if(pLsRec->typels0_pad1==1)
			{
				fprintf(fp,"%s\t-%dX%d\t\t%d\t%.1f\t%d\t%s\n",(char*)pLsRec->iSeg.ToString(),pLsRec->L,pLsRec->d,
				pLsRec->GetPartNum(),sumWeight,type,sNote);
			}
			else
			{
				fprintf(fp,"%s\t%sM%dX%d\t%d\t%.1f\t%d\t%s\n",(char*)pLsRec->iSeg.ToString(),pLsRec->grade,pLsRec->d,pLsRec->L,
				pLsRec->GetPartNum(),pLsRec->sum_weight,type,sNote);
			}
			
		}
	}
	fclose(fp);
}

void CLDSDoc::AddWorkPartsSheet(CTowerType &mat_tbl, LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Workpiece Data");
#else 
	excel_sheet.SetName("工件数据");
#endif
	//
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+1,14);
	//第一行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Part Lable"));
	map.SetValueAt(index,1,COleVariant("Segment Number"));
	map.SetValueAt(index,2,COleVariant("Material Name"));
	map.SetValueAt(index,3,COleVariant("Material Texture"));
	map.SetValueAt(index,4,COleVariant("Specification"));
	map.SetValueAt(index,5,COleVariant("Length"));
	map.SetValueAt(index,6,COleVariant("Single Number"));
	map.SetValueAt(index,7,COleVariant("Bend"));
	map.SetValueAt(index,8,COleVariant("Weld"));
	map.SetValueAt(index,9,COleVariant("Leg Cut"));
	map.SetValueAt(index,10,COleVariant("Cut Root"));
	map.SetValueAt(index,11,COleVariant("Cut Back"));
	map.SetValueAt(index,12,COleVariant("Open Merge Angle"));
	map.SetValueAt(index,13,COleVariant("Slot"));
#else 
	map.SetValueAt(index,0,COleVariant("构件编号"));
	map.SetValueAt(index,1,COleVariant("段号"));
	map.SetValueAt(index,2,COleVariant("材料名称"));
	map.SetValueAt(index,3,COleVariant("材质"));
	map.SetValueAt(index,4,COleVariant("规格"));
	map.SetValueAt(index,5,COleVariant("长度"));
	map.SetValueAt(index,6,COleVariant("单基件数"));
	map.SetValueAt(index,7,COleVariant("制弯"));
	map.SetValueAt(index,8,COleVariant("焊接"));
	map.SetValueAt(index,9,COleVariant("切角"));
	map.SetValueAt(index,10,COleVariant("刨根"));
	map.SetValueAt(index,11,COleVariant("铲背"));
	map.SetValueAt(index,12,COleVariant("开合角"));
	map.SetValueAt(index,13,COleVariant("坡口"));
#endif
	index++;
	//
	int i=1;
	for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		map.SetValueAt(index,0,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		map.SetValueAt(index,1,COleVariant(pRec->iSeg.ToString()));
		if(pRec->part_type==STEELTYPE_ANGLE)
			map.SetValueAt(index,2,COleVariant("L"));
		else if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,2,COleVariant("-"));
		map.SetValueAt(index,3,COleVariant(CString(pRec->cMaterial)));
		map.SetValueAt(index,4,COleVariant(pRec->sSpec));
		map.SetValueAt(index,5,COleVariant(pRec->length));
		map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));
		if(pRec->bZhiWan)
			map.SetValueAt(index,7,COleVariant("*"));
		if(pRec->bWeldPart)
			map.SetValueAt(index,8,COleVariant("*"));
		if(pRec->bCutAngle)
			map.SetValueAt(index,9,COleVariant("*"));
		if(pRec->bCutRoot)
			map.SetValueAt(index,10,COleVariant("*"));
		if(pRec->bCutBer)
			map.SetValueAt(index,11,COleVariant("*"));
		if(pRec->bKaiJiao)
			map.SetValueAt(index,12,COleVariant("*"));
		if(pRec->bNeedFillet)
			map.SetValueAt(index,13,COleVariant("*"));
	}
	char cell_end[16]="";
	sprintf(cell_end,"N%d",i);
	//设置构件编号格式
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A2",cell_end,COleVariant("@"));
	//设置水平对齐方式 居左
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"E1",cell_end,COleVariant((long)2));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//设置数字格式
	sprintf(cell_end,"F%d",i);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"F2",cell_end,COleVariant("0.00"));
	//
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddManuModuleSheet(LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Process Module");
#else 
	excel_sheet.SetName("加工呼高");
#endif
	//
	int index=0;
	CVariant2dArray map(Ta.Module.GetNodeNum()+1,3);
	//第一行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Module Name"));
	map.SetValueAt(index,1,COleVariant("Process Basic Number"));
	map.SetValueAt(index,2,COleVariant("Segment Scope"));
#else 
	map.SetValueAt(index,0,COleVariant("呼高名称"));
	map.SetValueAt(index,1,COleVariant("加工基数"));
	map.SetValueAt(index,2,COleVariant("段号范围"));
#endif
	index++;
	//
	int i=1;
	CXhChar100 seg_str;
	for(CLDSModule *pModule=Ta.Module.GetTail();pModule;pModule=Ta.Module.GetPrev(),index++)
	{
		i++;
		Ta.GetSegStr(seg_str,pModule);
		map.SetValueAt(index,0,COleVariant(pModule->description));
		map.SetValueAt(index,1,COleVariant((long)1));
		map.SetValueAt(index,2,COleVariant(seg_str));
	}
	char cell_end[15]="";
	sprintf(cell_end,"C%d",i);
	//设置单元格格式
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"C1",cell_end,COleVariant("@"));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddManuPartSheet(CTowerType& mat_tbl,LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Work Piece");
#else 
	excel_sheet.SetName("工件");
#endif
	//
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+1,9);
	//第一行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Number"));
	map.SetValueAt(index,2,COleVariant("Material Texture"));
	map.SetValueAt(index,3,COleVariant("Specification"));
	map.SetValueAt(index,4,COleVariant("Length"));
	map.SetValueAt(index,5,COleVariant("Amount"));
	map.SetValueAt(index,6,COleVariant("Piece Weight"));
	map.SetValueAt(index,7,COleVariant("Single Weight"));
	map.SetValueAt(index,8,COleVariant("Notes"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("编号"));
	map.SetValueAt(index,2,COleVariant("材质"));
	map.SetValueAt(index,3,COleVariant("规格"));
	map.SetValueAt(index,4,COleVariant("长度"));
	map.SetValueAt(index,5,COleVariant("数量"));
	map.SetValueAt(index,6,COleVariant("单重"));
	map.SetValueAt(index,7,COleVariant("总重"));
	map.SetValueAt(index,8,COleVariant("备注"));
#endif
	index++;
	//
	for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		char matStr[20]="";
		QuerySteelMatMark(pRec->cMaterial,matStr);
		//备注
		char sNotes[200]=" ";
		pRec->GetProcessNotes(sNotes);
		strcat(sNotes,pRec->sNotes);
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		map.SetValueAt(index,2,COleVariant(matStr));
		//法兰及遮水板规格按钢板规格显示	wht 12-05-18
		char sSpec[50]="";
		if(pRec->part_type==STEELTYPE_PLATE)
			sprintf(sSpec,"-%.f",pRec->thick);
		else 
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(sSpec));
		//
		double fWeight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)//根据设置输出重量 wht 11-06-01
			fWeight=pRec->fPieceMinWeight;
		else
			fWeight=pRec->fPieceNetWeight;
		fWeight=TruncateDouble(fWeight,2);
		double sumWeight=TruncateDouble(pRec->GetPartNum()*fWeight,1);
		double fLen=TruncateDouble(pRec->length,1);
		map.SetValueAt(index,4,COleVariant(fLen));
		map.SetValueAt(index,5,COleVariant((long)pRec->GetPartNum()));
		map.SetValueAt(index,6,COleVariant(fWeight));
		map.SetValueAt(index,7,COleVariant(sumWeight));
		map.SetValueAt(index,8,COleVariant(sNotes));
	}
	char cell_end[16]="";
	sprintf(cell_end,"I%d",index);
	//设置构件编号格式
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A2",cell_end,COleVariant("@"));
	//设置水平对齐方式 居左
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"I1",cell_end,COleVariant((long)2));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddBoltSheet(CTowerType& mat_tbl,LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Bolt");
#else 
	excel_sheet.SetName("螺栓");
#endif
	//
	int index=0;
	//
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+1,6);
	//第一行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Grade Specification"));
	map.SetValueAt(index,2,COleVariant("Amount"));
	map.SetValueAt(index,3,COleVariant("Weight"));
	map.SetValueAt(index,4,COleVariant("Type"));
	map.SetValueAt(index,5,COleVariant("Notes"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("级别规格"));
	map.SetValueAt(index,2,COleVariant("数量"));
	map.SetValueAt(index,3,COleVariant("重量"));
	map.SetValueAt(index,4,COleVariant("类型"));
	map.SetValueAt(index,5,COleVariant("备注"));
#endif
	index++;

	for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
	{
		int type;
		char sNote[30]="\0";
		if(pLsRec->typels0_pad1==1)
		{
			type=2;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sNote,"Washer");
#else 
			strcpy(sNote,"垫圈");
#endif
		}
		else if(pLsRec->bFootNail)
		{
			type=1;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sNote,"Footnail");
#else 
			strcpy(sNote,"脚钉");
#endif
		}
		else
		{
			type=0;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sNote,"Bolt");
#else 
			strcpy(sNote,"螺栓");
#endif
		}
		double sumWeight=0.0;
		if(pLsRec->typels0_pad1==1)
		{
			map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
			char sSpec[50]="";
			sprintf(sSpec,"-%dX%d",pLsRec->L,pLsRec->d);
			map.SetValueAt(index,1,COleVariant(sSpec));
			map.SetValueAt(index,2,COleVariant(pLsRec->GetPartNum()));
			sumWeight=TruncateDouble(pLsRec->sum_weight,1);
			map.SetValueAt(index,3,COleVariant(sumWeight));
			map.SetValueAt(index,4,COleVariant((long)type));
			map.SetValueAt(index,5,COleVariant(sNote));
		}
		else
		{
			map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
			char sSpec[50]="";
			sprintf(sSpec,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
			map.SetValueAt(index,1,COleVariant(sSpec));
			map.SetValueAt(index,2,COleVariant(pLsRec->GetPartNum()));
			sumWeight=TruncateDouble(pLsRec->sum_weight,1);
			map.SetValueAt(index,3,COleVariant(sumWeight));
			map.SetValueAt(index,4,COleVariant((long)type));
			map.SetValueAt(index,5,COleVariant(sNote));
		}
		index++;
	}
	char cell_end[15]="";
	sprintf(cell_end,"B%d",index);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B4",cell_end,COleVariant("@"));
	//设置表格内容
	sprintf(cell_end,"F%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddBodySheet(LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Module");
#else 
	excel_sheet.SetName("呼高");
#endif
	//
	int index=0;
	CVariant2dArray map(Ta.Module.GetNodeNum()+1,2);
	//第一行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Module Name"));
	map.SetValueAt(index,1,COleVariant("Segment Scope"));
#else 
	map.SetValueAt(index,0,COleVariant("呼高名称"));
	map.SetValueAt(index,1,COleVariant("段号范围"));
#endif
	index++;
	//
	CXhChar100 seg_str;
	for(CLDSModule *pModule=Ta.Module.GetTail();pModule;pModule=Ta.Module.GetPrev(),index++)
	{
		Ta.GetSegStr(seg_str,pModule);
		map.SetValueAt(index,0,COleVariant(pModule->description));
		map.SetValueAt(index,1,COleVariant(seg_str));
	}
	char cell_end[15]="";
	sprintf(cell_end,"B%d",index);
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddCommonPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet, BOOL bOrgMat, CString segStr,LDSPART_TYPE part_type/*=TA_OTHER*/, int iPartSortType)
{	
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cell[16];
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	PART *pRec;
	LPDISPATCH pRange;
	int i=0;

	CExcelTblManager ExcelManager;
	char path[MAX_PATH],syspath[MAX_PATH];
	GetSysPath(syspath);
	sprintf(path,"%sparttbl.txt",syspath);
	BOOL bDefTbl=ExcelManager.InitExcelDataDriver(path);
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		if (iPartSortType==1)
			mat_tbl.SortPart(2);
		else
			mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		if(part_type==TA_OTHER)
			excel_sheet.SetName("Tower Block Table");
		else if(part_type==STEELTYPE_ANGLE)
			excel_sheet.SetName("Angle Summary Table");
		else if(part_type==STEELTYPE_TUBE)
			excel_sheet.SetName("Tube Summary Table");
		else if(part_type==STEELTYPE_PLATE)
			excel_sheet.SetName("Plate Summary Table");
		else if(part_type==STEELTYPE_FLAT)
			excel_sheet.SetName("Flat Iron Summary Table");
		else if(part_type==STEELTYPE_SLOT)
			excel_sheet.SetName("U-steel Summary Table");
#else 
		if(part_type==TA_OTHER)
			excel_sheet.SetName("铁塔部件明细表");
		else if(part_type==STEELTYPE_ANGLE)
			excel_sheet.SetName("角钢汇总表");
		else if(part_type==STEELTYPE_TUBE)
			excel_sheet.SetName("钢管汇总表");
		else if(part_type==STEELTYPE_PLATE)
			excel_sheet.SetName("钢板汇总表");
		else if(part_type==STEELTYPE_FLAT)
			excel_sheet.SetName("扁铁汇总表");
		else if(part_type==STEELTYPE_SLOT)
			excel_sheet.SetName("槽钢汇总表");
#endif
	}
	if(part_type==TA_OTHER)
	{	//未指定统计的构件类型 对选中构件进行统计时不需要进行段号过滤
		CHashList<SEGI> segTable;
		GetSegNoHashTblBySegStr(segStr,segTable);
		for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(pRec->GetPartNum()==0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
				mat_tbl.part_recordset.DeleteCursor();
		}
		mat_tbl.part_recordset.Clean();
	}
	else //分类显示选中构件时 使用默认格式
		bDefTbl=FALSE;
	if(bDefTbl)
	{
		for(MERGECELL_CMD *pCmd=ExcelManager.mergecell_cmd.GetFirst();pCmd;pCmd=ExcelManager.mergecell_cmd.GetNext())
		{
			strcpy(cell_start,pCmd->cell_start);
			strcpy(cell_end,pCmd->cell_end);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			if(pCmd->cell_start[0]==pCmd->cell_end[0])	//纵向合并
				excel_range.Merge(COleVariant((BYTE)0));
			else										//横向合并
				excel_range.Merge(COleVariant((BYTE)1));
			pRange->Release();
		}
		for(CELL_CONTENT *pCellContent=ExcelManager.cell_content.GetFirst();pCellContent;pCellContent=ExcelManager.cell_content.GetNext())
		{
			pRange=excel_sheet.GetRange(COleVariant(pCellContent->cell_name));
			excel_range.AttachDispatch(pRange);
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			//excel_range.SetRowHeight(COleVariant(20.0));
			borders.AttachDispatch(excel_range.GetBorders(),FALSE);
			//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
			border.AttachDispatch(borders.GetItem(9),FALSE);
			border.SetLineStyle(COleVariant((short)1));	//xlDouble =-4119;
			border.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
			border.ReleaseDispatch();
			borders.ReleaseDispatch();
			/*font.AttachDispatch(excel_range.GetFont(),FALSE);
			font.SetBold(covTrue);
			font.SetSize(COleVariant(14.0));
			font.ReleaseDispatch();*/
			if(pCellContent->string0_keyword1==0)
			{
				excel_range.SetValue(COleVariant(pCellContent->cell_content));
			}
			else //if(pCellContent->string0_keyword1==1)
			{
				//if(stricmp(pCellContent->cell_content,"PART_NO")==0)
				//	excel_range.SetValue
			}
			excel_range.ReleaseDispatch();
		}
		i=ExcelManager.title_row;

		for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			i++;
			for(COLUMN_CELL *pColCell=ExcelManager.column_cell.GetFirst();pColCell;pColCell=ExcelManager.column_cell.GetNext())
			{
				_snprintf(cell,15,"%C%d",pColCell->cell_collumn,i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				if(stricmp(pColCell->cell_content,"PART_NO")==0)
				{
					excel_range.SetNumberFormatLocal(COleVariant("@"));
					excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
					excel_range.SetHorizontalAlignment(COleVariant((long)4));
				}
				else if(stricmp(pColCell->cell_content,"MAT_TYPE")==0)
				{
					QuerySteelMatMark(pRec->cMaterial,cell);
					excel_range.SetValue(COleVariant(cell));
				}
				else if(stricmp(pColCell->cell_content,"PART_GUIGE")==0)
				{
					excel_range.SetValue(COleVariant(pRec->sSpec));
				}
				else if(stricmp(pColCell->cell_content,"PART_PARA")==0)
				{
					char ss[30];
					sprintf(ss,"5.1f",pRec->length);
					excel_range.SetValue(COleVariant(ss));
				}
				else if(stricmp(pColCell->cell_content,"PART_NUM")==0)
				{
					excel_range.SetValue(COleVariant((long)pRec->GetPartNum()));
				}
				else if(stricmp(pColCell->cell_content,"SUM_LS_NUM")==0)
				{
					long nLs=pRec->nM16Ls+pRec->nM20Ls+pRec->nM24Ls+pRec->nMXLs;
					if(nLs>0)
						excel_range.SetValue(COleVariant(nLs));
				}
				else if(stricmp(pColCell->cell_content,"ZHIWAN")==0)
				{
					if(pRec->bZhiWan)
						excel_range.SetValue(COleVariant("√"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_ANGLE")==0)
				{
					if(pRec->bCutAngle)
						excel_range.SetValue(COleVariant("√"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_BER")==0)
				{
					if(pRec->bCutBer)
						excel_range.SetValue(COleVariant("√"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_ROOT")==0)
				{
					if(pRec->bCutRoot)
						excel_range.SetValue(COleVariant("√"));
				}
				excel_range.DetachDispatch();
				pRange->Release();
			}
		}
	}
	else
	{	// 第一行主标题行
		char sTitle[200]="";
		if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sTitle,"XXX Company Tower Raw Material Table");
#else 
			strcpy(sTitle,"XXX公司铁塔原始材料表");
#endif
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(part_type==TA_OTHER)
				strcpy(sTitle,"XXX Company Tower Block Table");
			else if(part_type==STEELTYPE_ANGLE)
				strcpy(sTitle,"XXX Company Tower Angle Summary Table");
			else if(part_type==STEELTYPE_TUBE)
				strcpy(sTitle,"XXX Company Tower Tube Summary Table");
			else if(part_type==STEELTYPE_PLATE)
				strcpy(sTitle,"XXX Company Tower Plate Summary Table");
			else if(part_type==STEELTYPE_FLAT)
				strcpy(sTitle,"XXX Company Tower Flat Iron Summary Table");
			else if(part_type==STEELTYPE_SLOT)
				strcpy(sTitle,"XXX Company Tower U-steel Summary Table");
#else 
			if(part_type==TA_OTHER)
				strcpy(sTitle,"XXX公司铁塔部件明细表"); 
			else if(part_type==STEELTYPE_ANGLE)
				strcpy(sTitle,"XXX公司铁塔角钢汇总表");
			else if(part_type==STEELTYPE_TUBE)
				strcpy(sTitle,"XXX公司铁塔钢管汇总表");
			else if(part_type==STEELTYPE_PLATE)
				strcpy(sTitle,"XXX公司铁塔钢板汇总表");
			else if(part_type==STEELTYPE_FLAT)
				strcpy(sTitle,"XXX公司铁塔扁铁汇总表");
			else if(part_type==STEELTYPE_SLOT)
				strcpy(sTitle,"XXX公司铁塔槽钢汇总表");
#endif
		}
		CExcelOper::SetMainTitle(excel_sheet,"A1","Q1",sTitle);
		
		int index=0;
		CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+2,17);
		// 第二行子标题行
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
		map.SetValueAt(index,0,COleVariant("塔型"));
#endif
		// 填写塔型数据
		strcpy(cell_start,"B2");
		strcpy(cell_end,"D2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
		// 工程名称
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,4,COleVariant("Project Name"));
#else 
		map.SetValueAt(index,4,COleVariant("工程名称"));
#endif
		// 填写工程名称数据
		strcpy(cell_start,"F2");
		strcpy(cell_end,"K2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,7,COleVariant(Ta.m_sPrjName));
		// 书写螺栓孔数量项
		strcpy(cell_start,"L2");
		strcpy(cell_end,"O2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,11,COleVariant("Bolt Hole Number"));
#else 
		map.SetValueAt(index,11,COleVariant("螺栓孔数量"));
#endif
		excel_range.ReleaseDispatch();
		// 书写基数项
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,15,COleVariant("Basic Number"));
#else 
		map.SetValueAt(index,15,COleVariant("基数"));
#endif
		map.SetValueAt(index,16,COleVariant((long)Ta.m_nTaNum));
		index++;
		
		//第三行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Segment Number"));
		map.SetValueAt(index,1,COleVariant("Number"));
		map.SetValueAt(index,2,COleVariant("Material Texture"));
		map.SetValueAt(index,3,COleVariant("Specification"));
		map.SetValueAt(index,4,COleVariant("Length"));
		map.SetValueAt(index,5,COleVariant("Single Basic Number"));
		map.SetValueAt(index,6,COleVariant("Total Number"));
		map.SetValueAt(index,7,COleVariant("Piece Weight"));
		map.SetValueAt(index,8,COleVariant("Total Weight"));
		map.SetValueAt(index,9,COleVariant("Weld Seam Edge Length"));
		map.SetValueAt(index,10,COleVariant("Edge Number"));
#else 
		map.SetValueAt(index,0,COleVariant("段号"));
		map.SetValueAt(index,1,COleVariant("编号"));
		map.SetValueAt(index,2,COleVariant("材质"));
		map.SetValueAt(index,3,COleVariant("规格"));
		map.SetValueAt(index,4,COleVariant("长度"));
		map.SetValueAt(index,5,COleVariant("单基数"));
		map.SetValueAt(index,6,COleVariant("总件数"));
		map.SetValueAt(index,7,COleVariant("单根重"));
		map.SetValueAt(index,8,COleVariant("总重"));
		map.SetValueAt(index,9,COleVariant("焊缝边长"));
		map.SetValueAt(index,10,COleVariant("边数"));
#endif
		map.SetValueAt(index,11,COleVariant("M16"));
		map.SetValueAt(index,12,COleVariant("M20"));
		map.SetValueAt(index,13,COleVariant("M24"));
		map.SetValueAt(index,14,COleVariant("M_X"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,15,COleVariant("Special Process"));
		map.SetValueAt(index,16,COleVariant("Special Process"));
#else 
		map.SetValueAt(index,15,COleVariant("特殊工艺"));
		map.SetValueAt(index,16,COleVariant("特殊工艺"));
#endif
		//设置列宽
		CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A3","C3");
		CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.5,"D3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.0,"E3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,5.5,"F3","I3");
		CExcelOper::SetRangeColWidth(excel_sheet,5.0,"H3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.0,"J3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,4.0,"K3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,2.5,"L3","O3");
		//合并列
		pRange=excel_sheet.GetRange(COleVariant("Q3"),COleVariant("P3"));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Special Process"));
#else 
		excel_range.SetValue(COleVariant("特殊工艺"));
#endif
		excel_range.SetColumnWidth(COleVariant(5.75));
		excel_range.SetHorizontalAlignment(COleVariant((long)3));
		excel_range.DetachDispatch();
		pRange->Release();
		//
		i=2;
		long seg_start_i=-1,n=mat_tbl.part_recordset.GetNodeNum();
		SEGI seg_start_no;
		if(part_type!=TA_OTHER)
		{	//对选中构件进行统计以前应该先得到当前要统计类型构件的记录数
			n=0;
			for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
			{
				if(pRec->part_type==part_type) 
					n++;	
			}
		}
		for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(part_type!=TA_OTHER&&pRec->part_type!=part_type)
				continue;	//指定构件类型时，跳过非指定类型构件
			i++;
			index++;
			_snprintf(cell,15,"A%d",i+1);//构件分段号
			if(pRec==NULL)
				continue;
			if (iPartSortType==1)
				map.SetValueAt(index, 0, COleVariant(pRec->iSeg.ToString()));
			else
			{
				if (seg_start_i == -1)
				{
					seg_start_i = i + 1;
					seg_start_no = pRec->iSeg;
					if (part_type == TA_OTHER)	//未指定构件类型时 需要显示段号
						map.SetValueAt(index, 0, COleVariant(seg_start_no.ToString()));
				}
				else if ((part_type == TA_OTHER && pRec->iSeg.iSeg != seg_start_no.iSeg) || i - 2 == n)
				{
					_snprintf(cell_start, 15, "A%d", seg_start_i);
					//合并最后一段段号,最后一段仅有一个构件时cell_end取i wht 13-03-11
					if (i - 2 == n && pRec->iSeg.iSeg == seg_start_no.iSeg)
						_snprintf(cell_end, 15, "A%d", i + 1);
					else
						_snprintf(cell_end, 15, "A%d", i);
					//合并行
					CExcelOper::MergeRowRange(excel_sheet, cell_start, cell_end);
					seg_start_i = i + 1;
					seg_start_no = pRec->iSeg;
					if (part_type == TA_OTHER)	//未指定构件类型时 需要显示段号
						map.SetValueAt(index, 0, COleVariant(seg_start_no.ToString()));

				}
			}
			map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			//材质
			char mat_str[MAX_TEMP_CHAR_50+1]="";
			QuerySteelMatMark(pRec->cMaterial,mat_str);
			map.SetValueAt(index,2,COleVariant(mat_str));
			map.SetValueAt(index,3,COleVariant(pRec->sSpec));	//规格
			//长度
			char str[30];
			double length=pRec->length;
			if(CLDSPlate::m_iPlankWeightStatType==2&&pRec->part_type==STEELTYPE_PLATE)
				length=pRec->real_area/pRec->wide;	//净面积输出时应根据钢板净面积与宽反推钢板长度
			sprintf(str,"%8.1f",length);
			map.SetValueAt(index,4,COleVariant(str));
			map.SetValueAt(index,5,COleVariant((long)pRec->GetPartNum()));	//单基数
			_snprintf(cell,15,"=F%d*Q2",i+1);	//总件数
			map.SetValueAt(index,6,COleVariant(cell));
			//单重
			if(CLDSPlate::m_iPlankWeightStatType<=1)//根据设置输出重量 wht 11-06-01
				sprintf(str,"%8.2f",pRec->fPieceMinWeight);
			else
				sprintf(str,"%8.2f",pRec->fPieceNetWeight);
			map.SetValueAt(index,7,COleVariant(str));
			//总重
			_snprintf(cell,15,"=H%d*G%d",i+1,i+1);
			map.SetValueAt(index,8,COleVariant(cell));
			//焊接边长
			CString st="";
			st.Format("%.1f",pRec->fWeldEdgeLen);
			map.SetValueAt(index,9,COleVariant((LPCTSTR)st));
			//边数
			map.SetValueAt(index,10,COleVariant((long)pRec->nEdgeNum));
			if(pRec->nM16Ls>0)	//M16螺栓数
				map.SetValueAt(index,11,COleVariant(pRec->nM16Ls));
			if(pRec->nM20Ls>0)	//M20螺栓数
				map.SetValueAt(index,12,COleVariant(pRec->nM20Ls));
			if(pRec->nM24Ls>0)	//M24螺栓数
				map.SetValueAt(index,13,COleVariant(pRec->nM24Ls));
			if(pRec->nMXLs>0)	//其余规格螺栓数
				map.SetValueAt(index,14,COleVariant(pRec->nMXLs));
			//特殊工艺
			sprintf(cell_start,"Q%d",i+1);
			sprintf(cell_end,"P%d",i+1);
			CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
			char ss[200]="";
			pRec->GetProcessNotes(ss);
			strcat(ss,pRec->sNotes);
			map.SetValueAt(index,15,COleVariant(ss));
			map.SetValueAt(index,16,COleVariant(ss));
		}
		//设置构件编号格式
		strcpy(cell_start,"B4");
		sprintf(cell_end,"B%d",i+1);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
		//设置段号水平对齐方式 居中对齐 wht 13-03-11
		strcpy(cell_start,"A2");
		sprintf(cell_end,"A%d",i+1);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)3));
		//设置表格内容
		strcpy(cell_start,"A2");
		sprintf(cell_end,"Q%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
		//设置边框
		CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	}
	excel_sheet.DetachDispatch();
}

//成都铁塔有限公司专用铁塔部件明细表
void CLDSDoc::AddChengDuPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	LPDISPATCH pRange;
	Range excel_range;
	char cell[16],sText[100]="";
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	PART *pRec;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	// 第一行主标题行
#ifdef AFX_TARG_ENU_ENGLISH
	if(bOrgMat)
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX Company Tower Raw Material Table");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX Company Tower Block Table");
#else 
	if(bOrgMat)
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX公司铁塔原始材料表");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX公司铁塔部件明细表");
#endif
	// 第二行加工基数信息
	// 书写基数项
	pRange = excel_sheet.GetRange(COleVariant("N2"));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number"));
#else 
	excel_range.SetValue(COleVariant("基数"));
#endif
	excel_range.ReleaseDispatch();
	// 书写基数项
	pRange = excel_sheet.GetRange(COleVariant("O2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant((long)Ta.m_nTaNum));
	excel_range.ReleaseDispatch();
	//按段号过滤记录
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+2,15);
	// 第三行子标题行
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("塔型"));
#endif
	// 填写塔型数据
	CExcelOper::MergeColRange(excel_sheet,"B3","C3");
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	//工程名称
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("工程名称"));
#endif
	// 填写工程名称数据
	CExcelOper::MergeColRange(excel_sheet,"E3","H3");
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	// 书写螺栓孔数量项
	CExcelOper::MergeColRange(excel_sheet,"I3","M3");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,8,COleVariant("Bolt Hole Number"));
#else 
	map.SetValueAt(index,8,COleVariant("螺栓孔数量"));
#endif
	//合并空格项	
	CExcelOper::MergeColRange(excel_sheet,"N3","O3");
	index++;
	//第四行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A4",NULL,TRUE);
	map.SetValueAt(index,1,COleVariant("Number"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B4",NULL,TRUE);
	map.SetValueAt(index,2,COleVariant("Specification"));
	CExcelOper::SetRangeColWidth(excel_sheet,11.0,"C4",NULL,TRUE);
	map.SetValueAt(index,3,COleVariant("Length"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"D4",NULL,TRUE);
	map.SetValueAt(index,4,COleVariant("Single Basic Number"));
	map.SetValueAt(index,5,COleVariant("Total Number"));
	map.SetValueAt(index,6,COleVariant("Piece Weight"));
	map.SetValueAt(index,7,COleVariant("Total Weight"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A4",NULL,TRUE);
	map.SetValueAt(index,1,COleVariant("编号"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B4",NULL,TRUE);
	map.SetValueAt(index,2,COleVariant("规格"));
	CExcelOper::SetRangeColWidth(excel_sheet,11.0,"C4",NULL,TRUE);
	map.SetValueAt(index,3,COleVariant("长度"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"D4",NULL,TRUE);
	map.SetValueAt(index,4,COleVariant("单基数"));
	map.SetValueAt(index,5,COleVariant("总件数"));
	map.SetValueAt(index,6,COleVariant("单件重"));
	map.SetValueAt(index,7,COleVariant("总重"));
#endif
	CExcelOper::SetRangeColWidth(excel_sheet,5.5,"E4","H4",TRUE);
	map.SetValueAt(index,8,COleVariant("M16"));
	map.SetValueAt(index,9,COleVariant("M20"));
	map.SetValueAt(index,10,COleVariant("M24"));
	map.SetValueAt(index,11,COleVariant("M_X"));
	CExcelOper::SetRangeColWidth(excel_sheet,2.5,"I4","L4",TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,12,COleVariant("Total"));
	CExcelOper::SetRangeColWidth(excel_sheet,3.5,"M4",NULL,TRUE);
	map.SetValueAt(index,13,COleVariant("Special Process"));
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"N4",NULL,TRUE);
	map.SetValueAt(index,14,COleVariant("Notes"));
#else 
	map.SetValueAt(index,12,COleVariant("合计"));
	CExcelOper::SetRangeColWidth(excel_sheet,3.5,"M4",NULL,TRUE);
	map.SetValueAt(index,13,COleVariant("特殊工艺"));
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"N4",NULL,TRUE);
	map.SetValueAt(index,14,COleVariant("备注"));
#endif
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"O4",NULL,TRUE);
	index++;
	i=3;
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//构件分段号
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//构件编号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//材质规格
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(sText,"-%.0f_%s",pRec->thick,steelmat);
		else
		{
			if(stricmp(steelmat,"Q235")==0)
				strcpy(sText,pRec->sSpec);
			else
				sprintf(sText,"%s_%s",(char*)pRec->sSpec,steelmat);
		}
		map.SetValueAt(index,2,COleVariant(sText));
		//长度
		char str[30];
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(str,"%.1fx%.1f",pRec->wide,pRec->length);
		else
			sprintf(str,"%8.1f",pRec->length);
		
		map.SetValueAt(index,3,COleVariant(str));
		//单基数
		map.SetValueAt(index,4,COleVariant((long)pRec->GetPartNum()));
		//总件数
		sprintf(cell,"=E%d*O2",i+1);	//总件数
		map.SetValueAt(index,5,COleVariant(cell));
		//单重
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			sprintf(str,"%8.2f",pRec->fPieceMinWeight);
		else
			sprintf(str,"%8.2f",pRec->fPieceNetWeight);
		map.SetValueAt(index,6,COleVariant(str));
		//总重
		sprintf(cell,"=G%d*F%d",i+1,i+1);	//总重
		map.SetValueAt(index,7,COleVariant(cell));
		//M16螺栓数
		if(pRec->nM16Ls>0)
			map.SetValueAt(index,8,COleVariant(pRec->nM16Ls));
		//M20螺栓数
		if(pRec->nM20Ls>0)
			map.SetValueAt(index,9,COleVariant(pRec->nM20Ls));
		//M24螺栓数
		if(pRec->nM24Ls>0)
			map.SetValueAt(index,10,COleVariant(pRec->nM24Ls));
		//其余规格螺栓数
		if(pRec->nMXLs>0)
			map.SetValueAt(index,11,COleVariant(pRec->nMXLs));
		//螺栓孔合计数
		sprintf(sText,"=I%d+J%d+K%d+L%d",i+1,i+1,i+1,i+1);	//总重
		map.SetValueAt(index,12,COleVariant(sText));
		//特殊工艺
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		map.SetValueAt(index,13,COleVariant(ss));
		//备注
		map.SetValueAt(index,14,COleVariant(pRec->sNotes));
	}
	strcpy(cell_start,"A3");
	sprintf(cell_end,"O%d",i+1);
	//设置构件编号格式
	strcpy(cell_start,"B5");
	sprintf(cell_end,"B%d",i+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置水平对齐方式 右对齐
	strcpy(cell_start,"C5");
	sprintf(cell_end,"D%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)2));
	//设置表格内容
	strcpy(cell_start,"A3");
	sprintf(cell_end,"O%d",i+1);	//cell_end应取i+1,否则数据少输出一行 wht 13-03-11
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
//浙江盛达送变电铁塔有限公司专用铁塔部件明细表
void CLDSDoc::AddZheJiangShengDaPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cell[16];
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	PART *pRec;
	LPDISPATCH pRange;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	// 第一行主标题行
	strcpy(cell_start,"A1");
	strcpy(cell_end,"Q1");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("XXX Tower Company"));
#else 
	excel_range.SetValue(COleVariant("XXX铁塔有限公司"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"A2");
	strcpy(cell_end,"Q2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Tower Raw Material Table"));
	else
		excel_range.SetValue(COleVariant("Tower Block Table"));
#else 
		excel_range.SetValue(COleVariant("铁塔原始材料表"));
	else
		excel_range.SetValue(COleVariant("铁塔零部件明细表"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	// 第三行
	strcpy(cell_start,"O3");
	strcpy(cell_end,"P3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number:"));
#else 
	excel_range.SetValue(COleVariant("基数:"));
#endif
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"Q3");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant("1"));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	// 第四行子标题行
	strcpy(cell_start,"A4");
	strcpy(cell_end,"Q4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	char sText[200];
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sText,"Project Name：%s  Tower Model：%s  Steel Seal：%s  Task List Number：",Ta.m_sPrjName,Ta.m_sTaType,Ta.m_sTaStampNo);
#else 
	sprintf(sText,"工程名称：%s  塔型：%s  钢印代号：%s  任务单编号：",Ta.m_sPrjName,Ta.m_sTaType,Ta.m_sTaStampNo);
#endif
	excel_range.SetValue(COleVariant(sText));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	//第五行
	strcpy(cell,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Sequence"));
#else 
	excel_range.SetValue(COleVariant("序"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.6));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Part Block"));
#else 
	excel_range.SetValue(COleVariant("零部"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Design"));
#else 
	excel_range.SetValue(COleVariant("设计"));
#endif
	excel_range.SetColumnWidth(COleVariant(10.5));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Length"));
#else 
	excel_range.SetValue(COleVariant("长度"));
#endif
	excel_range.SetColumnWidth(COleVariant(10.5));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"E5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Substitute"));
#else 
	excel_range.SetValue(COleVariant("代用"));
#endif
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"F5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
	excel_range.SetValue(COleVariant("单基"));
#endif
	excel_range.SetColumnWidth(COleVariant(4.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Total"));
#else 
	excel_range.SetValue(COleVariant("合计"));
#endif
	excel_range.SetColumnWidth(COleVariant(4.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"H5");
	strcpy(cell_end,"P5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Workload(Piece)"));
#else 
	excel_range.SetValue(COleVariant("工作量(单件)"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.ReleaseDispatch();
	//第六行
	strcpy(cell,"H6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Electric"));
#else 
	excel_range.SetValue(COleVariant("电"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"I6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Hole"));
#else 
	excel_range.SetValue(COleVariant("孔"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"J6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Bend"));
#else 
	excel_range.SetValue(COleVariant("弯"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"K6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Cut"));
#else 
	excel_range.SetValue(COleVariant("切"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"L6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Shovel"));
#else 
	excel_range.SetValue(COleVariant("铲"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"M6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Clear"));
#else 
	excel_range.SetValue(COleVariant("清"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"N6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Merge"));
#else 
	excel_range.SetValue(COleVariant("合"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"O6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Open"));
#else 
	excel_range.SetValue(COleVariant("开"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"P6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Press"));
#else 
	excel_range.SetValue(COleVariant("压"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"Q6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Notes"));
#else 
	excel_range.SetValue(COleVariant("备注"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	//第七行
	strcpy(cell,"A7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Number"));
#else 
	excel_range.SetValue(COleVariant("号"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"B7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Piece Number"));
#else 
	excel_range.SetValue(COleVariant("件号"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"C7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Specification"));
#else 
	excel_range.SetValue(COleVariant("规格"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"D7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant("(mm)"));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"E7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Specification"));
#else 
	excel_range.SetValue(COleVariant("规格"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"F7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Amount"));
#else 
	excel_range.SetValue(COleVariant("数量"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"G7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Amount"));
#else 
	excel_range.SetValue(COleVariant("数量"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"H7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Weld"));
#else 
	excel_range.SetValue(COleVariant("焊"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"I7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Number"));
#else 
	excel_range.SetValue(COleVariant("数"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"J7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Bent"));
#else 
	excel_range.SetValue(COleVariant("曲"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"K7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Angle"));
#else 
	excel_range.SetValue(COleVariant("角"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"L7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Ber"));
#else 
	excel_range.SetValue(COleVariant("背"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"M7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Root"));
#else 
	excel_range.SetValue(COleVariant("根"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"N7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Angle"));
#else 
	excel_range.SetValue(COleVariant("角"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"O7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Angle"));
#else 
	excel_range.SetValue(COleVariant("角"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"P7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Flat"));
#else 
	excel_range.SetValue(COleVariant("扁"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	//第八行数据项标题行，需要格式输出
	i=7;
	long n=mat_tbl.part_recordset.GetNodeNum();
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	//
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),17);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//构件编号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//材质
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(pRec->sSpec,"-%.0f",pRec->thick);
		strcpy(sText,pRec->sSpec);
		char steelmat[20];
		QuerySteelMatMark(toupper(pRec->cMaterial),steelmat);
		strcat(sText,"_");
		strcat(sText,steelmat);
		map.SetValueAt(index,2,COleVariant(sText));
		//长度
		if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant(ftol(pRec->length)));
		else if(pRec->part_type==STEELTYPE_PLATE)
		{	//连板
			sprintf(sText,"%.0fx%.0f",pRec->wide,pRec->length);
			map.SetValueAt(index,3,COleVariant(sText));
		}
		//E列为代用规格,暂为空
		//单基数
		map.SetValueAt(index,5,COleVariant((long)pRec->GetPartNum()));
		//总件数
		sprintf(cell,"=F%d*Q3",i);	//总件数
		map.SetValueAt(index,6,COleVariant(cell));
		//H列为电焊
		if(pRec->bWeldPart)	//是否电焊件
			map.SetValueAt(index,7,COleVariant("*"));
		//总孔数
		long nHole = pRec->nM16Ls+pRec->nM20Ls+pRec->nM24Ls+pRec->nMXLs;
		if(nHole>0)
			map.SetValueAt(index,8,COleVariant(nHole));
		//制弯
		if(pRec->bZhiWan)
			map.SetValueAt(index,9,COleVariant("*"));
		//切角
		if(pRec->bCutAngle)
			map.SetValueAt(index,10,COleVariant("*"));
		//铲背
		if(pRec->bCutBer)
			map.SetValueAt(index,11,COleVariant("*"));
		//清根
		if(pRec->bCutRoot)
			map.SetValueAt(index,12,COleVariant("*"));
		//合角
		//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-23
		if(pRec->part_type==STEELTYPE_ANGLE&&90-pRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
			map.SetValueAt(index,13,COleVariant("*"));
		//开角
		//使用用户设置的开合角阈值判断角钢是否需要进行开合角 wht 11-05-23
		if(pRec->part_type==STEELTYPE_ANGLE&&pRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
			map.SetValueAt(index,14,COleVariant("*"));
		//压扁
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,15,COleVariant("*"));
		//备注
		char ss[200]="";
		pRec->GetProcessNotes(ss,TRUE);	//工艺字符串包括开合角度数 wht 11-05-24
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,16,COleVariant(ss));
	}
	//设置构件编号格式
	strcpy(cell_start,"B8");
	sprintf(cell_end,"B%d",i);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置水平对齐方式 左对齐
	strcpy(cell_start,"C8");
	sprintf(cell_end,"D%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//设置表格内容
	strcpy(cell_start,"A8");
	sprintf(cell_end,"Q%d",i);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	strcpy(cell_start,"A4");
	sprintf(cell_end,"Q%d",i);
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	strcpy(cell_start,"A5");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlEdgeBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"A7");
	strcpy(cell_end,"Q7");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	strcpy(cell_start,"Q6");
	pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange,FALSE);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	excel_sheet.DetachDispatch();
}
void CLDSDoc::AddShanXiJinNengPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cell[16];
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	char sText[100];
	PART *pRec;
	LPDISPATCH pRange;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	// 第一行主标题行
	strcpy(cell_start,"A1");
	strcpy(cell_end,"Q1");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("XXX Tower Company"));
#else 
	excel_range.SetValue(COleVariant("XXX铁塔有限公司"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"A2");
	strcpy(cell_end,"Q2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Tower Raw Material Table"));
	else
		excel_range.SetValue(COleVariant("Tower Block Table"));
#else 
		excel_range.SetValue(COleVariant("铁塔原始材料表"));
	else
		excel_range.SetValue(COleVariant("铁塔零部件明细表"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	// 第三行
	strcpy(cell_start,"L3");
	strcpy(cell_end,"M3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tower Model:"));
#else 
	excel_range.SetValue(COleVariant("塔型:"));
#endif
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"N3");
	strcpy(cell_end,"Q3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant(Ta.m_sTaType));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	//第四～五行
	strcpy(cell_start,"A4");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Segment Number"));
#else 
	excel_range.SetValue(COleVariant("段号"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"B4");
	strcpy(cell_end,"B5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Part Number"));
#else 
	excel_range.SetValue(COleVariant("零件编号"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(4.5));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"C4");
	strcpy(cell_end,"C5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Material"));
#else 
	excel_range.SetValue(COleVariant("材料"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetWrapText(COleVariant((BYTE)1));
	excel_range.SetColumnWidth(COleVariant(3.0));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"D4");
	strcpy(cell_end,"E4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Material Specification"));
#else 
	excel_range.SetValue(COleVariant("材料规格"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"D5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Design Specification"));
#else 
	excel_range.SetValue(COleVariant("设计规格"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(7.0));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"E5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Material Texture"));
#else 
	excel_range.SetValue(COleVariant("材质"));
#endif
	excel_range.SetColumnWidth(COleVariant(4.5));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"F4");
	strcpy(cell_end,"F5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Length(mm)"));
#else 
	excel_range.SetValue(COleVariant("长度(mm)"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(5.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"G4");
	strcpy(cell_end,"G5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Width(mm)"));
#else 
	excel_range.SetValue(COleVariant("宽度(mm)"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(4.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"H4");
	strcpy(cell_end,"H5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Piece Weight"));
#else 
	excel_range.SetValue(COleVariant("单件重量"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(4.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"I4");
	strcpy(cell_end,"I5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Process Number"));
#else 
	excel_range.SetValue(COleVariant("加工数量"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetColumnWidth(COleVariant(4.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"J4");
	strcpy(cell_end,"Q4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Workload(Piece)"));
#else 
	excel_range.SetValue(COleVariant("工作量(单件)"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell_start,"R4");
	strcpy(cell_end,"R5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.SetVerticalAlignment(COleVariant((long)2));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Process Instruction"));
#else 
	excel_range.SetValue(COleVariant("加工说明"));
#endif
	excel_range.SetColumnWidth(COleVariant(15.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"J5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Groupe Weld"));
#else 
	excel_range.SetValue(COleVariant("组焊"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"K5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Hole Count"));
#else 
	excel_range.SetValue(COleVariant("孔数"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"L5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Bend"));
#else 
	excel_range.SetValue(COleVariant("弯曲"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"M5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Leg Cut"));
#else 
	excel_range.SetValue(COleVariant("切角"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"N5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Flat"));
#else 
	excel_range.SetValue(COleVariant("打扁"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"O5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Cut Back"));
#else 
	excel_range.SetValue(COleVariant("铲背"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"P5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Cut Angle"));	//含清根和坡口
#else 
	excel_range.SetValue(COleVariant("刨角"));	//含清根和坡口
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	strcpy(cell,"Q5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Drill Hole"));	//>=14mm
#else 
	excel_range.SetValue(COleVariant("钻孔"));	//>=14mm
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	//第六行数据项标题行，需要格式输出
	i=5;
	long n=mat_tbl.part_recordset.GetNodeNum();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),18);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//段号
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//构件编号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//材质
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(sText,"-",pRec->thick);
		else if(pRec->part_type==STEELTYPE_ANGLE)
			sprintf(sText,"∠");
		else
			strcpy(sText,"");
		map.SetValueAt(index,2,COleVariant(sText));
		//规格
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(sSpec,"%.0f",pRec->thick);
		else if(pRec->part_type==STEELTYPE_ANGLE)
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
		else
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(sSpec));
		//材质
		char steelmat[20];
		if(pRec->cMaterial!=CSteelMatLibrary::Q235BriefMark())//'S')
		{
			QuerySteelMatMark(toupper(pRec->cMaterial),steelmat);
			map.SetValueAt(index,4,COleVariant(steelmat));
		}
		//长度
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));
		//宽度
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));
		//单件重量
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		map.SetValueAt(index,7,COleVariant(weight));
		//单基数
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));
		//J列为电焊
		if(pRec->bWeldPart)
			map.SetValueAt(index,9,COleVariant("*"));
		//总孔数
		long nHole = pRec->nM16Ls+pRec->nM20Ls+pRec->nM24Ls+pRec->nMXLs;
		if(nHole>0)
			map.SetValueAt(index,10,COleVariant(nHole));
		//制弯
		if(pRec->bZhiWan)
			map.SetValueAt(index,11,COleVariant("*"));
		//切角
		if(pRec->bCutAngle)
			map.SetValueAt(index,12,COleVariant("*"));
		//打扁
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,13,COleVariant("*"));
		//铲背
		if(pRec->bCutBer)
			map.SetValueAt(index,14,COleVariant("*"));
		//刨角(含清根和坡口)
		if(pRec->bCutRoot||pRec->bNeedFillet)
			map.SetValueAt(index,15,COleVariant("*"));
		//钻孔
		if(pRec->thick>=14)
			map.SetValueAt(index,16,COleVariant("*"));
		//备注
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,17,COleVariant(ss));
	}
	//设置构件段号、编号格式
	strcpy(cell_start,"A6");
	sprintf(cell_end,"B%d",i);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置水平对齐方式 左对齐
	strcpy(cell_start,"C6");
	sprintf(cell_end,"H%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//设置表格内容
	strcpy(cell_start,"A6");
	sprintf(cell_end,"R%d",i);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	strcpy(cell_start,"A4");
	sprintf(cell_end,"R%d",i);
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//常熟专用铁塔部件明细表
void CLDSDoc::AddChangShuPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	char cell[16],sText[100]="";
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	PART *pRec;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+1,31);
	//第一行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number(Read Only)"));
	map.SetValueAt(index,1,COleVariant("Block Number"));
	map.SetValueAt(index,2,COleVariant("Material Texture"));
	map.SetValueAt(index,3,COleVariant("Material"));
	map.SetValueAt(index,4,COleVariant("Specification"));
	map.SetValueAt(index,5,COleVariant("Length(mm)"));
	map.SetValueAt(index,6,COleVariant("Width(mm)"));
	map.SetValueAt(index,7,COleVariant("Thickness(mm)"));
	map.SetValueAt(index,8,COleVariant("Single Basic Number(Piece)"));
	map.SetValueAt(index,9,COleVariant("Piece Weight(kg)"));
	map.SetValueAt(index,10,COleVariant("Total Weight(kg)"));
	map.SetValueAt(index,11,COleVariant("Single Hole"));
	map.SetValueAt(index,12,COleVariant("Notes"));
	map.SetValueAt(index,13,COleVariant("Weld(0/1)"));
	map.SetValueAt(index,14,COleVariant("Bend"));
	map.SetValueAt(index,15,COleVariant("Leg Cut(刀)"));
	map.SetValueAt(index,16,COleVariant("Cut Back(mm)"));
	map.SetValueAt(index,17,COleVariant("Cut Root(0/1)"));
	map.SetValueAt(index,18,COleVariant("Flat(0/1)"));
	map.SetValueAt(index,19,COleVariant("Open Merge Angle(0/1)"));
	map.SetValueAt(index,20,COleVariant("Drill Hole(0/1)"));
	map.SetValueAt(index,21,COleVariant("Open Slot(0/1)"));
	map.SetValueAt(index,22,COleVariant("Slot(0/1)"));
	map.SetValueAt(index,23,COleVariant("Character Code"));
	map.SetValueAt(index,24,COleVariant("If Special"));
	map.SetValueAt(index,25,COleVariant("Type"));
	map.SetValueAt(index,26,COleVariant("Edge Number"));
	map.SetValueAt(index,27,COleVariant("Perimeter"));
	map.SetValueAt(index,28,COleVariant("Hole Diameter 28~38mm"));
	map.SetValueAt(index,29,COleVariant("Hole Diameter 39~50mm"));
	map.SetValueAt(index,30,COleVariant("Hole Diameter More Than 50"));
#else 
	map.SetValueAt(index,0,COleVariant("段号(只读)"));
	map.SetValueAt(index,1,COleVariant("部件号"));
	map.SetValueAt(index,2,COleVariant("材质"));
	map.SetValueAt(index,3,COleVariant("材料"));
	map.SetValueAt(index,4,COleVariant("规格"));
	map.SetValueAt(index,5,COleVariant("长度(mm)"));
	map.SetValueAt(index,6,COleVariant("宽度(mm)"));
	map.SetValueAt(index,7,COleVariant("厚度(mm)"));
	map.SetValueAt(index,8,COleVariant("单基数量(件)"));
	map.SetValueAt(index,9,COleVariant("单件重量(kg)"));
	map.SetValueAt(index,10,COleVariant("总重量(kg)"));
	map.SetValueAt(index,11,COleVariant("单件孔数"));
	map.SetValueAt(index,12,COleVariant("备注"));
	map.SetValueAt(index,13,COleVariant("电焊(0/1)"));
	map.SetValueAt(index,14,COleVariant("火曲"));
	map.SetValueAt(index,15,COleVariant("切角(刀)"));
	map.SetValueAt(index,16,COleVariant("铲背(mm)"));
	map.SetValueAt(index,17,COleVariant("清根(0/1)"));
	map.SetValueAt(index,18,COleVariant("打扁(0/1)"));
	map.SetValueAt(index,19,COleVariant("开合角(0/1)"));
	map.SetValueAt(index,20,COleVariant("钻孔(0/1)"));
	map.SetValueAt(index,21,COleVariant("开槽(0/1)"));
	map.SetValueAt(index,22,COleVariant("打坡(0/1)"));
	map.SetValueAt(index,23,COleVariant("字码"));
	map.SetValueAt(index,24,COleVariant("是否专用"));
	map.SetValueAt(index,25,COleVariant("类型"));
	map.SetValueAt(index,26,COleVariant("边数"));
	map.SetValueAt(index,27,COleVariant("周长"));
	map.SetValueAt(index,28,COleVariant("孔径28~38mm"));
	map.SetValueAt(index,29,COleVariant("孔径39~50mm"));
	map.SetValueAt(index,30,COleVariant("孔径50以上"));
#endif
	index++;
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"A1","AE1");
	i=0;
	long n=mat_tbl.part_recordset.GetNodeNum();
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//构件分段号
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//构件编号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//材质
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));
		//材料
		char str[30]="";
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(str,"%s","Connect Plate");
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			sprintf(str,"%s","Angle");
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			sprintf(str,"%s","Tube");
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			sprintf(str,"%s","U-steel");
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			sprintf(str,"%s","Flat Angle");
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			sprintf(str,"%s","连板");
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			sprintf(str,"%s","角钢");
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			sprintf(str,"%s","钢管");
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			sprintf(str,"%s","槽钢");
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			sprintf(str,"%s","扁钢");
#endif
		map.SetValueAt(index,3,COleVariant(str));
		//规格
		char sSpec[MAX_TEMP_CHAR_50+1]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//标注角钢规格∠
		{
			char sSimpleSpec[50]="";
			CLDSLineAngle::GetAngleSimpleSpec(sSimpleSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			_snprintf(sSpec,MAX_TEMP_CHAR_50,"∠%s",sSimpleSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			_snprintf(sSpec,MAX_TEMP_CHAR_50,"-%.f",pRec->thick);
		else 
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(sSpec));
		//长度
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));
		//宽度
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));
		//厚度
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));
		//单基数量
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));
		//单件重量
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		map.SetValueAt(index,9,COleVariant(weight));
		//总重
		sprintf(cell,"=I%d*J%d",i+1,i+1);	//总重
		map.SetValueAt(index,10,COleVariant(cell));
		//单件孔数
		map.SetValueAt(index,11,COleVariant((long)pRec->nMSumLs));
		//备注
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,12,COleVariant(ss));
		//是否电焊件
		if(pRec->bWeldPart)
			map.SetValueAt(index,13,COleVariant("1"));
		else 
			map.SetValueAt(index,13,COleVariant("0"));
		//火曲
		if(pRec->bZhiWan)
			map.SetValueAt(index,14,COleVariant("1"));
		else 
			map.SetValueAt(index,14,COleVariant("0"));
		//切角
		if(pRec->bCutAngle)
			map.SetValueAt(index,15,COleVariant("1"));
		else
			map.SetValueAt(index,15,COleVariant("0"));
		//铲背
		if(pRec->bCutBer)
			map.SetValueAt(index,16,COleVariant("1"));
		else
			map.SetValueAt(index,16,COleVariant("0"));
		//清根
		if(pRec->bCutRoot)
			map.SetValueAt(index,17,COleVariant("1"));
		else
			map.SetValueAt(index,17,COleVariant("0"));
		//打扁
		if(pRec->nPushFlat)
			map.SetValueAt(index,18,COleVariant("1"));
		else 
			map.SetValueAt(index,18,COleVariant("0"));
		//开合角
		if(pRec->bKaiJiao||pRec->bHeJiao)
			map.SetValueAt(index,19,COleVariant("1"));
		else 
			map.SetValueAt(index,19,COleVariant("0"));
		//钻孔
		if(pRec->thick>=14)
			map.SetValueAt(index,20,COleVariant("1"));
		else
			map.SetValueAt(index,20,COleVariant("0"));
		//开槽
		if(pRec->IsTubeNotch())
			map.SetValueAt(index,21,COleVariant("1"));
		else
			map.SetValueAt(index,21,COleVariant("0"));
		//坡口
		if(pRec->bNeedFillet)
			map.SetValueAt(index,22,COleVariant("1"));
		else
			map.SetValueAt(index,22,COleVariant("0"));
		//字码	???
		map.SetValueAt(index,23,COleVariant(""));
		//是否专用	???
		map.SetValueAt(index,24,COleVariant("1"));
		//类型
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"支管");
			else 
				strcpy(sTypeName,"主管");
#endif
		}
		else if(pRec->part_type==STEELTYPE_PLATE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"Shelter Plate");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"Flange");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"U Insert Plate");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"Bottom Flange");
#else 
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"遮水板");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"法兰");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"插板");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"底法兰");
#endif
		}
		map.SetValueAt(index,25,COleVariant(sTypeName));
		//边数
		map.SetValueAt(index,26,COleVariant(pRec->nEdgeNum));
		//周长
		char sPerimeter[MAX_TEMP_CHAR_50+1]="";
		sprintf(sPerimeter,"%.1f",pRec->fPerimeter);
		map.SetValueAt(index,27,COleVariant(sPerimeter));
		//孔径28~38mm
		char sLsNum[50]="";
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(28,38));
		map.SetValueAt(index,28,COleVariant(sLsNum));
		//孔径39~50mm
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(39,50));
		map.SetValueAt(index,29,COleVariant(sLsNum));
		//孔径50mm以上
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(51));
		map.SetValueAt(index,30,COleVariant(sLsNum));
	}
	//设置构件编号格式
	strcpy(cell_start,"B2");
	sprintf(cell_end,"B%d",i+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置水平对齐方式 左对齐
	strcpy(cell_start,"F2");
	sprintf(cell_end,"L%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//设置表格内容
	strcpy(cell_start,"A1");
	sprintf(cell_end,"AE%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
}
//开槽组焊代号类型表
//nRuleType 开槽类型编号规则
//nRuleType==0 代号：LRM(左右中) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管 3.节点板和相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL 6.相贯 7.平面切割
//nRuleType==1 代号：LMR(左中右) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL
void CLDSDoc::AddSlotAndWeldCodeTblSheet(LPDISPATCH pSheet,int nRuleType)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Slot Group Weld Type Table");
#else 
	excel_sheet.SetName("开槽组焊代号类型表");
#endif
	//
	int nMidType=4,nType=7,nRowNum=0;
	int i=0,j=0,k=0,index=0;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//中间连接样式
		nType=5;	//两端连接样式
		nRowNum++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
				nRowNum++;
		}
	}
	nRowNum+=4;
	//
	CVariant2dArray map(nRowNum+1,3);
	//第一行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Sequence Number"));
	map.SetValueAt(index,1,COleVariant("Code Name"));
	map.SetValueAt(index,2,COleVariant("Instruction"));
#else 
	map.SetValueAt(index,0,COleVariant("序号"));
	map.SetValueAt(index,1,COleVariant("代号"));
	map.SetValueAt(index,2,COleVariant("说明"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,60.0,"C1",NULL);
	//
	int nCurRow=2;
#ifdef AFX_TARG_ENU_ENGLISH
	CString sMidTypeName[4]={"","Center Node Plate","Center Intersect Tube","Center Intersect Tube And Node Plate"};
	CString sTypeName[7]={"1-type Insert Plate","U Insert Plate","T Insert Plate","Cross Insert Plate","Flange","Intersect","Plane Cut"};
#else 
	CString sMidTypeName[4]={"","中间节点板","中间相贯管","中间相贯管及节点板"};
	CString sTypeName[7]={"一字型插板","U型插板","T型插板","十字插板","法兰","相贯","平面切割"};
#endif
	nMidType=4;
	nType=7;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//中间连接样式
		nType=5;	//两端连接样式
		map.SetValueAt(index,0,COleVariant("1"));
		map.SetValueAt(index,1,COleVariant("0"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,2,COleVariant("Main Tube Weld"));
#else 
		map.SetValueAt(index,2,COleVariant("主管焊接"));
#endif
		index++;
		nCurRow++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
			{
				char sID[50]="";
				sprintf(sID,"%d",nCurRow-1);
				map.SetValueAt(index,0,COleVariant(sID));
				char sCode[50]="";
				//编号
				if(nRuleType==0)
				{	//LRM
					if(k==0)
						sprintf(sCode,"%d%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,j,k);
				}
				else if(nRuleType==1)
				{	//LMR
					if(k==0)
						sprintf(sCode,"%d0%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,k,j);
				}
				map.SetValueAt(index,1,COleVariant(sCode));
				char sHelpStr[200]="";
				if(i==j)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sHelpStr,"Two End %s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"One End %s Anoter End %s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#else 
					sprintf(sHelpStr,"两端%s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"一端%s另一端%s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#endif
				map.SetValueAt(index,2,COleVariant(sHelpStr));
				index++;
				nCurRow++;
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	CString sHelpStr[4]={"Angle Weld Part","Handrail Weld Part","Ladder Weld Part","Other Weld Part"};
#else 
	CString sHelpStr[4]={"连板角钢焊件","扶手焊件","爬梯焊件","其他焊件"};
#endif
	for(i=0;i<4;i++)
	{
		char sID[50]="";
		sprintf(sID,"%d",nCurRow-1);
		map.SetValueAt(index,0,COleVariant(sID));
		char sCode[50]="";
		//编号
		if(nRuleType==0)
		{	//LRM
			if(i==2)
				strcpy(sCode,"A");
			else if(i==3)
				strcpy(sCode,"B");
			else
				sprintf(sCode,"%d",i+8);
		}
		else if(nRuleType==1)	//LMR
			sprintf(sCode,"%d00",i+6);
		map.SetValueAt(index,1,COleVariant(sCode));
		map.SetValueAt(index,2,COleVariant(sHelpStr[i]));
		nCurRow++;
		index++;
	}
	char cell_end[15]="";
	//设置表格内容
	sprintf(cell_end,"C%d",nCurRow-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
//广州增立铁塔部件明细表
void CLDSDoc::AddGuangZhouZengLiPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	PART *pRec;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	//按段号过滤
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();

	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(27);
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[0]="Segment Number";
	str_arr[1]="Block Number";
	str_arr[2]="Material Texture";
	str_arr[3]="Material";
	str_arr[4]="Specification";
	str_arr[5]="Length";
	str_arr[6]="Width";
	str_arr[7]="Thickness";
	str_arr[8]="Piece Amount";
	str_arr[9]="Single Hole";
	str_arr[10]="Notes";
	str_arr[11]="Weld";
	str_arr[12]="Bend";
	str_arr[13]="Leg Cut";
	str_arr[14]="Cut Back";
	str_arr[15]="Cut Root";
	str_arr[16]="Flat";
	str_arr[17]="Open Merge Angle";
	str_arr[18]="Drill Hole";
	str_arr[19]="Slot";
	str_arr[20]="Interset Line";
	str_arr[21]="Open Slot Type";
	str_arr[22]="Type";
	str_arr[23]="Edge Number";
	str_arr[24]="Perimeter";
	str_arr[25]="Superficial Area";
	str_arr[26]="Dif Hole Diameter Number";
#else 
	str_arr[0]="段号";
	str_arr[1]="部件号";
	str_arr[2]="材质";
	str_arr[3]="材料";
	str_arr[4]="规格";
	str_arr[5]="长度";
	str_arr[6]="宽度";
	str_arr[7]="厚度";
	str_arr[8]="单件数量";
	str_arr[9]="单件孔数";
	str_arr[10]="备注";
	str_arr[11]="电焊";
	str_arr[12]="火曲";
	str_arr[13]="切角";
	str_arr[14]="铲背";
	str_arr[15]="清根";
	str_arr[16]="打扁";
	str_arr[17]="开合角";
	str_arr[18]="钻孔";
	str_arr[19]="坡口";
	str_arr[20]="割相贯线";
	str_arr[21]="开槽形式";
	str_arr[22]="类型";
	str_arr[23]="边数";
	str_arr[24]="周长";
	str_arr[25]="表面积";
	str_arr[26]="各孔径孔数";
#endif
	double col_arr[27]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						7.5,2.5,2.5,2.5,2.5,
						2.5,2.5,4.0,2.5,2.5,
						4.0,4.0,4.5,2.5,3.0,
						4.0,8.5};
	AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	i=0;
	long n=mat_tbl.part_recordset.GetNodeNum();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),27);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//编号
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));		//材质
		//材料
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,3,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,3,COleVariant("连板"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("角钢"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("钢管"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("槽钢"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("扁钢"));
#endif
		//规格
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//标注角钢规格∠
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("∠%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));//长度
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));	//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));	//厚度
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));	//单基数量
		map.SetValueAt(index,9,COleVariant((long)pRec->nMSumLs));//单件孔数
		map.SetValueAt(index,10,COleVariant(pRec->sNotes));		//备注
		//是否电焊件
		if(pRec->bWeldPart)
			map.SetValueAt(index,11,COleVariant("1"));
		//火曲
		if(pRec->bZhiWan)
			map.SetValueAt(index,12,COleVariant("1"));
		//切角
		if(pRec->bCutAngle)
			map.SetValueAt(index,13,COleVariant("1"));
		//铲背
		if(pRec->bCutBer)
			map.SetValueAt(index,14,COleVariant("1"));
		//清根
		if(pRec->bCutRoot)
			map.SetValueAt(index,15,COleVariant("1"));
		//打扁
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,16,COleVariant("1"));
		//开合角
		if(pRec->bKaiJiao||pRec->bHeJiao)
			map.SetValueAt(index,17,COleVariant("1"));
		//钻孔
		if(pRec->thick>=14)
			map.SetValueAt(index,18,COleVariant("1"));
		//坡口
		if(pRec->bNeedFillet)
			map.SetValueAt(index,19,COleVariant("1"));
		//割相贯线
		if(pRec->IsTubeTransect())
			map.SetValueAt(index,20,COleVariant("1"));
		//开槽类型
		char sType[100]="";
		int nType=pRec->GetNotchType(sType,0);
		if(nType>0)
			map.SetValueAt(index,21,COleVariant(sType));
		//类型
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"支管");
			else 
				strcpy(sTypeName,"主管");
#endif
		}
		else if(pRec->part_type==STEELTYPE_PLATE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"Shelter Plate");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"Flange");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"U Insert Plate");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"Bottom Flange");
#else 
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"遮水板");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"法兰");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"插板");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"底法兰");
#endif
		}
		map.SetValueAt(index,22,COleVariant(sTypeName));
		//边数
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,23,COleVariant(pRec->nEdgeNum));
		//周长
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,24,COleVariant(pRec->fPerimeter));
		//面积
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,25,COleVariant(pRec->real_area));
		//孔数孔径
		char sHoleStr[500]="";
		if(pRec->GetLsHoleStr(sHoleStr)>0)
			map.SetValueAt(index,26,COleVariant(sHoleStr));
	}
	char cell_end[15]="";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置数字格式
	sprintf(cell_end,"Z%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"Y2",cell_end,COleVariant("0.0"));
	//设置表格内容
	sprintf(cell_end,"AA%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}

//潍坊长安铁塔部件明细表 wht 10-10-11
void CLDSDoc::AddWeiFangChangAnPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	PART *pRec = NULL;
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();

	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(29);
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[0]="Segment Number";
	str_arr[1]="Block Number";
	str_arr[2]="Material Texture";
	str_arr[3]="Material";
	str_arr[4]="Specification";
	str_arr[5]="Length";
	str_arr[6]="Width";
	str_arr[7]="Thickness";
	str_arr[8]="Piece Amount";
	str_arr[9]="Piece Weight";	//新增 10-10-12
	str_arr[10]="Single Hole";
	str_arr[11]="Notes";
	str_arr[12]="Weld";
	str_arr[13]="Bend";
	str_arr[14]="Leg Cut";
	str_arr[15]="Cut Back";
	str_arr[16]="Cut Root";
	str_arr[17]="Flat";
	str_arr[18]="Open Merge Angle";
	str_arr[19]="Drill Hole";
	str_arr[20]="Slot";
	str_arr[21]="Interset Line";
	str_arr[22]="Open Slot Type";
	str_arr[23]="Type";
	str_arr[24]="Edge Number";
	str_arr[25]="Perimeter";
	str_arr[26]="Superficial Area";
	str_arr[27]="Dif Hole Diameter Number";
	str_arr[28]="Weld Edge mm";//新增 10-10-12
#else 
	str_arr[0]="段号";
	str_arr[1]="部件号";
	str_arr[2]="材质";
	str_arr[3]="材料";
	str_arr[4]="规格";
	str_arr[5]="长度";
	str_arr[6]="宽度";
	str_arr[7]="厚度";
	str_arr[8]="单件数量";
	str_arr[9]="单件重量";	//新增 10-10-12
	str_arr[10]="单件孔数";
	str_arr[11]="备注";
	str_arr[12]="电焊";
	str_arr[13]="火曲";
	str_arr[14]="切角";
	str_arr[15]="铲背";
	str_arr[16]="清根";
	str_arr[17]="打扁";
	str_arr[18]="开合角";
	str_arr[19]="钻孔";
	str_arr[20]="坡口";
	str_arr[21]="割相贯线";
	str_arr[22]="开槽形式";
	str_arr[23]="类型";
	str_arr[24]="边数";
	str_arr[25]="周长";
	str_arr[26]="表面积";
	str_arr[27]="各孔径孔数";
	str_arr[28]="焊接边mm";//新增 10-10-12
#endif
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						5.0,7.5,2.5,2.5,2.5,
						2.5,2.5,2.5,4.0,2.5,
						2.5,4.0,4.0,4.5,2.5,
						3.0,4.0,8.5,5.5};
	AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//
	int i=0;
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),29);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//编号
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));		//材质
		//材料
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//钢板
		{
			if(pRec->sub_type==PARAMPLATE_FL)
				map.SetValueAt(index,3,COleVariant("Flange"));
			else if(pRec->sub_type==PARAMPLATE_WATER)
				map.SetValueAt(index,3,COleVariant("Shelter Plate"));
			else if(pRec->sub_type==PLATE_FL)
				map.SetValueAt(index,3,COleVariant("Bottom Flange"));
			else 
				map.SetValueAt(index,3,COleVariant("Plate"));
		}
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//钢板
		{
			if(pRec->sub_type==PARAMPLATE_FL)
				map.SetValueAt(index,3,COleVariant("法兰"));
			else if(pRec->sub_type==PARAMPLATE_WATER)
				map.SetValueAt(index,3,COleVariant("遮水板"));
			else if(pRec->sub_type==PLATE_FL)
				map.SetValueAt(index,3,COleVariant("底法兰"));
			else 
				map.SetValueAt(index,3,COleVariant("钢板"));
		}
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("角钢"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("钢管"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("槽钢"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("扁钢"));
#endif
		//规格
		CString str="";
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//标注角钢规格∠
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("∠%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
		if(CLDSPlate::m_iPlankWeightStatType==2&&pRec->part_type==STEELTYPE_PLATE)	//长安提出钢板宽度应根据<净面积/长度>计算所得 wjh-2014.4.09
			map.SetValueAt(index,5,COleVariant(ftol(pRec->real_area/pRec->wide)));		//宽度
		else
			map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));	//长度
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));		//厚度
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum())));	//单基数量
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		str.Format("%.2f",weight);			//单件重量
		map.SetValueAt(index,9,COleVariant(str));
		str.Format("%d",pRec->nMSumLs);		//单件孔数
		map.SetValueAt(index,10,COleVariant(str));
		char notes[200];
		pRec->GetProcessNotes(notes);
		str.Format("%s %s",notes,pRec->sNotes);//备注
		map.SetValueAt(index,11,COleVariant(str));
		if(pRec->bWeldPart)		//是否电焊件
			map.SetValueAt(index,12,COleVariant("1"));
		if(pRec->bZhiWan)		//火曲
			map.SetValueAt(index,13,COleVariant("1"));
		if(pRec->bCutAngle)		//切角
			map.SetValueAt(index,14,COleVariant("1"));
		if(pRec->bCutBer)		//铲背
			map.SetValueAt(index,15,COleVariant("1"));
		if(pRec->bCutRoot)		//清根
			map.SetValueAt(index,16,COleVariant("1"));
		if(pRec->nPushFlat>0)	//1 表示一端压扁 2 表示两端压扁 
			map.SetValueAt(index,17,COleVariant((long)pRec->nPushFlat));
		if(pRec->bKaiJiao)			//开角
			map.SetValueAt(index,18,COleVariant("1"));
		else if(pRec->bHeJiao)		//合角
			map.SetValueAt(index,18,COleVariant("2"));
		if(pRec->thick>=14)			//钻孔
			map.SetValueAt(index,19,COleVariant("1"));
		if(pRec->bNeedFillet)		//坡口
			map.SetValueAt(index,20,COleVariant("1"));
		if(pRec->IsTubeTransect())	//割相贯线
			map.SetValueAt(index,21,COleVariant("1"));
		//开槽类型
		char sType[100]="";
		int nType=pRec->GetNotchType(sType,1);
		if(nType>0)
			map.SetValueAt(index,22,COleVariant(sType));
		//类型
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"支管");
			else 
				strcpy(sTypeName,"主管");
#endif
		}
		else if(pRec->part_type==STEELTYPE_PLATE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"Shelter Plate");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"Flange");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"U Insert Plate");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"Bottom Flange");
#else 
			if(pRec->sub_type==PARAMPLATE_WATER)
				strcpy(sTypeName,"遮水板");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"法兰");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"插板");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"底法兰");
#endif
		}
		map.SetValueAt(index,23,COleVariant(sTypeName));
		if(pRec->part_type==STEELTYPE_PLATE)	//边数
			map.SetValueAt(index,24,COleVariant((long)pRec->nEdgeNum));
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//周长
			str.Format("%.1f",pRec->fPerimeter);
			map.SetValueAt(index,25,COleVariant(str));
		}
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//面积
			if(CLDSPlate::m_iPlankWeightStatType==2)
				str.Format("%.1f",pRec->real_area);
			else
				str.Format("%.1f",pRec->min_area);
			map.SetValueAt(index,26,COleVariant(str));
		}
			//孔数孔径
		char sHoleStr[500]="";
		if(pRec->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//焊接边mm
			str.Format("%.1f",pRec->fWeldEdgeLen);
			map.SetValueAt(index,28,COleVariant(str));
		}
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AC%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
}

//湖州飞剑
void CLDSDoc::AddHuZhouFeiJianPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	PART *pRec;
	int i=0;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(18);
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[0]="Tower Model";
	str_arr[1]="Segment Number";
	str_arr[2]="Piece Number";
	str_arr[3]="Material";
	str_arr[4]="Material Texture";
	str_arr[5]="Specification";
	str_arr[6]="Length";
	str_arr[7]="Piece Amount";
	str_arr[8]="Total Amount";
	str_arr[9]="Piece Weight";
	str_arr[10]="Total Weight";
#else 
	str_arr[0]="塔型";
	str_arr[1]="段号";
	str_arr[2]="件号";
	str_arr[3]="材料";
	str_arr[4]="材质";
	str_arr[5]="规格";
	str_arr[6]="长度";
	str_arr[7]="单件数量";
	str_arr[8]="总量";
	str_arr[9]="单件重";
	str_arr[10]="总  重";
#endif
	str_arr[11]="φ13.5";
	str_arr[12]="φ17.5";
	str_arr[13]="φ19.5";
	str_arr[14]="φ21.5";
	str_arr[15]="φ25.5";
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[16]="Total Hole Number";
	str_arr[17]="Process";
#else 
	str_arr[16]="总孔数";
	str_arr[17]="工  艺";
#endif
	double col_arr[18]={7.5,4.0,6.0,4.0,4.0,
						8.5,4.5,4.0,2.0,6.0,
						7.5,3.5,3.5,3.5,3.5,
						3.5,4.0,10.0};
	AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	i=0;
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),18);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		map.SetValueAt(index,0,COleVariant(Ta.m_sTaType));		//塔型
		map.SetValueAt(index,1,COleVariant(pRec->iSeg.ToString()));		//段号
		map.SetValueAt(index,2,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));		//件号
		//材料
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,3,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,3,COleVariant("连板"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,3,COleVariant("角钢"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,3,COleVariant("钢管"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,3,COleVariant("槽钢"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,3,COleVariant("扁钢"));
#endif
		//材质
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,4,COleVariant(steelmat));			
		//规格
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//标注角钢规格∠
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("∠%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,5,COleVariant(str));
		map.SetValueAt(index,6,COleVariant(ftol(pRec->length)));//长度
		map.SetValueAt(index,7,COleVariant(pRec->GetPartNum()));		//单基数量
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum()*Ta.m_nTaNum)));//总量
		if(CLDSPlate::m_iPlankWeightStatType<=1)//根据设置输出重量 wht 11-06-01
		{
			str.Format("%8.2f",pRec->fPieceMinWeight);	//单件重
			map.SetValueAt(index,9,COleVariant(str));
			str.Format("%8.2f",pRec->GetPartNum()*pRec->fPieceMinWeight);//总重
			map.SetValueAt(index,10,COleVariant(str));
		}
		else
		{
			str.Format("%8.2f",pRec->fPieceNetWeight);	//单件重
			map.SetValueAt(index,9,COleVariant(str));
			str.Format("%8.2f",pRec->GetPartNum()*pRec->fPieceNetWeight);//总重
			map.SetValueAt(index,10,COleVariant(str));
		}
		int nLsNum12=pRec->GetLsHoleNum(12,-1,FALSE,FALSE);
		int nLsNum16=pRec->GetLsHoleNum(16,-1,FALSE,FALSE);
		int nLsNum18=pRec->GetLsHoleNum(18,-1,FALSE,FALSE);
		int nLsNum20=pRec->GetLsHoleNum(20,-1,FALSE,FALSE);
		int nLsNum24=pRec->GetLsHoleNum(24,-1,FALSE,FALSE);
		if(nLsNum12>0)
			map.SetValueAt(index,11,COleVariant((long)nLsNum12));
		if(nLsNum16>0)
			map.SetValueAt(index,12,COleVariant((long)nLsNum16));
		if(nLsNum18>0)
			map.SetValueAt(index,13,COleVariant((long)nLsNum18));
		if(nLsNum20>0)
			map.SetValueAt(index,14,COleVariant((long)nLsNum20));
		if(nLsNum24>0)
			map.SetValueAt(index,15,COleVariant((long)nLsNum24));
		if(pRec->nMSumLs>0)
			map.SetValueAt(index,16,COleVariant((long)pRec->nMSumLs));
		char sNotes[100]="";
		pRec->GetProcessNotes(sNotes);
		map.SetValueAt(index,17,COleVariant(sNotes));
	}
	
	char cell_end[15]="";
	//设置右对齐
	sprintf(cell_end,"F%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"C2",cell_end,COleVariant((long)2));
	//设置数字格式
	sprintf(cell_end,"K%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"J2",cell_end,COleVariant("0.00"));
	//设置表格内容
	sprintf(cell_end,"R%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}

//浙江永达
void CLDSDoc::AddZheJiangYongDaPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	Range excel_range;
	_Worksheet excel_sheet;
	PART *pRec;

	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("放样原始材料表");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("铁塔部件明细表");
#endif
	}
	// 第一行主标题行
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX Tower Company Tower Raw Material Table");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX Tower Company Tower Block Table");
#else 
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX铁塔有限公司 铁塔原始材料表");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX铁塔有限公司 铁塔零部件明细表");
#endif
	// 第四行子标题行
	FontLegacy font;
	LPDISPATCH pRange;
	pRange = excel_sheet.GetRange(COleVariant("A2"),COleVariant("AA2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	char sText[200];
	CTime t=CTime::GetCurrentTime();
	CString sTime=t.Format("%Y/%m/%d");
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sText,"Contract No：                 Tower Model：%s                Make Date：%s",Ta.m_sTaType,sTime);
#else 
	sprintf(sText,"合同号：                 塔型：%s                制表日期：%s",Ta.m_sTaType,sTime);
#endif
	excel_range.SetValue(COleVariant(sText));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	//
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(27);
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[0]="Part Number";
	str_arr[1]="Material Name";
	str_arr[2]="Material Texture";
	str_arr[3]="Specification";
	str_arr[4]="Size";
	str_arr[5]="Piece Weight";
	str_arr[6]="Single Basic Number";
	str_arr[7]="Basic Number";
	str_arr[8]="Multiply Amount";
	str_arr[9]="Total Weight";
	str_arr[10]="Multiply Total Amount";
#else 
	str_arr[0]="零件编号";
	str_arr[1]="材料名称";
	str_arr[2]="材质";
	str_arr[3]="规格";
	str_arr[4]="尺寸";
	str_arr[5]="单重";
	str_arr[6]="单基数量";
	str_arr[7]="基数";
	str_arr[8]="多基数量";
	str_arr[9]="总  重";
	str_arr[10]="多基总重";
#endif
	str_arr[11]="φ13.5";
	str_arr[12]="φ17.5";
	str_arr[13]="φ19.5";
	str_arr[14]="φ21.5";
	str_arr[15]="φ25.5";
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[16]="Special Hole";
	str_arr[17]="Leg Cut";
	str_arr[18]="Bend";
	str_arr[19]="Cut Back";
	str_arr[20]="Cut Root";
	str_arr[21]="Weld";
	str_arr[22]="Open Angle ";
	str_arr[23]="Merge Angle ";
	str_arr[24]="Flat";
	str_arr[25]="Notes";
	str_arr[26]="Block Number";
#else 
	str_arr[16]="特孔";
	str_arr[17]="切角";
	str_arr[18]="弯曲";
	str_arr[19]="铲背";
	str_arr[20]="清根";
	str_arr[21]="电焊";
	str_arr[22]="开角";
	str_arr[23]="合角";
	str_arr[24]="打扁";
	str_arr[25]="备注";
	str_arr[26]="部件号";
#endif
	double col_arr[27]={7.5,4.0,3.5,8.0,8.5,
						4.5,4.0,2.0,4.0,6.0,
						7.5,4.0,4.0,4.0,4.0,
						4.0,4.0,2.0,2.0,2.0,
						2.0,2.0,2.0,2.0,2.0,
						8.5,8.5};
	AddRowToExcelSheet(excel_sheet,3,str_arr,col_arr,TRUE);
	//
	int i=3;
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),27);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		map.SetValueAt(index,0,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));			//编号
		//材料
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,1,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,1,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,1,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,1,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,1,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//连板
			map.SetValueAt(index,1,COleVariant("连板"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//角钢
			map.SetValueAt(index,1,COleVariant("角钢"));
		else if(pRec->part_type==STEELTYPE_TUBE)//钢管
			map.SetValueAt(index,1,COleVariant("钢管"));
		else if(pRec->part_type==STEELTYPE_SLOT)//槽钢
			map.SetValueAt(index,1,COleVariant("槽钢"));
		else if(pRec->part_type==STEELTYPE_FLAT)//扁钢
			map.SetValueAt(index,1,COleVariant("扁钢"));
#endif
		//材质
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));
		//规格
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//标注角钢规格∠
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("∠%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(str));
		//
		double fPieceWeight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)//根据设置输出重量 wht 11-06-01
			fPieceWeight=pRec->fPieceMinWeight;
		else
			fPieceWeight=pRec->fPieceNetWeight;
		str.Format("%.fx%.f",pRec->wide,pRec->length);//尺寸			
		map.SetValueAt(index,4,COleVariant(str));
		str.Format("%.1f",fPieceWeight);			//单重
		map.SetValueAt(index,5,COleVariant(str));
		map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//单基数量
		map.SetValueAt(index,7,COleVariant((long)Ta.m_nTaNum));	//基数
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum()*Ta.m_nTaNum)));//多基数量
		str.Format("%.2f",pRec->GetPartNum()*fPieceWeight);	//总重
		map.SetValueAt(index,9,COleVariant(str));
		str.Format("%.2f",pRec->GetPartNum()*fPieceWeight*Ta.m_nTaNum);//多基总重
		map.SetValueAt(index,10,COleVariant(str));
		int nLsNum12=pRec->GetLsHoleNum(12,-1,FALSE,FALSE);
		int nLsNum16=pRec->GetLsHoleNum(16,-1,FALSE,FALSE);
		int nLsNum18=pRec->GetLsHoleNum(18,-1,FALSE,FALSE);
		int nLsNum20=pRec->GetLsHoleNum(20,-1,FALSE,FALSE);
		int nLsNum24=pRec->GetLsHoleNum(24,-1,FALSE,FALSE);
		int nLsNumOther=pRec->nMSumLs-nLsNum12-nLsNum16-nLsNum18-nLsNum20-nLsNum24;
		if(nLsNum12>0)
			map.SetValueAt(index,11,COleVariant((long)nLsNum12));
		if(nLsNum16>0)
			map.SetValueAt(index,12,COleVariant((long)nLsNum16));
		if(nLsNum18>0)
			map.SetValueAt(index,13,COleVariant((long)nLsNum18));
		if(nLsNum20>0)
			map.SetValueAt(index,14,COleVariant((long)nLsNum20));
		if(nLsNum24>0)
			map.SetValueAt(index,15,COleVariant((long)nLsNum24));
		if(nLsNumOther)
			map.SetValueAt(index,16,COleVariant((long)nLsNumOther));
		//切角
		if(pRec->bCutAngle)
			map.SetValueAt(index,17,COleVariant("V"));
		//火曲
		if(pRec->bZhiWan)
			map.SetValueAt(index,18,COleVariant("V"));
		//铲背
		if(pRec->bCutBer)
			map.SetValueAt(index,19,COleVariant("V"));
		//清根
		if(pRec->bCutRoot)
			map.SetValueAt(index,20,COleVariant("V"));
		//电焊
		if(pRec->bWeldPart)
			map.SetValueAt(index,21,COleVariant("V"));
		//开角
		if(pRec->bKaiJiao)
			map.SetValueAt(index,22,COleVariant("V"));
		//合角
		if(pRec->bHeJiao)
			map.SetValueAt(index,23,COleVariant("V"));
		//打扁
		if(pRec->nPushFlat)
			map.SetValueAt(index,24,COleVariant("V"));
		//备注
		char sNotes[100]="";
		strcpy(sNotes,pRec->sNotes);
		if(pRec->bNeedFillet)
#ifdef AFX_TARG_ENU_ENGLISH
			strcat(sNotes," Slot");
#else 
			strcat(sNotes," 坡口");
#endif
		//割相贯线
		if(pRec->IsTubeTransect())
#ifdef AFX_TARG_ENU_ENGLISH
			strcat(sNotes," Interset Line");
#else 
			strcat(sNotes," 割相贯线");
#endif
		map.SetValueAt(index,25,COleVariant(sNotes));
	}
	
	char cell_end[15]="";
	//设置左对齐
	sprintf(cell_end,"E%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AA%d",i);
	CExcelOper::SetRangeValue(excel_sheet,"A4",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A3",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}

void CLDSDoc::AddLsSumMatTbl(CTowerType &mat_tbl, LPDISPATCH pSheet,int iStyle)//,BOOL bDispSpecPart/*=FALSE*/)
{
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	LPDISPATCH pRange;

	mat_tbl.SortLs();
	if(iStyle==0)	//按呼高统计样式一(普通塔厂)
	{
		excel_sheet.AttachDispatch(pSheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Bolt Summary Table");
#else 
		excel_sheet.SetName("螺栓汇总表");
#endif
		// 第一行主标题行
#ifdef AFX_TARG_ENU_ENGLISH
		CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX Company Bolt Summary Table",20.0,14.0);
#else 
		CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX公司铁塔螺栓汇总表",20.0,14.0);
#endif
		//
		int index=0;
		CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,9);
		// 第二行子标题行
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
		map.SetValueAt(index,0,COleVariant("塔型"));
#endif
		// 填写塔型数据
		map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
		CExcelOper::MergeColRange(excel_sheet,"B2","C2");
		//工程名称
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
		map.SetValueAt(index,3,COleVariant("工程名称"));
#endif
		// 填写工程名称数据
		map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
		CExcelOper::MergeColRange(excel_sheet,"E2","F2");
		//
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,6,COleVariant("Special Bolt Amount"));
#else 
		map.SetValueAt(index,6,COleVariant("其中特殊螺栓数量"));
#endif
		CExcelOper::MergeColRange(excel_sheet,"G2","I2");
		index++;
		//第三行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Sign Module"));
		map.SetValueAt(index,1,COleVariant("Basic Number"));
		map.SetValueAt(index,2,COleVariant("Grade Specification"));
		map.SetValueAt(index,3,COleVariant("L0"));
		map.SetValueAt(index,4,COleVariant("Single Weight"));
		map.SetValueAt(index,5,COleVariant("Single Basic Number"));
		map.SetValueAt(index,6,COleVariant("Double-nut Bolt"));
		map.SetValueAt(index,7,COleVariant("Anti-Theft Bolt"));
		map.SetValueAt(index,8,COleVariant("Spring Cushion"));
#else 
		map.SetValueAt(index,0,COleVariant("标志呼高"));
		map.SetValueAt(index,1,COleVariant("基数"));
		map.SetValueAt(index,2,COleVariant("级别规格"));
		map.SetValueAt(index,3,COleVariant("穿过厚度"));
		map.SetValueAt(index,4,COleVariant("单基重量"));
		map.SetValueAt(index,5,COleVariant("单基数量"));
		map.SetValueAt(index,6,COleVariant("双帽螺栓"));
		map.SetValueAt(index,7,COleVariant("防盗螺栓"));
		map.SetValueAt(index,8,COleVariant("弹簧垫"));
#endif
		index++;
		//设置列宽
		CExcelOper::SetRangeColWidth(excel_sheet,7.5,"A3","I3");
		CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B3",NULL,TRUE);
		CExcelOper::SetRangeColWidth(excel_sheet,10,"C3",NULL,TRUE);
		//
		int i=2;
		long module_start_i=-1,module_start_no=-1,n=mat_tbl.ls_recordset.GetNodeNum();
		CLsRecord *pLsRec,*pPrevLsRec=NULL;
		for(pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext(),index++)
		{
			i++;
			if(pLsRec)
			{
				if(module_start_i==-1)
				{
					module_start_i=i+1;
					module_start_no=pLsRec->iModuleNo;
					//合并后的单元格内容，仅需设置第一个单元格内容即可
					map.SetValueAt(index,0,COleVariant(pLsRec->sModule));
					map.SetValueAt(index,1,COleVariant("1"));
				}
				else if(pLsRec->iModuleNo!=module_start_no||i-2==n)
				{
					sprintf(cell_start,"A%d",module_start_i);
					if(i-2==n)
						sprintf(cell_end,"A%d",i+1);
					else
						sprintf(cell_end,"A%d",i);
					//合并列
					CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
					module_start_i=i+1;
					module_start_no=pLsRec->iModuleNo;
					cell_start[0]=cell_end[0]='B';
					//
					map.SetValueAt(index,0,COleVariant(pLsRec->sModule));
					map.SetValueAt(index,1,COleVariant("1"));
					//合并列
					CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
				}
				pPrevLsRec=pLsRec;
			}
			//规格
			char guige[20];
			if(pLsRec->typels0_pad1==0)
				sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
			else
				sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
			map.SetValueAt(index,2,COleVariant(guige));
			//穿过厚度;
			char ss[8];
			sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,3,COleVariant(ss));
			//单基重量
			map.SetValueAt(index,4,COleVariant(pLsRec->sum_weight));
			//单基数
			map.SetValueAt(index,5,COleVariant(pLsRec->GetPartNum()));
			//双帽数
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,6,COleVariant(pLsRec->nTwoCap));
			//防盗数量
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,7,COleVariant(pLsRec->nBurglarproof));
			//弹簧垫数量
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,8,COleVariant(pLsRec->nSpringPad));
		}
		//
		//设置数字格式
		strcpy(cell_start,"E4");
		sprintf(cell_end,"E%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,cell_start,cell_end,COleVariant("0.0"));
		//设置表格内容
		sprintf(cell_end,"I%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
		//设置边框
		CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
		excel_sheet.DetachDispatch();
	}
	else if(iStyle==1)	//按呼高统计样式一(甘肃天水铁塔厂)
	{
		excel_sheet.AttachDispatch(pSheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Bolt Summary Table");
#else 
		excel_sheet.SetName("螺栓汇总表");
#endif
		int i=1,num=0;
		int nCommonLs,nTwoCapLs,nCommonNail,nBurglarLs,nBurglarNail,nPad;
		int iCommonLs,iTwoCapLs,iCommonNail,iBurglarLs,iBurglarNail,iPad;
		int iCommonLsStart,iTwoCapLsStart,iCommonNailStart,iBurglarLsStart,iBurglarNailStart,iPadStart;
		long module_start_i=-1,module_start_no=-1,n=mat_tbl.ls_recordset.GetNodeNum();
		int iWear;//用来表示是垫圈的耗损率还是其它的耗损率
		int iBoltDNum=0;//用来记录防盗螺栓的规格的数量
		int boltDArr[10];//记录防盗螺栓每种数量的数组
		int iNutStart=0;
		bool bIsFirst=true;//是不是第一次进入防盗螺栓的添加区
		CLsRecord *pLsRec,*pPrevLsRec=NULL;
		BOOL bPrintTitle = FALSE;
		for(pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
		{
			if(module_start_i==-1||pLsRec->iModuleNo!=module_start_no)
			{	//刚处理第一个模型呼高,遇到一个新模型呼高
				if(module_start_i!=-1)
					i+=2;	//后续的呼高模型必须与前一模型隔开一行
				module_start_i=i;
				module_start_no=pLsRec->iModuleNo;
				bPrintTitle = TRUE;
			}
			if(bPrintTitle)
			{
				bPrintTitle = FALSE;	//保证只打印一次标题
				// 第一行主标题行
				sprintf(cell_start,"C%d",i);
				sprintf(cell_end,"M%d",i+1);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Firmware Statistic Table"));
#else 
				excel_range.SetValue(COleVariant("紧固件统计表"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetBold(covTrue);
				font.SetSize(COleVariant(14.0));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				pRange->Release();
				
				// 第二行子标题行
				//工程名称
				sprintf(cell_start,"A%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Project Name："));
#else 
				excel_range.SetValue(COleVariant("工程名称："));
#endif
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//呼高
				sprintf(cell_start,"N%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Module："));
#else 
				excel_range.SetValue(COleVariant("呼高："));
#endif
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//填写模型呼高数据
				sprintf(cell_start,"O%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
				excel_range.SetValue(COleVariant(pLsRec->sModule));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//第三行子标题行
				sprintf(cell_start,"A%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Tower Model："));
#else 
				excel_range.SetValue(COleVariant("塔    型："));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				// 填写塔型数据
				sprintf(cell_start,"B%d",i+2);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(Ta.m_sTaType));
				excel_range.DetachDispatch();
				pRange->Release();
				//基数
				sprintf(cell_start,"N%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Basic Number："));
#else 
				excel_range.SetValue(COleVariant("基数："));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				//填写基数
				sprintf(cell_start,"O%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
				excel_range.SetValue(COleVariant((short)1));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				
				sprintf(cell_start,"C%d",i+2);
				sprintf(cell_end,"M%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
				char ss[20];
				CTime t;
				t = CTime::GetCurrentTime();
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(ss,"%s",t.Format("%Y Year %m Month %d Day"));
#else 
				sprintf(ss,"%s",t.Format("%Y年 %m月 %d日"));
#endif
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//第四行数据项标题行，需要格式输出
				//紧固件名称
				sprintf(cell_start,"A%d",i+3);
				sprintf(cell_end,"A%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Name"));
#else 
				excel_range.SetValue(COleVariant("名称"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//级别规格
				sprintf(cell_start,"B%d",i+3);
				sprintf(cell_end,"B%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Grade Specification"));
#else 
				excel_range.SetValue(COleVariant("级别规格"));
#endif
				excel_range.SetColumnWidth(COleVariant(10.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				sprintf(cell_start,"C%d",i+3);
				sprintf(cell_end,"C%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("L0"));
#else 
				excel_range.SetValue(COleVariant("通过厚度"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//设计标题
				sprintf(cell_start,"D%d",i+3);
				sprintf(cell_end,"G%d",i+4);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("         Design      "));
#else 
				excel_range.SetValue(COleVariant("设                 计"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//损耗标题
				sprintf(cell_start,"H%d",i+3);
				sprintf(cell_end,"I%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Wastage（x%）"));
#else 
				excel_range.SetValue(COleVariant("损    耗（x%）"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//其他损耗
				sprintf(cell_start,"J%d",i+3);
				sprintf(cell_end,"J%d",i+3);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Others"));
#else 
				excel_range.SetValue(COleVariant("其他"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//垫圈损耗
				sprintf(cell_start,"J%d",i+4);
				sprintf(cell_end,"J%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Washer"));
#else 
				excel_range.SetValue(COleVariant("垫圈"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//填写损耗数据损耗标题
				sprintf(cell_start,"K%d",i+3);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant("3%"));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				borders.AttachDispatch(excel_range.GetBorders());
				//borders.SetValue(COleVariant((long)1));	//xlSingle
				excel_range.DetachDispatch();
				pRange->Release();
				sprintf(cell_start,"K%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant("3%"));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				borders.AttachDispatch(excel_range.GetBorders());
				//borders.SetValue(COleVariant((long)1));	//xlSingle
				excel_range.DetachDispatch();
				pRange->Release();
				//设计加损耗标题
				sprintf(cell_start,"L%d",i+3);
				sprintf(cell_end,"O%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Design  And   Wastage"));
#else 
				excel_range.SetValue(COleVariant("设   计   加   损   耗"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//第六行数据项标题行，需要格式输出
				//数量
				sprintf(cell_start,"D%d",i+5);
				sprintf(cell_end,"E%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount "));
#else 
				excel_range.SetValue(COleVariant("数      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//重量
				sprintf(cell_start,"F%d",i+5);
				sprintf(cell_end,"G%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("重      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//数量
				sprintf(cell_start,"H%d",i+5);
				sprintf(cell_end,"I%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount  "));
#else 
				excel_range.SetValue(COleVariant("数      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//重量
				sprintf(cell_start,"J%d",i+5);
				sprintf(cell_end,"K%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("重      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//数量
				sprintf(cell_start,"L%d",i+5);
				sprintf(cell_end,"M%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount  "));
#else 
				excel_range.SetValue(COleVariant("数      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//重量
				sprintf(cell_start,"N%d",i+5);
				sprintf(cell_end,"O%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("重      量"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//第七行数据项标题行，需要格式输出
				//单基
				sprintf(cell_start,"D%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"E%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//单基
				sprintf(cell_start,"F%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"G%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//单基
				sprintf(cell_start,"H%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"I%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//单基
				sprintf(cell_start,"J%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"K%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//单基
				sprintf(cell_start,"L%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"M%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//单基
				sprintf(cell_start,"N%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("单基"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//总计
				sprintf(cell_start,"O%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("总计"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				i+=7;
				//统计当前模型呼高内各种紧固件的数量及表格显示位置
				mat_tbl.ls_recordset.push_stack();
				nCommonLs=nTwoCapLs=nCommonNail=nBurglarLs=nBurglarNail=nPad=0;
				CLsRecord *pTmPrevLsRec=NULL;//用来临时记录上一个构件
				int iTmBoltNum=0;//用来临时记录当前规格防盗螺栓的数量
				for(CLsRecord *pTmLsRec=mat_tbl.ls_recordset.GetFirst();pTmLsRec;pTmLsRec=mat_tbl.ls_recordset.GetNext())
				{
					if(pTmLsRec->iModuleNo!=pLsRec->iModuleNo)
						continue;	//非当前模型呼高
					if(pTmLsRec->typels0_pad1==1)	//垫片
						nPad++;
					else	//螺栓
					{
						if(pTmLsRec->GetTwoCapLsCount()>0)	//双帽螺栓
							nTwoCapLs++;
						if(pTmLsRec->GetCommonNailCount()>0)
							nCommonNail++;		//普通脚钉
						if(pTmLsRec->GetBurglarproofLsCount()>0)
						{
							nBurglarLs++;		//普通防盗螺栓
							if(pTmPrevLsRec!=NULL&&pTmLsRec->d!=pTmPrevLsRec->d)
							{	//每一个直径的螺栓统计结束都把螺栓统计加一,预留螺母小计区
								boltDArr[iBoltDNum]=iTmBoltNum;
								nBurglarLs++;
								iBoltDNum++;
								iTmBoltNum=0;
							}
							iTmBoltNum++;
							pTmPrevLsRec=pTmLsRec;
						}
						if(pTmLsRec->GetBurglarproofNailCount()>0)
							nBurglarNail++;		//防盗脚钉
						if(pTmLsRec->GetCommonLsCount()>0)
							nCommonLs++;		//普通螺栓
					}
				}
				mat_tbl.ls_recordset.pop_stack();
				iCommonLs=iCommonLsStart=i;
				iTwoCapLs=iTwoCapLsStart=i+nCommonLs;
				if(nCommonLs>0)	//留出小计行
				{
					iTwoCapLs++;
					iTwoCapLsStart++;
				}
				iCommonNail=iCommonNailStart=iTwoCapLs+nTwoCapLs;
				if(nTwoCapLs>0)	//留出小计行
				{
					iCommonNail++;
					iCommonNailStart++;
				}
				iBurglarLs=iBurglarLsStart=iCommonNail+nCommonNail;
				if(nCommonNail>0)
				{
					iBurglarLs++;
					iBurglarLsStart++;
				}
				iBurglarNail=iBurglarNailStart=iBurglarLs+nBurglarLs;
				if(nBurglarLs>0)
				{
					iBurglarNail++;
					iBurglarNailStart++;
					//为最后一个防盗螺栓的螺母留出小计行
					boltDArr[iBoltDNum]=iTmBoltNum;
					nBurglarLs++;
					iBoltDNum++;
					iTmBoltNum=0;
					iBurglarNail++;
					iBurglarNailStart++;
				}
				iPad=iPadStart=iBurglarNail+nBurglarNail;
				if(nBurglarNail>0)
				{
					iPad++;
					iPadStart++;
				}
				for(int j=i;j<=iPad+nPad;j++)
				{
					char formula[20],sum_cell[20];
					if(nPad==0&&j==iPad)
						continue;	//垫片总数为零不需要小计行
					if(j==iCommonLsStart+nCommonLs&&nCommonLs>0)
					{	//螺栓小计区
						sprintf(sum_cell,"F%d",iCommonLsStart+nCommonLs);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)",iCommonLsStart,iCommonLsStart+nCommonLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
					}
					else if(j==iTwoCapLs+nTwoCapLs&&nTwoCapLs>0)
					{	//双帽螺栓小计区
						sprintf(sum_cell,"F%d",iTwoCapLs+nTwoCapLs);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)",iTwoCapLs,iTwoCapLs+nTwoCapLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
					}
					else if(j==iCommonNail+nCommonNail&&nCommonNail>0)
					{	//普通脚钉小计区
						sprintf(sum_cell,"F%d",iCommonNail+nCommonNail);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)",iCommonNail,iCommonNail+nCommonNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
					}
					else if(j==iBurglarLs+nBurglarLs&&nBurglarLs>0)
					{	//防盗螺栓小计区
						sprintf(sum_cell,"F%d",iBurglarLs+nBurglarLs);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)/2",iBurglarLs,iBurglarLs+nBurglarLs-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						//防盗螺母小计区
						for(num=0;num<iBoltDNum;num++)
						{
							if(num==0)
								iNutStart+=iBurglarLs;
							else
								iNutStart+=boltDArr[num-1]+num;
							sprintf(cell_start,"B%d",iNutStart+boltDArr[num]);
							sprintf(cell_end,"C%d",iNutStart+boltDArr[num]);
							pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
							excel_range.AttachDispatch(pRange,FALSE);
							excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
							excel_range.SetValue(COleVariant("Nut Statistic"));
#else 
							excel_range.SetValue(COleVariant("螺 母 小 计"));
#endif
							excel_range.SetHorizontalAlignment(COleVariant((short)3));
							excel_range.SetVerticalAlignment(COleVariant((short)2));
							excel_range.DetachDispatch();
							pRange->Release();
							sprintf(sum_cell,"D%d",iNutStart+boltDArr[num]);
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(D%d:D%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='E';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(E%d:E%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='F';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(F%d:F%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='G';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(G%d:G%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='J';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(J%d:J%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='K';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(K%d:K%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='N';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(N%d:N%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
							sum_cell[0]='O';
							pRange = excel_sheet.GetRange(COleVariant(sum_cell));
							excel_range.AttachDispatch(pRange,FALSE);
							sprintf(formula,"=SUM(O%d:O%d)",iNutStart,iNutStart+boltDArr[num]-1);
							excel_range.SetFormula(COleVariant(formula));
							excel_range.ReleaseDispatch();
						}
					}
					else if(j==iBurglarNail+nBurglarNail&&nBurglarNail>0)
					{	//防盗脚钉小计区
						sprintf(sum_cell,"F%d",iBurglarNail+nBurglarNail);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)",iBurglarNail,iBurglarNail+nBurglarNail-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
					}
					else if(j==iPad+nPad&&nPad>0)
					{	//垫圈小计区
						sprintf(sum_cell,"F%d",iPad+nPad);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(F%d:F%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='G';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(G%d:G%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='J';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(J%d:J%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='K';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(K%d:K%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='N';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(N%d:N%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
						sum_cell[0]='O';
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						sprintf(formula,"=SUM(O%d:O%d)",iPad,iPad+nPad-1);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.ReleaseDispatch();
					}
					else
					{
						//设计总计数量
						sprintf(formula,"=D%d*O%d",j,i-5);
						sprintf(sum_cell,"E%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计单基重量
						sprintf(sum_cell,"F%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计总计重量
						sprintf(formula,"=F%d*O%d",j,i-5);
						sprintf(sum_cell,"G%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						if(iPadStart<=j&&j<=(nPad+iPadStart))
							iWear=i-3;
						else
							iWear=i-4;
						//损耗单基数量
						sprintf(formula,"=D%d*K%d",j,iWear);
						sprintf(sum_cell,"H%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//损耗总计数量
						sprintf(formula,"=E%d*K%d",j,iWear);
						sprintf(sum_cell,"I%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//损耗单基重量
						sprintf(formula,"=F%d*K%d",j,iWear);
						sprintf(sum_cell,"J%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//损耗总计重量
						sprintf(formula,"=G%d*K%d",j,iWear);
						sprintf(sum_cell,"K%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计加损耗单基数量
						sprintf(formula,"=D%d+H%d",j,j);
						sprintf(sum_cell,"L%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计加损耗总计数量
						sprintf(formula,"=E%d+I%d",j,j);
						sprintf(sum_cell,"M%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计加损耗单基重量
						sprintf(formula,"=F%d+J%d",j,j);
						sprintf(sum_cell,"N%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//设计加损耗总计重量
						sprintf(formula,"=G%d+K%d",j,j);
						sprintf(sum_cell,"O%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
					}
				}
				i+=2+nCommonLs+nTwoCapLs+nCommonNail+nBurglarLs+nPad;
				//划定边框显示范围
				sprintf(cell_start,"A%d",module_start_i+3);
				if(nPad>0)
					sprintf(cell_end,"O%d",iPad+nPad);
				else
					sprintf(cell_end,"O%d",iPad+nPad-1);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetRowHeight(COleVariant(20.0));
				borders.AttachDispatch(excel_range.GetBorders(),FALSE);
				borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
				borders.SetLineStyle(COleVariant((long)1));	//xlSingle
				borders.ReleaseDispatch();
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				//普通螺栓区
				if(nCommonLs>0)
				{
					sprintf(cell_start,"A%d",iCommonLsStart);
					if(nCommonLs>1)
					{
						sprintf(cell_end,"A%d",iCommonLsStart+nCommonLs-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nCommonLs>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Ordinary Bolt"));
#else 
					excel_range.SetValue(COleVariant("普通螺栓"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iCommonLsStart+nCommonLs);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//双帽螺栓区
				if(nTwoCapLs>0)
				{
					sprintf(cell_start,"A%d",iTwoCapLsStart);
					if(nTwoCapLs>1)
					{
						sprintf(cell_end,"A%d",iTwoCapLsStart+nTwoCapLs-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nTwoCapLs>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Double-nut Bolt"));
#else 
					excel_range.SetValue(COleVariant("双帽螺栓"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iTwoCapLsStart+nTwoCapLs);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//普通脚钉区
				if(nCommonNail>0)
				{
					sprintf(cell_start,"A%d",iCommonNailStart);
					if(nCommonNail>1)
					{
						sprintf(cell_end,"A%d",iCommonNailStart+nCommonNail-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nCommonNail>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Ordinary Footnail"));
#else 
					excel_range.SetValue(COleVariant("普通脚钉"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iCommonNailStart+nCommonNail);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//防盗螺栓区
				if(nBurglarLs>0)
				{
					sprintf(cell_start,"A%d",iBurglarLsStart);
					if(nBurglarLs>1)
					{
						sprintf(cell_end,"A%d",iBurglarLsStart+nBurglarLs-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nBurglarLs>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Anti-Theft Bolt"));
#else 
					excel_range.SetValue(COleVariant("防盗螺栓"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iBurglarLsStart+nBurglarLs);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//防盗脚钉区
				if(nBurglarNail>0)
				{
					sprintf(cell_start,"A%d",iBurglarNailStart);
					if(nBurglarNail>1)
					{
						sprintf(cell_end,"A%d",iBurglarNailStart+nBurglarNail-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nBurglarNail>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Anti-Theft Footnail"));
#else 
					excel_range.SetValue(COleVariant("防盗脚钉"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iBurglarNailStart+nBurglarNail);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//垫片区
				if(nPad>0)
				{
					sprintf(cell_start,"A%d",iPadStart);
					if(nPad>1)
					{
						sprintf(cell_end,"A%d",iPadStart+nPad-1);
						pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
					}
					else 
						pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(nPad>1)
						excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Gasket"));
#else 
					excel_range.SetValue(COleVariant("垫片"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
					sprintf(cell_start,"A%d",iPadStart+nPad);
					pRange = excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Subtotal"));
#else 
					excel_range.SetValue(COleVariant("小计"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
			}

			if(pLsRec->typels0_pad1==1)
			{		//垫片
				sprintf(cell_start,"B%d",iPad);	//垫圈规格
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				char guige[20];
				sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
				excel_range.SetValue(COleVariant(guige));
				excel_range.DetachDispatch();
				pRange->Release();
				cell_start[0]='D';	//单基数量
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(pLsRec->GetPartNum()));
				excel_range.DetachDispatch();
				pRange->Release();
				cell_start[0]='F';	//单基重量
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(pLsRec->sum_weight));
				excel_range.SetNumberFormat(COleVariant("0.00"));
				excel_range.DetachDispatch();
				pRange->Release();
				iPad++;
			}
			else 	//螺栓
			{
				if(pLsRec->GetTwoCapLsCount()>0)
				{	//只要存在双帽就归为双帽，不管是否有其它属性
					sprintf(cell_start,"B%d",iTwoCapLs);	//螺栓级别规格
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//穿过厚度
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//单基数量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetTwoCapLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//单基重量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(pLsRec->GetTwoCapLsCount()*pLsRec->GetUnitWeight()));
					excel_range.SetNumberFormat(COleVariant("0.00"));
					excel_range.DetachDispatch();
					pRange->Release();
					iTwoCapLs++;
				}
				if(pLsRec->GetCommonNailCount()>0)
				{	//普通脚钉
					sprintf(cell_start,"B%d",iCommonNail);	//螺栓级别规格
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//单基数量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetCommonNailCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//单基重量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(pLsRec->GetCommonNailCount()*pLsRec->GetUnitWeight()));
					excel_range.SetNumberFormat(COleVariant("0.00"));
					excel_range.DetachDispatch();
					pRange->Release();
					iCommonNail++;
				}
				if(pLsRec->GetBurglarproofLsCount()>0)
				{	//普通防盗螺栓
					if(pPrevLsRec!=NULL&&pLsRec->d!=pPrevLsRec->d&&!bIsFirst)
						iBurglarLs++;
					bIsFirst=false;
					sprintf(cell_start,"B%d",iBurglarLs);	//螺栓级别规格
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//穿过厚度
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//单基数量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//单基重量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofLsCount()*pLsRec->GetUnitWeight(2)));
					excel_range.SetNumberFormat(COleVariant("0.00"));
					excel_range.DetachDispatch();
					pRange->Release();
					iBurglarLs++;
				}
				if(pLsRec->GetBurglarproofNailCount()>0)
				{		//防盗脚钉
					sprintf(cell_start,"B%d",iBurglarNail);	//螺栓级别规格
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//穿过厚度
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//单基数量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofNailCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//单基重量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofNailCount()*pLsRec->GetUnitWeight(2)));
					excel_range.SetNumberFormat(COleVariant("0.00"));
					excel_range.DetachDispatch();
					pRange->Release();
					iBurglarNail++;
				}
				if(pLsRec->GetCommonLsCount()>0)
				{		//普通螺栓
					sprintf(cell_start,"B%d",iCommonLs);	//螺栓级别规格
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//穿过厚度
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//单基数量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetCommonLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//单基重量
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(pLsRec->GetCommonLsCount()*pLsRec->GetUnitWeight()));
					excel_range.SetNumberFormat(COleVariant("0.00"));
					excel_range.DetachDispatch();
					pRange->Release();
					iCommonLs++;
				}
			}
			pPrevLsRec=pLsRec;
		}
		excel_sheet.DetachDispatch();
	}
	else if(iStyle==2)
		AddLsSumMatTblBySeg(mat_tbl,pSheet);
}

void CLDSDoc::AddPoleTblSheet(CTowerType &mat_tbl,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
    excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Sample Rod table");
#else 
	excel_sheet.SetName("样杆表");
#endif
	//添加表头
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","Product Material Table");
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","生产材料表");
#endif
	
	//统计行数
	int i=0,nRowNum=0;
	for(i=0;i<mat_tbl.part_recordset.GetNodeNum();i++)
	{
		PART *pRec=&mat_tbl.part_recordset[i];
		if(pRec->part_type!=STEELTYPE_ANGLE)
			continue;
		nRowNum++;
	}
	int index=0;
	CVariant2dArray map(nRowNum+2,12);
	//第二行标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	map.SetValueAt(index,3,COleVariant("Task No."));
	CExcelOper::MergeColRange(excel_sheet,"D2","E2");
	map.SetValueAt(index,6,COleVariant("Tower Height"));
	map.SetValueAt(index,8,COleVariant("Basic Number"));
	map.SetValueAt(index,9,COleVariant((long)Ta.m_nTaNum));
	map.SetValueAt(index,10,COleVariant("User"));
#else 
	map.SetValueAt(index,0,COleVariant("塔型"));
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	map.SetValueAt(index,3,COleVariant("任务单号"));
	CExcelOper::MergeColRange(excel_sheet,"D2","E2");
	map.SetValueAt(index,6,COleVariant("塔高"));
	map.SetValueAt(index,8,COleVariant("基数"));
	map.SetValueAt(index,9,COleVariant((long)Ta.m_nTaNum));
	map.SetValueAt(index,10,COleVariant("用户"));
#endif
	index++;
	// 第三行标题行
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"B3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"C3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"D3","E3");
	CExcelOper::SetRangeColWidth(excel_sheet,21.0,"F3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"G3","J3");
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"K3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,15.0,"L3",NULL);
	//
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Number"));
	map.SetValueAt(index,1,COleVariant("Specification"));
	map.SetValueAt(index,2,COleVariant("Length"));
	map.SetValueAt(index,3,COleVariant("Single Basic Number"));
	map.SetValueAt(index,4,COleVariant("Total Number"));
	map.SetValueAt(index,5,COleVariant("Sample Rod Data"));
	map.SetValueAt(index,6,COleVariant("Hole Count"));
	map.SetValueAt(index,7,COleVariant("Laying-off"));
	map.SetValueAt(index,8,COleVariant("Number Material"));
	map.SetValueAt(index,9,COleVariant("Punch"));
	map.SetValueAt(index,10,COleVariant("Complete Date"));
	map.SetValueAt(index,11,COleVariant("Notes"));
#else 
	map.SetValueAt(index,0,COleVariant("编号"));
	map.SetValueAt(index,1,COleVariant("规格"));
	map.SetValueAt(index,2,COleVariant("长度"));
	map.SetValueAt(index,3,COleVariant("单基数"));
	map.SetValueAt(index,4,COleVariant("总件数"));
	map.SetValueAt(index,5,COleVariant("样杆数据"));
	map.SetValueAt(index,6,COleVariant("孔数"));
	map.SetValueAt(index,7,COleVariant("下料"));
	map.SetValueAt(index,8,COleVariant("号料"));
	map.SetValueAt(index,9,COleVariant("冲孔"));
	map.SetValueAt(index,10,COleVariant("完成日期"));
	map.SetValueAt(index,11,COleVariant("备注"));
#endif
	index++;
	//
	int iRow=4;
	char ss[100]="";
	for(i=0;i<mat_tbl.part_recordset.GetNodeNum();i++)
	{
		PART *pRec=&mat_tbl.part_recordset[i];
		if(pRec->part_type!=STEELTYPE_ANGLE)
			continue;
		map.SetValueAt(index,0,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//编号
		sprintf(ss,"%s%C",(char*)pRec->sSpec,pRec->cMaterial);		//规格
		map.SetValueAt(index,1,COleVariant(ss));
		sprintf(ss,"%.2f",pRec->length);					//长度
		map.SetValueAt(index,2,COleVariant(ss));
		map.SetValueAt(index,3,COleVariant((long)pRec->GetPartNum()));	//单基数
		sprintf(ss,"=D%d*J2",iRow);	
		map.SetValueAt(index,4,COleVariant(ss));			//总件数
		//螺栓字符串
		int str_len=pRec->GetLsStr(NULL);
		char *ls_str=new char[str_len+1];
		pRec->GetLsStr(ls_str);
		map.SetValueAt(index,5,COleVariant(ls_str));
		delete []ls_str;
		map.SetValueAt(index,6,COleVariant(pRec->nMSumLs));	//孔数
		//备注
		char notes[200];
		pRec->GetProcessNotes(notes);
		sprintf(notes,"%s %s",notes,pRec->sNotes);
		map.SetValueAt(index,11,COleVariant(notes));
		iRow++;
		index++;
	}
	char cell_end[15]="";
	//设置自动换行
	sprintf(cell_end,"F%d",iRow-1);
	CExcelOper::SetRangeWrapText(excel_sheet,"F1",cell_end,COleVariant((short)TRUE));
	//
	sprintf(cell_end,"L%d",iRow-1);
	//设置竖直对齐方式 居中
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)-4108));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}

void CLDSDoc::AddPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet,BOOL bDispSpecPart/*=FALSE*/)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("组焊件明细表");
#endif
	mat_tbl.SortLs();
	// 第一行主标题行
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","Group Weld Part Table");
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","组焊件明细表");
#endif
	//统计行数
	int nRowNum=0;
	CStatWeldPart *pPartGroupRec=NULL;
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext())
	{
		if(pPartGroupRec->GetPartRefArrPtr())
			nRowNum += pPartGroupRec->GetPartRefArrPtr()->GetNodeNum();
		nRowNum++;
	}
	int index=0;
	CVariant2dArray map(nRowNum+2,12);
	// 第二行子标题行
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("塔型"));
#endif
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("工程名称"));
#endif
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));	// 填写工程名称数据
	CExcelOper::MergeColRange(excel_sheet,"E2","H2");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,8,COleVariant("Module"));
	map.SetValueAt(index,9,COleVariant("Basic Number"));
#else 
	map.SetValueAt(index,8,COleVariant("呼高"));
	map.SetValueAt(index,9,COleVariant("基数"));
#endif
	map.SetValueAt(index,10,COleVariant((short)1));	// 填写基数数据
	index++;
	//第三行数据项标题行，需要格式输出
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"B3","C3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"D3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"E3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"F3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"G3","H3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"I3","K3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"L3",NULL,TRUE);
	// 标题
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Group Weld Number"));
	map.SetValueAt(index,1,COleVariant("Piece Number"));
	map.SetValueAt(index,2,COleVariant("Material Texture"));
	map.SetValueAt(index,3,COleVariant("Specification"));
	map.SetValueAt(index,4,COleVariant("Material Length"));
	map.SetValueAt(index,5,COleVariant("Piece Amount"));
	map.SetValueAt(index,6,COleVariant("Piece Weight"));
	map.SetValueAt(index,7,COleVariant("Total Weight"));
	CExcelOper::MergeColRange(excel_sheet,"I3","K3");
	map.SetValueAt(index,8,COleVariant("Notes"));		
	map.SetValueAt(index,11,COleVariant("Group Weld Amount"));	
#else 
	map.SetValueAt(index,0,COleVariant("组焊编号"));
	map.SetValueAt(index,1,COleVariant("件号"));
	map.SetValueAt(index,2,COleVariant("材质"));
	map.SetValueAt(index,3,COleVariant("规格"));
	map.SetValueAt(index,4,COleVariant("料长"));
	map.SetValueAt(index,5,COleVariant("件数"));
	map.SetValueAt(index,6,COleVariant("单重"));
	map.SetValueAt(index,7,COleVariant("总重"));
	CExcelOper::MergeColRange(excel_sheet,"I3","K3");
	map.SetValueAt(index,8,COleVariant("备注"));		
	map.SetValueAt(index,11,COleVariant("组焊数量"));	
#endif
	index++;
	//
	int i=4;
	char cell_start[16]="",cell_end[16]="",sText[100]="";
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext(),index++)
	{
		//组焊件编号
		sprintf(cell_start,"A%d",i);
		sprintf(cell_end,"A%d",i+pPartGroupRec->GetPartRefArrPtr()->GetNodeNum());
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,0,COleVariant(pPartGroupRec->sPartNo));
		//因组焊件内不一定都是同一种材质，因此组焊件列表中组焊编号不显示材质字符 wjh-2013-5-20
		//PART *pPart=mat_tbl.GetPartFromPartNo(pPartGroupRec->sPartNo);
		//map.SetValueAt(index,0,COleVariant(pPart->GetPartNo(g_sysPara.m_nMatCharPosType)));
		//设置左对齐
		sprintf(cell_end,"B%d",i);
		sprintf(cell_end,"B%d",i+pPartGroupRec->GetPartRefArrPtr()->GetNodeNum()-1);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)2));
		//组焊数量
		sprintf(cell_start,"L%d",i);
		sprintf(cell_end,"L%d",i+pPartGroupRec->GetPartRefArrPtr()->GetNodeNum());
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,11,COleVariant(pPartGroupRec->num));
		//
		PARTREF *pPartRef=NULL;
		double sum_weight=0;
		for(pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetFirst();pPartRef;pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetNext(),index++)
		{
			i++;
			PART *pPart=mat_tbl.GetPartFromPartNo(pPartRef->sPartNo);
			if(pPart==NULL)
				continue;
			map.SetValueAt(index,1,COleVariant(pPart->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	// 件号
			QuerySteelMatMark(pPart->cMaterial,sText);
			map.SetValueAt(index,2,COleVariant(sText));				// 材质
			map.SetValueAt(index,3,COleVariant(pPart->sSpec));		// 厚X宽
			sprintf(sText,"%.1f",pPart->length);
			map.SetValueAt(index,4,COleVariant(sText));				// 料长
			map.SetValueAt(index,5,COleVariant((long)pPartRef->num));// 件数
			sprintf(sText,"%.1f",pPart->fPieceMinWeight);
			map.SetValueAt(index,6,COleVariant(sText));				// 单重
			sprintf(sText,"%.1f",pPart->fPieceMinWeight*pPartRef->num);	// 总重
			sum_weight+=pPart->fPieceMinWeight*pPartRef->num;		//统计总重
			map.SetValueAt(index,7,COleVariant(sText));
			map.SetValueAt(index,8,COleVariant(pPart->sNotes));		// 备注
			sprintf(cell_start,"I%d",i-1);
			sprintf(cell_end,"K%d",i-1);
			CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		}
		//总计
		sprintf(cell_start,"B%d",i);
		sprintf(cell_end,"D%d",i);
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,1,COleVariant("Total"));
#else 
		map.SetValueAt(index,1,COleVariant("总计"));
#endif
		//总重量
		sprintf(cell_start,"E%d",i);
		sprintf(cell_end,"K%d",i);
		sprintf(sText,"%.1f",sum_weight);
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,4,COleVariant(sText));
		i++;
	}
	//设置表格内容
	sprintf(cell_end,"L%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//广州增立组焊件明细表
void CLDSDoc::AddGuangZhouZengLiPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("组焊件明细表");
#endif
	//统计行数
	int nRowNum=0;
	CStatWeldPart *pPartGroupRec=NULL;
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext())
	{
		if(pPartGroupRec->GetPartRefArrPtr())
			nRowNum += pPartGroupRec->GetPartRefArrPtr()->GetNodeNum();
		nRowNum++;
	}
	int index=0;
	CVariant2dArray map(nRowNum+1,4);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Group Part No."));
	map.SetValueAt(index,2,COleVariant("Piece Part"));
	map.SetValueAt(index,3,COleVariant("Single Group Amount"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("组件号"));
	map.SetValueAt(index,2,COleVariant("零件"));
	map.SetValueAt(index,3,COleVariant("单组件数"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"D1",NULL);
	
	int i=2;
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext())
	{
		PARTREF *pPartRef=NULL;
		for(pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetFirst();pPartRef;pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetNext(),index++)
		{
			i++;
			PART *pPart=mat_tbl.GetPartFromPartNo(pPartRef->sPartNo);
			if(pPart==NULL)
				continue;
			map.SetValueAt(index,0,COleVariant(pPartGroupRec->iSeg.ToString()));	//段号
			map.SetValueAt(index,1,COleVariant(pPartGroupRec->sPartNo));//pPart->GetPartNo(g_sysPara.m_nMatCharPosType)));	//组件号
			map.SetValueAt(index,2,COleVariant(pPartRef->sPartNo));			//零件
			map.SetValueAt(index,3,COleVariant((long)pPartRef->num));		//单组数量
		}
	}
	char cell_end[15]="";
	//设置左对齐
	sprintf(cell_end,"C%d",i-1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"D%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//潍坊长安组焊明细表 wht 10-09-30
void CLDSDoc::AddWeiFangChangAnPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("组焊件明细表");
#endif

	int index=0;
	CVariant2dArray map(mat_tbl.partgroupset.GetNodeNum()+1,3);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Main No."));
	map.SetValueAt(index,2,COleVariant("Piece Part No."));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("主号"));
	map.SetValueAt(index,2,COleVariant("零件号"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,80.0,"C1",NULL);
	int i=2;
	CStatWeldPart *pPartGroupRec=NULL;
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext(),index++)
	{
		PARTREF *pPartRef=NULL;
		CXhChar500 sPartNoArr;
		for(pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetFirst();pPartRef;pPartRef=pPartGroupRec->GetPartRefArrPtr()->GetNext())
		{
			PART *pPart=mat_tbl.GetPartFromPartNo(pPartRef->sPartNo);
			if(pPart==NULL)
				continue;
			CXhChar50 sTemp("%s*%d",pPartRef->sPartNo,pPartRef->num);
			if(pPart->iSeg.iSeg!=pPartGroupRec->iSeg.iSeg)
				sTemp.Printf("%s/%s*%d",(char*)pPart->iSeg.ToString(),pPartRef->sPartNo,pPartRef->num);
			if(sPartNoArr.GetLength()>0)
				sPartNoArr.Append(",");
			sPartNoArr.Append(sTemp);
		}
		map.SetValueAt(index,0,COleVariant(pPartGroupRec->iSeg.ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pPartGroupRec->sPartNo));	//主号
		map.SetValueAt(index,2,COleVariant(sPartNoArr));				//零件号
		i++;
	}
	char cell_end[15]="";
	//设置表格内容
	sprintf(cell_end,"C%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();	
}

void CLDSDoc::AddTubeSpecialProcossSumTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet,int nTableType/*=0*/)
{	//钢管特殊工艺汇总表
	if(nTableType<0)	
		return;
	Range excel_range;
	_Worksheet excel_sheet;
	Borders borders;
	Border border;
	FontLegacy font;
    COleVariant covTrue((short)TRUE),covFalse((short)FALSE);
	char cell[16]="";
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	char sText[100]="";
	LPDISPATCH pRange;
	int i=0;

	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	if(nTableType==0)
		excel_sheet.SetName("Plane Intersect Tube Summary Table");
	else if(nTableType==1)
		excel_sheet.SetName("Column Intersect Tube Summary Table");
	else if(nTableType==2)
		excel_sheet.SetName("1-type Slot Summary Table");
	else if(nTableType==3)
		excel_sheet.SetName("Cross Slot Summary Table");
	else if(nTableType==4)
		excel_sheet.SetName("U-type Slot Summary Table");
	else if(nTableType==5)
		excel_sheet.SetName("T-type Slot Summary Table");
	else if(nTableType==6)
		excel_sheet.SetName("Both End Open Slot Summary Table");
#else 
	if(nTableType==0)
		excel_sheet.SetName("平面相贯钢管汇总表");
	else if(nTableType==1)
		excel_sheet.SetName("圆柱相贯钢管汇总表");
	else if(nTableType==2)
		excel_sheet.SetName("一字槽汇总表");
	else if(nTableType==3)
		excel_sheet.SetName("十字槽汇总表");
	else if(nTableType==4)
		excel_sheet.SetName("U型槽汇总表");
	else if(nTableType==5)
		excel_sheet.SetName("T型槽汇总表");
	else if(nTableType==6)
		excel_sheet.SetName("两端开槽汇总表");
#endif
	//else if(nTableType==7)
	//	excel_sheet.SetName("两端开槽汇总表---无U型");
	mat_tbl.SortLs();
	// 第一行主标题行
	strcpy(cell_start,"A1");
	strcpy(cell_end,"J1");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	if(nTableType==0)
		excel_range.SetValue(COleVariant("Plane Intersect Tube Summary Table"));
	else if(nTableType==1)
		excel_range.SetValue(COleVariant("Column Intersect Tube Summary Table"));
	else if(nTableType==2)
		excel_range.SetValue(COleVariant("1-type Slot Summary Table"));
	else if(nTableType==3)
		excel_range.SetValue(COleVariant("Cross Slot Summary Table"));
	else if(nTableType==4)
		excel_range.SetValue(COleVariant("U-type Slot Summary Table"));
	else if(nTableType==5)
		excel_range.SetValue(COleVariant("T-type Slot Summary Table"));
	else if(nTableType==6)
		excel_range.SetValue(COleVariant("Both End Open Slot Summary Table"));
#else 
	if(nTableType==0)
		excel_range.SetValue(COleVariant("平面相贯钢管汇总表"));
	else if(nTableType==1)
		excel_range.SetValue(COleVariant("圆柱相贯钢管汇总表"));
	else if(nTableType==2)
		excel_range.SetValue(COleVariant("一字槽汇总表"));
	else if(nTableType==3)
		excel_range.SetValue(COleVariant("十字槽汇总表"));
	else if(nTableType==4)
		excel_range.SetValue(COleVariant("U型槽汇总表"));
	else if(nTableType==5)
		excel_range.SetValue(COleVariant("T型槽汇总表"));
	else if(nTableType==6)
		excel_range.SetValue(COleVariant("两端开槽汇总表"));
#endif
	//else if(nTableType==7)
	//	excel_range.SetValue(COleVariant("两端开槽汇总表"));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	border.AttachDispatch(borders.GetItem(9),FALSE);
	border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
	border.SetWeight(COleVariant((short)4));	//xlThin=2;xlThick=4;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	// 第二行子标题行
	pRange=excel_sheet.GetRange(COleVariant("A2"));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tower Model"));
#else 
	excel_range.SetValue(COleVariant("塔型"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	// 填写塔型数据
	strcpy(cell_start,"B2");
	strcpy(cell_end,"C2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant(Ta.m_sTaType));
	
	pRange=excel_sheet.GetRange(COleVariant("D2"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Project Name"));
#else 
	excel_range.SetValue(COleVariant("工程名称"));
#endif
	excel_range.DetachDispatch();
	pRange->Release();
	// 填写工程名称数据
	strcpy(cell_start,"E2");
	strcpy(cell_end,"F2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant(Ta.m_sPrjName));
	excel_range.DetachDispatch();
	pRange->Release();
	// 呼高
	pRange=excel_sheet.GetRange(COleVariant("G2"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Module"));
#else 
	excel_range.SetValue(COleVariant("呼高"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 呼高值
	pRange=excel_sheet.GetRange(COleVariant("H2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 基数
	pRange=excel_sheet.GetRange(COleVariant("I2"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number"));
#else 
	excel_range.SetValue(COleVariant("基数"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 填写基数数据
	pRange=excel_sheet.GetRange(COleVariant("J2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant((short)1));
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.DetachDispatch();
	pRange->Release();
	//第三行数据项标题行，需要格式输出
	// 钢管编号
	pRange=excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tube Number"));
#else 
	excel_range.SetValue(COleVariant("钢管编号"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 材质
	pRange=excel_sheet.GetRange(COleVariant("B3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Material Texture"));
#else 
	excel_range.SetValue(COleVariant("材质"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 厚X宽
	pRange=excel_sheet.GetRange(COleVariant("C3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Specification"));
#else 
	excel_range.SetValue(COleVariant("规格"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// 插板槽型式
	pRange=excel_sheet.GetRange(COleVariant("D3"));
	excel_range.AttachDispatch(pRange,FALSE);
	if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Intersect Type"));
	else 
		excel_range.SetValue(COleVariant("Insert Plane Slot-Type"));
#else 
		excel_range.SetValue(COleVariant("相贯型式"));
	else 
		excel_range.SetValue(COleVariant("插板槽型式"));
#endif
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.DetachDispatch();
	pRange->Release();
	if(nTableType==0)
	{	//平面相贯
		strcpy(cell_start,"E3");
		strcpy(cell_end,"F3");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Angle A°"));
#else 
		excel_range.SetValue(COleVariant("角度A°"));
#endif
		excel_range.SetColumnWidth(COleVariant(5.0));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	else
	{
		// 插板槽长度
		pRange=excel_sheet.GetRange(COleVariant("E3"));
		excel_range.AttachDispatch(pRange,FALSE);
		if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Main Tube Specification"));
		else
			excel_range.SetValue(COleVariant("Insert Plate Slot-Length"));
#else 
			excel_range.SetValue(COleVariant("主管规格"));
		else
			excel_range.SetValue(COleVariant("插板槽长度"));
#endif
		excel_range.SetColumnWidth(COleVariant(10.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 插板槽宽度A
		pRange=excel_sheet.GetRange(COleVariant("F3"));
		excel_range.AttachDispatch(pRange,FALSE);
		if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Angle A°"));
		else
			excel_range.SetValue(COleVariant("Insert Plate Slot-Width A"));
#else 
			excel_range.SetValue(COleVariant("角度A°"));
		else
			excel_range.SetValue(COleVariant("插板槽宽度A"));
#endif
		excel_range.SetColumnWidth(COleVariant(10.0));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	if(nTableType==2||nTableType==3)
	{
		// 单基数
		pRange=excel_sheet.GetRange(COleVariant("G3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
		excel_range.SetValue(COleVariant("单基数"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 总数
		pRange=excel_sheet.GetRange(COleVariant("H3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Totality"));
#else 
		excel_range.SetValue(COleVariant("总数"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 备注
		strcpy(cell_start,"I3");
		strcpy(cell_end,"J3");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Notes"));
#else 
		excel_range.SetValue(COleVariant("备注"));
#endif
		excel_range.SetColumnWidth(COleVariant(8.0));
		pRange->Release();
	}
	else
	{
		// 插板槽宽度B
		pRange=excel_sheet.GetRange(COleVariant("G3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		if(nTableType==4)
			excel_range.SetValue(COleVariant("Insert Plate Slot-Arc R"));
		else if(nTableType==6)
			excel_range.SetValue(COleVariant("Slot Width B(Arc R)"));
		else if(nTableType==0||nTableType==1)
			excel_range.SetValue(COleVariant("Branch Tube Length L"));
		else 
			excel_range.SetValue(COleVariant("Insert Plate Slot-Width B"));
#else 
		if(nTableType==4)
			excel_range.SetValue(COleVariant("插板槽圆弧R"));
		else if(nTableType==6)
			excel_range.SetValue(COleVariant("槽宽B(圆弧R)"));
		else if(nTableType==0||nTableType==1)
			excel_range.SetValue(COleVariant("支管长度L"));
		else 
			excel_range.SetValue(COleVariant("插板槽宽B"));
#endif
		excel_range.SetColumnWidth(COleVariant(12.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 单基数
		pRange=excel_sheet.GetRange(COleVariant("H3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
		excel_range.SetValue(COleVariant("单基数"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 总数
		pRange=excel_sheet.GetRange(COleVariant("I3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Totality"));
#else 
		excel_range.SetValue(COleVariant("总数"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// 备注
		pRange=excel_sheet.GetRange(COleVariant("J3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Notes"));
#else 
		excel_range.SetValue(COleVariant("备注"));
#endif
		excel_range.SetColumnWidth(COleVariant(16.0));
		pRange->Release();
	}
	i=4;
	PART *pRec=NULL;
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->part_type!=STEELTYPE_TUBE)
			continue;
		TUBE_PROCESS *pStartTubeProcess=NULL, *pEndTubeProcess=NULL;
		sprintf(cell,"A%d",i);
		if(nTableType==0)
		{	//平面相贯钢管汇总表
			if(pRec->startProcess.type==PROCESSTYPE_PLANECUT&&pRec->endProcess.type==PROCESSTYPE_PLANECUT)
			{	//钢管两端均需割相贯线
				
			}
			else if(pRec->startProcess.type==PROCESSTYPE_PLANECUT)//钢管一端割相贯线
				pStartTubeProcess=&pRec->startProcess;
			else if(pRec->endProcess.type==PROCESSTYPE_PLANECUT)//钢管一端割相贯线
				pStartTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL)
				continue;
			//钢管编号
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//材质
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//规格
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//相贯型式
			cell[0]='D';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Plane Intersect"));
#else 
			excel_range.SetValue(COleVariant("平面相贯"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//角度A°
			sprintf(cell_start,"E%d",i);
			sprintf(cell_end,"F%d",i);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)1));
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(5.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//支管长度L
			cell[0]='G';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%.f",pStartTubeProcess->L);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//单基数
			cell[0]='H';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%d",pRec->GetPartNum());
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//总数
			cell[0]='I';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"=H%d*J2",i);	//总件数
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//备注
			cell[0]='J';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("One End Interset Line"));
#else 
			excel_range.SetValue(COleVariant("一端割相贯线"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		else if(nTableType==1)
		{	//圆柱面相贯钢管汇总表
			if(pRec->startProcess.type==PROCESSTYPE_TRANSECT&&pRec->endProcess.type==PROCESSTYPE_TRANSECT)
			{	//钢管两端均需割相贯线
				
			}
			else if(pRec->startProcess.type==PROCESSTYPE_TRANSECT)//钢管一端割相贯线
				pStartTubeProcess=&pRec->startProcess;
			else if(pRec->endProcess.type==PROCESSTYPE_TRANSECT)//钢管一端割相贯线
				pStartTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL)
				continue;
			//钢管编号
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//材质
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//规格
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//支管长度L
			cell[0]='D';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Column Intersect"));
#else 
			excel_range.SetValue(COleVariant("圆柱相贯"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//主管规格
			cell[0]='E';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->guige));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//角度A°
			cell[0]='F';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//支管长度L
			cell[0]='G';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%.f",pStartTubeProcess->L);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//单基数
			cell[0]='H';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%d",pRec->GetPartNum());
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//总数
			cell[0]='I';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"=H%d*J2",i);	//总件数
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//备注
			cell[0]='J';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("One End Interset Line"));
#else 
			excel_range.SetValue(COleVariant("一端割相贯线"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		else if(nTableType==2||nTableType==3||nTableType==4||nTableType==5)
		{	//一端开槽或者两端开槽类型相同
			if(pRec->startProcess.type<=2&&pRec->endProcess.type<=2)
				continue;	//跳过两端均无开口槽的钢管
			TUBE_PROCESS_TYPE curSlotType=PROCESSTYPE_NONE;	//当前要统计的开口槽类型
			if(nTableType==2)		//一字槽
				curSlotType=PROCESSTYPE_CSLOT;
			else if(nTableType==3)	//十字槽
				curSlotType=PROCESSTYPE_XSLOT;
			else if(nTableType==4)	//U型槽
				curSlotType=PROCESSTYPE_USLOT;
			else if(nTableType==5)	//T型槽
				curSlotType=PROCESSTYPE_TSLOT;
			
			if((pRec->startProcess.type>2&&pRec->startProcess.type!=curSlotType)
				||(pRec->endProcess.type>2&&pRec->endProcess.type!=curSlotType))
				continue;	//不统计开口槽当前类型的钢管	
			if(pRec->startProcess.type==curSlotType)
				pStartTubeProcess=&pRec->startProcess;
			if(pRec->endProcess.type==curSlotType)
				pEndTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL&&pEndTubeProcess==NULL)
				continue;
			if(pStartTubeProcess==NULL&&pEndTubeProcess)
			{	//如果仅有一端开槽则将数据存在pStartTubeProcess中
				pStartTubeProcess=pEndTubeProcess;
				pEndTubeProcess=NULL;
			}
			//应该判断始终端开口槽数据是否一致
			BOOL bSameSlot=TRUE;
			if(pStartTubeProcess&&pEndTubeProcess)
			{
				if(pStartTubeProcess->A!=pEndTubeProcess->A
					||pStartTubeProcess->B!=pEndTubeProcess->B
					||pStartTubeProcess->L!=pEndTubeProcess->L
					||pStartTubeProcess->fNormOffset!=pEndTubeProcess->fNormOffset)
					bSameSlot=FALSE;	//两端开口槽数据不一致
			}
			//钢管编号
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//材质
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//规格
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽形式
			cell[0]='D';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(nTableType==2)
				excel_range.SetValue(COleVariant("1-type Slot"));
			else if(nTableType==3)
				excel_range.SetValue(COleVariant("Cross Type Slot"));
			else if(nTableType==4)
				excel_range.SetValue(COleVariant("U Type Slot"));
			else if(nTableType==5)
				excel_range.SetValue(COleVariant("T Type Slot"));
#else 
			if(nTableType==2)
				excel_range.SetValue(COleVariant("一字型槽"));
			else if(nTableType==3)
				excel_range.SetValue(COleVariant("十字型槽"));
			else if(nTableType==4)
				excel_range.SetValue(COleVariant("U型槽"));
			else if(nTableType==5)
				excel_range.SetValue(COleVariant("T型槽"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽长度L
			cell[0]='E';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->L));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽宽度A
			cell[0]='F';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			if(nTableType==4||nTableType==5)
			{
				//插板槽宽度B
				cell[0]='G';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(pStartTubeProcess->B));
				excel_range.SetColumnWidth(COleVariant(10.0));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			short iOffset=0;
			if(nTableType==2||nTableType==3)
			{	//单基数
				cell[0]='G';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"%d",pRec->GetPartNum());
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//总数
				cell[0]='H';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"=G%d*J2",i);	//总件数
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//备注
				sprintf(cell_start,"I%d",i);
				sprintf(cell_end,"J%d",i);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
				if(pStartTubeProcess&&pEndTubeProcess)
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Both End Cut Slot"));
				else
					excel_range.SetValue(COleVariant("One End Cut Slot"));
#else 
					excel_range.SetValue(COleVariant("两端割槽"));
				else
					excel_range.SetValue(COleVariant("一端割槽"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			else
			{	//单基数
				cell[0]='H';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"%d",pRec->GetPartNum());
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//总数
				cell[0]='I';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"=H%d*J2",i);	//总件数
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//备注
				cell[0]='J';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				if(pStartTubeProcess&&pEndTubeProcess)
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Both End Cut Slot"));
				else
					excel_range.SetValue(COleVariant("One End Cut Slot"));
#else 
					excel_range.SetValue(COleVariant("两端割槽"));
				else
					excel_range.SetValue(COleVariant("一端割槽"));
#endif
				excel_range.SetColumnWidth(COleVariant(16.0));
				excel_range.DetachDispatch();
				pRange->Release();
			}
		}
		else if(nTableType==6||nTableType==7)
		{	//两端开槽 根据是否有U型槽可以分两类
			if(pRec->startProcess.type<3||pRec->endProcess.type<3)
				continue;	//只统计需要两端割槽的钢管
			if(pRec->startProcess.type==pRec->endProcess.type)
				continue;	//此处不统计两端开口槽类型一样的钢板
			pStartTubeProcess=&pRec->startProcess;
			pEndTubeProcess=&pRec->endProcess;
			//钢管编号
			sprintf(cell_start,"A%d",i);
			sprintf(cell_end,"A%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//材质
			sprintf(cell_start,"B%d",i);
			sprintf(cell_end,"B%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//规格
			sprintf(cell_start,"C%d",i);
			sprintf(cell_end,"C%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//插板槽形式
			sprintf(cell,"D%d",i);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pStartTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("1-type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("Cross Type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U Type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T Type Slot"));
#else 
			if(pStartTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("一型槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("十型槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U型槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T型槽"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			sprintf(cell,"D%d",i+1);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("1-type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("Cross Type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U Type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T Type Slot"));
#else 
			if(pEndTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("一型槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("十型槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U型槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T型槽"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽长度L
			sprintf(cell,"E%d",i);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->L));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			sprintf(cell,"E%d",i+1);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pEndTubeProcess->L));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽宽度A
			sprintf(cell,"F%d",i);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			sprintf(cell,"F%d",i+1);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pEndTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//插板槽宽度B
			sprintf(cell,"G%d",i);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->B));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			sprintf(cell,"G%d",i+1);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pEndTubeProcess->B));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//单基数
			sprintf(cell_start,"H%d",i);
			sprintf(cell_end,"H%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			sprintf(sText,"%d",pRec->GetPartNum());
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//总数
			sprintf(cell_start,"I%d",i);
			sprintf(cell_end,"I%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			sprintf(sText,"=H%d*J2",i);	//总件数
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//单基数
			sprintf(cell,"J%d",i);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pStartTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("One Cut 1-type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("One Cut Cross Type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("One Cut U Type Slot"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("One Cut T Type Slot"));
#else 
			if(pStartTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("一端割一字槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("一端割十字槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("一端割U型槽"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("一端割T型槽"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
			sprintf(cell,"J%d",i+1);
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			if(pEndTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("One Cut 1-type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("One Cut Cross Type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("One Cut U Type Slot"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("One Cut T Type Slot"));
#else 
			if(pEndTubeProcess->type==PROCESSTYPE_CSLOT)
				excel_range.SetValue(COleVariant("一端割一字槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("一端割十字槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("一端割U型槽"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("一端割T型槽"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
			i++;
		}
		i++;
	}
	strcpy(cell_start,"A2");
	sprintf(cell_end,"J%d",i-1);
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	borders.SetLineStyle(COleVariant((long)1));	//xlSingle
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}

void CLDSDoc::OnExportTubeSpecialProcessTable() 
{
	CTowerType mat_tbl;
	static CTubeSpecialProcessDlg process_dlg;
	if(process_dlg.DoModal()!=IDOK)
		return;
	int nSheet=0;
	if(process_dlg.m_bPlaneCut)
		nSheet++;
	if(process_dlg.m_bTransect)
		nSheet++;
	if(process_dlg.m_bSlotC)
		nSheet++;
	if(process_dlg.m_bSlotU)
		nSheet++;
	if(process_dlg.m_bSlotX)
		nSheet++;
	if(process_dlg.m_bSlotT)
		nSheet++;
	if(process_dlg.m_bTwoSlot)
		nSheet++;
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart(0);
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;	//pRange, 

	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}
	excel_app.SetSheetsInNewWorkbook(nSheet);	//新生成工作簿中默认工作表的数量√

	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);

	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int i=0,iSheet=1;
	//填写第一张工件数据表单
	LPDISPATCH pWorksheet;
	//nTableType 表格类型:0.平面相贯 1.圆柱相贯 2.一字槽 3.十字槽 4.U型槽 
	//					  5.T型槽 6.两端开槽
	if(process_dlg.m_bPlaneCut)
	{	//平面相贯
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,0);
		iSheet++;
	}
	if(process_dlg.m_bTransect)
	{	//圆柱面相贯
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,1);
		iSheet++;
	}
	if(process_dlg.m_bSlotC)
	{	//一字槽汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,2);
		iSheet++;
	}
	if(process_dlg.m_bSlotX)
	{	//十字槽汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,3);
		iSheet++;
	}
	if(process_dlg.m_bSlotU)
	{	//U型槽汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,4);
		iSheet++;
	}
	if(process_dlg.m_bSlotT)
	{	//T型槽汇总表
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,5);
		iSheet++;
	}
	if(process_dlg.m_bTwoSlot)
	{	//钢管两端割槽
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,6);
		iSheet++;
	}
	pWorksheet->Release();
	EndWaitCursor();
}
//按段统计螺栓
static int compare_func1(const CLsRecord &item1,const CLsRecord &item2)
{	
	if(item1.iSeg.iSeg>item2.iSeg.iSeg)
		return 1;
	else if(item1.iSeg.iSeg<item2.iSeg.iSeg)
		return -1;
	else if(item1.hFamily>item2.hFamily)
		return 1;
	else if(item1.hFamily<item2.hFamily)
		return -1;
	else if(stricmp(item1.grade,item2.grade)>0)
		return 1;
	else if(stricmp(item1.grade,item2.grade)==0)
	{
		if(item1.d>item2.d)
			return 1;
		else if(item1.d<item2.d)
			return -1;
		else if(item1.L>item2.L)
			return 1;
		else if(item1.L<item2.L)
			return -1;
		else
			return 0;
	}
	else
		return -1;
}
void CLDSDoc::AddBoltSumTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet)
{
	//将螺栓记录进行排序处理
	ARRAY_LIST<CLsRecord> lsRecorArr;
	lsRecorArr.SetSize(0,mat_tbl.ls_recordset.GetNodeNum());
	for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
		lsRecorArr.append(*pLsRec);
	CQuickSort<CLsRecord>::QuickSort(lsRecorArr.m_pData,lsRecorArr.GetSize(),compare_func1);
	int i=0;
	for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext(),i++)
		*pLsRec=lsRecorArr[i];
	//生成螺栓汇总表格
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.SetName("螺栓汇总表");
	CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX公司铁塔螺栓汇总表",20.0,14.0);
	//添加标题列
	int index=0;
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,9);
	map.SetValueAt(index,0,COleVariant("塔型"));
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	map.SetValueAt(index,3,COleVariant("工程名称"));
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	CExcelOper::MergeColRange(excel_sheet,"E2","F2");
	map.SetValueAt(index,6,COleVariant("特殊螺栓数量"));
	CExcelOper::MergeColRange(excel_sheet,"G2","I2");
	index++;
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("名称"));
	map.SetValueAt(index,2,COleVariant("级别规格"));
	map.SetValueAt(index,3,COleVariant("无扣长"));
	map.SetValueAt(index,4,COleVariant("单基重量"));
	map.SetValueAt(index,5,COleVariant("单基数量"));
	map.SetValueAt(index,6,COleVariant("双帽螺栓"));
	map.SetValueAt(index,7,COleVariant("防盗螺栓"));
	map.SetValueAt(index,8,COleVariant("弹簧垫"));
	//
	index++;
	CXhChar16 cell_start,cell_end;
	int iStartSeg=-1,iStartFamily=-1,iCell=3;
	SEGI iCurSeg;
	CLsFamily* pLsFamily=NULL,*pPreLsFamily=NULL;
	CLsRecord *pRec=NULL,*pTailRec=mat_tbl.ls_recordset.GetTail();
	for(pRec=mat_tbl.ls_recordset.GetFirst();pRec;pRec=mat_tbl.ls_recordset.GetNext(),index++)
	{
		iCell=index+1;
		pLsFamily=NULL;
		if(pRec->typels0_pad1==0)
		{
			pLsFamily=CLsLibrary::FromHandle(pRec->hFamily);
			if(pLsFamily==NULL)
				pLsFamily=CLsLibrary::DefaultFamily();
		}
		//
		if(iStartSeg==-1)
		{	//记录起始段
			iStartSeg=iCell+1;
			iCurSeg=pRec->iSeg;
			map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		}
		if(pRec->iSeg.iSeg==iCurSeg.iSeg)
		{	//同段内
			if(iStartFamily==-1)
			{
				iStartFamily=iCell+1;
				if(pLsFamily)
					map.SetValueAt(index,1,COleVariant(pLsFamily->name));
			}
			else if(pLsFamily!=pPreLsFamily)
			{
				CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartFamily),CXhChar16("B%d",iCell));
				iStartFamily=iCell+1;
				if(pLsFamily)
					map.SetValueAt(index,1,COleVariant(pLsFamily->name));
			}
		}
		if(pRec->iSeg.iSeg!=iCurSeg.iSeg)
		{	//跨段
			CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartFamily),CXhChar16("B%d",iCell));
			iStartFamily=iCell+1;
			if(pLsFamily)
				map.SetValueAt(index,1,COleVariant(pLsFamily->name));
			//
			CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartSeg),CXhChar16("A%d",iCell));
			iStartSeg=iCell+1;
			iCurSeg=pRec->iSeg;
			map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		}
		if(pRec==pTailRec)
		{
			CExcelOper::MergeRowRange(excel_sheet,CXhChar16("A%d",iStartSeg),CXhChar16("A%d",iCell+1));
			CExcelOper::MergeRowRange(excel_sheet,CXhChar16("B%d",iStartFamily),CXhChar16("B%d",iCell+1));
		}
		//级别规格
		if(pRec->typels0_pad1==0)
			map.SetValueAt(index,2,COleVariant(CXhChar50("%sM%dX%d",pRec->grade,pRec->d,pRec->L)));
		else
			map.SetValueAt(index,2,COleVariant(CXhChar50("-%dX%d",pRec->L,pRec->d)));
		//无扣长
		if(pRec->typels0_pad1==0)
			map.SetValueAt(index,3,COleVariant(CXhChar16("%d",pRec->no_thread_len)));
		//单基重
		map.SetValueAt(index,4,COleVariant(CXhChar16("%.1f",pRec->sum_weight)));
		//单基数
		map.SetValueAt(index,5,COleVariant(pRec->GetPartNum()));
		if(pRec->typels0_pad1==0)
		{	//特殊螺栓数量
			map.SetValueAt(index,6,COleVariant(pRec->nTwoCap));		//双帽数
			map.SetValueAt(index,7,COleVariant(pRec->nBurglarproof));	//防盗数量
			map.SetValueAt(index,8,COleVariant(pRec->nSpringPad));	//弹簧垫数量s
		}
		//
		pPreLsFamily=pLsFamily;
	}
	//设置表格内容
	sprintf(cell_end,"I%d",iCell+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.ReleaseDispatch();
}
void CLDSDoc::AddLsSumMatTblBySeg(CTowerType &mat_tbl, LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Bolt Summary Table");
#else 
	excel_sheet.SetName("螺栓汇总表");
#endif
	mat_tbl.SortLs();
	// 第一行主标题行
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","H1","XXX Company Bolt Summary Table",20.0,14.0);
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","H1","XXX公司铁塔螺栓汇总表",20.0,14.0);
#endif
	
	int index=0;
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,8);
	// 第二行子标题行
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("塔型"));
#endif
	// 填写塔型数据
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	//工程名称
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("工程名称"));
#endif
	// 填写工程名称数据
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	CExcelOper::MergeColRange(excel_sheet,"E2","F2");
	//
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,6,COleVariant("Special Bolt Amount"));
#else 
	map.SetValueAt(index,6,COleVariant("其中特殊螺栓数量"));
#endif
	CExcelOper::MergeColRange(excel_sheet,"G2","H2");
	index++;
	//
	//第三行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Grade Specification"));
	map.SetValueAt(index,2,COleVariant("L0"));
	map.SetValueAt(index,3,COleVariant("Single Weight"));
	map.SetValueAt(index,4,COleVariant("Single Basic Number"));
	map.SetValueAt(index,5,COleVariant("Double-nut Bolt"));
	map.SetValueAt(index,6,COleVariant("Anti-Theft Bolt"));
	map.SetValueAt(index,7,COleVariant("Spring Cushion"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("级别规格"));
	map.SetValueAt(index,2,COleVariant("穿过厚度"));
	map.SetValueAt(index,3,COleVariant("单基重量"));
	map.SetValueAt(index,4,COleVariant("单基数量"));
	map.SetValueAt(index,5,COleVariant("双帽螺栓"));
	map.SetValueAt(index,6,COleVariant("防盗螺栓"));
	map.SetValueAt(index,7,COleVariant("弹簧垫"));
#endif
	index++;
	//
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"A3","H3");
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"A3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,13.0,"B3",NULL,TRUE);
	//
	int i=2;
	SEGI seg_start_no=-1;
	int seg_start_i=-1;
	CLsRecord *pLsRec=NULL;
	BOOL bEnd = (mat_tbl.ls_recordset.GetNodeNum()>0);
	for(pLsRec=mat_tbl.ls_recordset.GetFirst();bEnd;pLsRec=mat_tbl.ls_recordset.GetNext(),index++)
	{
		if(pLsRec)
			i++;
		if(seg_start_i==-1&&pLsRec!=NULL)
		{
			seg_start_i=i+1;
			seg_start_no=pLsRec->iSeg;
			//设置段号
			map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
		}
		//添加段号 pLsRec==NULL时添加最后一段段号
		if((pLsRec&&pLsRec->iSeg.iSeg!=seg_start_no.iSeg)||pLsRec==NULL)
		{
			sprintf(cell_start,"A%d",seg_start_i);
			if (pLsRec!=NULL)
				sprintf(cell_end,"A%d",i);
			else//最后一段时i没有进行i++
				sprintf(cell_end,"A%d",i+1);
			//合并列
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
			if(pLsRec)
			{	//记录下一段开始行数及段号
				seg_start_i=i+1;
				seg_start_no=pLsRec->iSeg;
				//设置段号
				map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
			}
			else
			{	//添加完最后一段段号时跳出循环
				bEnd=FALSE;
				break;
			}
		}
		//规格
		char guige[20];
		if(pLsRec->typels0_pad1==0)
			sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
		else
			sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
		map.SetValueAt(index,1,COleVariant(guige));
		//穿过厚度;
		char ss[8];
		sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,2,COleVariant(ss));
		//单基重量
		map.SetValueAt(index,3,COleVariant(pLsRec->sum_weight));
		//单基数
		map.SetValueAt(index,4,COleVariant(pLsRec->GetPartNum()));
		//双帽数
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,5,COleVariant(pLsRec->nTwoCap));
		//防盗数量
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,6,COleVariant(pLsRec->nBurglarproof));
		//弹簧垫数量
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,7,COleVariant(pLsRec->nSpringPad));
	}
	//设置数字格式
	sprintf(cell_end,"D%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"D4",cell_end,COleVariant("0.0"));
	//设置表格内容
	sprintf(cell_end,"H%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}

void CLDSDoc::OnExportExcelFile2() 
{
	CInputAnStringValDlg dlg;
	dlg.m_sItemTitle="段号范围：";
	dlg.m_sTip="*表示输出所有段";
	dlg.m_sItemValue="*";
	if(dlg.DoModal()!=IDOK)
		return;
	CTowerType mat_tbl;
	int nSheet=2;
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	int i=0;
	mat_tbl.SortPart(0);
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(dlg.m_sItemValue,segTable);
	if(segTable.GetNodeNum()>0)
	{
		for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(segTable.GetValue(pRec->iSeg)==NULL)
				mat_tbl.part_recordset.DeleteCursor();
		}
		mat_tbl.part_recordset.Clean();
	}
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}

	excel_app.SetSheetsInNewWorkbook(nSheet);	//新生成工作簿中默认工作表的数量√
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	//角钢数、钢板数
	int nAngleNum=0,nPlate=0;
	for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->part_type==1||pRec->part_type==4)
			nAngleNum++;
		else if(pRec->part_type==3)
			nPlate++;
	}
	LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	for(int iWorkBook=0;iWorkBook<2;iWorkBook++)
	{
		i=0;
		//填写第一张工件数据表单
		LPDISPATCH pWorksheet;
		pWorksheet = excel_sheets.GetItem(COleVariant((short) (iWorkBook+1)));
		//
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName(CXhChar50("Workpiece Data-%d",iWorkBook+1));
#else 
		excel_sheet.SetName(CXhChar50("工件数据-%d",iWorkBook+1));
#endif
		//
		int index=0,nRow=0;
		if(iWorkBook==0)
			nRow = nAngleNum;
		else 
			nRow = nPlate;
		CVariant2dArray map(nRow+1,30);
		map.SetValueAt(index,0,COleVariant("xh"));		//序号
		map.SetValueAt(index,1,COleVariant("bjbh"));	//部件编号
		map.SetValueAt(index,2,COleVariant("sjgg"));	//设计规格
		map.SetValueAt(index,3,COleVariant("cz"));		//材质
		map.SetValueAt(index,4,COleVariant("fycc"));	//放样尺寸
		map.SetValueAt(index,5,COleVariant("dgzl"));	//单件重量
		map.SetValueAt(index,6,COleVariant("djsl"));	//单基数量
		map.SetValueAt(index,7,COleVariant("djzl"));	//单基重量
		map.SetValueAt(index,8,COleVariant("bjth"));	//部件图号
		map.SetValueAt(index,9,COleVariant("jgsl"));	//加工数量
		map.SetValueAt(index,10,COleVariant("jgzl"));	//加工重量
		map.SetValueAt(index,11,COleVariant("T-xl"));	//下料
		map.SetValueAt(index,12,COleVariant("T-zk"));	//制孔
		map.SetValueAt(index,13,COleVariant("T-qj"));	//切角
		map.SetValueAt(index,14,COleVariant("T-wq"));	//弯曲
		map.SetValueAt(index,15,COleVariant("T-zh"));	//组合
		map.SetValueAt(index,16,COleVariant("T-hanj")); //焊接
		map.SetValueAt(index,17,COleVariant("T-pj"));	//刨角
		map.SetValueAt(index,18,COleVariant("T-cb"));	//铲背
		map.SetValueAt(index,19,COleVariant("T-kj"));	//开角
		map.SetValueAt(index,20,COleVariant("T-hej"));	//合角
		map.SetValueAt(index,21,COleVariant("T-yb"));	//压扁
		map.SetValueAt(index,22,COleVariant("T-dk"));	//打口
		map.SetValueAt(index,23,COleVariant("T-qz"));	//切肢
		map.SetValueAt(index,24,COleVariant("T-mm"));	//倒角
		map.SetValueAt(index,25,COleVariant("T-dx"));	//镀锌
		map.SetValueAt(index,26,COleVariant("Bz"));		//备注
		map.SetValueAt(index,27,COleVariant("C"));		//备注
		map.SetValueAt(index,28,COleVariant("K"));		//备注
		map.SetValueAt(index,29,COleVariant("Gy"));		//工艺
		index++;
		for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(iWorkBook==0 && (pRec->part_type!=1&&pRec->part_type!=4))
				continue;	//角钢、套管 wht 18-11-07
			else if(iWorkBook==1 && pRec->part_type!=3)
				continue;
			i++;
			map.SetValueAt(index,0,COleVariant((long)i));	//序号
			map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			char mark[20]="";
			/*
			strcpy(mark,pRec->sPartNo);
			int str_len=strlen(mark);
			str_len=min(str_len,18);
			if(pRec->cMaterial!='S')
				mark[str_len]=pRec->cMaterial;
			mark[str_len+1]='\0';*/
			if(pRec->part_type==3)
			{
				char spec[8];
				sprintf(spec,"'-%.0f",pRec->thick);
				map.SetValueAt(index,2,COleVariant(spec));	//设计规格
			}
			else if(pRec->part_type==1)
			{	//宝鸡生产系统要求角钢规格必须为“L63X5”形式，不允许有丝毫变化 wjh-2015.4.1
				char* split=strchr(pRec->sSpec,'x');
				if(split)
					*split='X';
				while(pRec->sSpec[0]==' ')
				{
					memmove((char*)pRec->sSpec,&pRec->sSpec[1],pRec->sSpec.GetLength()-1);
					pRec->sSpec[pRec->sSpec.GetLength()-1]=0;
				}
				map.SetValueAt(index,2,COleVariant(pRec->sSpec));
			}
			else if(pRec->part_type==4)
			{	//套管
				CXhChar50 sSpec("φ%.f",pRec->wide);
				map.SetValueAt(index,2,COleVariant(sSpec));
			}
			QuerySteelMatMark(pRec->cMaterial,mark);
			map.SetValueAt(index,3,COleVariant(mark));	//材质
			if(iWorkBook==0)	// 角钢
				map.SetValueAt(index,4,COleVariant(ftol(pRec->length)));
			else				//放样尺寸
#ifdef AFX_TARG_ENU_ENGLISH
				map.SetValueAt(index,4,COleVariant("Sample"));	
#else 
				map.SetValueAt(index,4,COleVariant("样板"));	
#endif
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				map.SetValueAt(index,5,COleVariant(ftol(10*pRec->fPieceMinWeight)/10.0));	//单件重量
			else
				map.SetValueAt(index,5,COleVariant(ftol(10*pRec->fPieceNetWeight)/10.0));	//单件重量
			map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//单基数量
			double djzl=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				djzl=pRec->fPieceMinWeight*pRec->GetPartNum();
			else
				djzl=pRec->fPieceNetWeight*pRec->GetPartNum();
			map.SetValueAt(index,7,COleVariant(ftol(10*djzl)/10.0));	//单基重量
			map.SetValueAt(index,8,COleVariant(""));	//部件图号
			map.SetValueAt(index,9,COleVariant((long)pRec->GetPartNum()));	//加工数量
			map.SetValueAt(index,10,COleVariant(ftol(10*djzl)/10.0));	//加工重量,暂等于单基重量
			map.SetValueAt(index,11,COleVariant("V"));	//下料
			if(pRec->nMSumLs>0)
				map.SetValueAt(index,12,COleVariant("V"));	//制孔
			if(pRec->bCutAngle)
				map.SetValueAt(index,13,COleVariant("V"));	//切角
			if(pRec->bZhiWan)
				map.SetValueAt(index,14,COleVariant("V"));	//弯曲
			if(pRec->bWeldPart)
				map.SetValueAt(index,15,COleVariant("V"));	//组合
			if(pRec->bWeldPart)
				map.SetValueAt(index,16,COleVariant("V"));	//焊接
			if(pRec->bCutRoot)
				map.SetValueAt(index,17,COleVariant("V"));	//刨角(清根)
			if(pRec->bCutBer)
				map.SetValueAt(index,18,COleVariant("V"));	//铲背
			if(pRec->bKaiJiao)
				map.SetValueAt(index,19,COleVariant("V"));	//开角
			if(pRec->bHeJiao)
				map.SetValueAt(index,20,COleVariant("V"));	//合角
			if(pRec->nPushFlat>0)
				map.SetValueAt(index,21,COleVariant("V"));	//压扁
			if(pRec->bNeedFillet)
				map.SetValueAt(index,22,COleVariant("V"));	//打口
			if(CLDSPart::CustomerSerial!=CLIENT_ShanXi_BaoJi)
			{	//刘伟反馈宝鸡要求，X列不需要打V wht 18-11-07
				if(pRec->bCutAngle)
					map.SetValueAt(index,23,COleVariant("V"));	//切肢
			}
			map.SetValueAt(index,24,COleVariant(""));		//倒角
			map.SetValueAt(index,25,COleVariant("V"));		//镀锌
			map.SetValueAt(index,26,COleVariant(pRec->sNotes));		//备注
			if(pRec->part_type==3)
				map.SetValueAt(index,27,COleVariant(pRec->length));	//板长
			if(pRec->part_type==3)
				map.SetValueAt(index,28,COleVariant(pRec->wide));	//板宽
			char notes[200];
			pRec->GetProcessNotes(notes);
			map.SetValueAt(index,29,COleVariant(notes));	//工艺
			index++;
		}
		char cell_end[15]="";
		//设置编号格式
		sprintf(cell_end,"B%d",i);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B2",cell_end,COleVariant("@"));
		if(iWorkBook==1)
		{	//设置规格格式
			sprintf(cell_end,"C%d",i);
			CExcelOper::SetRangeNumberFormat(excel_sheet,"C2",cell_end,COleVariant("@"));
		}
		//设置数字格式
		sprintf(cell_end,"F%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"F2",cell_end,COleVariant("0.0"));
		sprintf(cell_end,"H%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"H2",cell_end,COleVariant("0.0"));
		sprintf(cell_end,"K%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"K2",cell_end,COleVariant("0.0"));
		//设置表格内容
		sprintf(cell_end,"AD%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
		//设置边框
		//CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
		//
		excel_sheet.ReleaseDispatch();
	}
	excel_sheets.ReleaseDispatch();
	excel_workbook.ReleaseDispatch();
	EndWaitCursor();
}
void CLDSDoc::OnExportExcelFile3() 
{
	CTowerType mat_tbl;
	int nSheet=1;
	BeginWaitCursor();
	//进行材料统计
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart(0);
	//输出到EXCEL文件
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	::CLSIDFromProgID(L"Excel.Application", &clsid); // from registry
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	} 
	else if(!excel_app.CreateDispatch("Excel.Application")) 
	{
#ifdef AFX_TARG_ENU_ENGLISH	
		AfxMessageBox("Excel program not found");
#else
		AfxMessageBox("未找到Excel程序!");
#endif
		return;
	}
	LPDISPATCH pWorkbooks;
	CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
	if (pWnd != NULL) 
	{
		pWnd->ShowWindow(SW_SHOWNORMAL);
		pWnd->UpdateWindow();
		pWnd->BringWindowToTop();
	}

	excel_app.SetSheetsInNewWorkbook(nSheet);	//新生成工作簿中默认工作表的数量√

	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	//角钢数、钢板数
	//for(int iWorkBook=0;iWorkBook<2;iWorkBook++)
	{
		LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
		excel_workbook.AttachDispatch(pWorkbook);
		LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
		ASSERT(pWorksheets != NULL);
		excel_sheets.AttachDispatch(pWorksheets);
		//填写第一张工件数据表单
		LPDISPATCH pWorksheet;
		pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
		//
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Workpiece Data");
#else 
		excel_sheet.SetName("工件数据");
#endif
		//
		int index=0,nRow=0;
		nRow = mat_tbl.part_recordset.GetNodeNum();
		CVariant2dArray map(nRow+1,24);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Segment Number(Read Only)"));//段号
		map.SetValueAt(index,1,COleVariant("Block Number"));			//部件编号
		map.SetValueAt(index,2,COleVariant("Material Texture"));		//材质
		map.SetValueAt(index,3,COleVariant("Specification"));			//设计规格
		map.SetValueAt(index,4,COleVariant("Length(mm)"));				//长度
		map.SetValueAt(index,5,COleVariant("Width"));					//宽度
		map.SetValueAt(index,6,COleVariant("Single Basic Number"));		//单段数
		map.SetValueAt(index,7,COleVariant("Piece Weight(Kg)"));		//单件重量
		map.SetValueAt(index,8,COleVariant("Total Weight"));			//单基重量
		map.SetValueAt(index,9,COleVariant("Single Hole"));			//单件孔数
		map.SetValueAt(index,10,COleVariant("Notes"));					//备注
		map.SetValueAt(index,11,COleVariant("Weld"));					//焊接
		map.SetValueAt(index,12,COleVariant("Bend"));					//弯曲
		map.SetValueAt(index,13,COleVariant("Leg Cut"));				//切肢
		map.SetValueAt(index,14,COleVariant("Cut Back"));				//铲背
		map.SetValueAt(index,15,COleVariant("Cut Root"));				//刨角
		map.SetValueAt(index,16,COleVariant("Flat"));					//压扁
		map.SetValueAt(index,17,COleVariant("Open Merge Angle"));		//开角
		map.SetValueAt(index,18,COleVariant("Type"));
		map.SetValueAt(index,19,COleVariant("Edge Number"));
		map.SetValueAt(index,20,COleVariant("Perimeter"));
		map.SetValueAt(index,21,COleVariant("Hole Diameter 28~38mm(A)"));	
		map.SetValueAt(index,22,COleVariant("Hole Diameter 39~50mm(B)"));	
		map.SetValueAt(index,23,COleVariant("Hole Diameter More Than 50mm(C)"));
#else 
		map.SetValueAt(index,0,COleVariant("段名(只读)"));	//段号
		map.SetValueAt(index,1,COleVariant("部件号"));		//部件编号
		map.SetValueAt(index,2,COleVariant("材质"));			//材质
		map.SetValueAt(index,3,COleVariant("规格"));			//设计规格
		map.SetValueAt(index,4,COleVariant("长度(mm)"));		//长度
		map.SetValueAt(index,5,COleVariant("宽度"));			//宽度
		map.SetValueAt(index,6,COleVariant("单段数"));		//单段数
		map.SetValueAt(index,7,COleVariant("单件重量(kg)"));//单件重量
		map.SetValueAt(index,8,COleVariant("总重量(kg)"));	//单基重量
		map.SetValueAt(index,9,COleVariant("单件孔数(个)"));//单件孔数
		map.SetValueAt(index,10,COleVariant("备注"));		//备注
		map.SetValueAt(index,11,COleVariant("电焊"));		//焊接
		map.SetValueAt(index,12,COleVariant("火曲"));		//弯曲
		map.SetValueAt(index,13,COleVariant("切角切肢"));	//切肢
		map.SetValueAt(index,14,COleVariant("铲背"));		//铲背
		map.SetValueAt(index,15,COleVariant("清根"));		//刨角
		map.SetValueAt(index,16,COleVariant("打扁"));		//压扁
		map.SetValueAt(index,17,COleVariant("开合角"));		//开角
		map.SetValueAt(index,18,COleVariant("类型"));
		map.SetValueAt(index,19,COleVariant("边数"));
		map.SetValueAt(index,20,COleVariant("周长"));
		map.SetValueAt(index,21,COleVariant("孔径28~38mm(A)"));	
		map.SetValueAt(index,22,COleVariant("孔径39~50mm(B)"));	
		map.SetValueAt(index,23,COleVariant("孔径50mm以上(C)"));
#endif
		//设置列宽
		CExcelOper::SetRangeColWidth(excel_sheet,11.0,"A1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,9.0,"E1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,13.5,"H1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,11.0,"I1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,13.5,"J1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,55.0,"K1",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,15.5,"V1","X1");
		index++;
		for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			map.SetValueAt(index,0,COleVariant(SEGI(pRec->iSeg).ToString()));	//段号
			map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//件号
			CXhChar16 sMark;
			QuerySteelMatMark(pRec->cMaterial,sMark);
			if(sMark.EqualNoCase("Q235")||sMark.Equal("Q345"))
				sMark.Append("B");
			map.SetValueAt(index,2,COleVariant((char*)sMark));	//材质
			CXhChar16 sSpec("%s",(char*)pRec->sSpec);
			if(pRec->part_type==3)
				sSpec.Printf("-%.0f",pRec->thick);
			map.SetValueAt(index,3,COleVariant(sSpec));					//设计规格
			map.SetValueAt(index,4,COleVariant(ftol(pRec->length)));	//长度
			map.SetValueAt(index,5,COleVariant(ftol(pRec->wide)));		//宽度
			map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//单基数量
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				map.SetValueAt(index,7,COleVariant(ftol(10*pRec->fPieceMinWeight)/10.0));	//单件重量
			else
				map.SetValueAt(index,7,COleVariant(ftol(10*pRec->fPieceNetWeight)/10.0));	//单件重量
			double djzl=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				djzl=pRec->fPieceMinWeight*pRec->GetPartNum();
			else
				djzl=pRec->fPieceNetWeight*pRec->GetPartNum();
			map.SetValueAt(index,8,COleVariant(ftol(10*djzl)/10.0));	//单基重量
			map.SetValueAt(index,9,COleVariant((long)pRec->nMSumLs));	//单件孔数
			CXhChar200 sNotes;
			pRec->GetProcessNotes(sNotes,TRUE);
			sNotes.Append(" ");
			sNotes.Append(pRec->sNotes);
			map.SetValueAt(index,10,COleVariant((char*)sNotes));		//备注
			if(pRec->bWeldPart)
				map.SetValueAt(index,11,COleVariant("1"));	//焊接
			else
				map.SetValueAt(index,11,COleVariant("0"));	//焊接
			if(pRec->bZhiWan)
				map.SetValueAt(index,12,COleVariant("1"));	//弯曲
			else
				map.SetValueAt(index,12,COleVariant("0"));	//弯曲
			if(pRec->bCutAngle==2)
				map.SetValueAt(index,13,COleVariant("2"));	//切角切肢
			else if(pRec->bCutAngle!=0)
				map.SetValueAt(index,13,COleVariant("1"));	//切角切肢
			else
				map.SetValueAt(index,13,COleVariant("0"));	//切角切肢
			if(pRec->bCutBer)
				map.SetValueAt(index,14,COleVariant("1"));	//铲背
			else
				map.SetValueAt(index,14,COleVariant("0"));	//铲背
			if(pRec->bCutRoot)
				map.SetValueAt(index,15,COleVariant("1"));	//清根
			else
				map.SetValueAt(index,15,COleVariant("0"));	//清根
			if(pRec->nPushFlat>0)
				map.SetValueAt(index,16,COleVariant("1"));	//压扁
			else
				map.SetValueAt(index,16,COleVariant("0"));	//压扁
			if(pRec->bKaiJiao||pRec->bHeJiao)
				map.SetValueAt(index,17,COleVariant("1"));	//开角合角
			else
				map.SetValueAt(index,17,COleVariant("0"));	//开角合角
			if(pRec->nEdgeNum>0)
				map.SetValueAt(index,19,COleVariant((char*)CXhChar16("%d",pRec->nEdgeNum)));		//边数
			if(pRec->fPerimeter>0)
				map.SetValueAt(index,20,COleVariant((char*)CXhChar16("%.0f",pRec->fPerimeter)));	//周长
			map.SetValueAt(index,21,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(28,38))));	//孔径28~38mm(A)
			map.SetValueAt(index,22,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(39,50))));	//孔径39~50mm(B)
			map.SetValueAt(index,23,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(51))));	//孔径50mm以上(C)
			index++;
		}
		char cell_end[15]="";
		//设置编号格式
		sprintf(cell_end,"B%d",index-1);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B2",cell_end,COleVariant("@"));
		//设置表格内容
		sprintf(cell_end,"X%d",index);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
		//
		excel_sheet.ReleaseDispatch();
		excel_sheets.ReleaseDispatch();
		excel_workbook.ReleaseDispatch();
	}
	EndWaitCursor();
}
//根据云南景云祥需求，按特定格式导出法兰明细表	wxc_2015-10-23
void CLDSDoc::OnExportFLExcelFile()
{
	//1、进行法兰盘统计
	BeginWaitCursor();
	CTowerType mat_tbl;
	mat_tbl.SummarizeFLs(&Ta);
	mat_tbl.SortFL();
	//2、导出法兰明细表
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets!= NULL);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pSheet=excel_sheets.GetItem(COleVariant((short)1));
	AddWorkFLSheet(mat_tbl,pSheet);
	excel_sheets.ReleaseDispatch();
	EndWaitCursor();
}
void CLDSDoc::AddWorkFLSheet(CTowerType &mat_tbl,LPDISPATCH pSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("法兰盘表");
	int nFlCount=mat_tbl.fl_recordset.GetNodeNum();
	CExcelOper::SetMainTitle(excel_sheet,"A1","G1","法兰盘明细表");
	CVariant2dArray map(nFlCount+1,7);
	CXhChar16 cell_start("A2"),cell_end("G%d",mat_tbl.fl_recordset.GetNodeNum()+1);
	//第一行列标题
	int index=0;
	map.SetValueAt(index,0,COleVariant("塔段"));
	map.SetValueAt(index,1,COleVariant("件号"));
	map.SetValueAt(index,2,COleVariant("规格"));
	map.SetValueAt(index,3,COleVariant("厚度"));
	map.SetValueAt(index,4,COleVariant("孔数*孔径"));
	map.SetValueAt(index,5,COleVariant("单套数量"));
	map.SetValueAt(index,6,COleVariant("备注"));
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A2","G2");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"C2");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"G2");
	index++;
	//填充二维表内容
	BOOL bEnd=FALSE;
	SEGI seg_start_no=-1;
	int seg_start_i=-1,seg_end_i=0;
	int i=2;
	for(CFlRecord* pFlRec=mat_tbl.fl_recordset.GetFirst();pFlRec;pFlRec=mat_tbl.fl_recordset.GetNext(),index++)
	{
		if(pFlRec)
			i++;
		if(index==mat_tbl.fl_recordset.GetNodeNum())
			bEnd=TRUE;
		if(seg_start_i==-1)
		{
			seg_start_i=i;
			seg_start_no=pFlRec->iSeg;
			map.SetValueAt(index,0,COleVariant(pFlRec->iSeg.ToString()));
		}
		if(pFlRec->iSeg.iSeg!=seg_start_no.iSeg || bEnd)
		{
			seg_end_i=i-1;
			if(bEnd)
				seg_end_i=i;
			if(seg_end_i-seg_start_i>0)
			{
				sprintf(cell_start,"A%d",seg_start_i);
				sprintf(cell_end,"A%d",seg_end_i);
				CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//合并列
			}
			if(index<mat_tbl.fl_recordset.GetNodeNum())
			{
				seg_start_i=i;
				seg_start_no=pFlRec->iSeg;
				map.SetValueAt(index,0,COleVariant(pFlRec->iSeg.ToString()));
			}
		}
		CXhChar50 sValue;
		map.SetValueAt(index,1,COleVariant(pFlRec->GetPartNo()));	//件号
		map.SetValueAt(index,2,COleVariant(pFlRec->GetSpec()));		//规格
		
		sValue.Printf("-%g",pFlRec->thick);
		map.SetValueAt(index,3,COleVariant(sValue));				//厚度
		sValue.Printf("%d*%d",pFlRec->nLsNum,pFlRec->ls_d);
		map.SetValueAt(index,4,COleVariant(sValue));				//螺栓信息
		sValue.Printf("%d",pFlRec->GetFLNum());
		map.SetValueAt(index,5,COleVariant(sValue));				//单基数量
		map.SetValueAt(index,6,COleVariant(pFlRec->sNotes));		//备注		
	}
	//设置对齐方式
	cell_end.Printf("G%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)3));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)2));
	cell_end.Printf("C%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	cell_end.Printf("G%d",index+1);
	//设置表格内容 
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);			
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.ReleaseDispatch();
}
#endif

void CLDSDoc::OnFileSaveAs()
{
	CString filename=GetTitle();
	char* separator=SearchChar(filename.GetBuffer(),'.',true);
	if(separator)
	{
		*separator=0;
		filename.ReleaseBuffer();
	}
#ifdef __TMA_FILE_
	CFileDialog dlg(FALSE,"tma",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Tower Manufacturing Assistant(*.tma)|*.tma|all files(*.*)|*.*||");
#else 
		"铁塔制造助手(*.tma)|*.tma|铁塔部件段文件(*.sgt)|*.sgt|所有文件(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(FALSE,"lma",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"格构铁塔制造助手(*.lma)|*.lma|铁塔部件段文件(*.sgt)|*.sgt|所有文件(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(FALSE,"tda",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔绘图助手(*.tda)|*.tda|铁塔部件段文件(*.sgt)|*.sgt|所有文件(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CFileDialog dlg(FALSE,"lds",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔设计绘图一体化软件(*.lds)|*.lds|铁塔部件段文件(*.sgt)|*.sgt|所有文件(*.*)|*.*||");
#elif defined(__TSA_FILE_)
	CFileDialog dlg(FALSE,"tsa",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"铁塔内力分析设计文件(*.tsa)|*.tsa|所有文件(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
#ifdef __COMMON_PART_INC_
	if(dlg.GetFileExt().CompareNoCase("sgt")==0)
	{
		CBlockModel block(FALSE);
		long nVersion=FromStringVersion(m_sFileVersion);
		block.ExportToBlkFile(dlg.GetPathName(),nVersion,&Ta,TRUE);
	}
	else
#endif
	{
		m_nTimer=2;	//启用备份文件存储
		//SetPathName(dlg.GetFileName());	//移至OnSaveDocument之后执行
		OnSaveDocument(dlg.GetPathName());
		//改动原因如下:1.windows系统下SetPathName中自动补齐路径有可能出错故改为调用GetPathName
		//2.但windows7下SetPathName中调用AddRecentFile时增加了文件是否存在的判断,故必须置于OnSaveDocument之后执行 wjh-2015.2.6
		SetPathName(dlg.GetPathName());
	}
}
#ifdef __CONNECT_REMOTE_SERVER_
void CLDSDoc::SerializeTowerFileBuffer(CBuffer &buffer,BOOL bStoring)
{
	DWORD cursor_pipeline_no=DogSerialNo();
	CWaitCursor waitCursor;
	CLogErrorLife loglife;
	try{
		char file_version[20],product_version[20];
		theApp.GetAppVersion(file_version,product_version);
		if(bStoring)
		{
			CString version;
#if defined(__TMA_FILE_)
			buffer.WriteString("铁塔制造助手");
#elif defined(__LMA_FILE_)
			buffer.WriteString("格构铁塔制造助手");
#elif defined(__TSA_FILE_)
			buffer.WriteString("铁塔内力分析设计");
#elif defined(__TDA_FILE_)
			buffer.WriteString("铁塔绘图助手");
#else //defined(__LDS_FILE_)||defined(__LDS_)
			buffer.WriteString("格构铁塔设计与绘图系统");
#endif
			//ar <<m_sDocType;	//直接用字符串代替此变量，以便减少黑客攻击导致数据破坏的可能性　wjh 2011.12.2
			//if(g_sysPara.uFileSaveVersion>0&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
			//{
			//	ToStringVersion(g_sysPara.uFileSaveVersion,file_version);
			//	g_sysPara.uFileSaveVersion=0;	//为避免错误存储为旧版本，每次执行完旧版存储立即恢复uFileSaveVersion=0;
			//}
			//else
				strcpy(Ta.version,file_version);
			buffer.WriteString(file_version);
			buffer.WriteDword(Ta.user_pipeline_no);
			buffer.WriteDword(cursor_pipeline_no);
			CBuffer towerBuffer(10000000);
			Ta.ToBuffer(towerBuffer,DogSerialNo(),file_version);
			bool bEncryptByAES=false;
#if defined(__TMA_FILE_)
			if(compareVersion(file_version,"4.1.3.0")>=0)
				bEncryptByAES=true;
#elif defined(__LMA_FILE_)
			if(compareVersion(file_version,"1.1.3.0")>=0)
				bEncryptByAES=true;
#elif defined(__TSA_FILE_)
			if(compareVersion(file_version,"2.1.0.0")>=0)
				bEncryptByAES=true;
#else//#elif defined(__TDA_FILE_)||defined(__LDS_FILE_)
			if(compareVersion(file_version,"1.3.0.0")>=0)
				bEncryptByAES=true;
#endif
			EncryptBuffer(towerBuffer,bEncryptByAES);
			DWORD dwFileLen = towerBuffer.GetLength();
			buffer.Write(&dwFileLen,sizeof(DWORD));
			buffer.Write(towerBuffer.GetBufferPtr(),dwFileLen);
#if defined(__PART_DESIGN_INC_)//||defined(__SMART_DRAW_INC_)
			//需要进行版本日志记录时应生成最新的模型修改日志数据
			if(g_sysPara.m_bSaveLogVersion)
			{
				CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
				CTower logTower;
				if(pLogModel->GetLength()>0)
				{
					logTower.FromBuffer(*pLogModel,0,true);
					Ta.CompareVersion(&logTower,&logger);
					CAttachBuffer* pLoggerBuffer=Ta.AttachBuffer.GetValue(CTower::LOGGER);
					logger.ToBuffer(*pLoggerBuffer);
				}
				CLogViewerDlg *pLogViewerDlg=((CMainFrame*)AfxGetMainWnd())->GetLogViewerPage();
				if (pLogViewerDlg)
					pLogViewerDlg->RefreshLogVersionList();
			}
#endif
#ifdef __TMA_
			if(PRODUCT_FUNC::IsHasInternalTest())
			{	//记录铁塔数据模型校审项
				CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
				pCheckBuffer->ClearContents();
				xDataCheckModel.ToBuffer(*pCheckBuffer);
			}
#endif
			CBuffer attachBuffer(1000000);
			Ta.ToAttachBuffer(attachBuffer);
			buffer.WriteInteger(attachBuffer.GetLength());	//附加缓存整体长度
			buffer.Write(attachBuffer.GetBufferPtr(),attachBuffer.GetLength());
			Ta.SetDocModifiedFlag(FALSE);
			//ResetAutoSaveTimer();
		}
		else// if(ar.IsLoading())
		{
			CXhChar50 sDocTypeName,sFileVersion;
			theApp.GetFileVersion(m_sFileVersion);
			buffer.ReadString(sDocTypeName);
#if   defined(__TMA_FILE_)
			if(!sDocTypeName.EqualNoCase("铁塔制造助手"))
#elif defined(__LMA_FILE_)
			if(!sDocTypeName.EqualNoCase("铁塔制造助手")&&!sDocTypeName.EqualNoCase("格构铁塔制造系统")&&!sDocTypeName.EqualNoCase("格构铁塔制造助手"))
#elif defined(__TSA_FILE_)
			if(!sDocTypeName.EqualNoCase("铁塔应力设计") !=0&&sDocTypeName.EqualNoCase("铁塔内力分析设计"))
#elif defined(__TDA_FILE_)
			if(!sDocTypeName.EqualNoCase("铁塔绘图助手")&&!sDocTypeName.EqualNoCase("铁塔制造助手")&&
				!sDocTypeName.EqualNoCase("格构铁塔制造系统")&&!sDocTypeName.EqualNoCase("格构铁塔制造助手"))
#else //defined(__LDS_FILE_)||defined(__LDS_)
			//theApp.m_bAnalysisWorkEnv=TRUE;
			if(theApp.m_eEnvType==SOLID_ENV)
				theApp.m_eEnvType=LINE_ENV;	//防止当前为显示实体状态时打开文件后，无法显示单线问题 wjh-2013.4.6
			g_pSolidSet->SetDisplayType(DISP_LINE);	//受力分析模式下显示单线
			if( !sDocTypeName.EqualNoCase("铁塔制造助手")&&
				!sDocTypeName.EqualNoCase("铁塔绘图助手")&&
				!sDocTypeName.EqualNoCase("格构铁塔制造系统")&&!sDocTypeName.EqualNoCase("格构铁塔制造助手")&&
				!sDocTypeName.EqualNoCase("格构铁塔设计与绘图系统"))
#endif
			{
				SetTitle("无标题");//throw CArchiveException(CArchiveException::badClass);
				throw "非预期的文件格式，文件打开失败!";
			}
			int doc_type=0;
			if(sDocTypeName.EqualNoCase("铁塔制造助手"))
				doc_type=1;
			else if(sDocTypeName.EqualNoCase("格构铁塔制造系统")||sDocTypeName.EqualNoCase("格构铁塔制造助手"))
				doc_type=2;
			else if(sDocTypeName.EqualNoCase("铁塔应力设计")||sDocTypeName.EqualNoCase("铁塔内力分析设计"))
				doc_type=3;
			else if(sDocTypeName.EqualNoCase("铁塔绘图助手"))
				doc_type=5;
			else //defined(__LDS_FILE_)||defined(__LDS_)
				doc_type=4;

			buffer.ReadString(sFileVersion);
			CXhChar16 productfile_version("%s",m_sFileVersion);
			_snprintf(Ta.version,19,"%s",(char*)sFileVersion);
#if defined(__TMA_FILE_)
			if(compareVersion(Ta.version,productfile_version)>0)
#elif defined(__LMA_FILE_)
			if(doc_type==2&&compareVersion(Ta.version,productfile_version)>0||
				(doc_type==1&&compareVersion(Ta.version,"4.2.2.0")>0))
#elif defined(__TDA_FILE_)
			if(doc_type==5&&compareVersion(Ta.version,productfile_version)>0||
				(doc_type==2&&compareVersion(Ta.version,"1.2.2.0")>0)||
				(doc_type==1&&compareVersion(Ta.version,"4.2.2.0")>0))
#elif defined(__LDS_FILE_)
			if(doc_type==4&&compareVersion(Ta.version,productfile_version)>0||
				(doc_type==5&&compareVersion(Ta.version,"1.2.2.0")>0)||
				(doc_type==2&&compareVersion(Ta.version,"1.2.2.0")>0)||
				(doc_type==1&&compareVersion(Ta.version,"4.2.2.0")>0))
#else
			if(compareVersion(Ta.version,productfile_version)>0)
#endif
				throw "此文件版本号太高,不能直接读取此文件";
			DWORD buffer_len;
#if defined(__TSA_FILE_)
			if(compareVersion(Ta.version,"1.7.0.0")<0)
				LoadTsaFile(ar);
			else
			{
#endif
				buffer.ReadDword(&Ta.user_pipeline_no);
				buffer.ReadDword(&cursor_pipeline_no);
				buffer.ReadDword(&buffer_len);
				BYTE *pBuf = new BYTE[buffer_len];
				buffer.Read(pBuf,buffer_len);
				CBuffer towerBuffer(buffer_len);
				towerBuffer.Write(pBuf,buffer_len);
				delete []pBuf;
				if((Ta.user_pipeline_no=DogSerialNo())==0)
					throw "找不到加密狗,无法打开数据文件!";

#if defined(__TMA_FILE_)
				if(compareVersion(Ta.version,"4.0.0.19")<0)
#elif defined(__LMA_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#elif defined(__TDA_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#elif defined(__LDS_FILE_)
				if(doc_type==2&&compareVersion(Ta.version,"1.0.0.73")<0||
					(doc_type==1&&compareVersion(Ta.version,"4.0.0.19")<0))
#endif
				{
					CXhChar50 version_convert_exefile(theApp.execute_path);
					if(doc_type==1)
						version_convert_exefile.Append("TMAVersion.exe");
					else //if(doc_type==2)
						version_convert_exefile.Append("LMAVersion.exe");
					PutMessage(theApp.m_pMainWnd,"正在进行旧格式文件版本转换，请耐心等待!");
					if(!VersionOldLTMAFile(theApp.m_sLicFile,version_convert_exefile,towerBuffer))
					{
						PutMessage(theApp.m_pMainWnd,"");
						throw "L/TMA旧文件转换失败!";
					}
					PutMessage(theApp.m_pMainWnd,"");
				}
				bool bEncryptByAES=false;
				if(doc_type==1&&compareVersion(Ta.version,"4.1.3.0")>=0)
					bEncryptByAES=true;
				if(doc_type==2&&compareVersion(Ta.version,"1.1.3.0")>=0)
					bEncryptByAES=true;
				if(doc_type==3&&compareVersion(Ta.version,"2.1.0.0")>=0)
					bEncryptByAES=true;
				if((doc_type==4||doc_type==5)&&compareVersion(Ta.version,"1.3.0.0")>=0)
					bEncryptByAES=true;
				DecryptBuffer(towerBuffer,bEncryptByAES,cursor_pipeline_no);
				if(!Ta.FromBuffer(towerBuffer,doc_type))
					DisplayProcess(100);
				//如果有附加缓存区
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
				DWORD dwCurPos=(DWORD)buffer.GetCursorPosition();
				DWORD dwFileLen=(DWORD)buffer.GetLength();
				if(dwFileLen>dwCurPos&&compareVersion(Ta.version,"1.2.0.3")<0)
				{	//有附加缓存区时读取智能绘图缓存区数据
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
					CAttachBuffer* pDrawBuff=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
					CXhChar50 bufname;
					buffer.ReadString(bufname);
					DWORD buf_len=0;
					buffer.ReadDword(&buf_len);	//BOM_CONFIG+SMART_DRAW两个缓存总长
					buffer.ReadDword(&buf_len);	//BOMCONFIG 缓存长度
					pBomBuff->SetBlockSize(buf_len);
					char* buf=new char[buf_len];
					buffer.Read(buf,buf_len);
					pBomBuff->Write(buf,buf_len);
					delete []buf;
					buffer.ReadDword(&buf_len);	//SMART_DRAW 缓存长度
					pDrawBuff->SetBlockSize(buf_len);
					buf=new char[buf_len];
					buffer.Read(buf,buf_len);
					pDrawBuff->Write(buf,buf_len);
					delete []buf;
					//ar.Write(m_attachBuffer.GetBufferPtr(),m_attachBuffer.GetLength());
				}
				else
#endif

					if((doc_type==5&&compareVersion(Ta.version,"1.2.0.3")>=0)||
						(doc_type==4&&compareVersion(Ta.version,"1.2.0.3")>=0)||
						(doc_type==3&&compareVersion(Ta.version,"2.0.0.3")>=0)||
						(doc_type==2&&compareVersion(Ta.version,"1.2.0.3")>=0)||
						(doc_type==1&&compareVersion(Ta.version,"4.2.0.3")>0))
					{
						DWORD buf_len=0;
						DWORD dwRead=buffer.Read(&buf_len,4);
						if(dwRead==4&&buf_len>0)
						{
							CHAR_ARRAY buf(buf_len);
							CBuffer attachBuffer(buf,buf_len);
							buffer.Read(buf,buf_len);
							Ta.FromAttachBuffer(attachBuffer);
						}
					}
#if defined(__PART_DESIGN_INC_)//||defined(__SMART_DRAW_INC_)
					//记录原始铁塔数据模型以便比对记录改动日志
					CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
					pLogModel->ClearContents();
					pLogModel->SetBlockSize(towerBuffer.GetLength());
					Ta.ToBuffer(*pLogModel,DogSerialNo(),file_version);
					CBuffer* pLoggerBuf=Ta.AttachBuffer.GetValue(CTower::LOGGER);
					logger.FromBuffer(*pLoggerBuf);
#endif
#ifdef __TMA_
					if(PRODUCT_FUNC::IsHasInternalTest())
					{	//记录铁塔数据模型校审项
						CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
						if(pCheckBuffer&&pCheckBuffer->GetLength()>0)
						{
							pCheckBuffer->SeekToBegin();
							xDataCheckModel.FromBuffer(*pCheckBuffer);
						}
					}
#endif
#if defined(__TSA_FILE_)||defined(__TSA_)
			}
#endif	
			SetModifiedFlag(FALSE);
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		DisplayProcess(100,NULL);
	}
}
#endif
