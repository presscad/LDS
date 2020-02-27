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
//���һ�����ݵ�Excel
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
//��ȡһ�����ݴ�Excel
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
	m_sDocType = "������������";
#elif defined(__LMA_FILE_)||defined(__LMA_)
	m_sDocType = "��������������";
#elif defined(__TSA_FILE_)||defined(__TSA_)
	m_sDocType = "���������������";	//������������������
#else //defined(__LDS_FILE_)||defined(__LDS_)
	m_sDocType = "������������ͼϵͳ";	//��������ͼ���������
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
		AfxMessageBox("�Ҳ������ܹ�,�޷���ȷ�������ļ�!");
#endif
		return FALSE;
	}
	if (!CDocument::OnNewDocument())
		return FALSE;
	UpdateAllViews(NULL);

#ifdef AFX_TARG_ENU_ENGLISH
	CLDSDoc::SetTitle("No Title");
#else 
	CLDSDoc::SetTitle("�ޱ���");
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
		"����������������ļ�(*.tsa)|*.tsa|�����ļ�(*.*)|*.*||");
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
		if(sDocTypeName.Compare("����Ӧ�����") !=0&&sDocTypeName.Compare("���������������") !=0)
		{
			SetTitle("�ޱ���");
			throw "��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!";
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
			throw "���ļ��汾��̫�߻�̫��,����ֱ�Ӷ�ȡ���ļ�";
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
/* �����޶��������������ļ���
void SaveAsFile(CBuffer& buffer)
{
	char* sFileVersion="4.0.1.11";
	int part_n_arr[7]={2991,2992,2993,2994,2995,2996,2997};
	int pos_arr[7]={2378775,2379105,2379449,2380822,2382211,2382235,2383624};
	for(int i=0;i<7;i++)
	{
		char filename[20];
		sprintf(filename,"c:\\rev%d.tma",i);
		char doctype[50]="������������";
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
				g_sysPara.iFileSaveDocType=0;	//Ϊ�������洢Ϊ�����ļ���ʽ��ÿ��ִ�������������ʽ�������ָ�iFileSaveDocType=0;
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
				ar<<CString("������������");
			else if(doc_type==2)
				ar<<CString("��������������");
			else if(doc_type==3)
				ar<<CString("���������������");
			else if(doc_type==5)
				ar<<CString("������ͼ����");
			else if(doc_type==4)
				ar<<CString("������������ͼϵͳ");
			//ar <<m_sDocType;	//ֱ�����ַ�������˱������Ա���ٺڿ͹������������ƻ��Ŀ����ԡ�wjh 2011.12.2
#ifdef __TMA_
			DWORD dwSaveFileVersion=ValidateLocalizeFeature(FEATURE::SAVEAS_FILE_VERSION);
#else
			DWORD dwSaveFileVersion=0;
#endif
			if(g_sysPara.uFileSaveVersion>0&&(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT)||dwSaveFileVersion>0))
			{
				ToStringVersion(g_sysPara.uFileSaveVersion,file_version);
				g_sysPara.uFileSaveVersion=0;	//Ϊ�������洢Ϊ�ɰ汾��ÿ��ִ����ɰ�洢�����ָ�uFileSaveVersion=0;
			}
			else if(!bSpecifiedVersion)
				strcpy(Ta.version,file_version);
			char bEncryptByAES=false;
			if(doc_type==1)
			{
				if(compareVersion(file_version,"5.1.7.0")>=0)
					bEncryptByAES=2;	//��ָ�Ƶļ���ģʽ
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
			//ͨ��EncryptFileEx����õ���dwSafeBuffSize��֤����ת��ʱ���ڴ�鳤�Ȳ���
			//���򳬴��ļ��洢ʱ�п��ܵ����ļ��ڴ��ļ��ڽڲ������λ���� wjh-2019.8.13
			DWORD SAFE_BUFFSIZE=0xa00000;	//SAFE_BUFFSIZE������16��������
			CBuffer buffer(SAFE_BUFFSIZE);//10000000);
			//������������ڴ���Ƭ���࣬д��ʱ�ڴ����ʧ�ܽ�������ϵͳ�쳣�ĵ�д��ʧ�ܵ���� wjh-2019.6.21
			//���������д��ʱ�Ķ��⸨���ļ�
			CXhChar500 szSaveBuffFile("%s__overflowsavebuf.tmplds",(char*)theApp.szWorkPath);
			FILE* fpOverflowBuffFile=fopen(szSaveBuffFile,"w+b");
			CSelfCloseFile closefile(fpOverflowBuffFile);
			UINT uiRWBuffSize= AfxGetApp()->GetProfileInt("Settings","FileRW_BUFFSIZE",50);
			UINT FileRW_BUFFSIZE=0x100000*uiRWBuffSize;	//Ĭ��50M����ռ�
			buffer.InitOverflowBuffFile(fpOverflowBuffFile,FileRW_BUFFSIZE);	//����40M������������ļ�
			Ta.ToBuffer(buffer,DogSerialNo(),file_version,doc_type);
			UINT uiOverflowBuffBytes=buffer.DetachOverflowBuffFile();	//������������ļ�
			if (!EncryptBuffer(buffer, bEncryptByAES))
				throw "��Ȩ֤���쳣������������Ȩ������!";
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
			//��Ҫ���а汾��־��¼ʱӦ�������µ�ģ���޸���־����
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
			//д��ҵ���Ϣģ��
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
			{	//��¼��������ģ��У����
				CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
				pCheckBuffer->ClearContents();
				xDataCheckModel.ToBuffer(*pCheckBuffer);
			}
#endif
			CBuffer attachBuffer(1000000);
			Ta.ToAttachBuffer(attachBuffer);
			ar<<attachBuffer.GetLength();	//���ӻ������峤��
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
			if(sDocTypeName.Compare("������������") !=0)
#elif defined(__LMA_FILE_)
			if(sDocTypeName.Compare("������������") !=0&&sDocTypeName.Compare("����������ϵͳ") !=0&&sDocTypeName.Compare("��������������") !=0)
#elif defined(__TSA_FILE_)
			if(sDocTypeName.Compare("����Ӧ�����") !=0&&sDocTypeName.Compare("���������������") !=0)
#elif defined(__TDA_FILE_)
			if( sDocTypeName.Compare("������ͼ����") !=0&&sDocTypeName.Compare("������������")!=0&&
				sDocTypeName.Compare("����������ϵͳ") !=0&&sDocTypeName.Compare("��������������") !=0)
#else //defined(__LDS_FILE_)||defined(__LDS_)
			//theApp.m_bAnalysisWorkEnv=TRUE;
			if(theApp.m_eEnvType==SOLID_ENV)
				theApp.m_eEnvType=LINE_ENV;	//��ֹ��ǰΪ��ʾʵ��״̬ʱ���ļ����޷���ʾ�������� wjh-2013.4.6
			g_pSolidSet->SetDisplayType(DISP_LINE);	//��������ģʽ����ʾ����
			if( sDocTypeName.Compare("������������") !=0&&
				sDocTypeName.Compare("������ͼ����") !=0&&
				sDocTypeName.Compare("����������ϵͳ") !=0&&sDocTypeName.Compare("��������������") !=0&&
				sDocTypeName.Compare("������������ͼϵͳ") !=0)
#endif
			{
#ifdef AFX_TARG_ENU_ENGLISH
				SetTitle("No Title");//throw CArchiveException(CArchiveException::badClass);
				throw "This is the unexpected file type,so the file open failed!";
#else 
				SetTitle("�ޱ���");//throw CArchiveException(CArchiveException::badClass);
				throw "��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!";
#endif
			}
			int doc_type=0;
			if(sDocTypeName.Compare("������������") ==0)
				doc_type=1;
			else if(sDocTypeName.Compare("����������ϵͳ") ==0||sDocTypeName.Compare("��������������") ==0)
				doc_type=2;
			else if(sDocTypeName.Compare("����Ӧ�����") ==0||sDocTypeName.Compare("���������������") ==0)
				doc_type=3;
			else if(sDocTypeName.Compare("������ͼ����") ==0)
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
				throw "���ļ��汾��̫��,����ֱ�Ӷ�ȡ���ļ�";
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
					throw "�Ҳ������ܹ�,�޷��������ļ�!";
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
					PutMessage(theApp.m_pMainWnd,"���ڽ��оɸ�ʽ�ļ��汾ת���������ĵȴ�!");
#endif
					if(!VersionOldLTMAFile(theApp.m_sLicFile,version_convert_exefile,buffer))
					{
						PutMessage(theApp.m_pMainWnd,"");
#ifdef AFX_TARG_ENU_ENGLISH
						throw "L/TMA old file converts failure!";
#else 
						throw "L/TMA���ļ�ת��ʧ��!";
#endif
					}
					PutMessage(theApp.m_pMainWnd,"");
				}
				char bEncryptByAES=false;
				if(doc_type==1)
				{
					if(compareVersion(Ta.version,"5.1.7.0")>=0)
						bEncryptByAES=2;	//��ָ�Ƶļ���ģʽ
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
					Ta.m_uiOriginalDogKey=cursor_pipeline_no;//�ļ��������ʱ�ļ�������
					Ta.m_uiLastSaveDogKey=cursor_pipeline_no;//�ļ����һ�α���ʹ�õļ�������
				}
				CLinkJGDlg::InitPreferAngleMat(&Ta);
				//����и��ӻ�����
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
				DWORD dwCurPos=(DWORD)ar.GetFile()->GetPosition();
				DWORD dwFileLen=(DWORD)ar.GetFile()->GetLength();
				if(dwFileLen>dwCurPos&&compareVersion(Ta.version,"1.2.0.3")<0)
				{	//�и��ӻ�����ʱ��ȡ���ܻ�ͼ����������
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
					CAttachBuffer* pDrawBuff=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
					CString bufname;
					ar>>bufname;
					DWORD buf_len=0;
					ar>>buf_len;	//BOM_CONFIG+SMART_DRAW���������ܳ�
					ar>>buf_len;	//BOMCONFIG ���泤��
					pBomBuff->SetBlockSize(buf_len);
					char* buf=new char[buf_len];
					ar.Read(buf,buf_len);
					pBomBuff->Write(buf,buf_len);
					delete []buf;
					ar>>buf_len;	//SMART_DRAW ���泤��
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
				//��¼ԭʼ��������ģ���Ա�ȶԼ�¼�Ķ���־
				CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
				pLogModel->ClearContents();
				pLogModel->SetBlockSize(buffer.GetLength());
				Ta.ToBuffer(*pLogModel,DogSerialNo(),file_version);
				CBuffer* pLoggerBuf=Ta.AttachBuffer.GetValue(CTower::LOGGER);
				if(pLoggerBuf&&pLoggerBuf->Length>0)
					logger.FromBuffer(*pLoggerBuf);
				else
					logger.EmptyLogVersion();
				//�ҵ�ģ�ͻ���
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
						globalDataCmpModel.ComparePartData();	//�ӻ���������ݺ�Ӧ���¶Ա����� wht 18-11-14
					}
					else
						globalDataCmpModel.Empty();
				}
				if(PRODUCT_FUNC::IsHasInternalTest())
				{	//��¼��������ģ��У����
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
	buffer.WriteInteger(DogSerialNo());	//����
	buffer.WriteString(Ta.m_sPrjName);
	buffer.WriteString(Ta.m_sTaType);
	//׼���
	buffer.WriteWord(zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.WriteWord(zhunju_table[i].wide);	//֫��
		buffer.WriteWord(zhunju_table[i].g);	//��׼׼��g
		buffer.WriteWord(zhunju_table[i].g1);	//׼��g1
		buffer.WriteWord(zhunju_table[i].g2);	//׼��g2
		buffer.WriteWord(zhunju_table[i].g3);	//׼��g3
	}
	//��˨����
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
	//��˨���ϵ����Ϣ
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
			buffer.WriteDouble(pXingHao->grade);			//��˨����
			buffer.WriteInteger(pXingHao->d);			//��˨ֱ��
			buffer.WriteString(pXingHao->guige);			//��˨���
			buffer.WriteInteger(pXingHao->valid_len);	//��Ч��
			buffer.WriteInteger(pXingHao->no_thread_len);//�޿۳�
			buffer.WriteInteger(pXingHao->down_lim);		//��˨ͨ������
			buffer.WriteInteger(pXingHao->up_lim);		//��˨ͨ������
			buffer.WriteInteger(pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
			buffer.WriteDouble(pXingHao->weight);		//��˨��������
		}
	}
	//���Ͽ�
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
	//������
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
	//��ϣ�������Ϣ����
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
	//������Ϣ
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
		CLDSObject::Log2File()->Log("ģ�ͼ�¼������!");
#endif
	buffer.WriteInteger(Ta.m_hActiveModule);
	for(i=0;i<4;i++)
		buffer.WriteWord(Ta.m_arrActiveQuadLegNo[i]);
	//��ͼ����
	stack.Initialize(&buffer,Ta.View.GetNodeNum());
	buffer.WriteInteger(Ta.View.GetNodeNum());
	for(CDisplayView* pView=Ta.View.GetFirst();pView;pView=Ta.View.GetNext())
	{
		if(pView->m_iViewType==2)
			continue;	//��Ϲ���������ͼ��������ͼ�����棬��ʾʱ��ʱ����
		buffer.WriteInteger(pView->handle);
		pView->ToCdfBuffer(buffer);
		stack.Increment();
	}
	stack.VerifyAndRestore();
	buffer.WriteInteger(CLDSObject::_console->GetActiveView()->handle);

	//�ڵ���Ϣ
	// ------------ 1. vvvvvvvv �ڵ� --------------
	buffer.WriteInteger(Ta.Node.GetNodeNum());
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		buffer.WriteInteger(pNode->handle);
		pNode->ToCdfBuffer(buffer);
	}
	// ------------ 2. vvvvvvvv ���� --------------
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
		//   10   XYZ-�Գ�     5.100     0.300    -4.900    ����    ����    ����    ����    ����    ����  1,2
	//�˼���Ϣ
		//��񣬲��ʣ�������˨������˫֫����
	// ------------ 3. vvvvvvvv �ֹܷ��������Ϣ --------------
	buffer.WriteInteger(Ta.hashConnectFl.GetNodeNum());
	for(CConnectFL *pFL=Ta.hashConnectFl.GetFirst();pFL;pFL=Ta.hashConnectFl.GetNext())
	{
		buffer.WriteDouble(pFL->m_fThickTube);			//���ܱں�(mm)
		buffer.WriteDouble(pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
		buffer.WriteDouble(pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
		buffer.WriteString(pFL->bolt_grade);
		buffer.WriteInteger(pFL->bolt_d);			//��˨ֱ�������
		buffer.WriteInteger(pFL->bolt_n);			//��˨���е���˨����
		buffer.WriteDouble(pFL->N);	//N ������(N)
		buffer.WriteDouble(pFL->M);	//M���(N*m)
		buffer.WriteDouble(pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
		buffer.WriteInteger(pFL->m_nThickFL);		//�����̺��(mm)
		buffer.WriteInteger(pFL->m_nWidthFL);		//�������δ���(mm)
		buffer.WriteInteger(pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
		buffer.WriteInteger(pFL->m_nHeightRib);		//�߰�߶�(mm)
		buffer.WriteInteger(pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
		buffer.WriteInteger(pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
		buffer.WriteInteger(pFL->m_nCoupleThickFL);	//�����̺��(mm)
		buffer.WriteInteger(pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
		buffer.WriteByte(pFL->cMaterial);
		buffer.WriteString(pFL->m_sPartNo);     //�������
	}
#ifdef __COMPLEX_PART_INC_
	//���沿������������ wht 12-09-10
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
	//�����������ϵ	wht 13-01-06
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
	CFileDialog dlg(TRUE,"cdf","����",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"˾��ͼģ��(*.cdf)|*.cdf|�����ļ�(*.*)|*.*||");
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
		AfxMessageBox("�����˾��ͼ��ʽ");
		return;
	}
	buffer.ReadString(str50,51);//version,"1.0");
	long liFileVersion=FromStringVersion(str50);
	int serial=0;
	buffer.ReadInteger(&serial);	//����
	buffer.ReadString(str50,51);	//Ta.m_sPrjName
	buffer.ReadString(str50,51);	//Ta.m_sTaType
	//׼���
	WORD wCount=0;
	buffer.ReadWord(&wCount);
	DYN_ARRAY<ANGLE_G_REC> garr(wCount);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&garr[i].wide);	//֫��
		buffer.ReadWord(&garr[i].g);	//��׼׼��g
		buffer.ReadWord(&garr[i].g1);	//׼��g1
		buffer.ReadWord(&garr[i].g2);	//׼��g2
		buffer.ReadWord(&garr[i].g3);	//׼��g3
		//TODO:��zhunju_arr���бȶ�
	}
	//��˨����
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
	//��˨���ϵ����Ϣ
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
			buffer.ReadDouble(&pXingHao->grade);			//��˨����
			buffer.ReadInteger(&pXingHao->d);			//��˨ֱ��
			buffer.ReadString(pXingHao->guige);			//��˨���
			buffer.ReadInteger(&pXingHao->valid_len);	//��Ч��
			buffer.ReadInteger(&pXingHao->no_thread_len);//�޿۳�
			buffer.ReadInteger(&pXingHao->down_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->up_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);*/		//��˨��������
		}
	}
	//���Ͽ�
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
	//������
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
	//��ϣ�������Ϣ����
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
			cls_typeid==CLS_LINETUBE||cls_typeid==CLS_LINEPART)	//CLDSPart���͹���
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
	//������Ϣ
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
	//��ͼ����
	buffer.ReadInteger(&uiCount);
	for(i=0;i<uiCount;i++)
	{
		CDisplayView view;//=Ta.View.append(FALSE);
		buffer.ReadInteger(&view.handle);
		view.FromCdfBuffer(buffer);
	}
	long hActiveView=0;
	buffer.ReadInteger(&hActiveView);

	//�ڵ���Ϣ
	// ------------ 1. vvvvvvvv �ڵ� --------------
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
	// ------------ 2. vvvvvvvv ���� --------------
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
		//   10   XYZ-�Գ�     5.100     0.300    -4.900    ����    ����    ����    ����    ����    ����  1,2
	//�˼���Ϣ
		//��񣬲��ʣ�������˨������˫֫����
	// ------------ 3. vvvvvvvv �ֹܷ��������Ϣ --------------
	/*buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//���ܱں�(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
		buffer.ReadString(pFL->bolt_grade);
		buffer.ReadInteger(&pFL->bolt_d);			//��˨ֱ�������
		buffer.ReadInteger(&pFL->bolt_n);			//��˨���е���˨����
		buffer.ReadDouble(&pFL->N);	//N ������(N)
		buffer.ReadDouble(&pFL->M);	//M���(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//�������δ���(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
		buffer.ReadInteger(&pFL->m_nHeightRib);		//�߰�߶�(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //�������
	}
#ifdef __COMPLEX_PART_INC_
	//���沿������������ wht 12-09-10
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
	//�����������ϵ	wht 13-01-06
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		CParaCS *pCS=Ta.Acs.Add(h);
		pCS->FromBuffer(buffer);
	}	
	Ta.GetMinAvailableHandle();*/
	//��ȡ���ӵ��ֶ�ӳ��ڵ㼯��
	CHashList<long> hashPointIById;	//��ֵΪ��ģ���нڵ�ľ��������Ϊ˾��ͼģ���еĽڵ���
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
	Input("�����У��Ĺ����κŷ�Χ:",&segIncStr,'s');
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
		AfxMessageBox("�����˾��ͼ��ʽ");
		return;
	}
	buffer.ReadString(str50,51);//version,"1.0");
	int serial=0;
	buffer.ReadInteger(&serial);	//����
	buffer.ReadString(Ta.m_sPrjName,51);
	buffer.ReadString(Ta.m_sTaType,51);
	//׼���
	buffer.ReadWord(&zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&zhunju_table[i].wide);	//֫��
		buffer.ReadWord(&zhunju_table[i].g);	//��׼׼��g
		buffer.ReadWord(&zhunju_table[i].g1);	//׼��g1
		buffer.ReadWord(&zhunju_table[i].g2);	//׼��g2
		buffer.ReadWord(&zhunju_table[i].g3);	//׼��g3
	}
	//��˨����
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
	//��˨���ϵ����Ϣ
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
			buffer.ReadDouble(&pXingHao->grade);			//��˨����
			buffer.ReadInteger(&pXingHao->d);			//��˨ֱ��
			buffer.ReadString(pXingHao->guige);			//��˨���
			buffer.ReadInteger(&pXingHao->valid_len);	//��Ч��
			buffer.ReadInteger(&pXingHao->no_thread_len);//�޿۳�
			buffer.ReadInteger(&pXingHao->down_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->up_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);		//��˨��������
		}
	}
	//���Ͽ�
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
	//������
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
	//��ϣ�������Ϣ����
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
		else	//CLDSPart���͹���
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
	//������Ϣ
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
	//��ͼ����
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

	//�ڵ���Ϣ
	// ------------ 1. vvvvvvvv �ڵ� --------------
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
	// ------------ 2. vvvvvvvv ���� --------------
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&h);
		//if(hashObjs.GetHashTableSize()>0&&hashObjs.GetValueAt(h,(CLDSObject*&)pPart))
		if((pPart=Ta.Parts.FromHandle(h))!=NULL)
			((CLDSLinePart*)pPart)->FromCdfBuffer(buffer);//,nVersion,doc_type);
	}
		//   10   XYZ-�Գ�     5.100     0.300    -4.900    ����    ����    ����    ����    ����    ����  1,2
	//�˼���Ϣ
		//��񣬲��ʣ�������˨������˫֫����
	// ------------ 3. vvvvvvvv �ֹܷ��������Ϣ --------------
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//���ܱں�(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
		buffer.ReadString(pFL->bolt_grade);
		buffer.ReadInteger(&pFL->bolt_d);			//��˨ֱ�������
		buffer.ReadInteger(&pFL->bolt_n);			//��˨���е���˨����
		buffer.ReadDouble(&pFL->N);	//N ������(N)
		buffer.ReadDouble(&pFL->M);	//M���(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//�������δ���(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
		buffer.ReadInteger(&pFL->m_nHeightRib);		//�߰�߶�(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //�������
	}
