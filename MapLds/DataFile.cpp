//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
//�����ļ�
#include "stdafx.h"
#include "stdarx.h"
#include "RxTools.h"
#include "MenuFunc.h"
#include "SysPara.h"
#include "Tower.h"
#include "FileVersion.h"
#include "FilePropertyDlg.h"
#include "CryptBuffer.h"
#include "Drawing.h"
#include "TowerBom.h"

#ifdef __SMART_DRAW_INC_
#include "SelectDrawingDlg.h"
void LoadScriptFromBuffer();//From DrawMapScript.cpp
#endif

static CFile *m_pFile;
static CArchive *m_pDoc;
char g_sFileName[100]="";
CString g_sDocTypeName="";
int g_nDocType=0;
char g_sTaskNo[101]="";
char g_sTolerance[101]="����";
void DisplayProcess(int percent,const char *sTitle);
BOOL LoadTsaFile(CArchive &ar);

//�ر��ļ�
void CloseTaFile(BOOL bPrompt,BOOL bClearTa)
{
	if( m_pDoc != NULL)
	{
		m_pDoc->Close();
		delete m_pDoc;
		m_pDoc = NULL;
	}
	if(	m_pFile != NULL)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
		if(bPrompt)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			acedAlert(L"Source file already closed,can't operate on it anymore.");
#else
			acedAlert("Source file already closed,can't operate on it anymore.");
#endif
#else
#ifdef _ARX_2007
			acedAlert(L"ԭ�ļ��ѹر�,�Ժ����ٶ�����в���");
#else
			acedAlert("ԭ�ļ��ѹر�,�Ժ����ٶ�����в���");
#endif
#endif
		}
	}
	if(bClearTa)
		Ta.Empty();
}
//��ֻ��ģʽ���ļ�
extern CXhChar200 g_sLdsExecutePath;	//from MapLds.cpp
extern CXhChar200 g_sLicFile;
int PutMessage(CWnd* pWnd, const char* FormatStr,...);
//From Drawing.cpp
#ifdef __SMART_DRAW_INC_
CDrawing* CreateDrawing(int idClsType,DWORD key,void* pContext);
BOOL DeleteDrawing(CDrawing *pDrawing);
#endif
#include "StackAcadVar.h"
void OpenTaFile(CString sFilePath,BOOL bPrompt,BOOL bReadAdditionalBuffer)
{
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	if((hFind=FindFirstFile(sFilePath,&FindFileData))==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(CXhChar200("δ�ҵ��ļ�\"%s\"",sFilePath));
		return;
	}
	else
		FindClose(hFind);
	Ta.Empty();
	m_pFile = new CFile(sFilePath,CFile::modeRead);
	m_pDoc  = new CArchive(m_pFile,CArchive::load);
	CLogErrorLife loglife;
	if(!bPrompt)
		Ta.DisplayProcess=NULL;
	else 
		Ta.DisplayProcess=DisplayProcess;
#ifdef __SMART_DRAW_INC_
	drawingList.CreateNewAtom=CreateDrawing;
	drawingList.DeleteAtom=DeleteDrawing;
	//ʹ���ļ����ݸ���ͼֽ�б�
	if(bReadAdditionalBuffer)
	{	//���ͼֽ�б�����,��ͼ�ű���������
		drawingList.Empty();	//���ͼֽ����
		//Ta.ParseDrawingScriptBuffer=LoadScriptFromBuffer;	//������ͼ�ű�����
	}
#endif
	*m_pDoc>>g_sDocTypeName;
#ifdef __LMA_FILE_
	if(g_sDocTypeName.Compare("����������ϵͳ")!=0&&g_sDocTypeName.Compare("��������������")!=0&&g_sDocTypeName.Compare("������������")!=0)
#elif defined(__TMA_FILE_)
	if(g_sDocTypeName.Compare("������������")!=0)
#elif defined(__TSA_FILE_)
	if(g_sDocTypeName.Compare("����Ӧ�����") !=0&&g_sDocTypeName.Compare("���������������")!=0)
#elif defined(__TDA_FILE_)
	if(g_sDocTypeName.Compare("������ͼ����") !=0&&g_sDocTypeName.Compare("����������ϵͳ")!=0&&g_sDocTypeName.Compare("��������������")!=0&&g_sDocTypeName.Compare("������������")!=0)
#else
	if( g_sDocTypeName.Compare("������������") !=0&&
		g_sDocTypeName.Compare("������ͼ����") !=0&&
		g_sDocTypeName.Compare("����������ϵͳ") !=0&&g_sDocTypeName.Compare("��������������") !=0&&
		g_sDocTypeName.Compare("������������ͼϵͳ") !=0)
#endif
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Unexpected file format.");
#else
		AfxMessageBox("��Ԥ�ڵ��ļ���ʽ");
#endif
		return;
	}
	if(g_sDocTypeName.Compare("������������") ==0)
		g_nDocType=1;
	else if(g_sDocTypeName.Compare("����������ϵͳ") ==0||g_sDocTypeName.Compare("��������������") ==0)
		g_nDocType=2;
	else if(g_sDocTypeName.Compare("����Ӧ�����") ==0||g_sDocTypeName.Compare("���������������") ==0)
		g_nDocType=3;
	else if(g_sDocTypeName.Compare("������ͼ����") ==0)
		g_nDocType=5;
	else //defined(__LDS_FILE_)||defined(__LDS_)
		g_nDocType=4;
	CString version;
	DWORD cursor_pipeline_no;
	*m_pDoc>>version;
	StrCopy(Ta.version,version,20);
	#if defined(__TMA_FILE_)
		if(compareVersion(Ta.version,"5.2.3.0")>0||compareVersion(Ta.version,"4.0.0.19")<0)
	#elif defined(__LMA_FILE_)
		if( (g_nDocType==1&&(compareVersion(Ta.version,"5.2.3.0")>0||compareVersion(Ta.version,"4.0.0.19")<0))||
			(g_nDocType==2&&(compareVersion(Ta.version,"2.1.3.0")>0||compareVersion(Ta.version,"1.0.0.73")<0)))
	#elif defined(__TSA_FILE_)
		if(compareVersion(Ta.version,"2.1.3.1")>0)
	#elif defined(__TDA_FILE_)
		if( (g_nDocType==1&&(compareVersion(Ta.version,"5.2.3.0")>0||compareVersion(Ta.version,"4.0.0.19")<0))||
			(g_nDocType==2&&(compareVersion(Ta.version,"2.1.3.0")>0||compareVersion(Ta.version,"1.0.0.73")<0))||
			(g_nDocType==5&& compareVersion(Ta.version,"1.3.12.0")>0))
	#else
	if( (g_nDocType==1&&(compareVersion(Ta.version,"5.2.3.0")>0||compareVersion(Ta.version,"4.0.0.19")<0))||
		(g_nDocType==2&&(compareVersion(Ta.version,"2.1.3.0")>0||compareVersion(Ta.version,"1.0.0.73")<0))||
		(g_nDocType==4&& compareVersion(Ta.version,"1.3.12.0")>0)||(g_nDocType==5&& compareVersion(Ta.version,"1.3.12.0")>0))
	#endif
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Version serial of this file is too high or low,can't read this file directly");
#else
			AfxMessageBox("���ļ��汾��̫�߻�̫��,����ֱ�Ӷ�ȡ���ļ�");
