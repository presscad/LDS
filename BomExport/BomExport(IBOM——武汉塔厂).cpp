// BomExport.cpp : 定义 DLL 的初始化例程。
// 山东华安
//TODO 没有对焊法兰

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "list.h"
#include "math.h"
#include "SortFunc.h"
#include "ToolKit.h"

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
	return 8;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

const int COL_COUNT=6;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=2;	//件号
		colIndexArr[1]=4;	//规格
		colIndexArr[2]=5;	//材质
		colIndexArr[3]=7;	//数量
		colIndexArr[4]=8;	//单重
		colIndexArr[5]=6;	//长度
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//构件处理
void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		BomArr.append(*pNode);
	}
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("图纸构件");	
	CStringArray str_arr;
	const int COL_COUNT=10;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="塔型";str_arr[1]="段号";str_arr[2]="部件编号";
	str_arr[3]="材料名称";str_arr[4]="设计规格";str_arr[5]="材质";
	str_arr[6]="长度";str_arr[7]="单基件数";str_arr[8]="单件重量";str_arr[9]="小计重量";
	double col_arr[10]={8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,10.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//获取Excel表格的范围
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(""));
		map.SetValueAt(index,1,COleVariant(SEGI(pBomPart->iSeg).ToString()));
		map.SetValueAt(index,2,COleVariant(pBomPart->sPartNo));
		if(pBomPart->cPartType==1)
			map.SetValueAt(index,4,COleVariant(CXhChar50("∠%d*%d",(int)pBomPart->wide),(int)pBomPart->thick));
		else if(pBomPart->cPartType==3)
			map.SetValueAt(index,4,COleVariant(CXhChar50("-%d",(int)pBomPart->thick)));
		if(pBomPart->cPartType==1)
			map.SetValueAt(index,3,COleVariant("角钢"));
		else if(pBomPart->cPartType==3)
			map.SetValueAt(index,3,COleVariant("板材"));
		else if(pBomPart->cPartType==4)
			map.SetValueAt(index,3,COleVariant("钢管"));
		else 
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		CXhChar16 materialStr=QuerySteelMatMark(pBomPart->cMaterial);
		if(stricmp(materialStr,"Q235")==0)
			map.SetValueAt(index,5,COleVariant("A3"));
		else
			map.SetValueAt(index,5,COleVariant(materialStr));
		if(pBomPart->cPartType==3)
		{
			map.SetValueAt(index,4,COleVariant(CXhChar50("-%d",(int)pBomPart->thick)));
			map.SetValueAt(index,6,COleVariant(CXhChar50("%dx%.f",(int)pBomPart->wide,pBomPart->length)));
		}
		else
		{
			map.SetValueAt(index,4,COleVariant(pBomPart->sSpec));
			if(pBomPart->cPartType==BOMPART::ACCESSORY&&pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index,6,COleVariant(CXhChar50("%.fX%.f",pBomPart->wide,pBomPart->length)));
			else
				map.SetValueAt(index,6,COleVariant(CXhChar50("%.f",pBomPart->length)));
		}
		map.SetValueAt(index,7,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,8,COleVariant(CXhChar50("%.2f",pBomPart->fPieceWeight)));
		map.SetValueAt(index,9,COleVariant(CXhChar50("=H%d*I%d",index+2,index+2)));
		index++;
	}
	CXhChar16 cell_end("J%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.ReleaseDispatch();
}
//////////////////////////////////////////////////////////////////////////
//
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	//
	LPDISPATCH pWorksheets=NULL,pWorksheet=NULL;
	pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//1 构件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}