#ifdef __COMPLEX_PART_INC_
	//���沿������������ wht 12-09-10
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
	//�����������ϵ	wht 13-01-06
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
		AfxMessageBox("ģ�͵�ǰ������߻�ȡʧ��");
		return;
	}
	CXhChar16 szFormatType = ciMode3DS0_STL1 == 0 ? "����3DSģ���ļ�" : "����STLģ���ļ�";
	CXhChar16 szPromptTitle = ciMode3DS0_STL1 == 0 ? "����3DSģ���ļ�" : "����STLģ���ļ�";
	pWorkingHeight->GetModuleScopeZ();
	//��ʼ��GIM����ϵ
	UCS_STRU stdcs;
	LoadDefaultUCS(&stdcs);
	UCS_STRU draw_ucs;
	draw_ucs.origin.Set(0, 0, pWorkingHeight->LowestZ());
	draw_ucs.axis_x.Set(1, 0, 0);
	draw_ucs.axis_y.Set(0, -1, 0);
	draw_ucs.axis_z.Set(0, 0, -1);
	//������άʵ��Ļ�����������Ϣ
	int serial = 1, liCountOfParts = Ta.Parts.GetNodeNum();
	I3DSData* p3dsFile = ciMode3DS0_STL1==0?C3DSFactory::Create3DSInstance():NULL;
	IStlData* pStlFile = ciMode3DS0_STL1 == 1 ? CStlFactory::CreateStl() : NULL;
	DisplayProcess(0, szPromptTitle);
	static const int UNIT_3DS_INCH	= 0;
	static const int UNIT_3DS_M		= 1;
	static const int UNIT_3DS_MM	= 2; //mm
	int UNIT_3DS_TYPE = UNIT_3DS_M;
	static char _localSolidBufPool[0x200000];	//���ʵ�建��2M
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
	DisplayProcess(0, "����װ�䲿������άģ����Ϣ....");
	for (CBlockReference *pBlockRef = Ta.BlockRef.GetFirst();pBlockRef;pBlockRef = Ta.BlockRef.GetNext())
	{
		CBlockModel *pBlock = Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if (pBlock == NULL)
			continue;
		UCS_STRU acs = pBlockRef->GetACS();
		UCS_STRU cs = pBlock->TransToACS(acs);
		PARTSET partSet;
		if (pBlock->IsEmbeded())
		{	//Ƕ��ʽ����
			for (CLDSPart *pPart = Ta.EnumPartFirst();pPart;pPart = Ta.EnumPartNext())
			{
				if (pPart->m_hBlock == pBlock->handle)
					partSet.append(pPart);
			}
		}
		else
		{	//��Ƕ��ʽ����
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
	//MessageBox("3DS�ļ����ɹ�");
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
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�ƽ����� wjh-2019.9.25
	CLogErrorLife life(&logerr);
	CWaitCursor waitCursor;
	//����Ƿ����MOD�ļ���������������MOD�ļ�
	char drive[4];
	char dir[MAX_PATH],fname[MAX_PATH],folder[MAX_PATH];
	_splitpath(lpszPathName,drive,dir,fname,NULL);
	strcpy(folder,drive);
	strcat(folder,dir);
	CXhChar500 mod_file("%s%s.mod",folder,fname);
	FILE* fp=fopen(mod_file,"rb");
#ifdef TEMP_HENAN_LOG
	logerr.Log("mod�ļ�����Ŀ¼:%s", (char*)mod_file);
	logerr.Log("mod-fp=%8X", fp);
#endif
	if(fp==NULL)
	{	//����MOD�ļ�
		CModDataFile modfile;
		LOCALE_PROPITEM* pLocaleItem=gxPortalLocalizer.GetLocaleItem("ExportTurnLeftGimFile");
		if (pLocaleItem&&pLocaleItem->bVal&&
			AfxMessageBox("����ģ���Ƿ������ת��", MB_YESNO) == IDYES)
			modfile.m_bTurnLeft = TRUE;
		modfile.ExportModFile(mod_file);
#ifdef TEMP_HENAN_LOG
		logerr.Log("����ExportModFile");
#endif
	}
	else
		fclose(fp);
	//�����Ի���¼��GIMͷ��Ϣ�͸���������Ϣ
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
	gim_file.PackGimFile();		//����ѹ�����
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step4");
#endif
	gim_file.UpdateGimFile();	//����GIMͷ����Ϣ
#ifdef TEMP_HENAN_LOG
	logerr.Log("run to step5");
#endif
#endif
}

void CLDSDoc::OnTowerGimProperty()
{
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�ƽ����� wjh-2019.9.25
	//�����Ի���¼��GIMͷ��Ϣ�͸���������Ϣ
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
		//ִ��Ta.Empty()ʱ�ѽ����е�Blockɾ��,�˴�Ӧ����ǰ����ģ���л�ΪTa,����Ĭ�ϼ���ģ��ΪBlockʱ����� wht 12-10-29
		console.ActivateModel(&Ta);	
		console.InitTowerModel();
#if defined(__PART_DESIGN_INC_)
		gxWirePointModel.Empty();
		try{
			postreport.Empty();
			logger.EmptyLogVersion();
			//TODO:���������ڷ���ִ��ʱ���������쳣 wjh-2019.5.28
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
		//�����ʾ��������ڴ�������
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
		AfxMessageBox("�ļ����зǷ����ݣ����ܴ���!");
#endif
		return FALSE;
	}
	CXhChar200 filepathname=lpszPathName;
#ifdef __LDS_FILE_
	//��ֹ�û�����TTA���ݻ�MYL���ݺ�ֱ�ӵ㱣��ʱ��ԭʼ�����ļ���� wjh-2018.5.5
	char* pszDotExt=SearchChar(filepathname,'.',true);
	if(pszDotExt==NULL)
		filepathname.Append(".lds");
	if (pszDotExt != NULL)
	{
		if (stricmp(pszDotExt, ".mod") == 0)
			return FALSE;	//�򿪵�MOD�ļ������б���
		if (stricmp(pszDotExt, ".myl") == 0 || stricmp(pszDotExt, ".dat") == 0 || stricmp(pszDotExt, ".tans") == 0)
			strcpy(pszDotExt, ".lds");
	}
#endif
	return CDocument::OnSaveDocument(filepathname);
}
void CorrectFileError(CString sFilePath);
BOOL CLDSDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	theApp.work_path[0]=0;	//�����ļ�ʱӦ���֮ǰ���ļ�����Ŀ¼������ᵼ��theApp.szWorkPath������ wjh-2019.9.22
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
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�ƽ����� wjh-2019.9.25
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
			//��¼GIM��Ϣ�͸���������Ϣ
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
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�����������ģ�͹��� wjh-2019.9.25
#if defined(__PART_DESIGN_INC_)||defined(__RAPID_ROUGH_3DMODEL_)
	CLDSDoc *pDoc=theApp.GetLDSDoc();
	CXhChar500 filter= "TTA�����ļ�(*.*)|*.*|"\
		"MYL�����ļ�(*.myl)|*.myl|"\
		"SmartTower�����ļ�(*.dat)|*.dat|"\
		"TANS�����ļ�(*.*)|*.*||";
	//"����Ժ�����ļ�(*.dat)|*.dat|"
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
	DeleteContents();	//��������DoModal()֮�󣬷�������û�ֻҪִ�д�����ͻ�ɾ���ĵ� wjh-2019.11.29
	char ciFileFormatType = -1;
	CXhChar50 szCurrFormat = xarrFileFormats[dlg.m_ofn.nFilterIndex - 1];
	if (strstr(szCurrFormat, "TTA") != NULL)	//"������"
		ciFileFormatType = -1;// CTtaDataFile::FILETYPE_TTA;��ʱ��������ļ���չ���ж��ļ���ʽ���� wjh-2019.7.10
	else if (strstr(szCurrFormat, "TANS") != NULL)	//"������"
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
	CXhChar500 filter("TTA�����ļ��㼰����ļ�(*.*)|*.*||");
	CFileDialog dlg(TRUE,"tower","LDS",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,filter);

	DWORD nFileNumbers = 100;    //CFileDialog���ѡ���ļ�����
	TCHAR *pBufOld = dlg.m_ofn.lpstrFile;   //����ɵ�ָ��
	DWORD dwMaxOld = dlg.m_ofn.nMaxFile;    //����ɵ�����
	dlg.m_ofn.lpstrFile = new TCHAR[ nFileNumbers * MAX_PATH];
	ZeroMemory(dlg.m_ofn.lpstrFile,sizeof(TCHAR) * nFileNumbers * MAX_PATH);
	dlg.m_ofn.nMaxFile = nFileNumbers * MAX_PATH;    //�������,��MSDN����ΪfileDlg.m_ofn.lpstrFileָ��Ļ��������ַ���
	if(dlg.DoModal()!=IDOK)
		return;
	CXhSimpleList<CXhChar200> listBraceFiles;
	POSITION pos=dlg.GetStartPosition();
	while(pos)
	{
		CString sPathName=dlg.GetNextPathName(pos);		//��ȡ�ļ��� 
		listBraceFiles.Append(CXhChar200(sPathName));
	}
	delete [](dlg.m_ofn.lpstrFile);        //������ڴ�
	dlg.m_ofn.lpstrFile = pBufOld;        //��ԭ֮ǰ��ָ��,��Ϊû�������CFileDialogԴ�����,��������Ƿ�����,�����Ȼ�ԭ�ϰ�,
	dlg.m_ofn.nMaxFile = dwMaxOld;        //��ԭ֮ǰ�������
	CUndoOperObject undo(&Ta,true);
	for(CXhChar200* pszFile=listBraceFiles.EnumObjectFirst();pszFile;pszFile=listBraceFiles.EnumObjectNext())
	{
		CTtaDataFile bracetta;
		bracetta.ReadBraceElemsDataFile(*pszFile);
	}
	//�Զ�����ѧ����ת��Ϊʵ��˼�
	logerr.GlobalEnabled(false);
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->GetClassTypeId()!=pRod->size_idClassType&&pRod->size_idClassType!=0)			//ת����������
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
	m_bStartAutoSaveThread=true;	//�����Զ����̹���
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
		"������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(TRUE,"lma","LMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"��������������(*.lma)|*.lma|������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"������ͼ����(*.tda)|*.tda|��������������(*.lma)|*.lma|������������(*.tma)|*.tma|˾��ͼģ��(*.cdf)|*.cdf|�����ļ�(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CXhChar500 filter("������ƻ�ͼһ�廯���(*.lds)|*.lds");
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("|����������������ļ�(*.tsa)|*.tsa|ԭʼ�����Ϣ�ļ�(*.tdf)|*.tdf");
	//filter.Append("|TTA�ӿ����������ļ�(*.ini)|*.ini");
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		filter.Append("|�����ƽ�����ģ���ļ�(*.mod)|*.mod");
		filter.Append("|�����ƽ�GIM�ļ�(*.gim)|*.gim");
		filter.Append("|������������(*.tma)|*.tma|��������������(*.lma)|*.lma|������ͼ����(*.tda)|*.tda|˾��ͼģ��(*.cdf)|*.cdf");
	}
	filter.Append("|�����ļ�(*.*)|*.*||");
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
#elif defined(__TSA_FILE_)
	CFileDialog dlg(TRUE,"tsa","TSA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"����������������ļ�(*.tsa)|*.tsa|ԭʼ�����Ϣ�ļ�(*.tdf)|*.tdf|�����ļ�(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	//CorrectFileError(dlg.GetPathName());
	OnOpenDocument(dlg.GetPathName());
	//���ֱ�ӴӲ˵���ѡ���������ļ���ʱ��ϵͳ�ڵ�����OnOpenDocument��
	//���Զ�����CView::OnInitialUpdate����������ǰ��OnFileOpen�в��ᴥ���˺�������˱���
	//��ʾ�������º�����������ͼ
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
		"������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(TRUE,"lma","LMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"��������������(*.lma)|*.lma|������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"������ͼ����(*.tda)|*.tda|��������������(*.lma)|*.lma|������������(*.tma)|*.tma|˾��ͼģ��(*.cdf)|*.cdf|�����ļ�(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CXhChar500 filter("������ƻ�ͼһ�廯���(*.lds)|*.lds");
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("|����������������ļ�(*.tsa)|*.tsa|ԭʼ�����Ϣ�ļ�(*.tdf)|*.tdf|TTA�ӿ����������ļ�(*.ini)|*.ini");
	if(!theApp.m_bAnalysisWorkEnvOnly)
		filter.Append("|������������(*.tma)|*.tma|��������������(*.lma)|*.lma|������ͼ����(*.tda)|*.tda|˾��ͼģ��(*.cdf)|*.cdf");
	filter.Append("|�����ļ�(*.*)|*.*||");
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
//#elif defined(__TSA_FILE_)
//	CFileDialog dlg(TRUE,"tsa","TSA",
//		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
//		"����������������ļ�(*.tsa)|*.tsa|ԭʼ�����Ϣ�ļ�(*.tdf)|*.tdf|TTA�ӿ����������ļ�(*.ini)|*.ini|�����ļ�(*.*)|*.*||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	CString szNewFilePathName=dlg.GetPathName();
	//�����ǰ���д��ļ�������������һ��exe�����ļ�����
	CXhChar500 exe_path("%s%s.exe ",APP_PATH,theApp.m_pszExeName);
	exe_path.Append(szNewFilePathName,' ');
	WinExec(exe_path,SW_SHOW);
}
void CLDSDoc::OnFileSave() 
{
	CLDSView* pWorkView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pWorkView->SaveCursorView();
	if(theApp.m_bCooperativeWork)
	{	//������Эͬ��ʽ����
		BeginWaitCursor();
		SaveDataFile();
		Ta.SetDocModifiedFlag(FALSE);
		SetModifiedFlag(FALSE);
		EndWaitCursor();
	}
	/* ֱ�Ӵ浽�������ݷ��������ϲ�ʵ�� wjh-2017.6.22
#ifdef __CONNECT_REMOTE_SERVER_
	else if(theApp.m_bNetModelWork)
	{
		CServerTowerFile *pTowerFile=AgentServer.GetActiveTowerFile();
		if(pTowerFile==NULL)
		{
			AfxMessageBox("δ�������������ļ����޷��ϴ����ݣ�");
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
			AfxMessageBox("�ϴ�����ʧ�ܣ������ԣ�");
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
			AfxMessageBox("Current data model is empty��continue to store may be at risk "\
						  "(empty data file with the same name)",MB_YESNO)!=IDYES)
#else
			AfxMessageBox("��ǰ����ģ��Ϊ�գ������洢�������������ͬ�������ļ���",MB_YESNO)!=IDYES)
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
		m_nTimer=2;	//���ñ����ļ��洢
		CDocument::OnFileSave();
	}
}

//From Tower.cpp ����/��ȡ��׼�� wht 11-05-23
extern void WriteStdPartLibToBuffer(CBuffer &buffer);	//�����׼��
extern void ReadStdPartLibFromBuffer(CBuffer &buffer,CTowerArchiveLibrary* pLoadToArchive,long version=0,long doc_type=0);	//��ȡ��׼��
void CLDSDoc::OnReloadFile() 
{
	BeginWaitCursor();
	//�����������ָ��
	DWORD dwBytes;
	int retCode;
	LDS_MSGHEAD msg_head;
	CBuffer buffer(1000000);
	msg_head.command_id=ASK_DATA_CHECK_OUT;
	msg_head.msg_length=4;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	buffer.Write(&msg_head,9);
	buffer.WriteInteger(FALSE);		//�������Ȩ����Ϣ
	buffer.WriteDword(0xffffffff);	//�趨����Ȩ�޷�Χ
	WriteFile(theApp.m_hPipeServerListen,buffer.GetBufferPtr(),buffer.GetLength(),&dwBytes,NULL);
	//�ӷ�������������
	Ta.Empty();
	BYTE memory_pool[PACK_SIZE];
	DWORD dwRead;
	ReadFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwRead,NULL);
	WriteFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwBytes,NULL);
	//����Эͬ���ķ���������������Դ����ȷ��
	ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
	if(retCode==0)
	{//Эͬ����������Դ��ռ��
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"Collaborative center data resource temporarily occupied by other users, please be patient��");
#else 
		PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ�ݱ������û�ռ�ã������ĵȴ���");
#endif
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
		//PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ�ѿ��ã�����׼���������ݣ������ĵȴ���");
	}
	//else
	//	PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ���ã�����׼���������ݣ������ĵȴ���");
	DWORD nDataSize;
	BYTE verify[17]="",verify2[17]="";
	int i,j,iIteration=0;
	while(1)
	{
		ReadFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwRead,NULL);
		ReadFile(theApp.m_hPipeServerListen,&verify,16,&dwRead,NULL);
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center, please be patient��");
#else 
		PutMessage(theApp.GetMainWnd(),"���ڴ�Эͬ���Ľ������ݣ������ĵȴ���");
#endif
		buffer.ClearBuffer();
		while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
		{
			buffer.Write(memory_pool,PACK_SIZE-1);
			int percent=min(100,buffer.GetLength()*100/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center��Read %d%%����",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"���ڴ�Эͬ���Ľ������ݣ��Ѷ�ȡ%d%%����",percent);
#endif
			if(memory_pool[PACK_SIZE-1]==0)
				break;	//���ݽ��ս���
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
				return;	//��������Ժ����,�����ǿͻ��˴�ʱ�����쳣�ж����ݴ��䵼��-wjh 2009.5.4
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
					AfxMessageBox("�������ʧ��!");
#endif
					return;
				}
				else
#ifdef AFX_TARG_ENU_ENGLISH
					PutMessage(theApp.GetMainWnd(),"The data transmits errors��so asking to resend, please be patient��");
#else 
					PutMessage(theApp.GetMainWnd(),"��Эͬ���Ľ��յ������ݴ�����ִ�������Ҫ���ط��������ĵȴ���");
#endif
			}
		}
	}
	PutMessage(theApp.GetMainWnd(),"");
	buffer.SeekToBegin();
	//�������û�����Ϣ
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
	buffer.ReadByte(&cnThickItemCount);	//����������Ŀ
	buffer.ReadByte(&cnAngleItemCount);	//����Ƕ��������Ŀ
	if(cnAngleItemCount<1||cnThickItemCount<1)
	{
		buffer.ReadDouble(&Ta.m_fDeformCoef);
		buffer.ReadDouble(&Ta.m_fHuoquR);
	}
	else
	{	//������Ϊ�������飬������Ϊ����Ƕȷ���(��)
		DYN_ARRAY<double> thick_header(cnThickItemCount),angle_header(cnAngleItemCount);
		DYN_ARRAY<double> mapdata(cnThickItemCount*cnAngleItemCount);
		buffer.Read(thick_header,sizeof(double)*cnThickItemCount);
		buffer.Read(angle_header,sizeof(double)*cnAngleItemCount);
		//�������Բ�ϵ����ֵ��
		buffer.Read(mapdata,sizeof(double)*mapdata.Size());
		Ta.mapNeutroCoef.Init(cnAngleItemCount,cnThickItemCount,mapdata);
		Ta.mapNeutroCoef.InitHeader(thick_header,angle_header);
		//������������Բ����ֵ��
		buffer.Read(mapdata,sizeof(double)*mapdata.Size());
		Ta.mapBendR.Init(cnAngleItemCount,cnThickItemCount,mapdata);
		Ta.mapBendR.InitHeader(thick_header,angle_header);
	}
	//buffer.ReadInteger(&CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	char sText[100]="";
	buffer.ReadString(sText,100);		
	Ta.m_sCompanyName=sText;		//��Ƶ�λ
	Ta.weather.FromBuffer(buffer);	//��������
	buffer.ReadInteger(&Ta.volt_grade);//��ѹ�ȼ�:330,500
	buffer.ReadByte(&Ta.m_cLandscape);	//��λ����:'A','B','C'
	buffer.ReadDouble(&Ta.m_fTowerHeight);
	/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
	0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
	1:����������ѹ��·�ĸ������(gama0=1.0)��
	2:������ʱʹ�õĸ������(gama0=0.9).*/
	buffer.ReadWord(&Ta.m_siSafetyGrade);
	buffer.ReadDouble(&Ta.m_fSafetyCoef);
	buffer.ReadDouble(&Ta.m_fIceDensity);	//���������ض�kg/m3
#endif
	//����׼�����
	buffer.ReadWord(&zhunju_N);
	for(i=0;i<zhunju_N;i++)
	{
		buffer.ReadWord(&zhunju_table[i].wide);	//֫��
		buffer.ReadWord(&zhunju_table[i].g);	//��׼׼��g
		buffer.ReadWord(&zhunju_table[i].g1);	//׼��g1
		buffer.ReadWord(&zhunju_table[i].g2);	//׼��g2
		buffer.ReadWord(&zhunju_table[i].g3);	//׼��g3
	}
	
    //��˨����
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
	//��ȡ��˨�����
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
	//��˨���ϵ�б�
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
			buffer.ReadDouble(&pXingHao->grade);			//��˨����
			buffer.ReadInteger(&pXingHao->d);			//��˨ֱ��
			buffer.ReadString(pXingHao->guige,15);			//��˨���
			buffer.ReadInteger(&pXingHao->valid_len);	//��Ч��
			buffer.ReadInteger(&pXingHao->no_thread_len);//�޿۳�
			buffer.ReadInteger(&pXingHao->down_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->up_lim);		//��˨ͨ������
			buffer.ReadInteger(&pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
			buffer.ReadDouble(&pXingHao->weight);		//��˨��������
		}
	}
	//��˨���ѡ������
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
	//���ʿ�
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
	//�����������
	buffer.ReadByte(&Ta.EarthQuake.grade);			//�����Ҷ�6,7,8,9
	buffer.ReadDouble(&Ta.EarthQuake.alfa_max);		//ˮƽ����Ӱ��ϵ�����ֵ
	buffer.ReadByte(&Ta.EarthQuake.cLandType);		//�������1,2,3,4
	buffer.ReadByte(&Ta.EarthQuake.cDesignGroup);	//��Ƶ������1,2,3
	buffer.ReadDouble(&Ta.EarthQuake.Tg);			//��������,s
	buffer.ReadDouble(&Ta.EarthQuake.zeta);			//����ϵ����=0.02s
	buffer.ReadDouble(&Ta.EarthQuake.gamaRE);		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
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
	//LMA��LDS֧�ֱ���ֹܶ����ӱ�׼�� wht 11-05-03
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
	//����ģ����
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
	//��ϣ�������Ϣ����
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
		else	//CLDSPart���͹���
		{
			pPart=Ta.Parts.append(cls_typeid,FALSE);
			pPart->handle=h;
		}
	}
	Ta.ReBuildObjsHashTable();	//����ȫ�������ϣ��
	//��ʾ��ͼ����
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CDisplayView* pDispView=Ta.View.append(FALSE);
		buffer.ReadInteger(&pDispView->handle);
		pDispView->FromBuffer(buffer);
	}
	buffer.ReadInteger(&h);
	console.SetActiveView(h);
	//Բ��Ԥ��������
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
	//����������
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CBlockModel* pBlock=Ta.Block.append(FALSE);
		buffer.ReadInteger(&pBlock->handle);
		pBlock->FromBuffer(buffer);
	}

	long buffer_len=buffer.GetLength();
	DisplayProcess(0);
	//��׼��洢��
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pPoint=(CLDSPoint*)Ta.hashObjs.GetValue(h);
		pPoint->FromBuffer(buffer,0);
	}
	//��׼�ߴ洢��
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pLine=(CLDSLine*)Ta.hashObjs.GetValue(h);
		pLine->FromBuffer(buffer);
	}
	//��׼��洢��
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pPlane=(CLDSPlane*)Ta.hashObjs.GetValue(h);
		pPlane->FromBuffer(buffer);
	}
	DisplayProcess(buffer.GetCursorPosition()*100/buffer_len);
	//�ڵ�洢��
	buffer.ReadInteger(&nn);
	for(j=0;j<nn;j++)
	{
		buffer.ReadInteger(&h);
		pNode=(CLDSNode*)Ta.hashObjs.GetValue(h);
		pNode->FromBuffer(buffer);
		DisplayProcess(buffer.GetCursorPosition()*100/buffer_len);
	}
	//�������ݴ洢��
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

		//ԭʼ�����������
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			int iNo=0;
			buffer.ReadInteger(&iNo);
			CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.Add(iNo);
			pWireModel->iNo=iNo;
			pWireModel->FromBuffer(buffer);
		}
		//���������Ϣ
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			long key=0;
			buffer.ReadInteger(&key);
			RAWWORKSTATUS_STRU* pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.Add(key);
			buffer.ReadString(pRawStatus->name);
			buffer.ReadString(pRawStatus->lim_str);	//�¼�
			buffer.ReadInteger(&pRawStatus->m_hWorkStatus);			//��Ӧ��CWorkStatus�ľ��ֵ
			buffer.ReadByte(&pRawStatus->m_cMainLoadType);			//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
			buffer.ReadBooleanThin(&pRawStatus->m_bCheckWorkCond);	//���㹤��
			buffer.ReadBooleanThin(&pRawStatus->m_bMinVertSpace);	//��С��ֱ���๤��
			buffer.ReadDouble(&pRawStatus->ice_thick);				//�����������(mm)
			buffer.ReadInteger(&pRawStatus->WindLoad.azimuth);		//�����,�����(��)
			buffer.ReadDouble(&pRawStatus->WindLoad.velocity);		//�����,����(m/s)
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
			//д����λ��Ӧ��Ĭ��ԭʼ������ʶ
			long j,k,n2,n3,key2;
			buffer.ReadInteger(&n2);
			for(j=0;j<n2;j++)
			{
				buffer.ReadInteger(&key);
				buffer.ReadInteger(&key2);
				pRoom->hashWorkEnv.SetValue(key,key2);
			}
			//д�뵱ǰ���λ��Ӧ�ĵ����Ϣ
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
	//����״��
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
	//���ӷ����
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
	//��ѹ����Ϣ
	buffer.ReadInteger(&nn);
	pObjNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
	pObjNoGroup->EmptyNo();
	for(i=0;i<nn;i++)
	{
		CWindSegment *pWindSeg=Ta.WindLoadSeg.append();
		pWindSeg->FromBuffer(buffer);
		pObjNoGroup->SetNoState(pWindSeg->iNo);
	}
	//�ֹܷ��������Ϣ
	Ta.hashConnectFl.Empty();
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		CConnectFL *pFL=Ta.hashConnectFl.append();
		buffer.ReadDouble(&pFL->m_fThickTube);			//���ܱں�(mm)
		buffer.ReadDouble(&pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
		buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
		buffer.ReadString(pFL->bolt_grade,8);		//��˨����
		buffer.ReadInteger(&pFL->bolt_d);			//��˨ֱ�������
		buffer.ReadInteger(&pFL->bolt_n);			//��˨���е���˨����
		buffer.ReadDouble(&pFL->N);
		buffer.ReadDouble(&pFL->M);				//N ������(N),M���(N*m)
		buffer.ReadDouble(&pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
		buffer.ReadInteger(&pFL->m_nThickFL);		//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nWidthFL);		//�������δ���(mm)
		buffer.ReadInteger(&pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
		buffer.ReadInteger(&pFL->m_nHeightRib);		//�߰�߶�(mm)
		buffer.ReadInteger(&pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//�����̺��(mm)
		buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
		buffer.ReadByte(&pFL->cMaterial);
		buffer.ReadString(pFL->m_sPartNo);     //�������
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
	PutMessage(theApp.GetMainWnd(),"It is reading update permission information from server��");
#else 
	PutMessage(theApp.GetMainWnd(),"���ڴӷ�������ȡ����Ȩ����Ϣ��");
#endif
	buffer.ClearBuffer();
	//��ȡ��Ϣ���ݣ���ȡ���ݵ��û���Χ��
	while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
	{
		buffer.Write(memory_pool,PACK_SIZE-1);
		if(memory_pool[PACK_SIZE-1]==0)
			break;	//���ݽ��ս���
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

	//�����������ָ����ȡ�û���Ϣ
	LDS_MSGHEAD msg_head;
	DWORD dwBytes,dwRead;
	CBuffer buffer(1000000);
	BYTE memory_pool[PACK_SIZE];
	theApp.GetUserInfo(update_dlg.user_arr);
	if(update_dlg.DoModal()==IDOK)
		update_word=update_dlg.m_dwUpdateUser;
	else
		return;

	//�����������ָ��
	BeginWaitCursor();
	msg_head.command_id=ASK_DATA_CHECK_OUT;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	WriteFile(theApp.m_hPipeServerListen,&theApp.m_bNeedUpdatePerm,4,&dwBytes,NULL);
	WriteFile(theApp.m_hPipeServerListen,&update_word,4,&dwBytes,NULL);
	if(theApp.m_bNeedUpdatePerm)
	{	//��Ҫ����Ȩ����Ϣ
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
		AfxMessageBox("Client and collaborative center operate different file, update failed��");	//
#else 
		AfxMessageBox("�ͻ�����Эͬ���Ĳ������ǲ�ͬ���ļ�������ʧ�ܣ�");	//
#endif
		return;
	}
	//����Эͬ���ķ���������������Դ����ȷ��
	ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
	if(retCode==0)
	{//Эͬ����������Դ��ռ��
#ifdef AFX_TARG_ENU_ENGLISH
		PutMessage(theApp.GetMainWnd(),"Collaborative center data resource temporarily occupied by other users, please be patient��");
#else 
		PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ�ݱ������û�ռ�ã������ĵȴ���");
#endif
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwRead,NULL);
		//PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ�ѿ��ã�����׼���������ݣ������ĵȴ���");
	}
	//else
	//	PutMessage(theApp.GetMainWnd(),"Эͬ����������Դ���ã�����׼���������ݣ������ĵȴ���");
	DWORD nDataSize;
	char verify[17]="",verify2[17]="";
	int iIteration=0;
	while(1)
	{
		ReadFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwRead,NULL);
		ReadFile(theApp.m_hPipeServerListen,verify,16,&dwRead,NULL);
		//�ӷ��������ո�������
		buffer.ClearBuffer();
		int nRecSize=0;
		while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwRead,NULL))
		{
			buffer.Write(memory_pool,PACK_SIZE-1);
			nRecSize+=PACK_SIZE-1;
			int percent=min(100,buffer.GetLength()*100/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is receiving data from the collaborative center��Read %d%%����",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"���ڴ�Эͬ���Ľ������ݣ��Ѷ�ȡ%d%%����",percent);
#endif
			if(memory_pool[PACK_SIZE-1]==0)
				break;	//���ݽ��ս���
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
			PutMessage(theApp.GetMainWnd(),"The data transmits errors��so asking to resend, please be patient��");
#else 
			PutMessage(theApp.GetMainWnd(),"��Эͬ���Ľ��յ������ݴ�����ִ�������Ҫ���ط��������ĵȴ���");
#endif
		}
		iIteration++;
		if(iIteration>=2)
		{
			PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data update failed!");
#else 
			AfxMessageBox("���ݸ���ʧ��!");
#endif
			return;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"Data reads over, began to update the stored data��");
#else 
	PutMessage(theApp.GetMainWnd(),"���ݶ�ȡ��������ʼ�����ڴ����ݣ�");
#endif
	long i,nn;
	buffer.SeekToBegin();
	CObjNoGroup *pNoGroup=NULL;
	if(update_word&PERM_TOWER_ADMIN)	//����Ա
	{	//������Ϣ��
		buffer.ReadString(Ta.version);		//�����ļ��汾��
		buffer.ReadDword(&Ta.user_pipeline_no);
		buffer.ReadString(Ta.m_sPrjName);	//������Ŀ����
		buffer.ReadString(Ta.m_sTaType);	//��������
		buffer.ReadString(Ta.m_sTaStampNo);//����������ӡ��
		buffer.ReadString(Ta.m_sTaAlias);	//������������
		buffer.ReadDouble(&Ta.m_fMaxXieJGLen);	//�������б�ĳ���
		buffer.Read(Ta.cfgword.flag.word,24);	//��������ĺ�
		buffer.ReadByte(&Ta.m_ciRollProcessType);
		buffer.ReadByte(&Ta.m_ciRollOffsetType);
		buffer.ReadByte(&Ta.m_ciDeformType);
		BYTE cnThickItemCount=0,cnAngleItemCount=0;
		buffer.ReadByte(&cnThickItemCount);	//����������Ŀ
		buffer.ReadByte(&cnAngleItemCount);	//����Ƕ��������Ŀ
		if(cnAngleItemCount<1||cnThickItemCount<1)
		{
			buffer.ReadDouble(&Ta.m_fDeformCoef);
			buffer.ReadDouble(&Ta.m_fHuoquR);
		}
		else
		{	//������Ϊ�������飬������Ϊ����Ƕȷ���(��)
			DYN_ARRAY<double> thick_header(cnThickItemCount),angle_header(cnAngleItemCount);
			DYN_ARRAY<double> mapdata(cnThickItemCount*cnAngleItemCount);
			buffer.Read(thick_header,sizeof(double)*cnThickItemCount);
			buffer.Read(angle_header,sizeof(double)*cnAngleItemCount);
			//�������Բ�ϵ����ֵ��
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			Ta.mapNeutroCoef.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			Ta.mapNeutroCoef.InitHeader(thick_header,angle_header);
			//������������Բ����ֵ��
			buffer.Read(mapdata,sizeof(double)*mapdata.Size());
			Ta.mapBendR.Init(cnAngleItemCount,cnThickItemCount,mapdata);
			Ta.mapBendR.InitHeader(thick_header,angle_header);
		}
		//buffer.ReadInteger(&CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		char sText[100]="";
		buffer.ReadString(sText);		
		Ta.m_sCompanyName=sText;		//��Ƶ�λ
		//��������
		Ta.weather.FromBuffer(buffer);
		buffer.ReadInteger(&Ta.volt_grade);//��ѹ�ȼ�:330,500
		buffer.ReadByte(&Ta.m_cLandscape);	//��λ����:'A','B','C'
		buffer.ReadDouble(&Ta.m_fTowerHeight);
		/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
		0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
		1:����������ѹ��·�ĸ������(gama0=1.0)��
		2:������ʱʹ�õĸ������(gama0=0.9).*/
		buffer.ReadWord(&Ta.m_siSafetyGrade);
		buffer.ReadDouble(&Ta.m_fSafetyCoef);
		buffer.ReadDouble(&Ta.m_fIceDensity);	//���������ض�kg/m3
#endif
		//����׼�����
		buffer.ReadWord(&zhunju_N);	//��������׼����Ŀ
		for(i=0;i<zhunju_N;i++)
		{
			buffer.ReadWord(&zhunju_table[i].wide);	//֫��
			buffer.ReadWord(&zhunju_table[i].g);	//��׼׼��g
			buffer.ReadWord(&zhunju_table[i].g1);	//׼��g1
			buffer.ReadWord(&zhunju_table[i].g2);	//׼��g2
			buffer.ReadWord(&zhunju_table[i].g3);	//׼��g3
		}
		//��˨����
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
		//��ȡ��˨�����
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
		//��˨���ϵ����Ϣ
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
				buffer.ReadDouble(&pXingHao->grade);			//��˨����
				buffer.ReadInteger(&pXingHao->d);			//��˨ֱ��
				buffer.ReadString(pXingHao->guige);			//��˨���
				buffer.ReadInteger(&pXingHao->valid_len);	//��Ч��
				buffer.ReadInteger(&pXingHao->no_thread_len);//�޿۳�
				buffer.ReadInteger(&pXingHao->down_lim);		//��˨ͨ������
				buffer.ReadInteger(&pXingHao->up_lim);		//��˨ͨ������
				buffer.ReadInteger(&pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
				buffer.ReadDouble(&pXingHao->weight);		//��˨��������
			}
		}
		//��˨���ѡ������
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
		//���ʿ�
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
		//�����������
		buffer.ReadByte(&Ta.EarthQuake.grade);			//�����Ҷ�6,7,8,9
		buffer.ReadDouble(&Ta.EarthQuake.alfa_max);		//ˮƽ����Ӱ��ϵ�����ֵ
		buffer.ReadByte(&Ta.EarthQuake.cLandType);		//�������1,2,3,4
		buffer.ReadByte(&Ta.EarthQuake.cDesignGroup);	//��Ƶ������1,2,3
		buffer.ReadDouble(&Ta.EarthQuake.Tg);			//��������,s
		buffer.ReadDouble(&Ta.EarthQuake.zeta);			//����ϵ����=0.02s
		buffer.ReadDouble(&Ta.EarthQuake.gamaRE);		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
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
		//LMA��LDS֧�ֱ���ֹܶ����ӱ�׼�� wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LMA_)||defined(__LDS_FILE_)||defined(__LDS_)
		ReadStdPartLibFromBuffer(buffer,NULL);
#endif
		//����ģ����
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
	//��ϣ�������Ϣ����
	Ta.ReBuildObjsHashTable();	//����ȫ�������ϣ��
	buffer.ReadInteger(&nn);
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		buffer.ReadInteger(&cls_typeid);
		if((pObj=Ta.hashObjs.GetValue(h))!=NULL)
		{
			if(cls_typeid==pObj->GetClassTypeId())
				continue;	//�Ѵ��ڴ˹������������
			else
			{
				//Ta.DispPartSet.DeleteNode(h);
				console.DeletePart(h,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Object' handle appear repeated, please contact the software provider��");
#else 
				AfxMessageBox("���ֶ������ظ������������Ӧ����ϵ��");
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
		else	//CLDSPart���͹���
		{
			pPart=Ta.Parts.append(cls_typeid,FALSE);
			pPart->handle=h;
			Ta.hashObjs.SetValue(h,pPart);
		}
	}
	//��ʾ��ͼ����
	buffer.ReadInteger(&nn);
	CDisplayView* pDispView;
	for(pDispView=Ta.GetFirstDisplayView();pDispView;pDispView=Ta.GetNextDisplayView())
		pDispView->feature=0;	//��ɾ�����
	long hView;
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&hView);
		pDispView=Ta.View.FromHandle(hView);
		if(pDispView==NULL)
			pDispView=Ta.View.append(FALSE);
		pDispView->handle=hView;
		pDispView->FromBuffer(buffer);
		pDispView->feature=1;	//�ñ������
	}
	//Эͬ���ĵ�ǰ��ͼ���˴�ֻ�������´��ļ�ʱ���ã�������������²��޸��û��ĵ�ǰ��ͼ
	buffer.ReadInteger(&hView);
	for(pDispView=Ta.GetFirstDisplayView();pDispView;pDispView=Ta.GetNextDisplayView())
	{
		if(pDispView->feature==0&&pDispView->dwPermission&update_word)
		{
			if(pDispView==console.GetActiveView())
				console.SetActiveView(0);	//��ΪϵͳĬ����ͼ
			if(pDispView!=console.GetActiveView())	//����ȷ��ɾ����ͼΪ��ǰ��ͼ����²�����
				Ta.View.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
		}
	}
	Ta.View.Clean();
	//Բ��Ԥ��������
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
			Ta.ArcLift.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
	}
	Ta.ArcLift.Clean();
	//����������
	buffer.ReadInteger(&nn);
	CBlockModel* pBlock;
	for(pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
		pBlock->feature=0;	//��ɾ�����
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
		pBlock->feature=1;	//�ñ������
	}
	for(pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		if(pBlock->feature==0&&pBlock->dwPermission&update_word)
			Ta.Block.DeleteCursor();
	}
	Ta.Block.Clean();

	//��׼��洢��
	buffer.ReadInteger(&nn);
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
		pPoint->feature=0;	//��ɾ�����
	for(i=0;i<nn;i++)
	{
		buffer.ReadInteger(&h);
		pPoint=(CLDSPoint*)Ta.hashObjs.GetValue(h);
		pPoint->FromBuffer(buffer,0);
		pPoint->feature=1;	//�ñ������
	}
	for(pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		if(pPoint->feature==0&&pPoint->dwPermission&update_word)
			Ta.Point.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
	}
	Ta.Point.Clean();
	//��׼�ߴ洢��
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
			Ta.Line.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
	}
	Ta.Line.Clean();
	//��׼��洢��
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
			Ta.Plane.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
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
			Ta.Node.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
	}
	Ta.Node.Clean();
	//������Ϣ��
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"It is updating parts' data��");
#else 
	PutMessage(theApp.GetMainWnd(),"���ڸ��¹������ݣ�");