#endif
			return;
		}
	try{
#if defined(__TSA_FILE_)||defined(__TSA_)
		if(compareVersion(Ta.version,"1.7.0.0")<0)
			LoadTsaFile(*m_pDoc);
		else
		{
#endif
			*m_pDoc>>Ta.user_pipeline_no;
			*m_pDoc>>cursor_pipeline_no;
			DWORD buffer_len=0;
			*m_pDoc>>buffer_len;
			BYTE *pBuf = new BYTE[buffer_len];
			m_pDoc->Read(pBuf,buffer_len);
			CBuffer buffer;
			buffer.Write(pBuf,buffer_len);
			delete []pBuf;
			if((g_nDocType==1&&compareVersion(Ta.version,"4.0.0.19")<0)||
			   (g_nDocType==2&&compareVersion(Ta.version,"1.0.0.73")<0))
			{
				CXhChar200 version_convert_exefile=g_sLdsExecutePath;
				if(g_nDocType==1)
					version_convert_exefile.Append("TMAVersion.exe");
				else //if(doc_type==2)
					version_convert_exefile.Append("LMAVersion.exe");
#ifdef AFX_TARG_ENU_ENGLISH
				PutMessage(CWnd::FromHandle(adsw_acadMainWnd()),"On version conversion for old format file,please wait patiently!");
#else
				PutMessage(CWnd::FromHandle(adsw_acadMainWnd()),"���ڽ��оɸ�ʽ�ļ��汾ת���������ĵȴ�!");
#endif
				if(!VersionOldLTMAFile(g_sLicFile,version_convert_exefile,buffer))
				{
					PutMessage(CWnd::FromHandle(adsw_acadMainWnd()),"");
#ifdef AFX_TARG_ENU_ENGLISH
					throw "L/TMA old file filed convert!";
#else
					throw "L/TMA���ļ�ת��ʧ��!";
#endif
				}
				PutMessage(CWnd::FromHandle(adsw_acadMainWnd()),"");
			}
			char bEncryptByAES=false;
			if(g_nDocType==1)
			{
				if(compareVersion(Ta.version,"5.1.7.0")>=0)
					bEncryptByAES=2;	//��ָ�Ƶļ���ģʽ
				else if(compareVersion(Ta.version,"4.1.3.0")>=0)
					bEncryptByAES=true;
			}
			else if(g_nDocType==2)
			{
				if(compareVersion(Ta.version,"2.0.8.0")>=0)
					bEncryptByAES=2;	//LMA
				else if(compareVersion(Ta.version,"1.1.3.0")>=0)
					bEncryptByAES=true;	//LMA
			}
			else if(g_nDocType==3&&compareVersion(Ta.version,"2.1.0.0")>=0)
				bEncryptByAES=true;	//TSA
			else if(g_nDocType==4||g_nDocType==5)
			{
				if(compareVersion(Ta.version,"1.3.8.0")>=0)
					bEncryptByAES=2;	//LMA
				else if(compareVersion(Ta.version,"1.3.0.0")>=0)
					bEncryptByAES=true;	//LDS or TDA
			}
			//logerr.Log("step1 %d",bEncryptByAES);
			DecryptBuffer(buffer,bEncryptByAES);
			//logerr.Log("step2");
			Ta.FromBuffer(buffer,g_nDocType);
			//logerr.Log("Ta.FromBuffer finished");
			bom.Empty();
#if defined(__TSA_FILE_)||defined(__TSA_)
		}
#else
		//���°�����˨�׵ĵ����ߺ�
		CSuperSmartPtr<CLDSPart>pSuperPart;
		for(CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.GetFirst(CLS_BOLT);pBolt;pBolt=(CLDSBolt*)Ta.Parts.GetNext(CLS_BOLT))
			memset(pBolt->relaRayArr,0,sizeof(CLDSBolt::RELA_RAYPART)*4);
		for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
			pSuperPart->ResetSonBoltRayNo();	//����������˨���ߺ�,�����޷���ȷ������˨�������ߺŽ���������ȷ��ע��˨������ wht 11-07-28
		for(pSuperPart=Ta.Parts.GetFirst();pSuperPart.IsHasPtr();pSuperPart=Ta.Parts.GetNext())
		{
			if(pSuperPart->GetClassTypeId()==CLS_PLATE)
				pSuperPart.PlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pSuperPart);
			else if(pSuperPart->GetClassTypeId()==CLS_PARAMPLATE)
				pSuperPart.ParamPlatePointer()->GetLsRefList()->ResetBoltRefRayNo(pSuperPart);
		}
		if(sys.stru_map.bReadAdditionalBuffer)
		{	//���������ж��Ƿ���Ҫ��ȡ���ӻ��� wht 11-08-03
			if(bReadAdditionalBuffer)
			{	//��ȡ���ӻ���
				DWORD dwCurPos=(DWORD)m_pFile->GetPosition();
				DWORD dwFileLen=(DWORD)m_pFile->GetLength();
#if defined(__TDA_FILE_)||defined(__LDS_FILE_)
				if(dwFileLen>dwCurPos&&compareVersion(Ta.version,"1.2.0.3")<0)
				{	//��ȡ���ӻ�����
					CAttachBuffer* pBomBuff=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
					CAttachBuffer* pDrawBuff=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
					CString bufname;
					*m_pDoc>>bufname;
					DWORD buf_len=0;
					*m_pDoc>>buf_len;	//BOM_CONFIG+SMART_DRAW���������ܳ�
					*m_pDoc>>buf_len;	//BOMCONFIG ���泤��
					pBomBuff->SetBlockSize(buf_len);
					char* buf=new char[buf_len];
					m_pDoc->Read(buf,buf_len);
					pBomBuff->Write(buf,buf_len);
					delete []buf;
					*m_pDoc>>buf_len;	//SMART_DRAW ���泤��
					pDrawBuff->SetBlockSize(buf_len);
					buf=new char[buf_len];
					m_pDoc->Read(buf,buf_len);
					pDrawBuff->Write(buf,buf_len);
					delete []buf;
					/*if(sAdditionalBuffer.CompareNoCase("AdditionalBuffer")==0)
					{
						DWORD buffer_len=0;
						*m_pDoc>>buffer_len;
						BYTE *pAdditionalBuf = new BYTE[buffer_len];
						m_pDoc->Read(pAdditionalBuf,buffer_len);
						CBuffer additional_buffer(buffer_len);
						additional_buffer.Write(pAdditionalBuf,buffer_len);
						delete []pAdditionalBuf;
						Ta.ReadAdditionalBuffer(additional_buffer);
					}*/
				}
				else
#endif
				if( (g_nDocType==5&&compareVersion(Ta.version,"1.2.0.3")>=0)||
					(g_nDocType==4&&compareVersion(Ta.version,"1.2.0.3")>=0)||
					(g_nDocType==3&&compareVersion(Ta.version,"2.0.0.3")>=0)||
					(g_nDocType==2&&compareVersion(Ta.version,"1.2.0.3")>=0)||
					(g_nDocType==1&&compareVersion(Ta.version,"4.2.0.3")>0))
				{
					DWORD buf_len=0;
					DWORD dwRead=m_pDoc->Read(&buf_len,4);
					if(dwRead==4&&buf_len>0)
					{
						char* buf=new char[buf_len];
						CBuffer attachBuffer(buf,buf_len);
						m_pDoc->Read(buf,buf_len);
						Ta.FromAttachBuffer(attachBuffer);
						delete[] buf;
					}
				}
		#ifdef __SMART_DRAW_INC_
				LoadScriptFromBuffer();
		#endif
			}
		#ifdef __SMART_DRAW_INC_
			else 
			{	//���»�ͼ�ű���������ص�ָ�� wht 11-07-22
				CDrawing *pDrawing=NULL;
				for(pDrawing=drawingList.GetFirst();pDrawing;pDrawing=drawingList.GetNext())
					pDrawing->UpdatePtrByHandle();
			}
		#endif
		}
