// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
// ��ɽ�罨�ģ£ϣ��嵥�ļ����
//

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	ע�⣡
//
//		����� DLL ��̬���ӵ� MFC
//		DLL���Ӵ� DLL ������
//		���� MFC ���κκ����ں�������ǰ��
//		��������� AFX_MANAGE_STATE �ꡣ
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CBomExportApp

BEGIN_MESSAGE_MAP(CBomExportApp, CWinApp)
END_MESSAGE_MAP()


// CBomExportApp ����

CBomExportApp::CBomExportApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CBomExportApp ����

CBomExportApp theApp;


// CBomExportApp ��ʼ��

BOOL CBomExportApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
TOWER_BOM_EXPORTS_API DWORD GetSupportDataBufferVersion()
{
	return 1;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"
long ftol(double fVal)
{
	return (long)(fVal+0.5);
}
CXhChar16 QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("16Mn");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else //if('S'==cMat)
		sMatMark.Copy("A3F");
	return sMatMark;
}
//���һ�����ݵ�Excel
struct PART_SERIES
{
	CXhChar50 sSpec;
	CXhSuperList<BOMPART> listParts;
};
void MakeDirectory(const char *path)
{
	char bak_path[MAX_PATH],drive[MAX_PATH];
	strcpy(bak_path,path);
	char *dir = strtok(bak_path,"/\\");
	if(strlen(dir)==2&&dir[1]==':')
	{
		strcpy(drive,dir);
		strcat(drive,"\\");
		_chdir(drive);
		dir = strtok(NULL,"/\\");
	}
	while(dir)
	{
		_mkdir(dir);
		_chdir(dir);
		dir = strtok(NULL,"/\\");
	}
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	MakeDirectory(file_name);
	//1.��ȡBOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer);
	}
	//2.�������ʽ��Ǹַ���
	CXhSimpleList<PART_SERIES> partSeriesList;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL||pPart->cPartType!=BOMPART::ANGLE)
			continue;
		CXhChar50 sPec;
		CXhChar16 sSteelMat=QuerySteelMatMark(pPart->cMaterial);
		if(pPart->wingWideY>0&&pPart->wingWideY!=pPart->wide)
			sPec.Printf("L %.fx%.fx%.f  %s",pPart->wide,pPart->wingWideY,pPart->thick,(char*)sSteelMat);
		else 
			sPec.Printf("L %.fx%.f  %s",pPart->wide,pPart->thick,(char*)sSteelMat);
		LIST_NODE<PART_SERIES> *pPartTemp=NULL;
		for(pPartTemp =partSeriesList.EnumFirst();pPartTemp;pPartTemp=partSeriesList.EnumNext())
		{
			if(sPec.Equal(pPartTemp->data.sSpec))
				break;
		}
		if(pPartTemp==NULL)
		{
			pPartTemp=partSeriesList.AttachNode();
			pPartTemp->data.sSpec.Copy(sPec);
		}
		pPartTemp->data.listParts.AttachNode(pPart);
	}
	//3.����txt�ı�
	for(LIST_NODE<PART_SERIES> *pPartNode=partSeriesList.EnumFirst();pPartNode;pPartNode=partSeriesList.EnumNext())
	{
		PART_SERIES *pPartSeries=&pPartNode->data;
		CXhChar500 sFileName("%s%s.txt",file_name,(char*)pPartSeries->sSpec);
		FILE *pFile ;
		pFile=fopen(sFileName,"w+");
		int nNum=0;
		for(SUPERLIST_NODE<BOMPART> *pBomPart=pPartSeries->listParts.EnumFirst();pBomPart;pBomPart=pPartSeries->listParts.EnumNext())
			nNum++;
		fprintf(pFile,"�������       %d\n",nNum);
		fprintf(pFile,"�Ǹ����� 1\n");
		fprintf(pFile,"�������� %s\n\n",(char*)bom.m_sTowerTypeName);
		fprintf(pFile,"������      ����      ����\n");
		for(SUPERLIST_NODE<BOMPART> *pBomPart=pPartSeries->listParts.EnumFirst();pBomPart;pBomPart=pPartSeries->listParts.EnumNext())
		{
			BOMPART *pPart=pBomPart->pDataObj;
			if(pPart==NULL)
				continue;
			fprintf(pFile,"%8s%8d.%8d\n",(char*)pPart->sPartNo,ftol(pPart->length),pPart->GetPartNum());
		}
		fprintf(pFile,"����   :A/G     ���    %s\n",(char*)pPartSeries->sSpec);
		fprintf(pFile,"�϶���� 15.0   ������� 30.0  �ϳ���϶ 1000.0\n");
		fprintf(pFile,"��ӡ���� 1\n");
		fprintf(pFile,"�㷨ѡ�� 1\n");
		fprintf(pFile,"���ϳ���\n");
		fprintf(pFile,"13000.   0");
		fclose(pFile);
	}
	CString ss;
	ss.Format("Txt�ļ������ɵ�{%s}��!",file_name);
	MessageBoxA(NULL,(LPCSTR)ss,"����Ŀ¼",NULL);
	return 1;
}