#endif
	buffer.ReadInteger(&nn);
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		pPart->feature=0;	//��Ϊ0��ʾɾ��
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
			{	//��ֹɾ����˨�󻹴���ָ�����˨����˨����
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
			Ta.Parts.DeleteCursor(FALSE,FALSE);	//ɾ��ʱ����Эͬ�������ݶ����ܽ���Ȩ����֤,������Ȼͨ���� wjh-2016.3.29
		}
	}
#endif
	if(update_word&PERM_TOWER_ADMIN)	//����Ա
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
		//����״��
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
		//���ӷ����
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
		//��ѹ����Ϣ
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
		//�ֹܷ��������Ϣ
		Ta.hashConnectFl.Empty();
		buffer.ReadInteger(&nn);
		for(i=0;i<nn;i++)
		{
			CConnectFL *pFL=Ta.hashConnectFl.append();
			buffer.ReadDouble(&pFL->m_fThickTube);			//���ܱں�(mm)
			buffer.ReadDouble(&pFL->m_fInnerRadius);			//��׼�����ڻ��뾶(mm)
			buffer.ReadDouble(&pFL->m_fCoupleInnerRadius);	//��Է����ڻ��뾶(mm)
			buffer.ReadString(pFL->bolt_grade,8);		//��˨����
			buffer.ReadInteger(&pFL->bolt_d);			//��˨ֱ�������
			buffer.ReadInteger(&pFL->bolt_n);			//��˨���е���˨����
			buffer.ReadDouble(&pFL->N);
			buffer.ReadDouble(&pFL->M);				//N ������(N),M���(N*m)
			buffer.ReadDouble(&pFL->m_fBoltRadius);	//��˨Բ�뾶(mm)
			buffer.ReadInteger(&pFL->m_nThickFL);		//�����̺��(mm)
			buffer.ReadInteger(&pFL->m_nWidthFL);		//�������δ���(mm)
			buffer.ReadInteger(&pFL->m_nThickRib);		//�߰���(mm),���Ϊ0��ʾ�޼Ӿ���
			buffer.ReadInteger(&pFL->m_nHeightRib);		//�߰�߶�(mm)
			buffer.ReadInteger(&pFL->m_nRibS);			//�߰庸���нǸ�(��)��(mm)
			buffer.ReadInteger(&pFL->m_nCoupleRibS);	//����߰庸���нǸ�(��)��(mm)
			buffer.ReadInteger(&pFL->m_nCoupleThickFL);	//�����̺��(mm)
			buffer.ReadInteger(&pFL->m_nCoupleHeightRib);//����߰�߶�(mm)
			buffer.ReadByte(&pFL->cMaterial);
			buffer.ReadString(pFL->m_sPartNo);     //�������
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
	if(theApp.m_nUserId==0)	//����Ա
		update_word=PERM_TOWER_ADMIN;
	else					//��ͨ�û�
		update_word=GetSingleWord(theApp.m_nUserId);

	//�����������ָ��
	LDS_MSGHEAD msg_head;
	DWORD dwBytes;
	CBuffer buffer(1000000);
	msg_head.command_id=ASK_DATA_CHECK_IN;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	//������Ϣͷ
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	//�����������Э��һ�µ��ļ�ID
	WriteFile(theApp.m_hPipeServerListen,&m_idServerFile,4,&dwBytes,NULL);
	int retCode;
	//����Эͬ���ķ����������ļ�IDһ��ȷ��
	if(!ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL))
		return FALSE;
	else if(retCode==0)	//�ļ�ID��ƥ�䣬�жϴ���
		return FALSE;
	WriteFile(theApp.m_hPipeServerListen,&theApp.m_bNeedUpdatePerm,4,&dwBytes,NULL);
	if(theApp.m_bNeedUpdatePerm)
		UpdatePermFromServer();//��Ҫ����Ȩ����Ϣ
	//����������͸�������
#ifdef AFX_TARG_ENU_ENGLISH
	PutMessage(theApp.GetMainWnd(),"Client data is packaged,It is ready to send to collabrative center, please be patient��");
#else 
	PutMessage(theApp.GetMainWnd(),"���ڽ��ͻ������ݴ����׼����Эͬ���ķ��ͣ������ĵȴ���");
#endif
	buffer.ClearBuffer();
	if(theApp.m_nUserId==0)	//����Ա
	{	//������Ϣ��
		buffer.WriteString(Ta.version);		//�����ļ��汾��
		buffer.WriteDword(Ta.user_pipeline_no);
		buffer.WriteString(Ta.m_sPrjName);	//������Ŀ����
		buffer.WriteString(Ta.m_sTaType);	//��������
		buffer.WriteString(Ta.m_sTaStampNo);//����������ӡ��
		buffer.WriteString(Ta.m_sTaAlias);	//������������
		buffer.WriteDouble(Ta.m_fMaxXieJGLen);	//�������б�ĳ���
		buffer.Write(Ta.cfgword.flag.word,24);	//��������ĺ�
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
		{	//������Ϊ�������飬������Ϊ����Ƕȷ���(��)
			buffer.Write(Ta.mapNeutroCoef.RowHeader(),sizeof(double)*cnThickItemCount);
			buffer.Write(Ta.mapNeutroCoef.ColHeader(),sizeof(double)*cnAngleItemCount);
			//д�����Բ�ϵ����ֵ��
			buffer.Write(Ta.mapNeutroCoef.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
			//д����������Բ����ֵ��
			buffer.Write(Ta.mapBendR.MapData,sizeof(double)*cnThickItemCount*cnAngleItemCount);
		}
		//buffer.WriteInteger(CLDSPlate::m_iPlankWeightStatType);
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
		buffer.WriteString(Ta.m_sCompanyName.GetBuffer(100));	//��Ƶ�λ
		Ta.weather.ToBuffer(buffer);		//��������
		buffer.WriteInteger(Ta.volt_grade);	//��ѹ�ȼ�:330,500
		buffer.WriteByte(Ta.m_cLandscape);	//��λ����:'A','B','C'
		buffer.WriteDouble(Ta.m_fTowerHeight);
		/*��ȫ�ȼ�(�����ṹ��Ҫ��ϵ��gama0)��
		0:һ���ر���Ҫ�ĸ����ṹ(gama0=1.1)��
		1:����������ѹ��·�ĸ������(gama0=1.0)��
		2:������ʱʹ�õĸ������(gama0=0.9).*/
		buffer.WriteWord(Ta.m_siSafetyGrade);
		buffer.WriteDouble(Ta.m_fSafetyCoef);
		buffer.WriteDouble(Ta.m_fIceDensity);	//���������ض�kg/m3
#endif
		//����׼�����
		buffer.WriteWord(zhunju_N);	//��������׼����Ŀ
		for(WORD i=0;i<zhunju_N;i++)
		{
			buffer.WriteWord(zhunju_table[i].wide);	//֫��
			buffer.WriteWord(zhunju_table[i].g);	//��׼׼��g
			buffer.WriteWord(zhunju_table[i].g1);	//׼��g1
			buffer.WriteWord(zhunju_table[i].g2);	//׼��g2
			buffer.WriteWord(zhunju_table[i].g3);	//׼��g3
		}
		//��˨����
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
		{	//д����˨�����
			buffer.WriteInteger(CLsLibrary::GetLsSubPartFamilyNum());
			for(CLsSubPartFamily* pLsSubPart=CLsLibrary::GetFirstLsSubPartFamily();pLsSubPart;pLsSubPart=CLsLibrary::GetNextLsSubPartFamily())
			{
				buffer.WriteWord(pLsSubPart->m_widType);
				buffer.WriteString(pLsSubPart->name);
			}
		}
		//��˨���ϵ����Ϣ
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
				buffer.WriteDouble(pXingHao->grade);			//��˨����
				buffer.WriteInteger(pXingHao->d);			//��˨ֱ��
				buffer.WriteString(pXingHao->guige);			//��˨���
				buffer.WriteInteger(pXingHao->valid_len);	//��Ч��
				buffer.WriteInteger(pXingHao->no_thread_len);//�޿۳�
				buffer.WriteInteger(pXingHao->down_lim);		//��˨ͨ������
				buffer.WriteInteger(pXingHao->up_lim);		//��˨ͨ������
				buffer.WriteInteger(pXingHao->washer_thick);	//��Ȧ���pXingHao->pad_thick
				buffer.WriteDouble(pXingHao->weight);		//��˨��������
			}
		}
		//��˨���ѡ������
		buffer.WriteInteger(Ta.listSelBoltCond.GetNodeNum());
		for(BOLT_SELCOND *pSelBolt=Ta.listSelBoltCond.GetFirst();pSelBolt;pSelBolt=Ta.listSelBoltCond.GetNext())
		{
			buffer.WriteInteger(pSelBolt->d);
			buffer.WriteString(pSelBolt->grade);
			buffer.WriteString(pSelBolt->matStr);
			buffer.WriteInteger(pSelBolt->upWidthLim);
		}
		//���ʿ�
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
		//�����������
		buffer.WriteByte(Ta.EarthQuake.grade);			//�����Ҷ�6,7,8,9
		buffer.WriteDouble(Ta.EarthQuake.alfa_max);		//ˮƽ����Ӱ��ϵ�����ֵ
		buffer.WriteByte(Ta.EarthQuake.cLandType);		//�������1,2,3,4
		buffer.WriteByte(Ta.EarthQuake.cDesignGroup);	//��Ƶ������1,2,3
		buffer.WriteDouble(Ta.EarthQuake.Tg);			//��������,s
		buffer.WriteDouble(Ta.EarthQuake.zeta);			//����ϵ����=0.02s
		buffer.WriteDouble(Ta.EarthQuake.gamaRE);		//�������������ϵ��,һ���Խ��ȡ0.85��������ȡ0.8
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
		//LMA��LDS֧�ֱ���ֹܶ����ӱ�׼�� wht 11-05-03
#ifdef __COMPLEX_PART_INC_//defined(__LMA_FILE_)||defined(__LMA_)||defined(__LDS_FILE_)||defined(__LDS_)
		WriteStdPartLibToBuffer(buffer);
#endif
		//����ģ����
		buffer.WriteInteger(Ta.Module.GetNodeNum());
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			buffer.WriteInteger(pModule->handle);
			pModule->ToBuffer(buffer);
		}
	}
	//��ϣ�������Ϣ����
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
	//��ͼ�洢
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
	buffer.RecallPosition();//�ٻر�ǵ�λ�ã����ڵ����洢λ�ã�
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//���ص���ǰ��ȷ�洢λ��
#ifdef __COMMON_PART_INC_
	//Ԥ��������
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
	//������洢
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
	buffer.RecallPosition();//�ٻر�ǵ�λ�ã����ڵ����洢λ�ã�
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//���ص���ǰ��ȷ�洢λ��
	//��׼��洢��
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
	buffer.RecallPosition();//�ٻر�ǵ�λ�ã����ڵ����洢λ�ã�
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//���ص���ǰ��ȷ�洢λ��
	//��׼�ߴ洢��
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
	buffer.RecallPosition();//�ٻر�ǵ�λ�ã����ڵ����洢λ�ã�
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//���ص���ǰ��ȷ�洢λ��
	//��׼��洢��
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
	buffer.RecallPosition();//�ٻر�ǵ�λ�ã�����׼�����洢λ�ã�
	buffer.WriteInteger(nn);
	buffer.RecallPosition();//���ص���ǰ��ȷ�洢λ��
	
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
	//������Ϣ��
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
		//�������ݴ�С
		WriteFile(theApp.m_hPipeServerListen,&nDataSize,4,&dwBytes,NULL);
		//�������ݱ���MD5ժҪ
		WriteFile(theApp.m_hPipeServerListen,verify,16,&dwBytes,NULL);
		int iOrder=0;
		buffer.SeekToBegin();
		while(buffer.GetRemnantSize()>PACK_SIZE-1)
		{
			buffer.Read(memory_pool,PACK_SIZE-1);
			memory_pool[PACK_SIZE-1]=1;	//���ݷ���δ����
			WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL);
			int percent=min(100,100*iOrder*PACK_SIZE/nDataSize);
#ifdef AFX_TARG_ENU_ENGLISH
			PutMessage(theApp.GetMainWnd(),"It is sending date to collaborative center��Sent %d%%����",percent);
#else 
			PutMessage(theApp.GetMainWnd(),"������Эͬ���ķ������ݣ��ѷ���%d%%����",percent);
#endif
			iOrder++;
		}
		buffer.Read(memory_pool,buffer.GetRemnantSize());
		memory_pool[PACK_SIZE-1]=0;		//���ݷ����ѽ���
		WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL);
		//����Эͬ���ķ��������ݽ���ȷ���֣���������뱨��MD5ժҪ��һ�·���FALSE��
		ReadFile(theApp.m_hPipeServerListen,&retCode,4,&dwBytes,NULL);
		if(retCode==1)
			break;
		if(retCode==-1)
		{
			PutMessage(theApp.GetMainWnd(),"");
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Send failed!");
#else 
			AfxMessageBox("����ʧ��!");
#endif
			return FALSE;
		}
	}
	PutMessage(theApp.GetMainWnd(),"");
	return TRUE;
}

void CLDSDoc::UpdatePermList()
{
	//��Ҫ����Ȩ����Ϣ
	DWORD dwBytes;
	LDS_MSGHEAD msg_head;
	BYTE memory_pool[PACK_SIZE];
	msg_head.command_id=ASK_PERM_MODIFY_OUT;
	msg_head.msg_length=0;
	msg_head.src_code=(BYTE)theApp.m_nUserId;
	WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwBytes,NULL);
	CBuffer buffer;
	//��ȡ��Ϣ���ݣ���ȡ���ݵ��û���Χ��
	while(ReadFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwBytes,NULL))
	{
		buffer.Write(memory_pool,PACK_SIZE-1);
		if(memory_pool[PACK_SIZE-1]==0)
			break;	//���ݽ��ս���
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
		int ret=AfxMessageBox("Whether the modified data stored in the server��",MB_YESNOCANCEL|MB_ICONQUESTION);
#else 
		int ret=AfxMessageBox("�Ƿ��޸ĺ�����ݴ洢����������",MB_YESNOCANCEL|MB_ICONQUESTION);
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
		AfxMessageBox("Not open any files to export any data exchange information��");
#else 
		AfxMessageBox("δ���κ��ļ����޷������κ����ݽ�����Ϣ��");
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
		filter.Copy("ͳ����Ϣ(*.bom)|*.bom|Excelͳ����Ϣ(*.xls)|*.xls|");
#endif
#ifdef __PART_DESIGN_INC_
	if(!theApp.m_bDrawingWorkEnvOnly)
		filter.Append("ԭʼ�����Ϣ�ļ�(*.tdf)|*.tdf|");
	filter.Append("˾��ͼģ��(*.cdf)|*.cdf|");
#endif
#ifdef __RAPID_ROUGH_3DMODEL_
	if(!theApp.m_bAnalysisWorkEnvOnly)
	{
		filter.Append("����ģ����Ϣչʾ�ļ�(*.tid)|*.tid|");//|ʵ��ģ���ı��ļ�(*.txf)|*.txf|");
		filter.Append("3DS����ģ���ļ�(*.3ds)|*.3ds|");
		filter.Append("STL����ģ��ASCII�ļ�(*.stl)|*.stl|");
		filter.Append("STL����ģ�Ͷ������ļ�(*.stl)|*.stl|");
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�ƽ����� wjh-2019.9.25
		filter.Append("�����ƽ�����ģ���ļ�(*.mod)|*.mod|");
		filter.Append("�����ƽ�GIM�ļ�(*.gim)|*.gim|");
#endif
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	filter.Append("all files(*.*)|*.*||");
#else 
	filter.Append("�����ļ�(*.*)|*.*||");
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
			AfxMessageBox("ȱ���������ģ����Ϣչʾ�ļ���ʹ����Ȩ");
		else
			Ta.ExportTowerSolidDataExchangeFile(filename.GetBuffer());
	}
#ifndef __STATION_	//��ʱ��繹��SDA�������Ҫ�ƽ����� wjh-2019.9.25
	else if(ext.CompareNoCase("mod")==0)
	{
		CModDataFile modfile;
		LOCALE_PROPITEM* pLocaleItem = gxPortalLocalizer.GetLocaleItem("ExportTurnLeftGimFile");
		if (pLocaleItem&&pLocaleItem->bVal&&
			AfxMessageBox("����ģ���Ƿ������ת��", MB_YESNO) == IDYES)
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
		if (strstr(szCurrFormat,"������")!=NULL)	//"������"
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
		logfile.Log("                      �˼���ϸ�嵥\n");
		logfile.Log("�˼���ʶ     ���   ����(mm) �������(m2) ����(kg) ����ϵ�� ��Ȩ����(kg)");
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
				spec.InsertBefore("��",0);//��
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
		logfile.Log("�ϼ�  �������:  %8.3f m2",sumstat.area);
		logfile.Log("        ��  ��: %8.2f kg     ЧӦ����: %8.2f kg",sumstat.weight,sumstat.efficWeight);
		return;
	}
#endif
#ifdef __COMMON_PART_INC_
	CTowerType mat_tbl;
	mat_tbl.m_bStatLsBySegI=true;	//����ͳ�ƹ���
	for(i=0;i<n;i++)
	{
		CLDSPart *pPart=Ta.Parts.FromHandle(id_arr[i]);
		if(pPart==NULL)
			continue;
		else if(pPart->m_bVirtualPart)	//�����Ǹֲ�ͳ��
			continue;
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//���ϻ���ʱ��ͳ����ϽǸ�(����)���� wht 11-03-11
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
	//�����EXCEL�ļ�

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
		AfxMessageBox("δ�ҵ�Excel����!");
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
	
	excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������
	
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
	{	//������˨���ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddLsSumMatTbl(mat_tbl,pWorksheet,2);//,TRUE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineAngleSumMatTbl)
	{	//���ɽǸֻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_ANGLE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineTubeSumMatTbl)
	{	//���ɸֹܻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_TUBE);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineFlatSumMatTbl)
	{	//���ɱ������ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_FLAT);
		pWorksheet->Release();
	}
	if(option_dlg.m_bLineSlotSumMatTbl)
	{	//���ɲ۸ֻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		AddCommonPartSpecTblSheet(mat_tbl,pWorksheet,FALSE, "",STEELTYPE_SLOT);
		pWorksheet->Release();
	}
	if(option_dlg.m_bPlateSumMatTbl)
	{	//���ɸְ���ܱ�
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
	option_dlg.m_sSegStr=seg_str;	//�κ��ַ���
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
		nSheet++;	//��������麸���ű�
	if(option_dlg.m_bLsSumMatTbl)
		nSheet++;
	if(option_dlg.m_bWeldPartTbl)
		nSheet++;
	if(option_dlg.m_bPoleExcelFile)
		nSheet++;
	const int BOM_SHEET_PART		= 0x01;	//������ϸ��
	const int BOM_SHEET_BOLT		= 0x02;	//��˨��ϸ��
	const int BOM_SHEET_WELDPART	= 0x04;	//�麸����ϸ��
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
		
		CXhChar500 filePath("D:/EXCELģ��.xls");
		if (CLDSApp::RecogniseReport!=NULL&&PathFileExists(filePath))
			CLDSApp::RecogniseReport(buffer.GetBufferPtr(), buffer.GetLength(), GetPathName(), 0);
		else if (CLDSApp::CreateExcelBomFile != NULL && CLDSApp::GetSupportDataBufferVersion != NULL)
			CLDSApp::CreateExcelBomFile(buffer.GetBufferPtr(),buffer.GetLength(),GetPathName(),0);
		//�ж��Ƿ���ѡ����������û��ֻ������Ʊ�
		if (nSheet== 0)
			return;
	}
	BeginWaitCursor();
	//���в���ͳ��
	bool bIncLsSegI=false;
	if(option_dlg.m_iLsTblStyle==2)	//����ͳ����˨
		bIncLsSegI=true;
	mat_tbl.RunSummarize(&Ta,bIncLsSegI);
	int i=0;
	mat_tbl.SortPart(0);
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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

	excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������

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
	//��д��һ�Ź������ݱ�
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
	// ��д���ɹ�����ϸ���
	if(option_dlg.m_bOrgMatTbl||option_dlg.m_bPartTbl)
	{
		for(int ii=0;ii<2;ii++)
		{
			if(!option_dlg.m_bOrgMatTbl&&!option_dlg.m_bPartTbl)
				break;		//����Ҫ���ԭʼ���ϱ�򲿼���ϸ��
			else if(ii==0&&!option_dlg.m_bOrgMatTbl)
				continue;	//����Ҫ���ԭʼ���ϱ�
			else if(ii==1&&!option_dlg.m_bPartTbl)
				break;	//����Ҫ���������ϸ��
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
			else if(option_dlg.m_iOrgmatTblStyle==8)	//Ϋ������ wht 10-10-11
				AddWeiFangChangAnPartSpecTblSheet(mat_tbl,pWorksheet,ii==0,option_dlg.m_sSegStr);
			else //if(option_dlg.m_iOrgmatTblStyle==0)	//һ����ʽ
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
		{	//Ϋ������ wht 10-10-11
			pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
			AddSlotAndWeldCodeTblSheet(pWorksheet,1);
			iSheet++;
		}
	}
//������˨���ܱ�
	if(option_dlg.m_bLsSumMatTbl)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iLsTblStyle==2)	//����ͳ����˨
			AddBoltSumTblSheet(mat_tbl,pWorksheet);
		else
			AddLsSumMatTbl(mat_tbl,pWorksheet,option_dlg.m_iLsTblStyle);
		pWorksheet->Release();
	}
//���ɺ��������ϸ��
	if(option_dlg.m_bWeldPartTbl)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iWeldPartTblStyle==1)		//��������
			AddGuangZhouZengLiPartGroupSumMatTbl(mat_tbl,pWorksheet);
		else if(option_dlg.m_iWeldPartTblStyle==2)	//Ϋ�������麸��ϸ�� wht 10-09-30
			AddWeiFangChangAnPartGroupSumMatTbl(mat_tbl,pWorksheet);
		else 
			AddPartGroupSumMatTbl(mat_tbl,pWorksheet);
		pWorksheet->Release();
	}
	/*VARIANT vt;
	vt.pdispVal = (LPDISPATCH)pWorksheet;
	vt.vt = VT_DISPATCH;
	excel_sheets.Add(COleVariant(DISP_E_PARAMNOTFOUND,VT_ERROR),COleVariant(DISP_E_PARAMNOTFOUND,VT_ERROR),COleVariant((short)1),COleVariant((short)1));*/
//�������˱�
	if(option_dlg.m_bPoleExcelFile)
	{
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		iSheet++;
		if(option_dlg.m_iPoleSortType==0)//�����ͳ��
			AddPoleTblSheet(mat_tbl,pWorksheet);
		else if(option_dlg.m_iPoleSortType==1)//������������ֶκ����ֲ���
		{
			mat_tbl.SortPart(2);
			AddPoleTblSheet(mat_tbl,pWorksheet);
		}
		else if(option_dlg.m_iPoleSortType==2)//������������ֶκŲ����ֲ���
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
	//���в���ͳ��
	BeginWaitCursor();
	mat_tbl.RunSummarize(&Ta,TRUE);
	mat_tbl.SortPart(0);
	mat_tbl.SortLs();
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	int i=0;
	mat_tbl.SortPart(0);
	mat_tbl.SortPart(1,TRUE,FALSE);
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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
	excel_app.SetSheetsInNewWorkbook(1);	//�����ɹ�������Ĭ�Ϲ������������
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
	//�������˱�
	pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	AddPoleTblSheet(mat_tbl,pWorksheet);
	pWorksheet->Release();
	EndWaitCursor();
}
//������������
void CLDSDoc::OnExportTxtPartData() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"clb","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.clb)|*.clb||");
#else 
		"�ı����ϱ������ļ�(*.clb)|*.clb||");
#endif
	BeginWaitCursor();
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file��");
#else 
			AfxMessageBox("�򲻿�ָ���ļ���");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"Segment Number,Part Lable,Specification,Material Texture,Length/Area,Amount,Weld,Bend,Leg Cut,Cut Back,Cut Root,M16,M20,M24,Other Bolts,Open Merge Angle\n");
#else 
			fprintf(fp,"�κ�,�������,���,����,����/���,����,����,����,�н�,����,���,M16,M20,M24,������˨,���Ͻ�\n");
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
					if(CLDSPart::CustomerSerial==11)	//�����۵���
						sprintf(pPartRec->sSpec,"-%.fx%.fx%.f",pPartRec->thick,pPartRec->wide,pPartRec->length);
				}
				char kaihe[50]="";
				if(pPartRec->part_type==STEELTYPE_ANGLE)
				{	//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-23
					double fKaiHeAngle=pPartRec->wing_angle-90;
					fKaiHeAngle*=(fKaiHeAngle<0)?-1:1;
					if(CLDSPart::CustomerSerial==11)	//�����۵���
						fKaiHeAngle=pPartRec->wing_angle;
					if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f��",fKaiHeAngle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"����%.1f��",fKaiHeAngle);
#endif
					else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f��",fKaiHeAngle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"�Ͻ�%.1f��",fKaiHeAngle);
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
//������ʯ��
void CLDSDoc::OnExportTxtPartData2() 
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"dat","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.dat)|*.dat||");
#else 
		"�ı����ϱ������ļ�(*.dat)|*.dat||");
