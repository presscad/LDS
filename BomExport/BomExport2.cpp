// BomExport.cpp : 定义 DLL 的初始化例程。
// 唐山电建的ＢＯＭ清单文件输出
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
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CBomExportApp

BEGIN_MESSAGE_MAP(CBomExportApp, CWinApp)
END_MESSAGE_MAP()


// CBomExportApp 构造

CBomExportApp::CBomExportApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBomExportApp 对象

CBomExportApp theApp;


// CBomExportApp 初始化

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
//添加一行数据到Excel
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
	//1.获取BOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer);
	}
	//2.按规格材质将角钢分类
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
	//3.生成txt文本
	for(LIST_NODE<PART_SERIES> *pPartNode=partSeriesList.EnumFirst();pPartNode;pPartNode=partSeriesList.EnumNext())
	{
		PART_SERIES *pPartSeries=&pPartNode->data;
		CXhChar500 sFileName("%s%s.txt",file_name,(char*)pPartSeries->sSpec);
		FILE *pFile ;
		pFile=fopen(sFileName,"w+");
		int nNum=0;
		for(SUPERLIST_NODE<BOMPART> *pBomPart=pPartSeries->listParts.EnumFirst();pBomPart;pBomPart=pPartSeries->listParts.EnumNext())
			nNum++;
		fprintf(pFile,"零件数量       %d\n",nNum);
		fprintf(pFile,"角钢种类 1\n");
		fprintf(pFile,"塔型名称 %s\n\n",(char*)bom.m_sTowerTypeName);
		fprintf(pFile,"零件编号      长度      件数\n");
		for(SUPERLIST_NODE<BOMPART> *pBomPart=pPartSeries->listParts.EnumFirst();pBomPart;pBomPart=pPartSeries->listParts.EnumNext())
		{
			BOMPART *pPart=pBomPart->pDataObj;
			if(pPart==NULL)
				continue;
			fprintf(pFile,"%8s%8d.%8d\n",(char*)pPart->sPartNo,ftol(pPart->length),pPart->GetPartNum());
		}
		fprintf(pFile,"材质   :A/G     规格    %s\n",(char*)pPartSeries->sSpec);
		fprintf(pFile,"料端损耗 15.0   下料损耗 30.0  料长间隙 1000.0\n");
		fprintf(pFile,"打印控制 1\n");
		fprintf(pFile,"算法选择 1\n");
		fprintf(pFile,"用料长度\n");
		fprintf(pFile,"13000.   0");
		fclose(pFile);
	}
	CString ss;
	ss.Format("Txt文件已生成到{%s}中!",file_name);
	MessageBoxA(NULL,(LPCSTR)ss,"生成目录",NULL);
	return 1;
}
