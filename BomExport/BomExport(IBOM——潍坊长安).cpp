// BomExport.cpp : 定义 DLL 的初始化例程。
// 湖州飞剑

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "ToolKit.h"
#include "HashTable.h"
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
	return 10;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

static CXhChar16 LocalQuerySteelMatMark(BOMPART *pPart)
{
	CXhChar16 sMatMark;
	if ('H' == pPart->cMaterial)
		sMatMark.Copy("Q345");
	else if ('G' == pPart->cMaterial)
		sMatMark.Copy("Q390");
	else if ('P' == pPart->cMaterial)
		sMatMark.Copy("Q420");
	else if ('T' == pPart->cMaterial)
		sMatMark.Copy("Q460");
	else if ('S' == pPart->cMaterial)
		sMatMark.Copy("");
	else
		sMatMark.Copy(pPart->sMaterial);
	return sMatMark;
}
static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec = pPart->GetSpec();
	CXhChar16 sMaterial = LocalQuerySteelMatMark(pPart);
	if (GetSupportDataBufferVersion() >= 2)
	{
		if (pPart->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%.fX%.f", pPart->thick, pPart->wide);
		else if (pPart->cPartType == BOMPART::ANGLE)
			sSpec.Printf("L%.fX%.f", pPart->wide, pPart->thick);
		PART_PLATE *pPlate = (PART_PLATE*)pPart;
		if (pPlate->IsFL() || pPlate->siSubType == BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("Φ%.f/%.f", pPlate->fFlOutterD, pPlate->fFlInnerD);
	}
	return CXhChar16("%s%s",(char*)sMaterial, (char*)sSpec);
}

void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("sheet");
	//添加实时数据的列标题
	//2.2 输出材料表
	//2.2.1 添加标题栏
	const int COL_COUNT = 8;
	CStringArray str_arr;
	str_arr.SetSize(11);
	str_arr[0] = ""; str_arr[1] = ""; str_arr[2] = ""; str_arr[3] = "";  
	str_arr[4] = "编号"; str_arr[5] = "规格"; str_arr[6] = "长度\n(mm)"; str_arr[7] = "数\n量";
	str_arr[8] = "一件"; str_arr[9] = "小计"; str_arr[10] = "备注"; 
	double col_arr[11] = { 8.38,8.38,8.38,8.38,8.38,12.38,8.38,8.38,
								8.38,8.38,8.38 };
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
	//2.2.2 输出材料表
	int i = 0, nCount = 0;
	int index = 0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN, COL_COUNT);
	for (SUPERLIST_NODE<BOMPART> *pNode = bom.listParts.EnumFirst(); pNode; pNode = bom.listParts.EnumNext())
	{
		BOMPART *pPart = pNode->pDataObj;
		if (pPart == NULL)
			continue;
		i++;
		map.SetValueAt(index, 0, COleVariant(SEGI(pPart->iSeg).ToString()));	//段号
		map.SetValueAt(index, 1, COleVariant(pPart->sPartNo));		//编号
		map.SetValueAt(index, 2, COleVariant(GetPartSpec(pPart)));	//规格
		map.SetValueAt(index, 3, COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index, 4, COleVariant(pPart->GetPartNum()));	//单基数量

		double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
		str.Format("%.2f", weight);
		map.SetValueAt(index, 5, COleVariant(str));					//单件重量
		double sum_weight = ftol(pPart->fPieceWeight*pPart->GetPartNum() * 100) / 100.0;
		str.Format("%.2f", sum_weight);
		map.SetValueAt(index, 6, COleVariant(str));		//总重

		map.SetValueAt(index, 7, COleVariant(pPart->sNotes));//备注
		index++;
	}
	CExcelOper::MergeRowRange(excel_sheet, "E4", "E5");
	CExcelOper::MergeRowRange(excel_sheet, "F4", "F5");
	CExcelOper::MergeRowRange(excel_sheet, "G4", "G5");
	CExcelOper::MergeRowRange(excel_sheet, "H4", "H5");
	CExcelOper::SetMainTitle(excel_sheet, "I4", "J4", "重量(kg)", 20, 11, 2, 2, false);
	CExcelOper::MergeRowRange(excel_sheet, "K4", "K5");
	char cell_end[15] = " ";
	//设置右对齐
	sprintf(cell_end, "K%d", i + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)4));
	//设置表格内容
	sprintf(cell_end, "K%d", i + 5);
	CExcelOper::SetRangeValue(excel_sheet, "D6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 13.50, "D6", cell_end);
	//设置边框
	//CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(11.0));
	//CExcelOper::SetFontSize(excel_sheet, "A1", "L1", 14, true);
	CExcelOper::SetFontSize(excel_sheet, "D4", cell_end, 11);
	excel_sheet.DetachDispatch();
}

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	//1.获取BOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	//2.生成ＥＸＣＥＬ材料表
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//2.1 构件明细表
	AddPartSpecTblSheet(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}