#endif
	BeginWaitCursor();
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file��");
#else 
			AfxMessageBox("�򲻿�ָ���ļ���");
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			fprintf(fp,"Block Number,Material Name,Design Specification,Borrow Specification,Material Texture,Length,Amount,Screw One Heavy,Weld One Heavy,Single Hole,Bend,Cold Bend,Leg Cut,Cut Back,Cut Root,Uncoated,Notes\n");
#else 
			fprintf(fp,"�������,��������,��ƹ��,���ù��,����,����,����,��˿����,���Ӽ���,��������,����,����,�н�,����,���,��Ϳ,��ע\n");
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
				{	//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-23
					if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f��",pPartRec->wing_angle-90);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"����%.1f��",pPartRec->wing_angle-90);
#endif
					else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f��",90-pPartRec->wing_angle);
#else 
						_snprintf(kaihe,MAX_TEMP_CHAR_50,"�Ͻ�%.1f��",90-pPartRec->wing_angle);
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
				fprintf(fp,"%s,%s,%s, ,%s,%.0f,%d,%.3f,%.3f,%d,%C,%C,%C,%C,%C,%C,%s,\n", //������ʯҪ����������һ������ wht 11-06-01
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
		"�ı����ϱ������ļ�(*.txt)|*.txt||");
#endif
	BeginWaitCursor();
	//���в���ͳ��
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
				AfxMessageBox("Can't open the specified file��");
#else 
				AfxMessageBox("�򲻿�ָ���ļ���");
#endif
			}
			else
			{
				//fprintf(fp,"�������,��������,��ƹ��,���ù��,����,����,����,��˿����,���Ӽ���,��������,����,����,�н�,����,���,��Ϳ,��ע\n");
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
						strcpy(parttype,"�Ǹ�");
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
						strcpy(parttype,"�ְ�");
#endif
						_snprintf(spec,MAX_TEMP_CHAR_50,"%.0f",pPartRec->thick);
						_snprintf(spec2,MAX_TEMP_CHAR_50,"%.0f	%.0f",pPartRec->wide,pPartRec->length);
						_snprintf(spec3,MAX_TEMP_CHAR_50,"-%.0f",pPartRec->thick);
						_snprintf(spec4,MAX_TEMP_CHAR_50,"%.0fx%.0f",pPartRec->wide,pPartRec->length);
						bHuoQu=pPartRec->bZhiWan;
					}
					char kaihe[MAX_TEMP_CHAR_50+1]="";
					if(pPartRec->part_type==STEELTYPE_ANGLE)
					{	//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-23
						if(pPartRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"Open Angle %.1f��",pPartRec->wing_angle-90);
#else 
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"����%.1f��",pPartRec->wing_angle-90);
#endif
						else if(90-pPartRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
#ifdef AFX_TARG_ENU_ENGLISH
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"Merge Angle %.1f��",90-pPartRec->wing_angle);
#else 
							_snprintf(kaihe,MAX_TEMP_CHAR_50,"�Ͻ�%.1f��",90-pPartRec->wing_angle);
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
					fprintf(fp,"%s	%s	�Ǹ���	%s	%s	%s	%s	%s	%.0f	%d	%.2f	0	%d	0	%d	%d		%C	%C	%C	%C	%C	%s	0	0	%s		%s		%C	%C\n",
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

//�㽭������ wht 11-07-12
void CLDSDoc::OnExportTxtPartData4()
{
	CTowerType mat_tbl;
	CFileDialog dlg(FALSE,"txt","part",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Text material table's data files(*.txt)|*.txt||");
#else 
		"�ı����ϱ������ļ�(*.txt)|*.txt||");
#endif
	BeginWaitCursor();
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart();
	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file��");
#else 
			AfxMessageBox("�򲻿�ָ���ļ���");
#endif
		}
		else
		{
			//fprintf(fp,"������,����(L/-),���, ,����(A3/16Mn),����,��������,����, 0.00,����,����,����,�н�,����, , ,��ע,����\n");
			for(PART *pPartRec=mat_tbl.part_recordset.GetFirst();pPartRec;pPartRec=mat_tbl.part_recordset.GetNext())
			{
				char sPartNo[16]=" ";	//���(���Ϊ��ʱ���һ���ո�)
				char sValue[16]=" ";
				sprintf(sValue,"%s",pPartRec->GetPartNo());
				if(strlen(sValue)>0&&pPartRec->iSeg.iSeg<10			//��Ų�Ϊ���Ҷκ�С��10
					&&sValue[0]>'0'&&sValue[0]<='9')	//��ŵ�һ���ַ�Ϊ1~9
				{
					if(g_sysPara.m_nMatCharPosType==1)
						sprintf(sPartNo,"%c0%s",pPartRec->cMaterial,sValue);				//���ǰ��0,���ڴӱ���еõ��κ���Ϣ
					else if(g_sysPara.m_nMatCharPosType==2)
						sprintf(sPartNo,"0%s%c",sValue,pPartRec->cMaterial);
					else
						sprintf(sPartNo,"0%s",sValue);
				}
				else 
					strcpy(sPartNo,pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator));	

				char sPartType[16]=" ";	//��������
				char sSpec[100]=" ";	//���
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
					strcpy(sPartType,"��1");
					sprintf(sSpec,"%.0fx%.0f",pPartRec->wide,pPartRec->thick);
				}
				else if(pPartRec->part_type==STEELTYPE_SLOT)
					strcpy(sPartType,"[");
				//���� A3->Q235(S) 16Mn->Q345(H)
				char sMat[10]=" ";
				if(pPartRec->cMaterial==CSteelMatLibrary::Q235BriefMark())//'S')
					strcpy(sMat,"A3");
				else if(pPartRec->cMaterial==CSteelMatLibrary::Q345BriefMark())//'H')
					strcpy(sMat,"16Mn");
				//�������нǡ�����
				char cHuoQu=' ',cCutAngle=' ',cCutBer=' ';
				if(pPartRec->bZhiWan)
					cHuoQu='1';
				if(pPartRec->bCutAngle)
					cCutAngle='1';
				if(pPartRec->bCutBer)
					cCutBer='1';
				//��ע
				char sNotes[200]=" ";
				strcat(sNotes,pPartRec->sNotes);
				double length=pPartRec->length;
				if(CLDSPlate::m_iPlankWeightStatType==2&&pPartRec->part_type==STEELTYPE_PLATE)
					length=pPartRec->real_area/pPartRec->wide;	//�㽭��������ְ峤��Ӧ����<�����/���>�������� wjh-2014.4.09
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
		"�ı���˨�������ļ�(*.lsb)|*.lsb||");
#endif
	BeginWaitCursor();
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta,TRUE);
	mat_tbl.SortLs();

	if(dlg.DoModal()==IDOK)
	{
		FILE *fp = fopen(dlg.GetPathName(),"wt");
		if(fp==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Can't open the specified file��");
#else 
			AfxMessageBox("�򲻿�ָ���ļ���");
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
					strcpy(name,"��Ȧ");
#endif
				else if(pLsRec->bFootNail)
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(name,"Footnail");
#else 
					strcpy(name,"�Ŷ�");
#endif
				else
#ifdef AFX_TARG_ENU_ENGLISH
					strcpy(name,"Bolt");
#else 
					strcpy(name,"��˨");
#endif
				fprintf(fp,"%s, %s, M%dX%d, %s, %d~%d, %3d, %.1f\n",(char*)pLsRec->iSeg.ToString(),name,
					pLsRec->d,pLsRec->L,pLsRec->grade,pLsRec->L0_downlim,pLsRec->L0_uplim,pLsRec->GetPartNum(),
					pLsRec->sum_weight);
			}
			fclose(fp);
		}
	}
}
//���ϻ�������
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
		AfxMessageBox("Can't open the specified file��");
#else 
		AfxMessageBox("�򲻿�ָ���ļ���");
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
			//���(���Ϊ��ʱ���һ���ո�)
			CXhChar16 sPartNo(pPartRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator));
			if(strlen(sPartNo)<5)
				sPartNo.ResizeLength(5,' ',true);
			//���� A3->Q235(S) 16Mn->Q345(H)
			char matStr[20]="";
			QuerySteelMatMark(pPartRec->cMaterial,matStr);
			//��ע
			char sNotes[200]=" ";
			pPartRec->GetProcessNotes(sNotes);
			strcat(sNotes,pPartRec->sNotes);
			//��������ˮ���񰴸ְ�����ʾ	wht 12-05-18
			char sSpec[50]="";
			if(pPartRec->part_type==STEELTYPE_PLATE)
				sprintf(sSpec,"-%.f",pPartRec->thick);
			else 
				strcpy(sSpec,pPartRec->sSpec);
			//
			double fWeight=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)//��������������� wht 11-06-01
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
				strcpy(sNote,"��Ȧ");
#endif
			}
			else if(pLsRec->bFootNail)
			{
				type=1;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sNote,"Footnail");
#else 
				strcpy(sNote,"�Ŷ�");
#endif
			}
			else
			{
				type=0;
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sNote,"Bolt");
#else 
				strcpy(sNote,"��˨");
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
	excel_sheet.SetName("��������");
#endif
	//
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+1,14);
	//��һ�б���
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
	map.SetValueAt(index,0,COleVariant("�������"));
	map.SetValueAt(index,1,COleVariant("�κ�"));
	map.SetValueAt(index,2,COleVariant("��������"));
	map.SetValueAt(index,3,COleVariant("����"));
	map.SetValueAt(index,4,COleVariant("���"));
	map.SetValueAt(index,5,COleVariant("����"));
	map.SetValueAt(index,6,COleVariant("��������"));
	map.SetValueAt(index,7,COleVariant("����"));
	map.SetValueAt(index,8,COleVariant("����"));
	map.SetValueAt(index,9,COleVariant("�н�"));
	map.SetValueAt(index,10,COleVariant("�ٸ�"));
	map.SetValueAt(index,11,COleVariant("����"));
	map.SetValueAt(index,12,COleVariant("���Ͻ�"));
	map.SetValueAt(index,13,COleVariant("�¿�"));
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
	//���ù�����Ÿ�ʽ
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A2",cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ ����
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"E1",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//�������ָ�ʽ
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
	excel_sheet.SetName("�ӹ�����");
#endif
	//
	int index=0;
	CVariant2dArray map(Ta.Module.GetNodeNum()+1,3);
	//��һ�б���
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Module Name"));
	map.SetValueAt(index,1,COleVariant("Process Basic Number"));
	map.SetValueAt(index,2,COleVariant("Segment Scope"));
#else 
	map.SetValueAt(index,0,COleVariant("��������"));
	map.SetValueAt(index,1,COleVariant("�ӹ�����"));
	map.SetValueAt(index,2,COleVariant("�κŷ�Χ"));
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
	//���õ�Ԫ���ʽ
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"C1",cell_end,COleVariant("@"));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
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
	excel_sheet.SetName("����");
#endif
	//
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+1,9);
	//��һ�б���
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
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("���"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("���"));
	map.SetValueAt(index,4,COleVariant("����"));
	map.SetValueAt(index,5,COleVariant("����"));
	map.SetValueAt(index,6,COleVariant("����"));
	map.SetValueAt(index,7,COleVariant("����"));
	map.SetValueAt(index,8,COleVariant("��ע"));
#endif
	index++;
	//
	for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		char matStr[20]="";
		QuerySteelMatMark(pRec->cMaterial,matStr);
		//��ע
		char sNotes[200]=" ";
		pRec->GetProcessNotes(sNotes);
		strcat(sNotes,pRec->sNotes);
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		map.SetValueAt(index,2,COleVariant(matStr));
		//��������ˮ���񰴸ְ�����ʾ	wht 12-05-18
		char sSpec[50]="";
		if(pRec->part_type==STEELTYPE_PLATE)
			sprintf(sSpec,"-%.f",pRec->thick);
		else 
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(sSpec));
		//
		double fWeight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)//��������������� wht 11-06-01
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
	//���ù�����Ÿ�ʽ
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A2",cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ ����
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"I1",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
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
	excel_sheet.SetName("��˨");
#endif
	//
	int index=0;
	//
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+1,6);
	//��һ�б���
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Grade Specification"));
	map.SetValueAt(index,2,COleVariant("Amount"));
	map.SetValueAt(index,3,COleVariant("Weight"));
	map.SetValueAt(index,4,COleVariant("Type"));
	map.SetValueAt(index,5,COleVariant("Notes"));
#else 
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("������"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("����"));
	map.SetValueAt(index,4,COleVariant("����"));
	map.SetValueAt(index,5,COleVariant("��ע"));
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
			strcpy(sNote,"��Ȧ");
#endif
		}
		else if(pLsRec->bFootNail)
		{
			type=1;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sNote,"Footnail");
#else 
			strcpy(sNote,"�Ŷ�");
#endif
		}
		else
		{
			type=0;
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sNote,"Bolt");
#else 
			strcpy(sNote,"��˨");
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
	//���ñ������
	sprintf(cell_end,"F%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
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
	excel_sheet.SetName("����");
#endif
	//
	int index=0;
	CVariant2dArray map(Ta.Module.GetNodeNum()+1,2);
	//��һ�б���
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Module Name"));
	map.SetValueAt(index,1,COleVariant("Segment Scope"));
#else 
	map.SetValueAt(index,0,COleVariant("��������"));
	map.SetValueAt(index,1,COleVariant("�κŷ�Χ"));
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
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
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
			excel_sheet.SetName("����������ϸ��");
		else if(part_type==STEELTYPE_ANGLE)
			excel_sheet.SetName("�Ǹֻ��ܱ�");
		else if(part_type==STEELTYPE_TUBE)
			excel_sheet.SetName("�ֹܻ��ܱ�");
		else if(part_type==STEELTYPE_PLATE)
			excel_sheet.SetName("�ְ���ܱ�");
		else if(part_type==STEELTYPE_FLAT)
			excel_sheet.SetName("�������ܱ�");
		else if(part_type==STEELTYPE_SLOT)
			excel_sheet.SetName("�۸ֻ��ܱ�");
#endif
	}
	if(part_type==TA_OTHER)
	{	//δָ��ͳ�ƵĹ������� ��ѡ�й�������ͳ��ʱ����Ҫ���жκŹ���
		CHashList<SEGI> segTable;
		GetSegNoHashTblBySegStr(segStr,segTable);
		for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(pRec->GetPartNum()==0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
				mat_tbl.part_recordset.DeleteCursor();
		}
		mat_tbl.part_recordset.Clean();
	}
	else //������ʾѡ�й���ʱ ʹ��Ĭ�ϸ�ʽ
		bDefTbl=FALSE;
	if(bDefTbl)
	{
		for(MERGECELL_CMD *pCmd=ExcelManager.mergecell_cmd.GetFirst();pCmd;pCmd=ExcelManager.mergecell_cmd.GetNext())
		{
			strcpy(cell_start,pCmd->cell_start);
			strcpy(cell_end,pCmd->cell_end);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			if(pCmd->cell_start[0]==pCmd->cell_end[0])	//����ϲ�
				excel_range.Merge(COleVariant((BYTE)0));
			else										//����ϲ�
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
						excel_range.SetValue(COleVariant("��"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_ANGLE")==0)
				{
					if(pRec->bCutAngle)
						excel_range.SetValue(COleVariant("��"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_BER")==0)
				{
					if(pRec->bCutBer)
						excel_range.SetValue(COleVariant("��"));
				}
				else if(stricmp(pColCell->cell_content,"CUT_ROOT")==0)
				{
					if(pRec->bCutRoot)
						excel_range.SetValue(COleVariant("��"));
				}
				excel_range.DetachDispatch();
				pRange->Release();
			}
		}
	}
	else
	{	// ��һ����������
		char sTitle[200]="";
		if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sTitle,"XXX Company Tower Raw Material Table");
#else 
			strcpy(sTitle,"XXX��˾����ԭʼ���ϱ�");
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
				strcpy(sTitle,"XXX��˾����������ϸ��"); 
			else if(part_type==STEELTYPE_ANGLE)
				strcpy(sTitle,"XXX��˾�����Ǹֻ��ܱ�");
			else if(part_type==STEELTYPE_TUBE)
				strcpy(sTitle,"XXX��˾�����ֹܻ��ܱ�");
			else if(part_type==STEELTYPE_PLATE)
				strcpy(sTitle,"XXX��˾�����ְ���ܱ�");
			else if(part_type==STEELTYPE_FLAT)
				strcpy(sTitle,"XXX��˾�����������ܱ�");
			else if(part_type==STEELTYPE_SLOT)
				strcpy(sTitle,"XXX��˾�����۸ֻ��ܱ�");
#endif
		}
		CExcelOper::SetMainTitle(excel_sheet,"A1","Q1",sTitle);
		
		int index=0;
		CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+2,17);
		// �ڶ����ӱ�����
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
		map.SetValueAt(index,0,COleVariant("����"));
#endif
		// ��д��������
		strcpy(cell_start,"B2");
		strcpy(cell_end,"D2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
		// ��������
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,4,COleVariant("Project Name"));
#else 
		map.SetValueAt(index,4,COleVariant("��������"));
#endif
		// ��д������������
		strcpy(cell_start,"F2");
		strcpy(cell_end,"K2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,7,COleVariant(Ta.m_sPrjName));
		// ��д��˨��������
		strcpy(cell_start,"L2");
		strcpy(cell_end,"O2");
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,11,COleVariant("Bolt Hole Number"));
#else 
		map.SetValueAt(index,11,COleVariant("��˨������"));
#endif
		excel_range.ReleaseDispatch();
		// ��д������
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,15,COleVariant("Basic Number"));
#else 
		map.SetValueAt(index,15,COleVariant("����"));
#endif
		map.SetValueAt(index,16,COleVariant((long)Ta.m_nTaNum));
		index++;
		
		//����������������У���Ҫ��ʽ���
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
		map.SetValueAt(index,0,COleVariant("�κ�"));
		map.SetValueAt(index,1,COleVariant("���"));
		map.SetValueAt(index,2,COleVariant("����"));
		map.SetValueAt(index,3,COleVariant("���"));
		map.SetValueAt(index,4,COleVariant("����"));
		map.SetValueAt(index,5,COleVariant("������"));
		map.SetValueAt(index,6,COleVariant("�ܼ���"));
		map.SetValueAt(index,7,COleVariant("������"));
		map.SetValueAt(index,8,COleVariant("����"));
		map.SetValueAt(index,9,COleVariant("����߳�"));
		map.SetValueAt(index,10,COleVariant("����"));
#endif
		map.SetValueAt(index,11,COleVariant("M16"));
		map.SetValueAt(index,12,COleVariant("M20"));
		map.SetValueAt(index,13,COleVariant("M24"));
		map.SetValueAt(index,14,COleVariant("M_X"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,15,COleVariant("Special Process"));
		map.SetValueAt(index,16,COleVariant("Special Process"));
#else 
		map.SetValueAt(index,15,COleVariant("���⹤��"));
		map.SetValueAt(index,16,COleVariant("���⹤��"));
#endif
		//�����п�
		CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A3","C3");
		CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.5,"D3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.0,"E3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,5.5,"F3","I3");
		CExcelOper::SetRangeColWidth(excel_sheet,5.0,"H3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,7.0,"J3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,4.0,"K3",NULL);
		CExcelOper::SetRangeColWidth(excel_sheet,2.5,"L3","O3");
		//�ϲ���
		pRange=excel_sheet.GetRange(COleVariant("Q3"),COleVariant("P3"));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Special Process"));
#else 
		excel_range.SetValue(COleVariant("���⹤��"));
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
		{	//��ѡ�й�������ͳ����ǰӦ���ȵõ���ǰҪͳ�����͹����ļ�¼��
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
				continue;	//ָ����������ʱ��������ָ�����͹���
			i++;
			index++;
			_snprintf(cell,15,"A%d",i+1);//�����ֶκ�
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
					if (part_type == TA_OTHER)	//δָ����������ʱ ��Ҫ��ʾ�κ�
						map.SetValueAt(index, 0, COleVariant(seg_start_no.ToString()));
				}
				else if ((part_type == TA_OTHER && pRec->iSeg.iSeg != seg_start_no.iSeg) || i - 2 == n)
				{
					_snprintf(cell_start, 15, "A%d", seg_start_i);
					//�ϲ����һ�ζκ�,���һ�ν���һ������ʱcell_endȡi wht 13-03-11
					if (i - 2 == n && pRec->iSeg.iSeg == seg_start_no.iSeg)
						_snprintf(cell_end, 15, "A%d", i + 1);
					else
						_snprintf(cell_end, 15, "A%d", i);
					//�ϲ���
					CExcelOper::MergeRowRange(excel_sheet, cell_start, cell_end);
					seg_start_i = i + 1;
					seg_start_no = pRec->iSeg;
					if (part_type == TA_OTHER)	//δָ����������ʱ ��Ҫ��ʾ�κ�
						map.SetValueAt(index, 0, COleVariant(seg_start_no.ToString()));

				}
			}
			map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			//����
			char mat_str[MAX_TEMP_CHAR_50+1]="";
			QuerySteelMatMark(pRec->cMaterial,mat_str);
			map.SetValueAt(index,2,COleVariant(mat_str));
			map.SetValueAt(index,3,COleVariant(pRec->sSpec));	//���
			//����
			char str[30];
			double length=pRec->length;
			if(CLDSPlate::m_iPlankWeightStatType==2&&pRec->part_type==STEELTYPE_PLATE)
				length=pRec->real_area/pRec->wide;	//��������ʱӦ���ݸְ徻�������Ƹְ峤��
			sprintf(str,"%8.1f",length);
			map.SetValueAt(index,4,COleVariant(str));
			map.SetValueAt(index,5,COleVariant((long)pRec->GetPartNum()));	//������
			_snprintf(cell,15,"=F%d*Q2",i+1);	//�ܼ���
			map.SetValueAt(index,6,COleVariant(cell));
			//����
			if(CLDSPlate::m_iPlankWeightStatType<=1)//��������������� wht 11-06-01
				sprintf(str,"%8.2f",pRec->fPieceMinWeight);
			else
				sprintf(str,"%8.2f",pRec->fPieceNetWeight);
			map.SetValueAt(index,7,COleVariant(str));
			//����
			_snprintf(cell,15,"=H%d*G%d",i+1,i+1);
			map.SetValueAt(index,8,COleVariant(cell));
			//���ӱ߳�
			CString st="";
			st.Format("%.1f",pRec->fWeldEdgeLen);
			map.SetValueAt(index,9,COleVariant((LPCTSTR)st));
			//����
			map.SetValueAt(index,10,COleVariant((long)pRec->nEdgeNum));
			if(pRec->nM16Ls>0)	//M16��˨��
				map.SetValueAt(index,11,COleVariant(pRec->nM16Ls));
			if(pRec->nM20Ls>0)	//M20��˨��
				map.SetValueAt(index,12,COleVariant(pRec->nM20Ls));
			if(pRec->nM24Ls>0)	//M24��˨��
				map.SetValueAt(index,13,COleVariant(pRec->nM24Ls));
			if(pRec->nMXLs>0)	//��������˨��
				map.SetValueAt(index,14,COleVariant(pRec->nMXLs));
			//���⹤��
			sprintf(cell_start,"Q%d",i+1);
			sprintf(cell_end,"P%d",i+1);
			CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
			char ss[200]="";
			pRec->GetProcessNotes(ss);
			strcat(ss,pRec->sNotes);
			map.SetValueAt(index,15,COleVariant(ss));
			map.SetValueAt(index,16,COleVariant(ss));
		}
		//���ù�����Ÿ�ʽ
		strcpy(cell_start,"B4");
		sprintf(cell_end,"B%d",i+1);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
		//���öκ�ˮƽ���뷽ʽ ���ж��� wht 13-03-11
		strcpy(cell_start,"A2");
		sprintf(cell_end,"A%d",i+1);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)3));
		//���ñ������
		strcpy(cell_start,"A2");
		sprintf(cell_end,"Q%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
		//���ñ߿�
		CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	}
	excel_sheet.DetachDispatch();
}

//�ɶ��������޹�˾ר������������ϸ��
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	// ��һ����������
#ifdef AFX_TARG_ENU_ENGLISH
	if(bOrgMat)
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX Company Tower Raw Material Table");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX Company Tower Block Table");
#else 
	if(bOrgMat)
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX��˾����ԭʼ���ϱ�");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","O1","XXX��˾����������ϸ��");
#endif
	// �ڶ��мӹ�������Ϣ
	// ��д������
	pRange = excel_sheet.GetRange(COleVariant("N2"));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number"));
#else 
	excel_range.SetValue(COleVariant("����"));
#endif
	excel_range.ReleaseDispatch();
	// ��д������
	pRange = excel_sheet.GetRange(COleVariant("O2"));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue(COleVariant((long)Ta.m_nTaNum));
	excel_range.ReleaseDispatch();
	//���κŹ��˼�¼
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum()+2,15);
	// �������ӱ�����
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("����"));
#endif
	// ��д��������
	CExcelOper::MergeColRange(excel_sheet,"B3","C3");
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	//��������
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("��������"));
#endif
	// ��д������������
	CExcelOper::MergeColRange(excel_sheet,"E3","H3");
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	// ��д��˨��������
	CExcelOper::MergeColRange(excel_sheet,"I3","M3");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,8,COleVariant("Bolt Hole Number"));
#else 
	map.SetValueAt(index,8,COleVariant("��˨������"));
#endif
	//�ϲ��ո���	
	CExcelOper::MergeColRange(excel_sheet,"N3","O3");
	index++;
	//����������������У���Ҫ��ʽ���
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
	map.SetValueAt(index,0,COleVariant("�κ�"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A4",NULL,TRUE);
	map.SetValueAt(index,1,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"B4",NULL,TRUE);
	map.SetValueAt(index,2,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,11.0,"C4",NULL,TRUE);
	map.SetValueAt(index,3,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"D4",NULL,TRUE);
	map.SetValueAt(index,4,COleVariant("������"));
	map.SetValueAt(index,5,COleVariant("�ܼ���"));
	map.SetValueAt(index,6,COleVariant("������"));
	map.SetValueAt(index,7,COleVariant("����"));
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
	map.SetValueAt(index,12,COleVariant("�ϼ�"));
	CExcelOper::SetRangeColWidth(excel_sheet,3.5,"M4",NULL,TRUE);
	map.SetValueAt(index,13,COleVariant("���⹤��"));
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"N4",NULL,TRUE);
	map.SetValueAt(index,14,COleVariant("��ע"));
#endif
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"O4",NULL,TRUE);
	index++;
	i=3;
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//�����ֶκ�
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//�������
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//���ʹ��
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(sText,"-%.0f_%s",pRec->thick,steelmat);
		else
		{
			if(stricmp(steelmat,"Q235")==0)
				strcpy(sText,pRec->sSpec);
			else
				sprintf(sText,"%s_%s",(char*)pRec->sSpec,steelmat);
		}
		map.SetValueAt(index,2,COleVariant(sText));
		//����
		char str[30];
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(str,"%.1fx%.1f",pRec->wide,pRec->length);
		else
			sprintf(str,"%8.1f",pRec->length);
		
		map.SetValueAt(index,3,COleVariant(str));
		//������
		map.SetValueAt(index,4,COleVariant((long)pRec->GetPartNum()));
		//�ܼ���
		sprintf(cell,"=E%d*O2",i+1);	//�ܼ���
		map.SetValueAt(index,5,COleVariant(cell));
		//����
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			sprintf(str,"%8.2f",pRec->fPieceMinWeight);
		else
			sprintf(str,"%8.2f",pRec->fPieceNetWeight);
		map.SetValueAt(index,6,COleVariant(str));
		//����
		sprintf(cell,"=G%d*F%d",i+1,i+1);	//����
		map.SetValueAt(index,7,COleVariant(cell));
		//M16��˨��
		if(pRec->nM16Ls>0)
			map.SetValueAt(index,8,COleVariant(pRec->nM16Ls));
		//M20��˨��
		if(pRec->nM20Ls>0)
			map.SetValueAt(index,9,COleVariant(pRec->nM20Ls));
		//M24��˨��
		if(pRec->nM24Ls>0)
			map.SetValueAt(index,10,COleVariant(pRec->nM24Ls));
		//��������˨��
		if(pRec->nMXLs>0)
			map.SetValueAt(index,11,COleVariant(pRec->nMXLs));
		//��˨�׺ϼ���
		sprintf(sText,"=I%d+J%d+K%d+L%d",i+1,i+1,i+1,i+1);	//����
		map.SetValueAt(index,12,COleVariant(sText));
		//���⹤��
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		map.SetValueAt(index,13,COleVariant(ss));
		//��ע
		map.SetValueAt(index,14,COleVariant(pRec->sNotes));
	}
	strcpy(cell_start,"A3");
	sprintf(cell_end,"O%d",i+1);
	//���ù�����Ÿ�ʽ
	strcpy(cell_start,"B5");
	sprintf(cell_end,"B%d",i+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ �Ҷ���
	strcpy(cell_start,"C5");
	sprintf(cell_end,"D%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)2));
	//���ñ������
	strcpy(cell_start,"A3");
	sprintf(cell_end,"O%d",i+1);	//cell_endӦȡi+1,�������������һ�� wht 13-03-11
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
//�㽭ʢ���ͱ���������޹�˾ר������������ϸ��
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
#endif
	}
	// ��һ����������
	strcpy(cell_start,"A1");
	strcpy(cell_end,"Q1");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("XXX Tower Company"));
#else 
	excel_range.SetValue(COleVariant("XXX�������޹�˾"));
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
		excel_range.SetValue(COleVariant("����ԭʼ���ϱ�"));
	else
		excel_range.SetValue(COleVariant("�����㲿����ϸ��"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	// ������
	strcpy(cell_start,"O3");
	strcpy(cell_end,"P3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number:"));
#else 
	excel_range.SetValue(COleVariant("����:"));
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

	// �������ӱ�����
	strcpy(cell_start,"A4");
	strcpy(cell_end,"Q4");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	char sText[200];
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sText,"Project Name��%s  Tower Model��%s  Steel Seal��%s  Task List Number��",Ta.m_sPrjName,Ta.m_sTaType,Ta.m_sTaStampNo);
#else 
	sprintf(sText,"�������ƣ�%s  ���ͣ�%s  ��ӡ���ţ�%s  ���񵥱�ţ�",Ta.m_sPrjName,Ta.m_sTaType,Ta.m_sTaStampNo);