#endif
		if(bPrompt)
		{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			acedPrompt(L"Data import successfully!\n");
#else
			acedPrompt("Data import successfully!\n");
#endif
#else
	#ifdef _ARX_2007
			acedPrompt(L"����˳������\n");
	#else
			acedPrompt("����˳������\n");
	#endif
#endif
		}
#ifdef _ARX_2007
		acedCommand(RTSTR,L"pdmode",RTSTR,L"1",RTNONE);
		acedCommand(RTSTR,L"textstyle",RTSTR,L"standard",RTNONE);
#else
		acedCommand(RTSTR,"pdmode",RTSTR,"1",RTNONE);
		acedCommand(RTSTR,"textstyle",RTSTR,"standard",RTNONE);
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	GetCurDwg()->setLtscale(20);
}
#include "LicFuncDef.h"
void ImportTma()
{
	AcDbObjectId layerId;
    CAcModuleResourceOverride resOverride;
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef __LMA_FILE_
	CFileDialog dlg(TRUE,"lma","LatticeTower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"LatticeTower(*.lma)|*.lma|All Files(*.*)|*.*||");
#elif defined(__TMA_FILE_)
	CFileDialog dlg(TRUE,"tma","TMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TMA File(*.tma)|*.tma|All Files(*.*)|*.*||");
#elif defined(__TSA_FILE_)
	CFileDialog dlg(TRUE,"tsa","TSA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TSA File(*.tsa)|*.tsa|All Files(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TDA File(*.tda)|*.tda|All Files(*.*)|*.*||");
#else
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"LDS File(*.lds)|*.lds|All Files(*.*)|*.*||");
#endif
#else
#ifdef __LMA_FILE_
	CFileDialog dlg(TRUE,"lma","LMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"��������������(*.lma)|*.lma|������������(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#elif defined(__TMA_FILE_)
	CFileDialog dlg(TRUE,"tma","TMA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TMA�ļ�(*.tma)|*.tma|�����ļ�(*.*)|*.*||");
#elif defined(__TSA_FILE_)
	CFileDialog dlg(TRUE,"tsa","TSA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TSA�ļ�(*.tsa)|*.tsa|�����ļ�(*.*)|*.*||");
#elif defined(__TDA_FILE_)
	CFileDialog dlg(TRUE,"tda","TDA",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"TDA�ļ�(*.tda)|*.tda|TMA�ļ�(*.tma)|*.tma|��������������(*.lma)|*.lma|�����ļ�(*.*)|*.*||");
#else
	char* lpszFilter="LDS�ļ�(*.lds)|*.lds|TDA�ļ�(*.tda)|*.tda|������������(*.tma)|*.tma|��������������(*.lma)|*.lma|�����ļ�(*.*)|*.*||";
	bool bInteralTestEnable=false;
	if(GetLicVersion()>=1000003)
		bInteralTestEnable=(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST)!=FALSE);
	if(bInteralTestEnable)
		lpszFilter="LDS�ļ�(*.lds)|*.lds|TMA�ļ�(*.tma)|*.tma|��������������(*.lma)|*.lma|TDA�ļ�(*.tda)|*.tda|�����ļ�(*.*)|*.*||";
	CFileDialog dlg(TRUE,"lds","LDS",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszFilter);
#endif
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	InitDrawingEnv();
	CString filename,path;
	filename = dlg.GetFileName();
	//�����ϻ��ܱ��õ��ļ���
	strncpy(g_sFileName,filename,strlen(filename)-4);
	//�����ļ�����·��������׺��
	path = dlg.GetPathName();
	sprintf(sys.sFilePath,"%s",path); 
	
	CloseTma();
	OpenTaFile(path,TRUE,TRUE);
	sys.GetDrawingSysParaFromReg("Settings","Critic");	//���³�ʼ��Ta.m_sCritic���ԣ�������Tower��δ���� wht 18-11-13
}

#ifdef __SMART_DRAW_INC_
extern CSelectDrawingDlg drawingDlg;
#endif
void CloseTma()
{	//�رմ���
	CloseTaFile(TRUE,TRUE);
#ifdef __SMART_DRAW_INC_
	drawingList.Empty();
	drawingDlg.m_pDrawing=NULL;	//����Ӧ��CSelectDrawingDlg�Ի����м�¼Hash��ֵ�Ա���״̬���������ֻ����ʱ�����쳣������ʩ wjh-2011.8.31
	drawingDlg.m_pComponent=NULL;
#endif
}
void FileProp()
{
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
	CFilePropertyDlg dlg;
	dlg.m_sFileVersion = Ta.version;
	dlg.m_nNodeNum = Ta.Node.GetNodeNum();
	dlg.m_nPartNum = Ta.Parts.GetNodeNum();
	dlg.m_sPrjName = Ta.m_sPrjName;
	dlg.m_sTaType = Ta.m_sTaType;
	dlg.m_nTaNum   = Ta.m_nTaNum;
	dlg.m_sTaAlias = Ta.m_sTaAlias;
	dlg.m_sTaStampNo = Ta.m_sTaStampNo;
	dlg.m_sTblMaker= Ta.m_sOperator;
	dlg.m_sCritic  = Ta.m_sCritic;
	dlg.m_sAuditor = Ta.m_sAuditor;
	dlg.m_sCompanyName = sys.m_sCompanyName;
	dlg.m_sTaskNo = g_sTaskNo;
	dlg.m_sTolerance = g_sTolerance;
#ifdef __COMMON_PART_INC_
	dlg.m_iPlankAreaStatType = CLDSPlate::m_iPlankWeightStatType-1;
	dlg.m_iCirclePlankAreaType= CLDSParamPlate::m_iCirclePlankAreaType-1;
#endif
	if(dlg.DoModal()==IDOK)
	{
		sprintf(Ta.m_sPrjName,"%s",dlg.m_sPrjName);
		sprintf(Ta.m_sTaType,"%s",dlg.m_sTaType);
		sprintf(Ta.m_sTaAlias,"%s",dlg.m_sTaAlias);
		sprintf(Ta.m_sTaStampNo,"%s",dlg.m_sTaStampNo);
		sprintf(Ta.m_sOperator,"%s",dlg.m_sTblMaker);
		sprintf(Ta.m_sCritic,"%s",dlg.m_sCritic);
		sprintf(Ta.m_sAuditor,"%s",dlg.m_sAuditor);
		sprintf(sys.m_sCompanyName,"%s",dlg.m_sCompanyName);
		sprintf(g_sTaskNo,"%s",dlg.m_sTaskNo);
		sprintf(g_sTolerance,"%s",dlg.m_sTolerance);
		Ta.m_nTaNum = dlg.m_nTaNum;
#ifdef __COMMON_PART_INC_
		CLDSPlate::m_iPlankWeightStatType = dlg.m_iPlankAreaStatType+1;
		CLDSParamPlate::m_iCirclePlankAreaType=dlg.m_iCirclePlankAreaType+1;
#endif
		sys.WriteDrawingSysParaToReg("Settings","CompanyName");
		sys.WriteDrawingSysParaToReg("Settings","Critic");
	}
}

//�����ļ�
void SaveTma()
#ifdef __SMART_DRAW_INC_
{
	//�ȹرյ�ǰ���ļ�,���������ģ������
	CloseTaFile(FALSE,FALSE);
	//���ļ���ȡģ������
	CFile original_file;
	if(!original_file.Open(sys.sFilePath, CFile::modeRead))
		return;
	CArchive original_ar(&original_file,CArchive::load);
	original_ar>>g_sDocTypeName;
	CString sVersion="";
	original_ar>>sVersion;
	original_ar>>Ta.user_pipeline_no;
	DWORD cursor_pipeline_no=0;
	original_ar>>cursor_pipeline_no;
	DWORD buffer_len=0;
	original_ar>>buffer_len;
	BYTE *pBuf = new BYTE[buffer_len];
	original_ar.Read(pBuf,buffer_len);
	CBuffer model_buffer;
	model_buffer.Write(pBuf,buffer_len);
	delete []pBuf;
	original_ar.Close();
	original_file.Close();
	//�����ļ�
	CFile file;
	if(!file.Open(sys.sFilePath, CFile::modeWrite))
		return;
	file.SetLength(0);	//����ļ�����
	CArchive ar(&file,CArchive::store);
	ar<<g_sDocTypeName;
	ar<<sVersion;
	ar<<Ta.user_pipeline_no;
	ar<<cursor_pipeline_no;
	//����ģ������
	buffer_len=model_buffer.GetLength();
	ar<<buffer_len;
	ar.Write(model_buffer.GetBufferPtr(),buffer_len);
	ar.Flush(); //����������д���ļ���
	
	CBuffer attachBuffer(1000000);
	//Ta.WriteAdditionalBuffer(additonal_buffer);
	CAttachBuffer *pSmartDrawBuf=Ta.AttachBuffer.GetValue(CTower::SMART_DRAW);
	CAttachBuffer *pBomBuf=Ta.AttachBuffer.GetValue(CTower::BOM_CONFIG);
	Ta.ToAttachBuffer(attachBuffer);

	//ar<<CString("AdditionalBuffer");*/
	//DWORD dwFileLen = additonal_buffer.GetLength();
	//ar.Write(&dwFileLen,sizeof(DWORD));
	//ar.Write(additonal_buffer.GetBufferPtr(),dwFileLen);
	ar<<attachBuffer.GetLength();	//���ӻ������峤��
	ar.Write(attachBuffer.GetBufferPtr(),attachBuffer.GetLength());
	//���渽�ӻ�����
	Ta.SetDocModifiedFlag(FALSE);
	ar.Close();
	file.Close();
	//�д��Ż���ʱ0.485s wjh-2011.8.6
	//���´򿪱����Ժ���ļ�,�����ļ��������¶�ȡ��ģ������
	m_pFile = new CFile(sys.sFilePath,CFile::modeRead);
	m_pDoc  = new CArchive(m_pFile,CArchive::load);
	//OpenTaFile(sys.sFilePath,FALSE,FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	acedAlert(L"Successed to save file!\n");
#else
	acedAlert("Successed to save file!\n");
#endif
#else
#ifdef _ARX_2007
	acedAlert(L"�ļ�����ɹ�!\n");
#else
	acedAlert("�ļ�����ɹ�!\n");
#endif
#endif
}
#else
{}
#endif