#endif
	excel_range.SetValue(COleVariant(sText));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetSize(COleVariant(10.5));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();

	//������
	strcpy(cell,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Sequence"));
#else 
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("�㲿"));
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
	excel_range.SetValue(COleVariant("���"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("�ϼ�"));
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
	excel_range.SetValue(COleVariant("������(����)"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((long)3));
	excel_range.ReleaseDispatch();
	//������
	strcpy(cell,"H6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Electric"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"I6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Hole"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"J6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Bend"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"K6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Cut"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"L6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Shovel"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"M6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Clear"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"N6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Merge"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"O6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Open"));
#else 
	excel_range.SetValue(COleVariant("��"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"P6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Press"));
#else 
	excel_range.SetValue(COleVariant("ѹ"));
#endif
	excel_range.SetColumnWidth(COleVariant(2.5));
	excel_range.ReleaseDispatch();
	strcpy(cell,"Q6");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Notes"));
#else 
	excel_range.SetValue(COleVariant("��ע"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.5));
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	//������
	strcpy(cell,"A7");
	pRange = excel_sheet.GetRange(COleVariant(cell));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Number"));
#else 
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("���"));
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
	excel_range.SetValue(COleVariant("���"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
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
	excel_range.SetValue(COleVariant("��"));
#endif
	borders.AttachDispatch(excel_range.GetBorders(),FALSE);
	border.AttachDispatch(borders.GetItem(8),FALSE);//xlEdgeTop = 8
	border.SetLineStyle(COleVariant((short)-4142));	//xlNone =-4142;
	border.ReleaseDispatch();
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	//�ڰ�������������У���Ҫ��ʽ���
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
		//�������
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//����
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(pRec->sSpec,"-%.0f",pRec->thick);
		strcpy(sText,pRec->sSpec);
		char steelmat[20];
		QuerySteelMatMark(toupper(pRec->cMaterial),steelmat);
		strcat(sText,"_");
		strcat(sText,steelmat);
		map.SetValueAt(index,2,COleVariant(sText));
		//����
		if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant(ftol(pRec->length)));
		else if(pRec->part_type==STEELTYPE_PLATE)
		{	//����
			sprintf(sText,"%.0fx%.0f",pRec->wide,pRec->length);
			map.SetValueAt(index,3,COleVariant(sText));
		}
		//E��Ϊ���ù��,��Ϊ��
		//������
		map.SetValueAt(index,5,COleVariant((long)pRec->GetPartNum()));
		//�ܼ���
		sprintf(cell,"=F%d*Q3",i);	//�ܼ���
		map.SetValueAt(index,6,COleVariant(cell));
		//H��Ϊ�纸
		if(pRec->bWeldPart)	//�Ƿ�纸��
			map.SetValueAt(index,7,COleVariant("*"));
		//�ܿ���
		long nHole = pRec->nM16Ls+pRec->nM20Ls+pRec->nM24Ls+pRec->nMXLs;
		if(nHole>0)
			map.SetValueAt(index,8,COleVariant(nHole));
		//����
		if(pRec->bZhiWan)
			map.SetValueAt(index,9,COleVariant("*"));
		//�н�
		if(pRec->bCutAngle)
			map.SetValueAt(index,10,COleVariant("*"));
		//����
		if(pRec->bCutBer)
			map.SetValueAt(index,11,COleVariant("*"));
		//���
		if(pRec->bCutRoot)
			map.SetValueAt(index,12,COleVariant("*"));
		//�Ͻ�
		//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-23
		if(pRec->part_type==STEELTYPE_ANGLE&&90-pRec->wing_angle>g_sysPara.m_fKaiHeJiaoThreshold)
			map.SetValueAt(index,13,COleVariant("*"));
		//����
		//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-23
		if(pRec->part_type==STEELTYPE_ANGLE&&pRec->wing_angle-90>g_sysPara.m_fKaiHeJiaoThreshold)
			map.SetValueAt(index,14,COleVariant("*"));
		//ѹ��
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,15,COleVariant("*"));
		//��ע
		char ss[200]="";
		pRec->GetProcessNotes(ss,TRUE);	//�����ַ����������ϽǶ��� wht 11-05-24
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,16,COleVariant(ss));
	}
	//���ù�����Ÿ�ʽ
	strcpy(cell_start,"B8");
	sprintf(cell_end,"B%d",i);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ �����
	strcpy(cell_start,"C8");
	sprintf(cell_end,"D%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//���ñ������
	strcpy(cell_start,"A8");
	sprintf(cell_end,"Q%d",i);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//���ñ߿�
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
#endif
	}
	// ��һ����������
	strcpy(cell_start,"A1");
	strcpy(cell_end,"Q1");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("XXX Tower Company"));
#else 
	excel_range.SetValue(COleVariant("XXX�������޹�˾"));
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
		excel_range.SetValue(COleVariant("����ԭʼ���ϱ�"));
	else
		excel_range.SetValue(COleVariant("�����㲿����ϸ��"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.SetRowHeight(COleVariant(20.0));
	font.AttachDispatch(excel_range.GetFont(),FALSE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(14.0));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	// ������
	strcpy(cell_start,"L3");
	strcpy(cell_end,"M3");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tower Model:"));
#else 
	excel_range.SetValue(COleVariant("����:"));
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

	//���ġ�����
	strcpy(cell_start,"A4");
	strcpy(cell_end,"A5");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetWrapText(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Segment Number"));
#else 
	excel_range.SetValue(COleVariant("�κ�"));
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
	excel_range.SetValue(COleVariant("������"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("���Ϲ��"));
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
	excel_range.SetValue(COleVariant("��ƹ��"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("����(mm)"));
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
	excel_range.SetValue(COleVariant("���(mm)"));
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
	excel_range.SetValue(COleVariant("��������"));
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
	excel_range.SetValue(COleVariant("�ӹ�����"));
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
	excel_range.SetValue(COleVariant("������(����)"));
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
	excel_range.SetValue(COleVariant("�ӹ�˵��"));
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
	excel_range.SetValue(COleVariant("�麸"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("�н�"));
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
	excel_range.SetValue(COleVariant("���"));
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
	excel_range.SetValue(COleVariant("����"));
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
	excel_range.SetValue(COleVariant("Cut Angle"));	//��������¿�
#else 
	excel_range.SetValue(COleVariant("�ٽ�"));	//��������¿�
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
	excel_range.SetValue(COleVariant("���"));	//>=14mm
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
	//����������������У���Ҫ��ʽ���
	i=5;
	long n=mat_tbl.part_recordset.GetNodeNum();
	int index=0;
	CVariant2dArray map(mat_tbl.part_recordset.GetNodeNum(),18);
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//�κ�
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//�������
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//����
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(sText,"-",pRec->thick);
		else if(pRec->part_type==STEELTYPE_ANGLE)
			sprintf(sText,"��");
		else
			strcpy(sText,"");
		map.SetValueAt(index,2,COleVariant(sText));
		//���
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(sSpec,"%.0f",pRec->thick);
		else if(pRec->part_type==STEELTYPE_ANGLE)
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
		else
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(sSpec));
		//����
		char steelmat[20];
		if(pRec->cMaterial!=CSteelMatLibrary::Q235BriefMark())//'S')
		{
			QuerySteelMatMark(toupper(pRec->cMaterial),steelmat);
			map.SetValueAt(index,4,COleVariant(steelmat));
		}
		//����
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));
		//���
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));
		//��������
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		map.SetValueAt(index,7,COleVariant(weight));
		//������
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));
		//J��Ϊ�纸
		if(pRec->bWeldPart)
			map.SetValueAt(index,9,COleVariant("*"));
		//�ܿ���
		long nHole = pRec->nM16Ls+pRec->nM20Ls+pRec->nM24Ls+pRec->nMXLs;
		if(nHole>0)
			map.SetValueAt(index,10,COleVariant(nHole));
		//����
		if(pRec->bZhiWan)
			map.SetValueAt(index,11,COleVariant("*"));
		//�н�
		if(pRec->bCutAngle)
			map.SetValueAt(index,12,COleVariant("*"));
		//���
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,13,COleVariant("*"));
		//����
		if(pRec->bCutBer)
			map.SetValueAt(index,14,COleVariant("*"));
		//�ٽ�(��������¿�)
		if(pRec->bCutRoot||pRec->bNeedFillet)
			map.SetValueAt(index,15,COleVariant("*"));
		//���
		if(pRec->thick>=14)
			map.SetValueAt(index,16,COleVariant("*"));
		//��ע
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,17,COleVariant(ss));
	}
	//���ù����κš���Ÿ�ʽ
	strcpy(cell_start,"A6");
	sprintf(cell_end,"B%d",i);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ �����
	strcpy(cell_start,"C6");
	sprintf(cell_end,"H%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//���ñ������
	strcpy(cell_start,"A6");
	sprintf(cell_end,"R%d",i);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//���ñ߿�
	strcpy(cell_start,"A4");
	sprintf(cell_end,"R%d",i);
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//����ר������������ϸ��
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
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
	//��һ������������У���Ҫ��ʽ���
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
	map.SetValueAt(index,15,COleVariant("Leg Cut(��)"));
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
	map.SetValueAt(index,0,COleVariant("�κ�(ֻ��)"));
	map.SetValueAt(index,1,COleVariant("������"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("����"));
	map.SetValueAt(index,4,COleVariant("���"));
	map.SetValueAt(index,5,COleVariant("����(mm)"));
	map.SetValueAt(index,6,COleVariant("���(mm)"));
	map.SetValueAt(index,7,COleVariant("���(mm)"));
	map.SetValueAt(index,8,COleVariant("��������(��)"));
	map.SetValueAt(index,9,COleVariant("��������(kg)"));
	map.SetValueAt(index,10,COleVariant("������(kg)"));
	map.SetValueAt(index,11,COleVariant("��������"));
	map.SetValueAt(index,12,COleVariant("��ע"));
	map.SetValueAt(index,13,COleVariant("�纸(0/1)"));
	map.SetValueAt(index,14,COleVariant("����"));
	map.SetValueAt(index,15,COleVariant("�н�(��)"));
	map.SetValueAt(index,16,COleVariant("����(mm)"));
	map.SetValueAt(index,17,COleVariant("���(0/1)"));
	map.SetValueAt(index,18,COleVariant("���(0/1)"));
	map.SetValueAt(index,19,COleVariant("���Ͻ�(0/1)"));
	map.SetValueAt(index,20,COleVariant("���(0/1)"));
	map.SetValueAt(index,21,COleVariant("����(0/1)"));
	map.SetValueAt(index,22,COleVariant("����(0/1)"));
	map.SetValueAt(index,23,COleVariant("����"));
	map.SetValueAt(index,24,COleVariant("�Ƿ�ר��"));
	map.SetValueAt(index,25,COleVariant("����"));
	map.SetValueAt(index,26,COleVariant("����"));
	map.SetValueAt(index,27,COleVariant("�ܳ�"));
	map.SetValueAt(index,28,COleVariant("�׾�28~38mm"));
	map.SetValueAt(index,29,COleVariant("�׾�39~50mm"));
	map.SetValueAt(index,30,COleVariant("�׾�50����"));
#endif
	index++;
	//�����п�
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"A1","AE1");
	i=0;
	long n=mat_tbl.part_recordset.GetNodeNum();
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext(),index++)
	{
		i++;
		//�����ֶκ�
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		//�������
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
		//����
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));
		//����
		char str[30]="";
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(str,"%s","Connect Plate");
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			sprintf(str,"%s","Angle");
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			sprintf(str,"%s","Tube");
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			sprintf(str,"%s","U-steel");
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			sprintf(str,"%s","Flat Angle");
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			sprintf(str,"%s","����");
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			sprintf(str,"%s","�Ǹ�");
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			sprintf(str,"%s","�ֹ�");
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			sprintf(str,"%s","�۸�");
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			sprintf(str,"%s","���");
#endif
		map.SetValueAt(index,3,COleVariant(str));
		//���
		char sSpec[MAX_TEMP_CHAR_50+1]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//��ע�Ǹֹ���
		{
			char sSimpleSpec[50]="";
			CLDSLineAngle::GetAngleSimpleSpec(sSimpleSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			_snprintf(sSpec,MAX_TEMP_CHAR_50,"��%s",sSimpleSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			_snprintf(sSpec,MAX_TEMP_CHAR_50,"-%.f",pRec->thick);
		else 
			strcpy(sSpec,pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(sSpec));
		//����
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));
		//���
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));
		//���
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));
		//��������
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));
		//��������
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		map.SetValueAt(index,9,COleVariant(weight));
		//����
		sprintf(cell,"=I%d*J%d",i+1,i+1);	//����
		map.SetValueAt(index,10,COleVariant(cell));
		//��������
		map.SetValueAt(index,11,COleVariant((long)pRec->nMSumLs));
		//��ע
		char ss[200]="";
		pRec->GetProcessNotes(ss);
		strcat(ss,pRec->sNotes);
		map.SetValueAt(index,12,COleVariant(ss));
		//�Ƿ�纸��
		if(pRec->bWeldPart)
			map.SetValueAt(index,13,COleVariant("1"));
		else 
			map.SetValueAt(index,13,COleVariant("0"));
		//����
		if(pRec->bZhiWan)
			map.SetValueAt(index,14,COleVariant("1"));
		else 
			map.SetValueAt(index,14,COleVariant("0"));
		//�н�
		if(pRec->bCutAngle)
			map.SetValueAt(index,15,COleVariant("1"));
		else
			map.SetValueAt(index,15,COleVariant("0"));
		//����
		if(pRec->bCutBer)
			map.SetValueAt(index,16,COleVariant("1"));
		else
			map.SetValueAt(index,16,COleVariant("0"));
		//���
		if(pRec->bCutRoot)
			map.SetValueAt(index,17,COleVariant("1"));
		else
			map.SetValueAt(index,17,COleVariant("0"));
		//���
		if(pRec->nPushFlat)
			map.SetValueAt(index,18,COleVariant("1"));
		else 
			map.SetValueAt(index,18,COleVariant("0"));
		//���Ͻ�
		if(pRec->bKaiJiao||pRec->bHeJiao)
			map.SetValueAt(index,19,COleVariant("1"));
		else 
			map.SetValueAt(index,19,COleVariant("0"));
		//���
		if(pRec->thick>=14)
			map.SetValueAt(index,20,COleVariant("1"));
		else
			map.SetValueAt(index,20,COleVariant("0"));
		//����
		if(pRec->IsTubeNotch())
			map.SetValueAt(index,21,COleVariant("1"));
		else
			map.SetValueAt(index,21,COleVariant("0"));
		//�¿�
		if(pRec->bNeedFillet)
			map.SetValueAt(index,22,COleVariant("1"));
		else
			map.SetValueAt(index,22,COleVariant("0"));
		//����	???
		map.SetValueAt(index,23,COleVariant(""));
		//�Ƿ�ר��	???
		map.SetValueAt(index,24,COleVariant("1"));
		//����
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"֧��");
			else 
				strcpy(sTypeName,"����");
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
				strcpy(sTypeName,"��ˮ��");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"����");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"���");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"�׷���");
#endif
		}
		map.SetValueAt(index,25,COleVariant(sTypeName));
		//����
		map.SetValueAt(index,26,COleVariant(pRec->nEdgeNum));
		//�ܳ�
		char sPerimeter[MAX_TEMP_CHAR_50+1]="";
		sprintf(sPerimeter,"%.1f",pRec->fPerimeter);
		map.SetValueAt(index,27,COleVariant(sPerimeter));
		//�׾�28~38mm
		char sLsNum[50]="";
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(28,38));
		map.SetValueAt(index,28,COleVariant(sLsNum));
		//�׾�39~50mm
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(39,50));
		map.SetValueAt(index,29,COleVariant(sLsNum));
		//�׾�50mm����
		sprintf(sLsNum,"%d",pRec->GetLsHoleNum(51));
		map.SetValueAt(index,30,COleVariant(sLsNum));
	}
	//���ù�����Ÿ�ʽ
	strcpy(cell_start,"B2");
	sprintf(cell_end,"B%d",i+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//����ˮƽ���뷽ʽ �����
	strcpy(cell_start,"F2");
	sprintf(cell_end,"L%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)4));
	//���ñ������
	strcpy(cell_start,"A1");
	sprintf(cell_end,"AE%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
}
//�����麸�������ͱ�
//nRuleType �������ͱ�Ź���
//nRuleType==0 ���ţ�LRM(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ� 3.�ڵ������ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL 6.��� 7.ƽ���и�
//nRuleType==1 ���ţ�LMR(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL
void CLDSDoc::AddSlotAndWeldCodeTblSheet(LPDISPATCH pSheet,int nRuleType)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Slot Group Weld Type Table");
#else 
	excel_sheet.SetName("�����麸�������ͱ�");
#endif
	//
	int nMidType=4,nType=7,nRowNum=0;
	int i=0,j=0,k=0,index=0;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//�м�������ʽ
		nType=5;	//����������ʽ
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
	//��һ������������У���Ҫ��ʽ���
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Sequence Number"));
	map.SetValueAt(index,1,COleVariant("Code Name"));
	map.SetValueAt(index,2,COleVariant("Instruction"));
#else 
	map.SetValueAt(index,0,COleVariant("���"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("˵��"));
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
	CString sMidTypeName[4]={"","�м�ڵ��","�м�����","�м����ܼ��ڵ��"};
	CString sTypeName[7]={"һ���Ͳ��","U�Ͳ��","T�Ͳ��","ʮ�ֲ��","����","���","ƽ���и�"};
#endif
	nMidType=4;
	nType=7;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//�м�������ʽ
		nType=5;	//����������ʽ
		map.SetValueAt(index,0,COleVariant("1"));
		map.SetValueAt(index,1,COleVariant("0"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,2,COleVariant("Main Tube Weld"));
#else 
		map.SetValueAt(index,2,COleVariant("���ܺ���"));
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
				//���
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
					sprintf(sHelpStr,"����%s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"һ��%s��һ��%s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
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
	CString sHelpStr[4]={"����Ǹֺ���","���ֺ���","���ݺ���","��������"};
#endif
	for(i=0;i<4;i++)
	{
		char sID[50]="";
		sprintf(sID,"%d",nCurRow-1);
		map.SetValueAt(index,0,COleVariant(sID));
		char sCode[50]="";
		//���
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
	//���ñ������
	sprintf(cell_end,"C%d",nCurRow-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
//������������������ϸ��
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
#endif
	}
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(segStr,segTable);
	//���κŹ���
	for(pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
	{
		if(pRec->GetPartNum()<=0||segTable.GetValue(pRec->iSeg.iSeg)==NULL)
			mat_tbl.part_recordset.DeleteCursor();
	}
	mat_tbl.part_recordset.Clean();

	//��һ������������У���Ҫ��ʽ���
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
	str_arr[0]="�κ�";
	str_arr[1]="������";
	str_arr[2]="����";
	str_arr[3]="����";
	str_arr[4]="���";
	str_arr[5]="����";
	str_arr[6]="���";
	str_arr[7]="���";
	str_arr[8]="��������";
	str_arr[9]="��������";
	str_arr[10]="��ע";
	str_arr[11]="�纸";
	str_arr[12]="����";
	str_arr[13]="�н�";
	str_arr[14]="����";
	str_arr[15]="���";
	str_arr[16]="���";
	str_arr[17]="���Ͻ�";
	str_arr[18]="���";
	str_arr[19]="�¿�";
	str_arr[20]="�������";
	str_arr[21]="������ʽ";
	str_arr[22]="����";
	str_arr[23]="����";
	str_arr[24]="�ܳ�";
	str_arr[25]="�����";
	str_arr[26]="���׾�����";
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
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//���
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));		//����
		//����
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,3,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,3,COleVariant("����"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("�Ǹ�"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("�ֹ�"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("�۸�"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("���"));
#endif
		//���
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//��ע�Ǹֹ���
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("��%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
		map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));//����
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));	//���
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));	//���
		map.SetValueAt(index,8,COleVariant((long)pRec->GetPartNum()));	//��������
		map.SetValueAt(index,9,COleVariant((long)pRec->nMSumLs));//��������
		map.SetValueAt(index,10,COleVariant(pRec->sNotes));		//��ע
		//�Ƿ�纸��
		if(pRec->bWeldPart)
			map.SetValueAt(index,11,COleVariant("1"));
		//����
		if(pRec->bZhiWan)
			map.SetValueAt(index,12,COleVariant("1"));
		//�н�
		if(pRec->bCutAngle)
			map.SetValueAt(index,13,COleVariant("1"));
		//����
		if(pRec->bCutBer)
			map.SetValueAt(index,14,COleVariant("1"));
		//���
		if(pRec->bCutRoot)
			map.SetValueAt(index,15,COleVariant("1"));
		//���
		if(pRec->nPushFlat>0)
			map.SetValueAt(index,16,COleVariant("1"));
		//���Ͻ�
		if(pRec->bKaiJiao||pRec->bHeJiao)
			map.SetValueAt(index,17,COleVariant("1"));
		//���
		if(pRec->thick>=14)
			map.SetValueAt(index,18,COleVariant("1"));
		//�¿�
		if(pRec->bNeedFillet)
			map.SetValueAt(index,19,COleVariant("1"));
		//�������
		if(pRec->IsTubeTransect())
			map.SetValueAt(index,20,COleVariant("1"));
		//��������
		char sType[100]="";
		int nType=pRec->GetNotchType(sType,0);
		if(nType>0)
			map.SetValueAt(index,21,COleVariant(sType));
		//����
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"֧��");
			else 
				strcpy(sTypeName,"����");
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
				strcpy(sTypeName,"��ˮ��");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"����");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"���");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"�׷���");
#endif
		}
		map.SetValueAt(index,22,COleVariant(sTypeName));
		//����
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,23,COleVariant(pRec->nEdgeNum));
		//�ܳ�
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,24,COleVariant(pRec->fPerimeter));
		//���
		if(pRec->part_type==STEELTYPE_PLATE)
			map.SetValueAt(index,25,COleVariant(pRec->real_area));
		//�����׾�
		char sHoleStr[500]="";
		if(pRec->GetLsHoleStr(sHoleStr)>0)
			map.SetValueAt(index,26,COleVariant(sHoleStr));
	}
	char cell_end[15]="";
	//�����Ҷ���
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//�������ָ�ʽ
	sprintf(cell_end,"Z%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"Y2",cell_end,COleVariant("0.0"));
	//���ñ������
	sprintf(cell_end,"AA%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}

//Ϋ����������������ϸ�� wht 10-10-11
void CLDSDoc::AddWeiFangChangAnPartSpecTblSheet(CTowerType &mat_tbl, LPDISPATCH pSheet,BOOL bOrgMat,CString segStr)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Loft Raw Material Table");
#else 
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
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

	//��һ������������У���Ҫ��ʽ���
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
	str_arr[9]="Piece Weight";	//���� 10-10-12
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
	str_arr[28]="Weld Edge mm";//���� 10-10-12
#else 
	str_arr[0]="�κ�";
	str_arr[1]="������";
	str_arr[2]="����";
	str_arr[3]="����";
	str_arr[4]="���";
	str_arr[5]="����";
	str_arr[6]="���";
	str_arr[7]="���";
	str_arr[8]="��������";
	str_arr[9]="��������";	//���� 10-10-12
	str_arr[10]="��������";
	str_arr[11]="��ע";
	str_arr[12]="�纸";
	str_arr[13]="����";
	str_arr[14]="�н�";
	str_arr[15]="����";
	str_arr[16]="���";
	str_arr[17]="���";
	str_arr[18]="���Ͻ�";
	str_arr[19]="���";
	str_arr[20]="�¿�";
	str_arr[21]="�������";
	str_arr[22]="������ʽ";
	str_arr[23]="����";
	str_arr[24]="����";
	str_arr[25]="�ܳ�";
	str_arr[26]="�����";
	str_arr[27]="���׾�����";
	str_arr[28]="���ӱ�mm";//���� 10-10-12
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
		map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//���
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));		//����
		//����
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//�ְ�
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
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//�ְ�
		{
			if(pRec->sub_type==PARAMPLATE_FL)
				map.SetValueAt(index,3,COleVariant("����"));
			else if(pRec->sub_type==PARAMPLATE_WATER)
				map.SetValueAt(index,3,COleVariant("��ˮ��"));
			else if(pRec->sub_type==PLATE_FL)
				map.SetValueAt(index,3,COleVariant("�׷���"));
			else 
				map.SetValueAt(index,3,COleVariant("�ְ�"));
		}
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("�Ǹ�"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("�ֹ�"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("�۸�"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("���"));
#endif
		//���
		CString str="";
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//��ע�Ǹֹ���
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("��%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
		if(CLDSPlate::m_iPlankWeightStatType==2&&pRec->part_type==STEELTYPE_PLATE)	//��������ְ���Ӧ����<�����/����>�������� wjh-2014.4.09
			map.SetValueAt(index,5,COleVariant(ftol(pRec->real_area/pRec->wide)));		//���
		else
			map.SetValueAt(index,5,COleVariant(ftol(pRec->length)));	//����
		map.SetValueAt(index,6,COleVariant(ftol(pRec->wide)));		//���
		map.SetValueAt(index,7,COleVariant(ftol(pRec->thick)));		//���
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum())));	//��������
		double weight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)
			weight=ftol(pRec->fPieceMinWeight*100)/100.0;
		else
			weight=ftol(pRec->fPieceNetWeight*100)/100.0;
		str.Format("%.2f",weight);			//��������
		map.SetValueAt(index,9,COleVariant(str));
		str.Format("%d",pRec->nMSumLs);		//��������
		map.SetValueAt(index,10,COleVariant(str));
		char notes[200];
		pRec->GetProcessNotes(notes);
		str.Format("%s %s",notes,pRec->sNotes);//��ע
		map.SetValueAt(index,11,COleVariant(str));
		if(pRec->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,12,COleVariant("1"));
		if(pRec->bZhiWan)		//����
			map.SetValueAt(index,13,COleVariant("1"));
		if(pRec->bCutAngle)		//�н�
			map.SetValueAt(index,14,COleVariant("1"));
		if(pRec->bCutBer)		//����
			map.SetValueAt(index,15,COleVariant("1"));
		if(pRec->bCutRoot)		//���
			map.SetValueAt(index,16,COleVariant("1"));
		if(pRec->nPushFlat>0)	//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
			map.SetValueAt(index,17,COleVariant((long)pRec->nPushFlat));
		if(pRec->bKaiJiao)			//����
			map.SetValueAt(index,18,COleVariant("1"));
		else if(pRec->bHeJiao)		//�Ͻ�
			map.SetValueAt(index,18,COleVariant("2"));
		if(pRec->thick>=14)			//���
			map.SetValueAt(index,19,COleVariant("1"));
		if(pRec->bNeedFillet)		//�¿�
			map.SetValueAt(index,20,COleVariant("1"));
		if(pRec->IsTubeTransect())	//�������
			map.SetValueAt(index,21,COleVariant("1"));
		//��������
		char sType[100]="";
		int nType=pRec->GetNotchType(sType,1);
		if(nType>0)
			map.SetValueAt(index,22,COleVariant(sType));
		//����
		char sTypeName[100]="";
		if(pRec->part_type==STEELTYPE_TUBE)
		{
			if(pRec->bWeldPart)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sTypeName,"Branch Tube");
			else 
				strcpy(sTypeName,"Main Tube");
#else 
				strcpy(sTypeName,"֧��");
			else 
				strcpy(sTypeName,"����");
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
				strcpy(sTypeName,"��ˮ��");
			else if(pRec->sub_type==PARAMPLATE_FL)
				strcpy(sTypeName,"����");
			else if(pRec->sub_type==PARAMPLATE_C
				||pRec->sub_type==PARAMPLATE_U
				||pRec->sub_type==PARAMPLATE_X)
				strcpy(sTypeName,"���");
			else if(pRec->sub_type==PLATE_FL)
				strcpy(sTypeName,"�׷���");
#endif
		}
		map.SetValueAt(index,23,COleVariant(sTypeName));
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,24,COleVariant((long)pRec->nEdgeNum));
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//�ܳ�
			str.Format("%.1f",pRec->fPerimeter);
			map.SetValueAt(index,25,COleVariant(str));
		}
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//���
			if(CLDSPlate::m_iPlankWeightStatType==2)
				str.Format("%.1f",pRec->real_area);
			else
				str.Format("%.1f",pRec->min_area);
			map.SetValueAt(index,26,COleVariant(str));
		}
			//�����׾�
		char sHoleStr[500]="";
		if(pRec->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		if(pRec->part_type==STEELTYPE_PLATE)
		{	//���ӱ�mm
			str.Format("%.1f",pRec->fWeldEdgeLen);
			map.SetValueAt(index,28,COleVariant(str));
		}
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"AC%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
}

//���ݷɽ�
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
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
	//��һ������������У���Ҫ��ʽ���
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
	str_arr[0]="����";
	str_arr[1]="�κ�";
	str_arr[2]="����";
	str_arr[3]="����";
	str_arr[4]="����";
	str_arr[5]="���";
	str_arr[6]="����";
	str_arr[7]="��������";
	str_arr[8]="����";
	str_arr[9]="������";
	str_arr[10]="��  ��";
#endif
	str_arr[11]="��13.5";
	str_arr[12]="��17.5";
	str_arr[13]="��19.5";
	str_arr[14]="��21.5";
	str_arr[15]="��25.5";
#ifdef AFX_TARG_ENU_ENGLISH
	str_arr[16]="Total Hole Number";
	str_arr[17]="Process";
#else 
	str_arr[16]="�ܿ���";
	str_arr[17]="��  ��";
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
		map.SetValueAt(index,0,COleVariant(Ta.m_sTaType));		//����
		map.SetValueAt(index,1,COleVariant(pRec->iSeg.ToString()));		//�κ�
		map.SetValueAt(index,2,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));		//����
		//����
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,3,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,3,COleVariant("����"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("�Ǹ�"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("�ֹ�"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("�۸�"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,3,COleVariant("���"));
#endif
		//����
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,4,COleVariant(steelmat));			
		//���
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//��ע�Ǹֹ���
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("��%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,5,COleVariant(str));
		map.SetValueAt(index,6,COleVariant(ftol(pRec->length)));//����
		map.SetValueAt(index,7,COleVariant(pRec->GetPartNum()));		//��������
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum()*Ta.m_nTaNum)));//����
		if(CLDSPlate::m_iPlankWeightStatType<=1)//��������������� wht 11-06-01
		{
			str.Format("%8.2f",pRec->fPieceMinWeight);	//������
			map.SetValueAt(index,9,COleVariant(str));
			str.Format("%8.2f",pRec->GetPartNum()*pRec->fPieceMinWeight);//����
			map.SetValueAt(index,10,COleVariant(str));
		}
		else
		{
			str.Format("%8.2f",pRec->fPieceNetWeight);	//������
			map.SetValueAt(index,9,COleVariant(str));
			str.Format("%8.2f",pRec->GetPartNum()*pRec->fPieceNetWeight);//����
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
	//�����Ҷ���
	sprintf(cell_end,"F%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"C2",cell_end,COleVariant((long)2));
	//�������ָ�ʽ
	sprintf(cell_end,"K%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"J2",cell_end,COleVariant("0.00"));
	//���ñ������
	sprintf(cell_end,"R%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}

//�㽭����
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
		excel_sheet.SetName("����ԭʼ���ϱ�");
#endif
	else
	{
		mat_tbl.SortPart(1);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Tower Block Table");
#else 
		excel_sheet.SetName("����������ϸ��");
#endif
	}
	// ��һ����������
	if(bOrgMat)
#ifdef AFX_TARG_ENU_ENGLISH
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX Tower Company Tower Raw Material Table");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX Tower Company Tower Block Table");
#else 
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX�������޹�˾ ����ԭʼ���ϱ�");
	else
		CExcelOper::SetMainTitle(excel_sheet,"A1","AA1","XXX�������޹�˾ �����㲿����ϸ��");
#endif
	// �������ӱ�����
	FontLegacy font;
	LPDISPATCH pRange;
	pRange = excel_sheet.GetRange(COleVariant("A2"),COleVariant("AA2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	char sText[200];
	CTime t=CTime::GetCurrentTime();
	CString sTime=t.Format("%Y/%m/%d");
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sText,"Contract No��                 Tower Model��%s                Make Date��%s",Ta.m_sTaType,sTime);
#else 
	sprintf(sText,"��ͬ�ţ�                 ���ͣ�%s                �Ʊ����ڣ�%s",Ta.m_sTaType,sTime);
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
	//��һ������������У���Ҫ��ʽ���
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
	str_arr[0]="������";
	str_arr[1]="��������";
	str_arr[2]="����";
	str_arr[3]="���";
	str_arr[4]="�ߴ�";
	str_arr[5]="����";
	str_arr[6]="��������";
	str_arr[7]="����";
	str_arr[8]="�������";
	str_arr[9]="��  ��";
	str_arr[10]="�������";
#endif
	str_arr[11]="��13.5";
	str_arr[12]="��17.5";
	str_arr[13]="��19.5";
	str_arr[14]="��21.5";
	str_arr[15]="��25.5";
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
	str_arr[16]="�ؿ�";
	str_arr[17]="�н�";
	str_arr[18]="����";
	str_arr[19]="����";
	str_arr[20]="���";
	str_arr[21]="�纸";
	str_arr[22]="����";
	str_arr[23]="�Ͻ�";
	str_arr[24]="���";
	str_arr[25]="��ע";
	str_arr[26]="������";
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
		map.SetValueAt(index,0,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));			//���
		//����
#ifdef AFX_TARG_ENU_ENGLISH
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,1,COleVariant("Connect Plate"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,1,COleVariant("Angle"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,1,COleVariant("Tube"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,1,COleVariant("U-steel"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,1,COleVariant("Flat Angle"));
#else 
		if(pRec->part_type==STEELTYPE_PLATE)	//����
			map.SetValueAt(index,1,COleVariant("����"));
		else if(pRec->part_type==STEELTYPE_ANGLE)//�Ǹ�
			map.SetValueAt(index,1,COleVariant("�Ǹ�"));
		else if(pRec->part_type==STEELTYPE_TUBE)//�ֹ�
			map.SetValueAt(index,1,COleVariant("�ֹ�"));
		else if(pRec->part_type==STEELTYPE_SLOT)//�۸�
			map.SetValueAt(index,1,COleVariant("�۸�"));
		else if(pRec->part_type==STEELTYPE_FLAT)//���
			map.SetValueAt(index,1,COleVariant("���"));
#endif
		//����
		char steelmat[10]="";
		if(!QuerySteelMatMark(pRec->cMaterial,steelmat))
			strcpy(steelmat,"Q235");
		map.SetValueAt(index,2,COleVariant(steelmat));
		//���
		CString str;
		char sSpec[30]="";
		if(pRec->part_type==STEELTYPE_ANGLE)	//��ע�Ǹֹ���
		{
			CLDSLineAngle::GetAngleSimpleSpec(sSpec,pRec->wide,pRec->y_wing_wide,pRec->thick);
			str.Format("��%s",sSpec);
		}
		else if(pRec->part_type==STEELTYPE_PLATE&&pRec->sub_type!=PARAMPLATE_WATER
			&&pRec->sub_type!=PARAMPLATE_FL&&pRec->sub_type!=PLATE_FL)
			str.Format("-%.f",pRec->thick);
		else 
			str.Format("%s",(char*)pRec->sSpec);
		map.SetValueAt(index,3,COleVariant(str));
		//
		double fPieceWeight=0;
		if(CLDSPlate::m_iPlankWeightStatType<=1)//��������������� wht 11-06-01
			fPieceWeight=pRec->fPieceMinWeight;
		else
			fPieceWeight=pRec->fPieceNetWeight;
		str.Format("%.fx%.f",pRec->wide,pRec->length);//�ߴ�			
		map.SetValueAt(index,4,COleVariant(str));
		str.Format("%.1f",fPieceWeight);			//����
		map.SetValueAt(index,5,COleVariant(str));
		map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//��������
		map.SetValueAt(index,7,COleVariant((long)Ta.m_nTaNum));	//����
		map.SetValueAt(index,8,COleVariant((long)(pRec->GetPartNum()*Ta.m_nTaNum)));//�������
		str.Format("%.2f",pRec->GetPartNum()*fPieceWeight);	//����
		map.SetValueAt(index,9,COleVariant(str));
		str.Format("%.2f",pRec->GetPartNum()*fPieceWeight*Ta.m_nTaNum);//�������
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
		//�н�
		if(pRec->bCutAngle)
			map.SetValueAt(index,17,COleVariant("V"));
		//����
		if(pRec->bZhiWan)
			map.SetValueAt(index,18,COleVariant("V"));
		//����
		if(pRec->bCutBer)
			map.SetValueAt(index,19,COleVariant("V"));
		//���
		if(pRec->bCutRoot)
			map.SetValueAt(index,20,COleVariant("V"));
		//�纸
		if(pRec->bWeldPart)
			map.SetValueAt(index,21,COleVariant("V"));
		//����
		if(pRec->bKaiJiao)
			map.SetValueAt(index,22,COleVariant("V"));
		//�Ͻ�
		if(pRec->bHeJiao)
			map.SetValueAt(index,23,COleVariant("V"));
		//���
		if(pRec->nPushFlat)
			map.SetValueAt(index,24,COleVariant("V"));
		//��ע
		char sNotes[100]="";
		strcpy(sNotes,pRec->sNotes);
		if(pRec->bNeedFillet)
#ifdef AFX_TARG_ENU_ENGLISH
			strcat(sNotes," Slot");
#else 
			strcat(sNotes," �¿�");
#endif
		//�������
		if(pRec->IsTubeTransect())
#ifdef AFX_TARG_ENU_ENGLISH
			strcat(sNotes," Interset Line");
#else 
			strcat(sNotes," �������");
#endif
		map.SetValueAt(index,25,COleVariant(sNotes));
	}
	
	char cell_end[15]="";
	//���������
	sprintf(cell_end,"E%d",i);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"AA%d",i);
	CExcelOper::SetRangeValue(excel_sheet,"A4",cell_end,map.var);
	//���ñ߿�
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
	if(iStyle==0)	//������ͳ����ʽһ(��ͨ����)
	{
		excel_sheet.AttachDispatch(pSheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Bolt Summary Table");
#else 
		excel_sheet.SetName("��˨���ܱ�");
#endif
		// ��һ����������
#ifdef AFX_TARG_ENU_ENGLISH
		CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX Company Bolt Summary Table",20.0,14.0);
#else 
		CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX��˾������˨���ܱ�",20.0,14.0);
#endif
		//
		int index=0;
		CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,9);
		// �ڶ����ӱ�����
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
		map.SetValueAt(index,0,COleVariant("����"));
#endif
		// ��д��������
		map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
		CExcelOper::MergeColRange(excel_sheet,"B2","C2");
		//��������
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
		map.SetValueAt(index,3,COleVariant("��������"));
#endif
		// ��д������������
		map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
		CExcelOper::MergeColRange(excel_sheet,"E2","F2");
		//
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,6,COleVariant("Special Bolt Amount"));
#else 
		map.SetValueAt(index,6,COleVariant("����������˨����"));
#endif
		CExcelOper::MergeColRange(excel_sheet,"G2","I2");
		index++;
		//����������������У���Ҫ��ʽ���
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
		map.SetValueAt(index,0,COleVariant("��־����"));
		map.SetValueAt(index,1,COleVariant("����"));
		map.SetValueAt(index,2,COleVariant("������"));
		map.SetValueAt(index,3,COleVariant("�������"));
		map.SetValueAt(index,4,COleVariant("��������"));
		map.SetValueAt(index,5,COleVariant("��������"));
		map.SetValueAt(index,6,COleVariant("˫ñ��˨"));
		map.SetValueAt(index,7,COleVariant("������˨"));
		map.SetValueAt(index,8,COleVariant("���ɵ�"));
#endif
		index++;
		//�����п�
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
					//�ϲ���ĵ�Ԫ�����ݣ��������õ�һ����Ԫ�����ݼ���
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
					//�ϲ���
					CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
					module_start_i=i+1;
					module_start_no=pLsRec->iModuleNo;
					cell_start[0]=cell_end[0]='B';
					//
					map.SetValueAt(index,0,COleVariant(pLsRec->sModule));
					map.SetValueAt(index,1,COleVariant("1"));
					//�ϲ���
					CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
				}
				pPrevLsRec=pLsRec;
			}
			//���
			char guige[20];
			if(pLsRec->typels0_pad1==0)
				sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
			else
				sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
			map.SetValueAt(index,2,COleVariant(guige));
			//�������;
			char ss[8];
			sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,3,COleVariant(ss));
			//��������
			map.SetValueAt(index,4,COleVariant(pLsRec->sum_weight));
			//������
			map.SetValueAt(index,5,COleVariant(pLsRec->GetPartNum()));
			//˫ñ��
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,6,COleVariant(pLsRec->nTwoCap));
			//��������
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,7,COleVariant(pLsRec->nBurglarproof));
			//���ɵ�����
			if(pLsRec->typels0_pad1==0)
				map.SetValueAt(index,8,COleVariant(pLsRec->nSpringPad));
		}
		//
		//�������ָ�ʽ
		strcpy(cell_start,"E4");
		sprintf(cell_end,"E%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,cell_start,cell_end,COleVariant("0.0"));
		//���ñ������
		sprintf(cell_end,"I%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
		//���ñ߿�
		CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
		excel_sheet.DetachDispatch();
	}
	else if(iStyle==1)	//������ͳ����ʽһ(������ˮ������)
	{
		excel_sheet.AttachDispatch(pSheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Bolt Summary Table");
#else 
		excel_sheet.SetName("��˨���ܱ�");
#endif
		int i=1,num=0;
		int nCommonLs,nTwoCapLs,nCommonNail,nBurglarLs,nBurglarNail,nPad;
		int iCommonLs,iTwoCapLs,iCommonNail,iBurglarLs,iBurglarNail,iPad;
		int iCommonLsStart,iTwoCapLsStart,iCommonNailStart,iBurglarLsStart,iBurglarNailStart,iPadStart;
		long module_start_i=-1,module_start_no=-1,n=mat_tbl.ls_recordset.GetNodeNum();
		int iWear;//������ʾ�ǵ�Ȧ�ĺ����ʻ��������ĺ�����
		int iBoltDNum=0;//������¼������˨�Ĺ�������
		int boltDArr[10];//��¼������˨ÿ������������
		int iNutStart=0;
		bool bIsFirst=true;//�ǲ��ǵ�һ�ν��������˨�������
		CLsRecord *pLsRec,*pPrevLsRec=NULL;
		BOOL bPrintTitle = FALSE;
		for(pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
		{
			if(module_start_i==-1||pLsRec->iModuleNo!=module_start_no)
			{	//�մ����һ��ģ�ͺ���,����һ����ģ�ͺ���
				if(module_start_i!=-1)
					i+=2;	//�����ĺ���ģ�ͱ�����ǰһģ�͸���һ��
				module_start_i=i;
				module_start_no=pLsRec->iModuleNo;
				bPrintTitle = TRUE;
			}
			if(bPrintTitle)
			{
				bPrintTitle = FALSE;	//��ֻ֤��ӡһ�α���
				// ��һ����������
				sprintf(cell_start,"C%d",i);
				sprintf(cell_end,"M%d",i+1);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Firmware Statistic Table"));
#else 
				excel_range.SetValue(COleVariant("���̼�ͳ�Ʊ�"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetBold(covTrue);
				font.SetSize(COleVariant(14.0));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				pRange->Release();
				
				// �ڶ����ӱ�����
				//��������
				sprintf(cell_start,"A%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Project Name��"));
#else 
				excel_range.SetValue(COleVariant("�������ƣ�"));
#endif
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//����
				sprintf(cell_start,"N%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Module��"));
#else 
				excel_range.SetValue(COleVariant("���ߣ�"));
#endif
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//��дģ�ͺ�������
				sprintf(cell_start,"O%d",i+1);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
				excel_range.SetValue(COleVariant(pLsRec->sModule));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.ReleaseDispatch();
				//�������ӱ�����
				sprintf(cell_start,"A%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Tower Model��"));
#else 
				excel_range.SetValue(COleVariant("��    �ͣ�"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				// ��д��������
				sprintf(cell_start,"B%d",i+2);
				pRange = excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(Ta.m_sTaType));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"N%d",i+2);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Basic Number��"));
#else 
				excel_range.SetValue(COleVariant("������"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				font.AttachDispatch(excel_range.GetFont(),FALSE);
				font.SetSize(COleVariant(10.5));
				font.ReleaseDispatch();
				excel_range.ReleaseDispatch();
				//��д����
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
				sprintf(ss,"%s",t.Format("%Y�� %m�� %d��"));
#endif
				excel_range.SetValue(COleVariant(ss));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����������������У���Ҫ��ʽ���
				//���̼�����
				sprintf(cell_start,"A%d",i+3);
				sprintf(cell_end,"A%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Name"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//������
				sprintf(cell_start,"B%d",i+3);
				sprintf(cell_end,"B%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Grade Specification"));
#else 
				excel_range.SetValue(COleVariant("������"));
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
				excel_range.SetValue(COleVariant("ͨ�����"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//��Ʊ���
				sprintf(cell_start,"D%d",i+3);
				sprintf(cell_end,"G%d",i+4);
				pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("         Design      "));
#else 
				excel_range.SetValue(COleVariant("��                 ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//��ı���
				sprintf(cell_start,"H%d",i+3);
				sprintf(cell_end,"I%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Wastage��x%��"));
#else 
				excel_range.SetValue(COleVariant("��    �ģ�x%��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�������
				sprintf(cell_start,"J%d",i+3);
				sprintf(cell_end,"J%d",i+3);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Others"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//��Ȧ���
				sprintf(cell_start,"J%d",i+4);
				sprintf(cell_end,"J%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Washer"));
#else 
				excel_range.SetValue(COleVariant("��Ȧ"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//��д���������ı���
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
				//��Ƽ���ı���
				sprintf(cell_start,"L%d",i+3);
				sprintf(cell_end,"O%d",i+4);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)0));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Design  And   Wastage"));
#else 
				excel_range.SetValue(COleVariant("��   ��   ��   ��   ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����������������У���Ҫ��ʽ���
				//����
				sprintf(cell_start,"D%d",i+5);
				sprintf(cell_end,"E%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"F%d",i+5);
				sprintf(cell_end,"G%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"H%d",i+5);
				sprintf(cell_end,"I%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount  "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"J%d",i+5);
				sprintf(cell_end,"K%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"L%d",i+5);
				sprintf(cell_end,"M%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Amount  "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"N%d",i+5);
				sprintf(cell_end,"O%d",i+5);
				pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant(" Weight  "));
#else 
				excel_range.SetValue(COleVariant("��      ��"));
#endif
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����������������У���Ҫ��ʽ���
				//����
				sprintf(cell_start,"D%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"E%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"F%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"G%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"H%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"I%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"J%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"K%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"L%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"M%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				sprintf(cell_start,"N%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
				excel_range.SetValue(COleVariant("����"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				//�ܼ�
				sprintf(cell_start,"O%d",i+6);
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
				excel_range.SetValue(COleVariant("Total"));
#else 
				excel_range.SetValue(COleVariant("�ܼ�"));
#endif
				excel_range.SetColumnWidth(COleVariant(7.0));
				excel_range.SetHorizontalAlignment(COleVariant((short)3));
				excel_range.SetVerticalAlignment(COleVariant((short)2));
				excel_range.DetachDispatch();
				pRange->Release();
				i+=7;
				//ͳ�Ƶ�ǰģ�ͺ����ڸ��ֽ��̼��������������ʾλ��
				mat_tbl.ls_recordset.push_stack();
				nCommonLs=nTwoCapLs=nCommonNail=nBurglarLs=nBurglarNail=nPad=0;
				CLsRecord *pTmPrevLsRec=NULL;//������ʱ��¼��һ������
				int iTmBoltNum=0;//������ʱ��¼��ǰ��������˨������
				for(CLsRecord *pTmLsRec=mat_tbl.ls_recordset.GetFirst();pTmLsRec;pTmLsRec=mat_tbl.ls_recordset.GetNext())
				{
					if(pTmLsRec->iModuleNo!=pLsRec->iModuleNo)
						continue;	//�ǵ�ǰģ�ͺ���
					if(pTmLsRec->typels0_pad1==1)	//��Ƭ
						nPad++;
					else	//��˨
					{
						if(pTmLsRec->GetTwoCapLsCount()>0)	//˫ñ��˨
							nTwoCapLs++;
						if(pTmLsRec->GetCommonNailCount()>0)
							nCommonNail++;		//��ͨ�Ŷ�
						if(pTmLsRec->GetBurglarproofLsCount()>0)
						{
							nBurglarLs++;		//��ͨ������˨
							if(pTmPrevLsRec!=NULL&&pTmLsRec->d!=pTmPrevLsRec->d)
							{	//ÿһ��ֱ������˨ͳ�ƽ���������˨ͳ�Ƽ�һ,Ԥ����ĸС����
								boltDArr[iBoltDNum]=iTmBoltNum;
								nBurglarLs++;
								iBoltDNum++;
								iTmBoltNum=0;
							}
							iTmBoltNum++;
							pTmPrevLsRec=pTmLsRec;
						}
						if(pTmLsRec->GetBurglarproofNailCount()>0)
							nBurglarNail++;		//�����Ŷ�
						if(pTmLsRec->GetCommonLsCount()>0)
							nCommonLs++;		//��ͨ��˨
					}
				}
				mat_tbl.ls_recordset.pop_stack();
				iCommonLs=iCommonLsStart=i;
				iTwoCapLs=iTwoCapLsStart=i+nCommonLs;
				if(nCommonLs>0)	//����С����
				{
					iTwoCapLs++;
					iTwoCapLsStart++;
				}
				iCommonNail=iCommonNailStart=iTwoCapLs+nTwoCapLs;
				if(nTwoCapLs>0)	//����С����
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
					//Ϊ���һ��������˨����ĸ����С����
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
						continue;	//��Ƭ����Ϊ�㲻��ҪС����
					if(j==iCommonLsStart+nCommonLs&&nCommonLs>0)
					{	//��˨С����
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
					{	//˫ñ��˨С����
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
					{	//��ͨ�Ŷ�С����
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
					{	//������˨С����
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
						//������ĸС����
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
							excel_range.SetValue(COleVariant("�� ĸ С ��"));
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
					{	//�����Ŷ�С����
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
					{	//��ȦС����
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
						//����ܼ�����
						sprintf(formula,"=D%d*O%d",j,i-5);
						sprintf(sum_cell,"E%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.DetachDispatch();
						pRange->Release();
						//��Ƶ�������
						sprintf(sum_cell,"F%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//����ܼ�����
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
						//��ĵ�������
						sprintf(formula,"=D%d*K%d",j,iWear);
						sprintf(sum_cell,"H%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//����ܼ�����
						sprintf(formula,"=E%d*K%d",j,iWear);
						sprintf(sum_cell,"I%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//��ĵ�������
						sprintf(formula,"=F%d*K%d",j,iWear);
						sprintf(sum_cell,"J%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//����ܼ�����
						sprintf(formula,"=G%d*K%d",j,iWear);
						sprintf(sum_cell,"K%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//��Ƽ���ĵ�������
						sprintf(formula,"=D%d+H%d",j,j);
						sprintf(sum_cell,"L%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//��Ƽ�����ܼ�����
						sprintf(formula,"=E%d+I%d",j,j);
						sprintf(sum_cell,"M%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0"));
						excel_range.DetachDispatch();
						pRange->Release();
						//��Ƽ���ĵ�������
						sprintf(formula,"=F%d+J%d",j,j);
						sprintf(sum_cell,"N%d",j);
						pRange = excel_sheet.GetRange(COleVariant(sum_cell));
						excel_range.AttachDispatch(pRange,FALSE);
						excel_range.SetFormula(COleVariant(formula));
						excel_range.SetNumberFormat(COleVariant("0.00"));
						excel_range.DetachDispatch();
						pRange->Release();
						//��Ƽ�����ܼ�����
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
				//�����߿���ʾ��Χ
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
				//��ͨ��˨��
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
					excel_range.SetValue(COleVariant("��ͨ��˨"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//˫ñ��˨��
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
					excel_range.SetValue(COleVariant("˫ñ��˨"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//��ͨ�Ŷ���
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
					excel_range.SetValue(COleVariant("��ͨ�Ŷ�"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//������˨��
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
					excel_range.SetValue(COleVariant("������˨"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//�����Ŷ���
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
					excel_range.SetValue(COleVariant("�����Ŷ�"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
				//��Ƭ��
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
					excel_range.SetValue(COleVariant("��Ƭ"));
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
					excel_range.SetValue(COleVariant("С��"));
#endif
					excel_range.SetHorizontalAlignment(COleVariant((short)3));
					excel_range.SetVerticalAlignment(COleVariant((short)2));
					excel_range.DetachDispatch();
					pRange->Release();
				}
			}

			if(pLsRec->typels0_pad1==1)
			{		//��Ƭ
				sprintf(cell_start,"B%d",iPad);	//��Ȧ���
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				char guige[20];
				sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
				excel_range.SetValue(COleVariant(guige));
				excel_range.DetachDispatch();
				pRange->Release();
				cell_start[0]='D';	//��������
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(pLsRec->GetPartNum()));
				excel_range.DetachDispatch();
				pRange->Release();
				cell_start[0]='F';	//��������
				pRange=excel_sheet.GetRange(COleVariant(cell_start));
				excel_range.AttachDispatch(pRange,FALSE);
				excel_range.SetValue(COleVariant(pLsRec->sum_weight));
				excel_range.SetNumberFormat(COleVariant("0.00"));
				excel_range.DetachDispatch();
				pRange->Release();
				iPad++;
			}
			else 	//��˨
			{
				if(pLsRec->GetTwoCapLsCount()>0)
				{	//ֻҪ����˫ñ�͹�Ϊ˫ñ�������Ƿ�����������
					sprintf(cell_start,"B%d",iTwoCapLs);	//��˨������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//�������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//��������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetTwoCapLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//��������
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
				{	//��ͨ�Ŷ�
					sprintf(cell_start,"B%d",iCommonNail);	//��˨������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//��������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetCommonNailCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//��������
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
				{	//��ͨ������˨
					if(pPrevLsRec!=NULL&&pLsRec->d!=pPrevLsRec->d&&!bIsFirst)
						iBurglarLs++;
					bIsFirst=false;
					sprintf(cell_start,"B%d",iBurglarLs);	//��˨������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//�������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//��������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//��������
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
				{		//�����Ŷ�
					sprintf(cell_start,"B%d",iBurglarNail);	//��˨������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//�������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//��������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetBurglarproofNailCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//��������
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
				{		//��ͨ��˨
					sprintf(cell_start,"B%d",iCommonLs);	//��˨������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char guige[20];
					sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
					excel_range.SetValue(COleVariant(guige));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='C';	//�������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					char ss[8];
					sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
					if(pLsRec->typels0_pad1==0)
						excel_range.SetValue(COleVariant(ss));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='D';	//��������
					pRange=excel_sheet.GetRange(COleVariant(cell_start));
					excel_range.AttachDispatch(pRange,FALSE);
					excel_range.SetValue(COleVariant(pLsRec->GetCommonLsCount()));
					excel_range.DetachDispatch();
					pRange->Release();
					cell_start[0]='F';	//��������
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
	excel_sheet.SetName("���˱�");
#endif
	//��ӱ�ͷ
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","Product Material Table");
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","�������ϱ�");
#endif
	
	//ͳ������
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
	//�ڶ��б���
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
	map.SetValueAt(index,0,COleVariant("����"));
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	map.SetValueAt(index,3,COleVariant("���񵥺�"));
	CExcelOper::MergeColRange(excel_sheet,"D2","E2");
	map.SetValueAt(index,6,COleVariant("����"));
	map.SetValueAt(index,8,COleVariant("����"));
	map.SetValueAt(index,9,COleVariant((long)Ta.m_nTaNum));
	map.SetValueAt(index,10,COleVariant("�û�"));
#endif
	index++;
	// �����б�����
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
	map.SetValueAt(index,0,COleVariant("���"));
	map.SetValueAt(index,1,COleVariant("���"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("������"));
	map.SetValueAt(index,4,COleVariant("�ܼ���"));
	map.SetValueAt(index,5,COleVariant("��������"));
	map.SetValueAt(index,6,COleVariant("����"));
	map.SetValueAt(index,7,COleVariant("����"));
	map.SetValueAt(index,8,COleVariant("����"));
	map.SetValueAt(index,9,COleVariant("���"));
	map.SetValueAt(index,10,COleVariant("�������"));
	map.SetValueAt(index,11,COleVariant("��ע"));
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
		map.SetValueAt(index,0,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//���
		sprintf(ss,"%s%C",(char*)pRec->sSpec,pRec->cMaterial);		//���
		map.SetValueAt(index,1,COleVariant(ss));
		sprintf(ss,"%.2f",pRec->length);					//����
		map.SetValueAt(index,2,COleVariant(ss));
		map.SetValueAt(index,3,COleVariant((long)pRec->GetPartNum()));	//������
		sprintf(ss,"=D%d*J2",iRow);	
		map.SetValueAt(index,4,COleVariant(ss));			//�ܼ���
		//��˨�ַ���
		int str_len=pRec->GetLsStr(NULL);
		char *ls_str=new char[str_len+1];
		pRec->GetLsStr(ls_str);
		map.SetValueAt(index,5,COleVariant(ls_str));
		delete []ls_str;
		map.SetValueAt(index,6,COleVariant(pRec->nMSumLs));	//����
		//��ע
		char notes[200];
		pRec->GetProcessNotes(notes);
		sprintf(notes,"%s %s",notes,pRec->sNotes);
		map.SetValueAt(index,11,COleVariant(notes));
		iRow++;
		index++;
	}
	char cell_end[15]="";
	//�����Զ�����
	sprintf(cell_end,"F%d",iRow-1);
	CExcelOper::SetRangeWrapText(excel_sheet,"F1",cell_end,COleVariant((short)TRUE));
	//
	sprintf(cell_end,"L%d",iRow-1);
	//������ֱ���뷽ʽ ����
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)-4108));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
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
	excel_sheet.SetName("�麸����ϸ��");
#endif
	mat_tbl.SortLs();
	// ��һ����������
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","Group Weld Part Table");
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","L1","�麸����ϸ��");
#endif
	//ͳ������
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
	// �ڶ����ӱ�����
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("����"));
#endif
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("��������"));
#endif
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));	// ��д������������
	CExcelOper::MergeColRange(excel_sheet,"E2","H2");
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,8,COleVariant("Module"));
	map.SetValueAt(index,9,COleVariant("Basic Number"));
#else 
	map.SetValueAt(index,8,COleVariant("����"));
	map.SetValueAt(index,9,COleVariant("����"));
#endif
	map.SetValueAt(index,10,COleVariant((short)1));	// ��д��������
	index++;
	//����������������У���Ҫ��ʽ���
	//�����п�
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"B3","C3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"D3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"E3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,4.5,"F3",NULL,TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"G3","H3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"I3","K3",TRUE);
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"L3",NULL,TRUE);
	// ����
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
	map.SetValueAt(index,0,COleVariant("�麸���"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("���"));
	map.SetValueAt(index,4,COleVariant("�ϳ�"));
	map.SetValueAt(index,5,COleVariant("����"));
	map.SetValueAt(index,6,COleVariant("����"));
	map.SetValueAt(index,7,COleVariant("����"));
	CExcelOper::MergeColRange(excel_sheet,"I3","K3");
	map.SetValueAt(index,8,COleVariant("��ע"));		
	map.SetValueAt(index,11,COleVariant("�麸����"));	
#endif
	index++;
	//
	int i=4;
	char cell_start[16]="",cell_end[16]="",sText[100]="";
	for(pPartGroupRec=mat_tbl.partgroupset.GetFirst();pPartGroupRec;pPartGroupRec=mat_tbl.partgroupset.GetNext(),index++)
	{
		//�麸�����
		sprintf(cell_start,"A%d",i);
		sprintf(cell_end,"A%d",i+pPartGroupRec->GetPartRefArrPtr()->GetNodeNum());
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,0,COleVariant(pPartGroupRec->sPartNo));
		//���麸���ڲ�һ������ͬһ�ֲ��ʣ�����麸���б����麸��Ų���ʾ�����ַ� wjh-2013-5-20
		//PART *pPart=mat_tbl.GetPartFromPartNo(pPartGroupRec->sPartNo);
		//map.SetValueAt(index,0,COleVariant(pPart->GetPartNo(g_sysPara.m_nMatCharPosType)));
		//���������
		sprintf(cell_end,"B%d",i);
		sprintf(cell_end,"B%d",i+pPartGroupRec->GetPartRefArrPtr()->GetNodeNum()-1);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)2));
		//�麸����
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
			map.SetValueAt(index,1,COleVariant(pPart->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	// ����
			QuerySteelMatMark(pPart->cMaterial,sText);
			map.SetValueAt(index,2,COleVariant(sText));				// ����
			map.SetValueAt(index,3,COleVariant(pPart->sSpec));		// ��X��
			sprintf(sText,"%.1f",pPart->length);
			map.SetValueAt(index,4,COleVariant(sText));				// �ϳ�
			map.SetValueAt(index,5,COleVariant((long)pPartRef->num));// ����
			sprintf(sText,"%.1f",pPart->fPieceMinWeight);
			map.SetValueAt(index,6,COleVariant(sText));				// ����
			sprintf(sText,"%.1f",pPart->fPieceMinWeight*pPartRef->num);	// ����
			sum_weight+=pPart->fPieceMinWeight*pPartRef->num;		//ͳ������
			map.SetValueAt(index,7,COleVariant(sText));
			map.SetValueAt(index,8,COleVariant(pPart->sNotes));		// ��ע
			sprintf(cell_start,"I%d",i-1);
			sprintf(cell_end,"K%d",i-1);
			CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		}
		//�ܼ�
		sprintf(cell_start,"B%d",i);
		sprintf(cell_end,"D%d",i);
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,1,COleVariant("Total"));
#else 
		map.SetValueAt(index,1,COleVariant("�ܼ�"));
#endif
		//������
		sprintf(cell_start,"E%d",i);
		sprintf(cell_end,"K%d",i);
		sprintf(sText,"%.1f",sum_weight);
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
		map.SetValueAt(index,4,COleVariant(sText));
		i++;
	}
	//���ñ������
	sprintf(cell_end,"L%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//���������麸����ϸ��
void CLDSDoc::AddGuangZhouZengLiPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("�麸����ϸ��");
#endif
	//ͳ������
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
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("�����"));
	map.SetValueAt(index,2,COleVariant("���"));
	map.SetValueAt(index,3,COleVariant("�������"));
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
			map.SetValueAt(index,0,COleVariant(pPartGroupRec->iSeg.ToString()));	//�κ�
			map.SetValueAt(index,1,COleVariant(pPartGroupRec->sPartNo));//pPart->GetPartNo(g_sysPara.m_nMatCharPosType)));	//�����
			map.SetValueAt(index,2,COleVariant(pPartRef->sPartNo));			//���
			map.SetValueAt(index,3,COleVariant((long)pPartRef->num));		//��������
		}
	}
	char cell_end[15]="";
	//���������
	sprintf(cell_end,"C%d",i-1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"D%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();
}

//Ϋ�������麸��ϸ�� wht 10-09-30
void CLDSDoc::AddWeiFangChangAnPartGroupSumMatTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("�麸����ϸ��");
#endif

	int index=0;
	CVariant2dArray map(mat_tbl.partgroupset.GetNodeNum()+1,3);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Main No."));
	map.SetValueAt(index,2,COleVariant("Piece Part No."));
#else 
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("�����"));
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
		map.SetValueAt(index,0,COleVariant(pPartGroupRec->iSeg.ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pPartGroupRec->sPartNo));	//����
		map.SetValueAt(index,2,COleVariant(sPartNoArr));				//�����
		i++;
	}
	char cell_end[15]="";
	//���ñ������
	sprintf(cell_end,"C%d",i-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();	
}

void CLDSDoc::AddTubeSpecialProcossSumTbl(CTowerType &mat_tbl,LPDISPATCH pWorkSheet,int nTableType/*=0*/)
{	//�ֹ����⹤�ջ��ܱ�
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
		excel_sheet.SetName("ƽ�����ֹܻ��ܱ�");
	else if(nTableType==1)
		excel_sheet.SetName("Բ�����ֹܻ��ܱ�");
	else if(nTableType==2)
		excel_sheet.SetName("һ�ֲۻ��ܱ�");
	else if(nTableType==3)
		excel_sheet.SetName("ʮ�ֲۻ��ܱ�");
	else if(nTableType==4)
		excel_sheet.SetName("U�Ͳۻ��ܱ�");
	else if(nTableType==5)
		excel_sheet.SetName("T�Ͳۻ��ܱ�");
	else if(nTableType==6)
		excel_sheet.SetName("���˿��ۻ��ܱ�");
#endif
	//else if(nTableType==7)
	//	excel_sheet.SetName("���˿��ۻ��ܱ�---��U��");
	mat_tbl.SortLs();
	// ��һ����������
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
		excel_range.SetValue(COleVariant("ƽ�����ֹܻ��ܱ�"));
	else if(nTableType==1)
		excel_range.SetValue(COleVariant("Բ�����ֹܻ��ܱ�"));
	else if(nTableType==2)
		excel_range.SetValue(COleVariant("һ�ֲۻ��ܱ�"));
	else if(nTableType==3)
		excel_range.SetValue(COleVariant("ʮ�ֲۻ��ܱ�"));
	else if(nTableType==4)
		excel_range.SetValue(COleVariant("U�Ͳۻ��ܱ�"));
	else if(nTableType==5)
		excel_range.SetValue(COleVariant("T�Ͳۻ��ܱ�"));
	else if(nTableType==6)
		excel_range.SetValue(COleVariant("���˿��ۻ��ܱ�"));
#endif
	//else if(nTableType==7)
	//	excel_range.SetValue(COleVariant("���˿��ۻ��ܱ�"));
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
	
	// �ڶ����ӱ�����
	pRange=excel_sheet.GetRange(COleVariant("A2"));
	excel_range.AttachDispatch(pRange);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tower Model"));
#else 
	excel_range.SetValue(COleVariant("����"));
#endif
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
	// ��д��������
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
	excel_range.SetValue(COleVariant("��������"));
#endif
	excel_range.DetachDispatch();
	pRange->Release();
	// ��д������������
	strcpy(cell_start,"E2");
	strcpy(cell_end,"F2");
	pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue(COleVariant(Ta.m_sPrjName));
	excel_range.DetachDispatch();
	pRange->Release();
	// ����
	pRange=excel_sheet.GetRange(COleVariant("G2"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Module"));
#else 
	excel_range.SetValue(COleVariant("����"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ����ֵ
	pRange=excel_sheet.GetRange(COleVariant("H2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ����
	pRange=excel_sheet.GetRange(COleVariant("I2"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Basic Number"));
#else 
	excel_range.SetValue(COleVariant("����"));
#endif
	excel_range.SetColumnWidth(COleVariant(5.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ��д��������
	pRange=excel_sheet.GetRange(COleVariant("J2"));
	excel_range.AttachDispatch(pRange,FALSE);
	excel_range.SetValue(COleVariant((short)1));
	excel_range.SetColumnWidth(COleVariant(7.5));
	excel_range.DetachDispatch();
	pRange->Release();
	//����������������У���Ҫ��ʽ���
	// �ֹܱ��
	pRange=excel_sheet.GetRange(COleVariant("A3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Tube Number"));
#else 
	excel_range.SetValue(COleVariant("�ֹܱ��"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ����
	pRange=excel_sheet.GetRange(COleVariant("B3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Material Texture"));
#else 
	excel_range.SetValue(COleVariant("����"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ��X��
	pRange=excel_sheet.GetRange(COleVariant("C3"));
	excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	excel_range.SetValue(COleVariant("Specification"));
#else 
	excel_range.SetValue(COleVariant("���"));
#endif
	excel_range.SetColumnWidth(COleVariant(8.0));
	excel_range.DetachDispatch();
	pRange->Release();
	// ������ʽ
	pRange=excel_sheet.GetRange(COleVariant("D3"));
	excel_range.AttachDispatch(pRange,FALSE);
	if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Intersect Type"));
	else 
		excel_range.SetValue(COleVariant("Insert Plane Slot-Type"));
#else 
		excel_range.SetValue(COleVariant("�����ʽ"));
	else 
		excel_range.SetValue(COleVariant("������ʽ"));
#endif
	excel_range.SetColumnWidth(COleVariant(10.0));
	excel_range.DetachDispatch();
	pRange->Release();
	if(nTableType==0)
	{	//ƽ�����
		strcpy(cell_start,"E3");
		strcpy(cell_end,"F3");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Angle A��"));
#else 
		excel_range.SetValue(COleVariant("�Ƕ�A��"));
#endif
		excel_range.SetColumnWidth(COleVariant(5.0));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	else
	{
		// ���۳���
		pRange=excel_sheet.GetRange(COleVariant("E3"));
		excel_range.AttachDispatch(pRange,FALSE);
		if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Main Tube Specification"));
		else
			excel_range.SetValue(COleVariant("Insert Plate Slot-Length"));
#else 
			excel_range.SetValue(COleVariant("���ܹ��"));
		else
			excel_range.SetValue(COleVariant("���۳���"));
#endif
		excel_range.SetColumnWidth(COleVariant(10.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ���ۿ��A
		pRange=excel_sheet.GetRange(COleVariant("F3"));
		excel_range.AttachDispatch(pRange,FALSE);
		if(nTableType==0||nTableType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Angle A��"));
		else
			excel_range.SetValue(COleVariant("Insert Plate Slot-Width A"));
#else 
			excel_range.SetValue(COleVariant("�Ƕ�A��"));
		else
			excel_range.SetValue(COleVariant("���ۿ��A"));
#endif
		excel_range.SetColumnWidth(COleVariant(10.0));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	if(nTableType==2||nTableType==3)
	{
		// ������
		pRange=excel_sheet.GetRange(COleVariant("G3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
		excel_range.SetValue(COleVariant("������"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ����
		pRange=excel_sheet.GetRange(COleVariant("H3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Totality"));
#else 
		excel_range.SetValue(COleVariant("����"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ��ע
		strcpy(cell_start,"I3");
		strcpy(cell_end,"J3");
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Notes"));
#else 
		excel_range.SetValue(COleVariant("��ע"));
#endif
		excel_range.SetColumnWidth(COleVariant(8.0));
		pRange->Release();
	}
	else
	{
		// ���ۿ��B
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
			excel_range.SetValue(COleVariant("����Բ��R"));
		else if(nTableType==6)
			excel_range.SetValue(COleVariant("�ۿ�B(Բ��R)"));
		else if(nTableType==0||nTableType==1)
			excel_range.SetValue(COleVariant("֧�ܳ���L"));
		else 
			excel_range.SetValue(COleVariant("���ۿ�B"));
#endif
		excel_range.SetColumnWidth(COleVariant(12.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ������
		pRange=excel_sheet.GetRange(COleVariant("H3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Single Basic Number"));
#else 
		excel_range.SetValue(COleVariant("������"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ����
		pRange=excel_sheet.GetRange(COleVariant("I3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Totality"));
#else 
		excel_range.SetValue(COleVariant("����"));
#endif
		excel_range.SetColumnWidth(COleVariant(6.0));
		excel_range.DetachDispatch();
		pRange->Release();
		// ��ע
		pRange=excel_sheet.GetRange(COleVariant("J3"));
		excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
		excel_range.SetValue(COleVariant("Notes"));
#else 
		excel_range.SetValue(COleVariant("��ע"));
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
		{	//ƽ�����ֹܻ��ܱ�
			if(pRec->startProcess.type==PROCESSTYPE_PLANECUT&&pRec->endProcess.type==PROCESSTYPE_PLANECUT)
			{	//�ֹ����˾���������
				
			}
			else if(pRec->startProcess.type==PROCESSTYPE_PLANECUT)//�ֹ�һ�˸������
				pStartTubeProcess=&pRec->startProcess;
			else if(pRec->endProcess.type==PROCESSTYPE_PLANECUT)//�ֹ�һ�˸������
				pStartTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL)
				continue;
			//�ֹܱ��
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//����
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//�����ʽ
			cell[0]='D';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Plane Intersect"));
#else 
			excel_range.SetValue(COleVariant("ƽ�����"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//�Ƕ�A��
			sprintf(cell_start,"E%d",i);
			sprintf(cell_end,"F%d",i);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)1));
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(5.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//֧�ܳ���L
			cell[0]='G';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%.f",pStartTubeProcess->L);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//������
			cell[0]='H';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%d",pRec->GetPartNum());
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//����
			cell[0]='I';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"=H%d*J2",i);	//�ܼ���
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//��ע
			cell[0]='J';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("One End Interset Line"));
#else 
			excel_range.SetValue(COleVariant("һ�˸������"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		else if(nTableType==1)
		{	//Բ�������ֹܻ��ܱ�
			if(pRec->startProcess.type==PROCESSTYPE_TRANSECT&&pRec->endProcess.type==PROCESSTYPE_TRANSECT)
			{	//�ֹ����˾���������
				
			}
			else if(pRec->startProcess.type==PROCESSTYPE_TRANSECT)//�ֹ�һ�˸������
				pStartTubeProcess=&pRec->startProcess;
			else if(pRec->endProcess.type==PROCESSTYPE_TRANSECT)//�ֹ�һ�˸������
				pStartTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL)
				continue;
			//�ֹܱ��
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//����
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//֧�ܳ���L
			cell[0]='D';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("Column Intersect"));
#else 
			excel_range.SetValue(COleVariant("Բ�����"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���ܹ��
			cell[0]='E';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->guige));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//�Ƕ�A��
			cell[0]='F';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//֧�ܳ���L
			cell[0]='G';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%.f",pStartTubeProcess->L);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//������
			cell[0]='H';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"%d",pRec->GetPartNum());
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//����
			cell[0]='I';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			sprintf(sText,"=H%d*J2",i);	//�ܼ���
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(6.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//��ע
			cell[0]='J';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
			excel_range.SetValue(COleVariant("One End Interset Line"));
#else 
			excel_range.SetValue(COleVariant("һ�˸������"));
#endif
			excel_range.SetColumnWidth(COleVariant(16.0));
			excel_range.DetachDispatch();
			pRange->Release();
		}
		else if(nTableType==2||nTableType==3||nTableType==4||nTableType==5)
		{	//һ�˿��ۻ������˿���������ͬ
			if(pRec->startProcess.type<=2&&pRec->endProcess.type<=2)
				continue;	//�������˾��޿��ڲ۵ĸֹ�
			TUBE_PROCESS_TYPE curSlotType=PROCESSTYPE_NONE;	//��ǰҪͳ�ƵĿ��ڲ�����
			if(nTableType==2)		//һ�ֲ�
				curSlotType=PROCESSTYPE_CSLOT;
			else if(nTableType==3)	//ʮ�ֲ�
				curSlotType=PROCESSTYPE_XSLOT;
			else if(nTableType==4)	//U�Ͳ�
				curSlotType=PROCESSTYPE_USLOT;
			else if(nTableType==5)	//T�Ͳ�
				curSlotType=PROCESSTYPE_TSLOT;
			
			if((pRec->startProcess.type>2&&pRec->startProcess.type!=curSlotType)
				||(pRec->endProcess.type>2&&pRec->endProcess.type!=curSlotType))
				continue;	//��ͳ�ƿ��ڲ۵�ǰ���͵ĸֹ�	
			if(pRec->startProcess.type==curSlotType)
				pStartTubeProcess=&pRec->startProcess;
			if(pRec->endProcess.type==curSlotType)
				pEndTubeProcess=&pRec->endProcess;
			if(pStartTubeProcess==NULL&&pEndTubeProcess==NULL)
				continue;
			if(pStartTubeProcess==NULL&&pEndTubeProcess)
			{	//�������һ�˿��������ݴ���pStartTubeProcess��
				pStartTubeProcess=pEndTubeProcess;
				pEndTubeProcess=NULL;
			}
			//Ӧ���ж�ʼ�ն˿��ڲ������Ƿ�һ��
			BOOL bSameSlot=TRUE;
			if(pStartTubeProcess&&pEndTubeProcess)
			{
				if(pStartTubeProcess->A!=pEndTubeProcess->A
					||pStartTubeProcess->B!=pEndTubeProcess->B
					||pStartTubeProcess->L!=pEndTubeProcess->L
					||pStartTubeProcess->fNormOffset!=pEndTubeProcess->fNormOffset)
					bSameSlot=FALSE;	//���˿��ڲ����ݲ�һ��
			}
			//�ֹܱ��
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//����
			cell[0]='B';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			QuerySteelMatMark(pRec->cMaterial,sText);
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���
			cell[0]='C';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetColumnWidth(COleVariant(8.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//������ʽ
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
				excel_range.SetValue(COleVariant("һ���Ͳ�"));
			else if(nTableType==3)
				excel_range.SetValue(COleVariant("ʮ���Ͳ�"));
			else if(nTableType==4)
				excel_range.SetValue(COleVariant("U�Ͳ�"));
			else if(nTableType==5)
				excel_range.SetValue(COleVariant("T�Ͳ�"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���۳���L
			cell[0]='E';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->L));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���ۿ��A
			cell[0]='F';
			pRange=excel_sheet.GetRange(COleVariant(cell));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(pStartTubeProcess->A));
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			if(nTableType==4||nTableType==5)
			{
				//���ۿ��B
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
			{	//������
				cell[0]='G';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"%d",pRec->GetPartNum());
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				cell[0]='H';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"=G%d*J2",i);	//�ܼ���
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//��ע
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
					excel_range.SetValue(COleVariant("���˸��"));
				else
					excel_range.SetValue(COleVariant("һ�˸��"));
#endif
				excel_range.SetColumnWidth(COleVariant(8.0));
				excel_range.DetachDispatch();
				pRange->Release();
			}
			else
			{	//������
				cell[0]='H';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"%d",pRec->GetPartNum());
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//����
				cell[0]='I';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				sprintf(sText,"=H%d*J2",i);	//�ܼ���
				excel_range.SetValue(COleVariant(sText));
				excel_range.SetColumnWidth(COleVariant(6.0));
				excel_range.DetachDispatch();
				pRange->Release();
				//��ע
				cell[0]='J';
				pRange=excel_sheet.GetRange(COleVariant(cell));
				excel_range.AttachDispatch(pRange,FALSE);
				if(pStartTubeProcess&&pEndTubeProcess)
#ifdef AFX_TARG_ENU_ENGLISH
					excel_range.SetValue(COleVariant("Both End Cut Slot"));
				else
					excel_range.SetValue(COleVariant("One End Cut Slot"));
#else 
					excel_range.SetValue(COleVariant("���˸��"));
				else
					excel_range.SetValue(COleVariant("һ�˸��"));
#endif
				excel_range.SetColumnWidth(COleVariant(16.0));
				excel_range.DetachDispatch();
				pRange->Release();
			}
		}
		else if(nTableType==6||nTableType==7)
		{	//���˿��� �����Ƿ���U�Ͳۿ��Է�����
			if(pRec->startProcess.type<3||pRec->endProcess.type<3)
				continue;	//ֻͳ����Ҫ���˸�۵ĸֹ�
			if(pRec->startProcess.type==pRec->endProcess.type)
				continue;	//�˴���ͳ�����˿��ڲ�����һ���ĸְ�
			pStartTubeProcess=&pRec->startProcess;
			pEndTubeProcess=&pRec->endProcess;
			//�ֹܱ��
			sprintf(cell_start,"A%d",i);
			sprintf(cell_end,"A%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			excel_range.SetValue(COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//����
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
			//���
			sprintf(cell_start,"C%d",i);
			sprintf(cell_end,"C%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			excel_range.SetValue(COleVariant(pRec->sSpec));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//������ʽ
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
				excel_range.SetValue(COleVariant("һ�Ͳ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("ʮ�Ͳ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U�Ͳ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T�Ͳ�"));
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
				excel_range.SetValue(COleVariant("һ�Ͳ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("ʮ�Ͳ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("U�Ͳ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("T�Ͳ�"));
#endif
			excel_range.SetColumnWidth(COleVariant(10.0));
			excel_range.DetachDispatch();
			pRange->Release();
			//���۳���L
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
			//���ۿ��A
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
			//���ۿ��B
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
			//������
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
			//����
			sprintf(cell_start,"I%d",i);
			sprintf(cell_end,"I%d",i+1);
			pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.Merge(COleVariant((BYTE)0));
			sprintf(sText,"=H%d*J2",i);	//�ܼ���
			excel_range.SetValue(COleVariant(sText));
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
			pRange->Release();
			//������
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
				excel_range.SetValue(COleVariant("һ�˸�һ�ֲ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("һ�˸�ʮ�ֲ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("һ�˸�U�Ͳ�"));
			else if(pStartTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("һ�˸�T�Ͳ�"));
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
				excel_range.SetValue(COleVariant("һ�˸�һ�ֲ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_XSLOT)
				excel_range.SetValue(COleVariant("һ�˸�ʮ�ֲ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_USLOT)
				excel_range.SetValue(COleVariant("һ�˸�U�Ͳ�"));
			else if(pEndTubeProcess->type==PROCESSTYPE_TSLOT)
				excel_range.SetValue(COleVariant("һ�˸�T�Ͳ�"));
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
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart(0);
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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
	excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������

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
	//��д��һ�Ź������ݱ�
	LPDISPATCH pWorksheet;
	//nTableType �������:0.ƽ����� 1.Բ����� 2.һ�ֲ� 3.ʮ�ֲ� 4.U�Ͳ� 
	//					  5.T�Ͳ� 6.���˿���
	if(process_dlg.m_bPlaneCut)
	{	//ƽ�����
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,0);
		iSheet++;
	}
	if(process_dlg.m_bTransect)
	{	//Բ�������
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,1);
		iSheet++;
	}
	if(process_dlg.m_bSlotC)
	{	//һ�ֲۻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,2);
		iSheet++;
	}
	if(process_dlg.m_bSlotX)
	{	//ʮ�ֲۻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,3);
		iSheet++;
	}
	if(process_dlg.m_bSlotU)
	{	//U�Ͳۻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,4);
		iSheet++;
	}
	if(process_dlg.m_bSlotT)
	{	//T�Ͳۻ��ܱ�
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,5);
		iSheet++;
	}
	if(process_dlg.m_bTwoSlot)
	{	//�ֹ����˸��
		pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTubeSpecialProcossSumTbl(mat_tbl,pWorksheet,6);
		iSheet++;
	}
	pWorksheet->Release();
	EndWaitCursor();
}
//����ͳ����˨
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
	//����˨��¼����������
	ARRAY_LIST<CLsRecord> lsRecorArr;
	lsRecorArr.SetSize(0,mat_tbl.ls_recordset.GetNodeNum());
	for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext())
		lsRecorArr.append(*pLsRec);
	CQuickSort<CLsRecord>::QuickSort(lsRecorArr.m_pData,lsRecorArr.GetSize(),compare_func1);
	int i=0;
	for(CLsRecord *pLsRec=mat_tbl.ls_recordset.GetFirst();pLsRec;pLsRec=mat_tbl.ls_recordset.GetNext(),i++)
		*pLsRec=lsRecorArr[i];
	//������˨���ܱ��
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.SetName("��˨���ܱ�");
	CExcelOper::SetMainTitle(excel_sheet,"A1","I1","XXX��˾������˨���ܱ�",20.0,14.0);
	//��ӱ�����
	int index=0;
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,9);
	map.SetValueAt(index,0,COleVariant("����"));
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	map.SetValueAt(index,3,COleVariant("��������"));
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	CExcelOper::MergeColRange(excel_sheet,"E2","F2");
	map.SetValueAt(index,6,COleVariant("������˨����"));
	CExcelOper::MergeColRange(excel_sheet,"G2","I2");
	index++;
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("������"));
	map.SetValueAt(index,3,COleVariant("�޿۳�"));
	map.SetValueAt(index,4,COleVariant("��������"));
	map.SetValueAt(index,5,COleVariant("��������"));
	map.SetValueAt(index,6,COleVariant("˫ñ��˨"));
	map.SetValueAt(index,7,COleVariant("������˨"));
	map.SetValueAt(index,8,COleVariant("���ɵ�"));
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
		{	//��¼��ʼ��
			iStartSeg=iCell+1;
			iCurSeg=pRec->iSeg;
			map.SetValueAt(index,0,COleVariant(pRec->iSeg.ToString()));
		}
		if(pRec->iSeg.iSeg==iCurSeg.iSeg)
		{	//ͬ����
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
		{	//���
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
		//������
		if(pRec->typels0_pad1==0)
			map.SetValueAt(index,2,COleVariant(CXhChar50("%sM%dX%d",pRec->grade,pRec->d,pRec->L)));
		else
			map.SetValueAt(index,2,COleVariant(CXhChar50("-%dX%d",pRec->L,pRec->d)));
		//�޿۳�
		if(pRec->typels0_pad1==0)
			map.SetValueAt(index,3,COleVariant(CXhChar16("%d",pRec->no_thread_len)));
		//������
		map.SetValueAt(index,4,COleVariant(CXhChar16("%.1f",pRec->sum_weight)));
		//������
		map.SetValueAt(index,5,COleVariant(pRec->GetPartNum()));
		if(pRec->typels0_pad1==0)
		{	//������˨����
			map.SetValueAt(index,6,COleVariant(pRec->nTwoCap));		//˫ñ��
			map.SetValueAt(index,7,COleVariant(pRec->nBurglarproof));	//��������
			map.SetValueAt(index,8,COleVariant(pRec->nSpringPad));	//���ɵ�����s
		}
		//
		pPreLsFamily=pLsFamily;
	}
	//���ñ������
	sprintf(cell_end,"I%d",iCell+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
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
	excel_sheet.SetName("��˨���ܱ�");
#endif
	mat_tbl.SortLs();
	// ��һ����������
#ifdef AFX_TARG_ENU_ENGLISH
	CExcelOper::SetMainTitle(excel_sheet,"A1","H1","XXX Company Bolt Summary Table",20.0,14.0);
#else 
	CExcelOper::SetMainTitle(excel_sheet,"A1","H1","XXX��˾������˨���ܱ�",20.0,14.0);
#endif
	
	int index=0;
	CVariant2dArray map(mat_tbl.ls_recordset.GetNodeNum()+2,8);
	// �ڶ����ӱ�����
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Tower Model"));
#else 
	map.SetValueAt(index,0,COleVariant("����"));
#endif
	// ��д��������
	map.SetValueAt(index,1,COleVariant(Ta.m_sTaType));
	CExcelOper::MergeColRange(excel_sheet,"B2","C2");
	//��������
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,3,COleVariant("Project Name"));
#else 
	map.SetValueAt(index,3,COleVariant("��������"));
#endif
	// ��д������������
	map.SetValueAt(index,4,COleVariant(Ta.m_sPrjName));
	CExcelOper::MergeColRange(excel_sheet,"E2","F2");
	//
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,6,COleVariant("Special Bolt Amount"));
#else 
	map.SetValueAt(index,6,COleVariant("����������˨����"));
#endif
	CExcelOper::MergeColRange(excel_sheet,"G2","H2");
	index++;
	//
	//����������������У���Ҫ��ʽ���
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
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("������"));
	map.SetValueAt(index,2,COleVariant("�������"));
	map.SetValueAt(index,3,COleVariant("��������"));
	map.SetValueAt(index,4,COleVariant("��������"));
	map.SetValueAt(index,5,COleVariant("˫ñ��˨"));
	map.SetValueAt(index,6,COleVariant("������˨"));
	map.SetValueAt(index,7,COleVariant("���ɵ�"));
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
			//���öκ�
			map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
		}
		//��Ӷκ� pLsRec==NULLʱ������һ�ζκ�
		if((pLsRec&&pLsRec->iSeg.iSeg!=seg_start_no.iSeg)||pLsRec==NULL)
		{
			sprintf(cell_start,"A%d",seg_start_i);
			if (pLsRec!=NULL)
				sprintf(cell_end,"A%d",i);
			else//���һ��ʱiû�н���i++
				sprintf(cell_end,"A%d",i+1);
			//�ϲ���
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
			if(pLsRec)
			{	//��¼��һ�ο�ʼ�������κ�
				seg_start_i=i+1;
				seg_start_no=pLsRec->iSeg;
				//���öκ�
				map.SetValueAt(index,0,COleVariant(pLsRec->iSeg.ToString()));
			}
			else
			{	//��������һ�ζκ�ʱ����ѭ��
				bEnd=FALSE;
				break;
			}
		}
		//���
		char guige[20];
		if(pLsRec->typels0_pad1==0)
			sprintf(guige,"%sM%dX%d",pLsRec->grade,pLsRec->d,pLsRec->L);
		else
			sprintf(guige,"-%dX%d",pLsRec->L,pLsRec->d);
		map.SetValueAt(index,1,COleVariant(guige));
		//�������;
		char ss[8];
		sprintf(ss,"%d~%d",pLsRec->L0_downlim,pLsRec->L0_uplim);
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,2,COleVariant(ss));
		//��������
		map.SetValueAt(index,3,COleVariant(pLsRec->sum_weight));
		//������
		map.SetValueAt(index,4,COleVariant(pLsRec->GetPartNum()));
		//˫ñ��
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,5,COleVariant(pLsRec->nTwoCap));
		//��������
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,6,COleVariant(pLsRec->nBurglarproof));
		//���ɵ�����
		if(pLsRec->typels0_pad1==0)
			map.SetValueAt(index,7,COleVariant(pLsRec->nSpringPad));
	}
	//�������ָ�ʽ
	sprintf(cell_end,"D%d",i+1);
	CExcelOper::SetRangeNumberFormat(excel_sheet,"D4",cell_end,COleVariant("0.0"));
	//���ñ������
	sprintf(cell_end,"H%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}

void CLDSDoc::OnExportExcelFile2() 
{
	CInputAnStringValDlg dlg;
	dlg.m_sItemTitle="�κŷ�Χ��";
	dlg.m_sTip="*��ʾ������ж�";
	dlg.m_sItemValue="*";
	if(dlg.DoModal()!=IDOK)
		return;
	CTowerType mat_tbl;
	int nSheet=2;
	BeginWaitCursor();
	//���в���ͳ��
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
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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

	excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������
	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	//�Ǹ������ְ���
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
		//��д��һ�Ź������ݱ�
		LPDISPATCH pWorksheet;
		pWorksheet = excel_sheets.GetItem(COleVariant((short) (iWorkBook+1)));
		//
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName(CXhChar50("Workpiece Data-%d",iWorkBook+1));
#else 
		excel_sheet.SetName(CXhChar50("��������-%d",iWorkBook+1));
#endif
		//
		int index=0,nRow=0;
		if(iWorkBook==0)
			nRow = nAngleNum;
		else 
			nRow = nPlate;
		CVariant2dArray map(nRow+1,30);
		map.SetValueAt(index,0,COleVariant("xh"));		//���
		map.SetValueAt(index,1,COleVariant("bjbh"));	//�������
		map.SetValueAt(index,2,COleVariant("sjgg"));	//��ƹ��
		map.SetValueAt(index,3,COleVariant("cz"));		//����
		map.SetValueAt(index,4,COleVariant("fycc"));	//�����ߴ�
		map.SetValueAt(index,5,COleVariant("dgzl"));	//��������
		map.SetValueAt(index,6,COleVariant("djsl"));	//��������
		map.SetValueAt(index,7,COleVariant("djzl"));	//��������
		map.SetValueAt(index,8,COleVariant("bjth"));	//����ͼ��
		map.SetValueAt(index,9,COleVariant("jgsl"));	//�ӹ�����
		map.SetValueAt(index,10,COleVariant("jgzl"));	//�ӹ�����
		map.SetValueAt(index,11,COleVariant("T-xl"));	//����
		map.SetValueAt(index,12,COleVariant("T-zk"));	//�ƿ�
		map.SetValueAt(index,13,COleVariant("T-qj"));	//�н�
		map.SetValueAt(index,14,COleVariant("T-wq"));	//����
		map.SetValueAt(index,15,COleVariant("T-zh"));	//���
		map.SetValueAt(index,16,COleVariant("T-hanj")); //����
		map.SetValueAt(index,17,COleVariant("T-pj"));	//�ٽ�
		map.SetValueAt(index,18,COleVariant("T-cb"));	//����
		map.SetValueAt(index,19,COleVariant("T-kj"));	//����
		map.SetValueAt(index,20,COleVariant("T-hej"));	//�Ͻ�
		map.SetValueAt(index,21,COleVariant("T-yb"));	//ѹ��
		map.SetValueAt(index,22,COleVariant("T-dk"));	//���
		map.SetValueAt(index,23,COleVariant("T-qz"));	//��֫
		map.SetValueAt(index,24,COleVariant("T-mm"));	//����
		map.SetValueAt(index,25,COleVariant("T-dx"));	//��п
		map.SetValueAt(index,26,COleVariant("Bz"));		//��ע
		map.SetValueAt(index,27,COleVariant("C"));		//��ע
		map.SetValueAt(index,28,COleVariant("K"));		//��ע
		map.SetValueAt(index,29,COleVariant("Gy"));		//����
		index++;
		for(PART *pRec=mat_tbl.part_recordset.GetFirst();pRec;pRec=mat_tbl.part_recordset.GetNext())
		{
			if(iWorkBook==0 && (pRec->part_type!=1&&pRec->part_type!=4))
				continue;	//�Ǹ֡��׹� wht 18-11-07
			else if(iWorkBook==1 && pRec->part_type!=3)
				continue;
			i++;
			map.SetValueAt(index,0,COleVariant((long)i));	//���
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
				map.SetValueAt(index,2,COleVariant(spec));	//��ƹ��
			}
			else if(pRec->part_type==1)
			{	//��������ϵͳҪ��Ǹֹ�����Ϊ��L63X5����ʽ����������˿���仯 wjh-2015.4.1
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
			{	//�׹�
				CXhChar50 sSpec("��%.f",pRec->wide);
				map.SetValueAt(index,2,COleVariant(sSpec));
			}
			QuerySteelMatMark(pRec->cMaterial,mark);
			map.SetValueAt(index,3,COleVariant(mark));	//����
			if(iWorkBook==0)	// �Ǹ�
				map.SetValueAt(index,4,COleVariant(ftol(pRec->length)));
			else				//�����ߴ�
#ifdef AFX_TARG_ENU_ENGLISH
				map.SetValueAt(index,4,COleVariant("Sample"));	
#else 
				map.SetValueAt(index,4,COleVariant("����"));	
#endif
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				map.SetValueAt(index,5,COleVariant(ftol(10*pRec->fPieceMinWeight)/10.0));	//��������
			else
				map.SetValueAt(index,5,COleVariant(ftol(10*pRec->fPieceNetWeight)/10.0));	//��������
			map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//��������
			double djzl=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				djzl=pRec->fPieceMinWeight*pRec->GetPartNum();
			else
				djzl=pRec->fPieceNetWeight*pRec->GetPartNum();
			map.SetValueAt(index,7,COleVariant(ftol(10*djzl)/10.0));	//��������
			map.SetValueAt(index,8,COleVariant(""));	//����ͼ��
			map.SetValueAt(index,9,COleVariant((long)pRec->GetPartNum()));	//�ӹ�����
			map.SetValueAt(index,10,COleVariant(ftol(10*djzl)/10.0));	//�ӹ�����,�ݵ��ڵ�������
			map.SetValueAt(index,11,COleVariant("V"));	//����
			if(pRec->nMSumLs>0)
				map.SetValueAt(index,12,COleVariant("V"));	//�ƿ�
			if(pRec->bCutAngle)
				map.SetValueAt(index,13,COleVariant("V"));	//�н�
			if(pRec->bZhiWan)
				map.SetValueAt(index,14,COleVariant("V"));	//����
			if(pRec->bWeldPart)
				map.SetValueAt(index,15,COleVariant("V"));	//���
			if(pRec->bWeldPart)
				map.SetValueAt(index,16,COleVariant("V"));	//����
			if(pRec->bCutRoot)
				map.SetValueAt(index,17,COleVariant("V"));	//�ٽ�(���)
			if(pRec->bCutBer)
				map.SetValueAt(index,18,COleVariant("V"));	//����
			if(pRec->bKaiJiao)
				map.SetValueAt(index,19,COleVariant("V"));	//����
			if(pRec->bHeJiao)
				map.SetValueAt(index,20,COleVariant("V"));	//�Ͻ�
			if(pRec->nPushFlat>0)
				map.SetValueAt(index,21,COleVariant("V"));	//ѹ��
			if(pRec->bNeedFillet)
				map.SetValueAt(index,22,COleVariant("V"));	//���
			if(CLDSPart::CustomerSerial!=CLIENT_ShanXi_BaoJi)
			{	//��ΰ��������Ҫ��X�в���Ҫ��V wht 18-11-07
				if(pRec->bCutAngle)
					map.SetValueAt(index,23,COleVariant("V"));	//��֫
			}
			map.SetValueAt(index,24,COleVariant(""));		//����
			map.SetValueAt(index,25,COleVariant("V"));		//��п
			map.SetValueAt(index,26,COleVariant(pRec->sNotes));		//��ע
			if(pRec->part_type==3)
				map.SetValueAt(index,27,COleVariant(pRec->length));	//�峤
			if(pRec->part_type==3)
				map.SetValueAt(index,28,COleVariant(pRec->wide));	//���
			char notes[200];
			pRec->GetProcessNotes(notes);
			map.SetValueAt(index,29,COleVariant(notes));	//����
			index++;
		}
		char cell_end[15]="";
		//���ñ�Ÿ�ʽ
		sprintf(cell_end,"B%d",i);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B2",cell_end,COleVariant("@"));
		if(iWorkBook==1)
		{	//���ù���ʽ
			sprintf(cell_end,"C%d",i);
			CExcelOper::SetRangeNumberFormat(excel_sheet,"C2",cell_end,COleVariant("@"));
		}
		//�������ָ�ʽ
		sprintf(cell_end,"F%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"F2",cell_end,COleVariant("0.0"));
		sprintf(cell_end,"H%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"H2",cell_end,COleVariant("0.0"));
		sprintf(cell_end,"K%d",i+1);
		CExcelOper::SetRangeNumberFormat(excel_sheet,"K2",cell_end,COleVariant("0.0"));
		//���ñ������
		sprintf(cell_end,"AD%d",i+1);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
		//���ñ߿�
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
	//���в���ͳ��
	mat_tbl.RunSummarize(&Ta);
	mat_tbl.SortPart(0);
	//�����EXCEL�ļ�
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
		AfxMessageBox("δ�ҵ�Excel����!");
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

	excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������

	Sheets excel_sheets;
	Workbooks excel_workbooks;
	_Workbook excel_workbook;
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	//�Ǹ������ְ���
	//for(int iWorkBook=0;iWorkBook<2;iWorkBook++)
	{
		LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
		excel_workbook.AttachDispatch(pWorkbook);
		LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
		ASSERT(pWorksheets != NULL);
		excel_sheets.AttachDispatch(pWorksheets);
		//��д��һ�Ź������ݱ�
		LPDISPATCH pWorksheet;
		pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
		//
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet,FALSE);
		excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
		excel_sheet.SetName("Workpiece Data");
#else 
		excel_sheet.SetName("��������");
#endif
		//
		int index=0,nRow=0;
		nRow = mat_tbl.part_recordset.GetNodeNum();
		CVariant2dArray map(nRow+1,24);
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,0,COleVariant("Segment Number(Read Only)"));//�κ�
		map.SetValueAt(index,1,COleVariant("Block Number"));			//�������
		map.SetValueAt(index,2,COleVariant("Material Texture"));		//����
		map.SetValueAt(index,3,COleVariant("Specification"));			//��ƹ��
		map.SetValueAt(index,4,COleVariant("Length(mm)"));				//����
		map.SetValueAt(index,5,COleVariant("Width"));					//���
		map.SetValueAt(index,6,COleVariant("Single Basic Number"));		//������
		map.SetValueAt(index,7,COleVariant("Piece Weight(Kg)"));		//��������
		map.SetValueAt(index,8,COleVariant("Total Weight"));			//��������
		map.SetValueAt(index,9,COleVariant("Single Hole"));			//��������
		map.SetValueAt(index,10,COleVariant("Notes"));					//��ע
		map.SetValueAt(index,11,COleVariant("Weld"));					//����
		map.SetValueAt(index,12,COleVariant("Bend"));					//����
		map.SetValueAt(index,13,COleVariant("Leg Cut"));				//��֫
		map.SetValueAt(index,14,COleVariant("Cut Back"));				//����
		map.SetValueAt(index,15,COleVariant("Cut Root"));				//�ٽ�
		map.SetValueAt(index,16,COleVariant("Flat"));					//ѹ��
		map.SetValueAt(index,17,COleVariant("Open Merge Angle"));		//����
		map.SetValueAt(index,18,COleVariant("Type"));
		map.SetValueAt(index,19,COleVariant("Edge Number"));
		map.SetValueAt(index,20,COleVariant("Perimeter"));
		map.SetValueAt(index,21,COleVariant("Hole Diameter 28~38mm(A)"));	
		map.SetValueAt(index,22,COleVariant("Hole Diameter 39~50mm(B)"));	
		map.SetValueAt(index,23,COleVariant("Hole Diameter More Than 50mm(C)"));
#else 
		map.SetValueAt(index,0,COleVariant("����(ֻ��)"));	//�κ�
		map.SetValueAt(index,1,COleVariant("������"));		//�������
		map.SetValueAt(index,2,COleVariant("����"));			//����
		map.SetValueAt(index,3,COleVariant("���"));			//��ƹ��
		map.SetValueAt(index,4,COleVariant("����(mm)"));		//����
		map.SetValueAt(index,5,COleVariant("���"));			//���
		map.SetValueAt(index,6,COleVariant("������"));		//������
		map.SetValueAt(index,7,COleVariant("��������(kg)"));//��������
		map.SetValueAt(index,8,COleVariant("������(kg)"));	//��������
		map.SetValueAt(index,9,COleVariant("��������(��)"));//��������
		map.SetValueAt(index,10,COleVariant("��ע"));		//��ע
		map.SetValueAt(index,11,COleVariant("�纸"));		//����
		map.SetValueAt(index,12,COleVariant("����"));		//����
		map.SetValueAt(index,13,COleVariant("�н���֫"));	//��֫
		map.SetValueAt(index,14,COleVariant("����"));		//����
		map.SetValueAt(index,15,COleVariant("���"));		//�ٽ�
		map.SetValueAt(index,16,COleVariant("���"));		//ѹ��
		map.SetValueAt(index,17,COleVariant("���Ͻ�"));		//����
		map.SetValueAt(index,18,COleVariant("����"));
		map.SetValueAt(index,19,COleVariant("����"));
		map.SetValueAt(index,20,COleVariant("�ܳ�"));
		map.SetValueAt(index,21,COleVariant("�׾�28~38mm(A)"));	
		map.SetValueAt(index,22,COleVariant("�׾�39~50mm(B)"));	
		map.SetValueAt(index,23,COleVariant("�׾�50mm����(C)"));
#endif
		//�����п�
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
			map.SetValueAt(index,0,COleVariant(SEGI(pRec->iSeg).ToString()));	//�κ�
			map.SetValueAt(index,1,COleVariant(pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)));	//����
			CXhChar16 sMark;
			QuerySteelMatMark(pRec->cMaterial,sMark);
			if(sMark.EqualNoCase("Q235")||sMark.Equal("Q345"))
				sMark.Append("B");
			map.SetValueAt(index,2,COleVariant((char*)sMark));	//����
			CXhChar16 sSpec("%s",(char*)pRec->sSpec);
			if(pRec->part_type==3)
				sSpec.Printf("-%.0f",pRec->thick);
			map.SetValueAt(index,3,COleVariant(sSpec));					//��ƹ��
			map.SetValueAt(index,4,COleVariant(ftol(pRec->length)));	//����
			map.SetValueAt(index,5,COleVariant(ftol(pRec->wide)));		//���
			map.SetValueAt(index,6,COleVariant((long)pRec->GetPartNum()));	//��������
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				map.SetValueAt(index,7,COleVariant(ftol(10*pRec->fPieceMinWeight)/10.0));	//��������
			else
				map.SetValueAt(index,7,COleVariant(ftol(10*pRec->fPieceNetWeight)/10.0));	//��������
			double djzl=0;
			if(CLDSPlate::m_iPlankWeightStatType<=1)
				djzl=pRec->fPieceMinWeight*pRec->GetPartNum();
			else
				djzl=pRec->fPieceNetWeight*pRec->GetPartNum();
			map.SetValueAt(index,8,COleVariant(ftol(10*djzl)/10.0));	//��������
			map.SetValueAt(index,9,COleVariant((long)pRec->nMSumLs));	//��������
			CXhChar200 sNotes;
			pRec->GetProcessNotes(sNotes,TRUE);
			sNotes.Append(" ");
			sNotes.Append(pRec->sNotes);
			map.SetValueAt(index,10,COleVariant((char*)sNotes));		//��ע
			if(pRec->bWeldPart)
				map.SetValueAt(index,11,COleVariant("1"));	//����
			else
				map.SetValueAt(index,11,COleVariant("0"));	//����
			if(pRec->bZhiWan)
				map.SetValueAt(index,12,COleVariant("1"));	//����
			else
				map.SetValueAt(index,12,COleVariant("0"));	//����
			if(pRec->bCutAngle==2)
				map.SetValueAt(index,13,COleVariant("2"));	//�н���֫
			else if(pRec->bCutAngle!=0)
				map.SetValueAt(index,13,COleVariant("1"));	//�н���֫
			else
				map.SetValueAt(index,13,COleVariant("0"));	//�н���֫
			if(pRec->bCutBer)
				map.SetValueAt(index,14,COleVariant("1"));	//����
			else
				map.SetValueAt(index,14,COleVariant("0"));	//����
			if(pRec->bCutRoot)
				map.SetValueAt(index,15,COleVariant("1"));	//���
			else
				map.SetValueAt(index,15,COleVariant("0"));	//���
			if(pRec->nPushFlat>0)
				map.SetValueAt(index,16,COleVariant("1"));	//ѹ��
			else
				map.SetValueAt(index,16,COleVariant("0"));	//ѹ��
			if(pRec->bKaiJiao||pRec->bHeJiao)
				map.SetValueAt(index,17,COleVariant("1"));	//���ǺϽ�
			else
				map.SetValueAt(index,17,COleVariant("0"));	//���ǺϽ�
			if(pRec->nEdgeNum>0)
				map.SetValueAt(index,19,COleVariant((char*)CXhChar16("%d",pRec->nEdgeNum)));		//����
			if(pRec->fPerimeter>0)
				map.SetValueAt(index,20,COleVariant((char*)CXhChar16("%.0f",pRec->fPerimeter)));	//�ܳ�
			map.SetValueAt(index,21,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(28,38))));	//�׾�28~38mm(A)
			map.SetValueAt(index,22,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(39,50))));	//�׾�39~50mm(B)
			map.SetValueAt(index,23,COleVariant((char*)CXhChar16("%d",pRec->GetLsHoleNum(51))));	//�׾�50mm����(C)
			index++;
		}
		char cell_end[15]="";
		//���ñ�Ÿ�ʽ
		sprintf(cell_end,"B%d",index-1);
		CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B2",cell_end,COleVariant("@"));
		//���ñ������
		sprintf(cell_end,"X%d",index);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
		//
		excel_sheet.ReleaseDispatch();
		excel_sheets.ReleaseDispatch();
		excel_workbook.ReleaseDispatch();
	}
	EndWaitCursor();
}
//�������Ͼ��������󣬰��ض���ʽ����������ϸ��	wxc_2015-10-23
void CLDSDoc::OnExportFLExcelFile()
{
	//1�����з�����ͳ��
	BeginWaitCursor();
	CTowerType mat_tbl;
	mat_tbl.SummarizeFLs(&Ta);
	mat_tbl.SortFL();
	//2������������ϸ��
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
	excel_sheet.SetName("�����̱�");
	int nFlCount=mat_tbl.fl_recordset.GetNodeNum();
	CExcelOper::SetMainTitle(excel_sheet,"A1","G1","��������ϸ��");
	CVariant2dArray map(nFlCount+1,7);
	CXhChar16 cell_start("A2"),cell_end("G%d",mat_tbl.fl_recordset.GetNodeNum()+1);
	//��һ���б���
	int index=0;
	map.SetValueAt(index,0,COleVariant("����"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("���"));
	map.SetValueAt(index,3,COleVariant("���"));
	map.SetValueAt(index,4,COleVariant("����*�׾�"));
	map.SetValueAt(index,5,COleVariant("��������"));
	map.SetValueAt(index,6,COleVariant("��ע"));
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A2","G2");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"C2");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"G2");
	index++;
	//����ά������
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
				CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//�ϲ���
			}
			if(index<mat_tbl.fl_recordset.GetNodeNum())
			{
				seg_start_i=i;
				seg_start_no=pFlRec->iSeg;
				map.SetValueAt(index,0,COleVariant(pFlRec->iSeg.ToString()));
			}
		}
		CXhChar50 sValue;
		map.SetValueAt(index,1,COleVariant(pFlRec->GetPartNo()));	//����
		map.SetValueAt(index,2,COleVariant(pFlRec->GetSpec()));		//���
		
		sValue.Printf("-%g",pFlRec->thick);
		map.SetValueAt(index,3,COleVariant(sValue));				//���
		sValue.Printf("%d*%d",pFlRec->nLsNum,pFlRec->ls_d);
		map.SetValueAt(index,4,COleVariant(sValue));				//��˨��Ϣ
		sValue.Printf("%d",pFlRec->GetFLNum());
		map.SetValueAt(index,5,COleVariant(sValue));				//��������
		map.SetValueAt(index,6,COleVariant(pFlRec->sNotes));		//��ע		
	}
	//���ö��뷽ʽ
	cell_end.Printf("G%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)3));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)2));
	cell_end.Printf("C%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	cell_end.Printf("G%d",index+1);
	//���ñ������ 
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);			
	//���ñ߿�
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
		"������������(*.tma)|*.tma|�����������ļ�(*.sgt)|*.sgt|�����ļ�(*.*)|*.*||");
#endif
#elif defined(__LMA_FILE_)
	CFileDialog dlg(FALSE,"lma",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"��������������(*.lma)|*.lma|�����������ļ�(*.sgt)|*.sgt|�����ļ�(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(FALSE,"tda",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"������ͼ����(*.tda)|*.tda|�����������ļ�(*.sgt)|*.sgt|�����ļ�(*.*)|*.*||");
#elif defined(__LDS_FILE_)
	CFileDialog dlg(FALSE,"lds",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"������ƻ�ͼһ�廯���(*.lds)|*.lds|�����������ļ�(*.sgt)|*.sgt|�����ļ�(*.*)|*.*||");
#elif defined(__TSA_FILE_)
	CFileDialog dlg(FALSE,"tsa",filename,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"����������������ļ�(*.tsa)|*.tsa|�����ļ�(*.*)|*.*||");
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
		m_nTimer=2;	//���ñ����ļ��洢
		//SetPathName(dlg.GetFileName());	//����OnSaveDocument֮��ִ��
		OnSaveDocument(dlg.GetPathName());
		//�Ķ�ԭ������:1.windowsϵͳ��SetPathName���Զ�����·���п��ܳ���ʸ�Ϊ����GetPathName
		//2.��windows7��SetPathName�е���AddRecentFileʱ�������ļ��Ƿ���ڵ��ж�,�ʱ�������OnSaveDocument֮��ִ�� wjh-2015.2.6
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
			buffer.WriteString("������������");
#elif defined(__LMA_FILE_)
			buffer.WriteString("��������������");
#elif defined(__TSA_FILE_)
			buffer.WriteString("���������������");
#elif defined(__TDA_FILE_)
			buffer.WriteString("������ͼ����");
#else //defined(__LDS_FILE_)||defined(__LDS_)
			buffer.WriteString("������������ͼϵͳ");
#endif
			//ar <<m_sDocType;	//ֱ�����ַ�������˱������Ա���ٺڿ͹������������ƻ��Ŀ����ԡ�wjh 2011.12.2
			//if(g_sysPara.uFileSaveVersion>0&&VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
			//{
			//	ToStringVersion(g_sysPara.uFileSaveVersion,file_version);
			//	g_sysPara.uFileSaveVersion=0;	//Ϊ�������洢Ϊ�ɰ汾��ÿ��ִ����ɰ�洢�����ָ�uFileSaveVersion=0;
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
			//��Ҫ���а汾��־��¼ʱӦ�������µ�ģ���޸���־����
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
			{	//��¼��������ģ��У����
				CAttachBuffer *pCheckBuffer=Ta.AttachBuffer.GetValue(CTower::MODEL_SIZE_CHECK);
				pCheckBuffer->ClearContents();
				xDataCheckModel.ToBuffer(*pCheckBuffer);
			}
#endif
			CBuffer attachBuffer(1000000);
			Ta.ToAttachBuffer(attachBuffer);
			buffer.WriteInteger(attachBuffer.GetLength());	//���ӻ������峤��
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
			if(!sDocTypeName.EqualNoCase("������������"))
#elif defined(__LMA_FILE_)
			if(!sDocTypeName.EqualNoCase("������������")&&!sDocTypeName.EqualNoCase("����������ϵͳ")&&!sDocTypeName.EqualNoCase("��������������"))
#elif defined(__TSA_FILE_)
			if(!sDocTypeName.EqualNoCase("����Ӧ�����") !=0&&sDocTypeName.EqualNoCase("���������������"))
#elif defined(__TDA_FILE_)
			if(!sDocTypeName.EqualNoCase("������ͼ����")&&!sDocTypeName.EqualNoCase("������������")&&
				!sDocTypeName.EqualNoCase("����������ϵͳ")&&!sDocTypeName.EqualNoCase("��������������"))
#else //defined(__LDS_FILE_)||defined(__LDS_)
			//theApp.m_bAnalysisWorkEnv=TRUE;
			if(theApp.m_eEnvType==SOLID_ENV)
				theApp.m_eEnvType=LINE_ENV;	//��ֹ��ǰΪ��ʾʵ��״̬ʱ���ļ����޷���ʾ�������� wjh-2013.4.6
			g_pSolidSet->SetDisplayType(DISP_LINE);	//��������ģʽ����ʾ����
			if( !sDocTypeName.EqualNoCase("������������")&&
				!sDocTypeName.EqualNoCase("������ͼ����")&&
				!sDocTypeName.EqualNoCase("����������ϵͳ")&&!sDocTypeName.EqualNoCase("��������������")&&
				!sDocTypeName.EqualNoCase("������������ͼϵͳ"))
#endif
			{
				SetTitle("�ޱ���");//throw CArchiveException(CArchiveException::badClass);
				throw "��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!";
			}
			int doc_type=0;
			if(sDocTypeName.EqualNoCase("������������"))
				doc_type=1;
			else if(sDocTypeName.EqualNoCase("����������ϵͳ")||sDocTypeName.EqualNoCase("��������������"))
				doc_type=2;
			else if(sDocTypeName.EqualNoCase("����Ӧ�����")||sDocTypeName.EqualNoCase("���������������"))
				doc_type=3;
			else if(sDocTypeName.EqualNoCase("������ͼ����"))
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
				throw "���ļ��汾��̫��,����ֱ�Ӷ�ȡ���ļ�";
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
					throw "�Ҳ������ܹ�,�޷��������ļ�!";

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
					PutMessage(theApp.m_pMainWnd,"���ڽ��оɸ�ʽ�ļ��汾ת���������ĵȴ�!");
					if(!VersionOldLTMAFile(theApp.m_sLicFile,version_convert_exefile,towerBuffer))
					{
						PutMessage(theApp.m_pMainWnd,"");
						throw "L/TMA���ļ�ת��ʧ��!";
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
				//����и��ӻ�����
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
				DWORD dwCurPos=(DWORD)buffer.GetCursorPosition();
				DWORD dwFileLen=(DWORD)buffer.GetLength();
				if(dwFileLen>dwCurPos&&compareVersion(Ta.version,"1.2.0.3")<0)
				{	//�и��ӻ�����ʱ��ȡ���ܻ�ͼ����������
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
					CAttachBuffer* pDrawBuff=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
					CXhChar50 bufname;
					buffer.ReadString(bufname);
					DWORD buf_len=0;
					buffer.ReadDword(&buf_len);	//BOM_CONFIG+SMART_DRAW���������ܳ�
					buffer.ReadDword(&buf_len);	//BOMCONFIG ���泤��
					pBomBuff->SetBlockSize(buf_len);
					char* buf=new char[buf_len];
					buffer.Read(buf,buf_len);
					pBomBuff->Write(buf,buf_len);
					delete []buf;
					buffer.ReadDword(&buf_len);	//SMART_DRAW ���泤��
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
					//��¼ԭʼ��������ģ���Ա�ȶԼ�¼�Ķ���־
					CAttachBuffer* pLogModel=Ta.AttachBuffer.GetValue(CTower::TOWER_MODEL);
					pLogModel->ClearContents();
					pLogModel->SetBlockSize(towerBuffer.GetLength());
					Ta.ToBuffer(*pLogModel,DogSerialNo(),file_version);
					CBuffer* pLoggerBuf=Ta.AttachBuffer.GetValue(CTower::LOGGER);
					logger.FromBuffer(*pLoggerBuf);
#endif
#ifdef __TMA_
					if(PRODUCT_FUNC::IsHasInternalTest())
					{	//��¼��������ģ��У����
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
