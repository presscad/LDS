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

static CXhChar100 GetProcessNotes(BOMPART* pBomPart, bool bIncKaiHeAngle = TRUE)
{
	CXhChar100 notes;
	long nHuoquLineCount = pBomPart->GetHuoquLineCount();
	if (pBomPart->cPartType == BOMPART::PLATE)
		nHuoquLineCount = ((PART_PLATE*)pBomPart)->m_cFaceN - 1;
	if (nHuoquLineCount > 0)	//是否需要制弯
		notes.Append("制弯", ' ');
	if (pBomPart->bWeldPart)
		notes.Append("焊接", ' ');
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
		if (pPlate->bNeedFillet)		//需要坡口
			notes.Append("坡口", ' ');
		for (LIST_NODE<BOMPROFILE_VERTEX> *pNode = pPlate->listVertex.EnumFirst(); pNode; pNode = pPlate->listVertex.EnumNext())
		{
			if (pNode->data.m_bRollEdge)
			{
				notes.Append(CXhChar16("制弯 卷边%dmm", abs(pNode->data.manu_space)), ' ');
				break;
			}
		}
	}
	else if (pBomPart->cPartType == BOMPART::TUBE)
	{
		PART_TUBE *pTube = (PART_TUBE*)pBomPart;
		if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("坡口", ' ');
		else if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD ||
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("坡口", ' ');
	}
	else if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
		if (pBomAngle->bCutAngle)		//是否切角
			notes.Append("切角", ' ');
		if (pBomAngle->bCutBer)			//是否需要铲背
			notes.Append("铲背", ' ');
		if (pBomAngle->bCutRoot)		//是否需要清根
			notes.Append("清根", ' ');
		if (pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle) > 0)		//是否需要开角
		{
			if (bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
				notes.Append(CXhChar16("开角(%.1f°)", pBomAngle->wing_angle - 90), ' ');
			else
				notes.Append("开角", ' ');
		}
		if (pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle) > 0)			//是否需要合角
		{
			if (bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
				notes.Append(CXhChar16("合角(%.1f°)", 90 - pBomAngle->wing_angle), ' ');
			else
				notes.Append("合角", ' ');
		}
		if (pBomAngle->nPushFlat > 0)		//是否压扁
		{
			if (pBomAngle->nPushFlat == 0x02)
				notes.Append("中间压扁", ' ');
			else
				notes.Append("压扁", ' ');
		}
	}
	return notes;
}

static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec = pPart->GetSpec();
	if (GetSupportDataBufferVersion() >= 2)
	{
		if (pPart->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%d",(int)pPart->thick);
		else if (pPart->cPartType == BOMPART::ANGLE)
			sSpec.Printf("L%.fx%.f", pPart->wide, pPart->thick);
		PART_PLATE *pPlate = (PART_PLATE*)pPart;
		if (pPlate->IsFL() || pPlate->siSubType == BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("Φ%.f/%.f", pPlate->fFlOutterD, pPlate->fFlInnerD);
	}
	return sSpec;
}


void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("总表");
	CHashList<SEGI> segNoHashList;
	for (SUPERLIST_NODE<BOMPART> *pBomPart = bom.listParts.EnumFirst(); pBomPart; pBomPart = bom.listParts.EnumNext())
	{
		if (segNoHashList.GetValue(pBomPart->pDataObj->iSeg)==NULL)
			segNoHashList.SetValue(pBomPart->pDataObj->iSeg,SEGI(pBomPart->pDataObj->iSeg));
	}
	//添加实时数据的列标题
	CExcelOper::SetMainTitle(excel_sheet, "A1", "L1", "车间工序加工件-总表", 28.5);
	char strTile[500] = "";
	CString strTile1;
	sprintf(strTile, "工程名称：%s", (char*)bom.m_sPrjName);
	CExcelOper::SetMainTitle(excel_sheet, "A2", "L2", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "塔型：%s", (char*)bom.m_sTowerTypeName);
	CExcelOper::SetMainTitle(excel_sheet, "A3", "C3", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "基数：%d基", bom.m_nTaNum);
	CExcelOper::SetMainTitle(excel_sheet, "D3", "H3", strTile, 20, 14, 3, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I3", "L3", "材料标准：国网标准", 20, 14, 3, 2, false);
	sprintf(strTile, "加工段数：%s", (char*)SegNoHashListToString(segNoHashList));
	CExcelOper::SetMainTitle(excel_sheet, "A4", "L4", strTile, 20, 14, 2, 2, false);
	//2.2 输出材料表
	//2.2.1 添加标题栏
	const int COL_COUNT = 12;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "件号";  str_arr[1] = "材质"; str_arr[2] = "规格"; str_arr[3] = "长度(mm)"; str_arr[4] = "宽度";
	str_arr[5] = "单段数"; str_arr[6] = "单基数"; str_arr[7] = "基数"; str_arr[8] = "总数"; str_arr[9] = "单重(kg)";	str_arr[10] = "总重(kg)";  str_arr[11] = "备注";
	double col_arr[COL_COUNT] = { 8.0,7.0,8.5,9.0,5.63,
								8,8,5.0,5.0,9.0,9.0,15.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet, 33, "A5", "L5");
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
		map.SetValueAt(index, 0, COleVariant(pPart->sPartNo));		//件号
		CXhChar16 sMat = QuerySteelMatMark(pPart->cMaterial);
		if (pPart->cMaterial == 'S')
			sMat.Empty();
		else if (sMat.GetLength() == 0)
			sMat.Copy(pPart->sMaterial);
		map.SetValueAt(index, 1, COleVariant(sMat));				//材质
		map.SetValueAt(index, 2, COleVariant(GetPartSpec(pPart)));	//规格
		map.SetValueAt(index, 3, COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index, 4, COleVariant(ftol(pPart->wide)));	//宽度
		map.SetValueAt(index, 5, COleVariant(pPart->GetPartNum()));	//单段数量
		map.SetValueAt(index, 6, COleVariant(pPart->GetPartNum()));	//单基数量

		map.SetValueAt(index, 7, COleVariant((long)bom.m_nTaNum));//基数
		map.SetValueAt(index, 8, COleVariant(CXhChar16("=G%d*H%d", index + 6, index + 6)));	//数量
		double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
		str.Format("%.2f", weight);
		map.SetValueAt(index, 9, COleVariant(str));					//单件重量
		double sum_weight = ftol(pPart->fPieceWeight*pPart->GetPartNum() * 100) / 100.0;
		str.Format("%.2f", sum_weight);
		map.SetValueAt(index, 10, COleVariant(str));		//总重
		
		map.SetValueAt(index, 11, COleVariant(GetProcessNotes(pPart)));
		index++;
	}
	char cell_end[15] = " ";
	//设置右对齐
	sprintf(cell_end, "L%d", i + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)3));
	//设置表格内容
	sprintf(cell_end, "L%d", i + 5);
	CExcelOper::SetRangeValue(excel_sheet, "A6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 21.75, "A2", cell_end);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(13.0));
	CExcelOper::SetFontSize(excel_sheet, "A1", "L1", 14,true);
	CExcelOper::SetFontSize(excel_sheet, "A2", "L4", 14);
	excel_sheet.DetachDispatch();
}

void AddPartGroupSumMatTbl(CModelBOM &bom, LPDISPATCH pWorkSheet)
{

	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet, FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("组焊明细表(单基）");
#endif

	CHashList<SEGI> segNoHashList;
	CHashStrList<BOMPART*>hashPartPtrByPartNo;
	for (SUPERLIST_NODE<BOMPART> *pBomPart = bom.listParts.EnumFirst(); pBomPart; pBomPart = bom.listParts.EnumNext())
	{
		if (segNoHashList.GetValue(pBomPart->pDataObj->iSeg) == NULL)
			segNoHashList.SetValue(pBomPart->pDataObj->iSeg, SEGI(pBomPart->pDataObj->iSeg));
		hashPartPtrByPartNo.SetValue(pBomPart->pDataObj->sPartNo, pBomPart->pDataObj);
	}
	//添加实时数据的列标题
	CExcelOper::SetMainTitle(excel_sheet, "A1", "K1", "车间工序加工件-组焊明细表", 28.5);
	char strTile[500]="";
	CString strTile1;
	//sprintf(strTile, "%.*s%.*s", sizeof("工程名称："), "工程名称：", sizeof(bom.m_sPrjName), bom.m_sPrjName);
	sprintf(strTile, "工程名称：%s", (char*)bom.m_sPrjName);
	CExcelOper::SetMainTitle(excel_sheet, "A2", "K2", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "塔型：%s", (char*)bom.m_sTowerTypeName);
	CExcelOper::SetMainTitle(excel_sheet, "A3", "C3", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "基数：%d基", bom.m_nTaNum);
	CExcelOper::SetMainTitle(excel_sheet, "D3", "H3", strTile, 20, 14, 3, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I3", "K3", "材料标准：国网标准", 20, 14, 3, 2, false);
	sprintf(strTile, "加工段数：%s", (char*)SegNoHashListToString(segNoHashList));
	CExcelOper::SetMainTitle(excel_sheet, "A4", "K4", strTile, 20, 14, 2, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I5", "J5", "", 33.0);
	//统计行数
	int nRowNum = 5;
	LIST_NODE<BOM_WELDPART> *pWeldPart = NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode = NULL;
	for (BOM_WELDPART *pWeldPart = bom.listWeldParts.EnumObjectFirst(); pWeldPart; pWeldPart = bom.listWeldParts.EnumObjectNext())
	{
		for (BOM_PARTREF *pPartRef = pWeldPart->listPartRef.EnumObjectFirst(); pPartRef; pPartRef = pWeldPart->listPartRef.EnumObjectNext())
			nRowNum++;
	}
	const int COL_COUNT = 11;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "组焊编号";  str_arr[1] = "件号"; str_arr[2] = "材质"; str_arr[3] = "规格"; str_arr[4] = "料长";
	str_arr[5] = "件数"; str_arr[6] = "单重"; str_arr[7] = "总重"; str_arr[8] = "备注"; 	str_arr[10] = "组焊数量"; 
	double col_arr[COL_COUNT] = { 8.5,7.5,7.5,10.75,8.38,
								8.38,8.38,8.38,8.38,5.0,
								8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
	//2.2.2 输出材料表
	int i = 0;
	int index = 0;
	CString str;
	CVariant2dArray map(nRowNum, COL_COUNT);
	for (BOM_WELDPART *pWeldPart = bom.listWeldParts.EnumObjectFirst(); pWeldPart; pWeldPart = bom.listWeldParts.EnumObjectNext())
	{
		char merge_start[15] = "", merge_end[15] = "";
		int nCount = 0;
		double weightCount=0;
		map.SetValueAt(index, 0, COleVariant(pWeldPart->sPartNo));			//组焊编号
		map.SetValueAt(index, 10, COleVariant((long)pWeldPart->nWeldPartNum));			//组焊数量
		for (BOM_PARTREF *pPartRef = pWeldPart->listPartRef.EnumObjectFirst(); pPartRef; pPartRef = pWeldPart->listPartRef.EnumObjectNext(), index++)
		{
			nCount++;
			BOMPART **ppPart = hashPartPtrByPartNo.GetValue(pPartRef->sPartNo);
			BOMPART *pPart = ppPart ? *ppPart : NULL;
			if (pPart==NULL)
				continue;
			map.SetValueAt(index, 1, COleVariant(pPart->sPartNo));		//件号
			CXhChar16 sMat = QuerySteelMatMark(pPart->cMaterial);
			if (pPart->cMaterial == 'S')
				sMat.Empty();
			else if (sMat.GetLength() == 0)
				sMat.Copy(pPart->sMaterial);
			map.SetValueAt(index, 2, COleVariant(sMat));				//材质
			map.SetValueAt(index, 3, COleVariant(GetPartSpec(pPart)));	//规格
			map.SetValueAt(index, 4, COleVariant(ftol(pPart->length)));	//长度
			int nPartRefNum = pPartRef->nPartNum;
			map.SetValueAt(index, 5, COleVariant((long)nPartRefNum));	//件数
			double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
			str.Format("%.2f", weight);
			map.SetValueAt(index, 6, COleVariant(str));					//单件重量
			double sum_weight = ftol(pPart->fPieceWeight*nPartRefNum * 100) / 100.0;
			str.Format("%.2f", sum_weight);
			map.SetValueAt(index, 7, COleVariant(str));		//总重
			weightCount += sum_weight;
			sprintf(merge_start, "I%d", index + 6);
			sprintf(merge_end, "J%d", index + 6);
			CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		}
		map.SetValueAt(index, 1, COleVariant("总计"));
		sprintf(merge_start, "B%d", index + 6);
		sprintf(merge_end, "D%d", index + 6);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		str.Format("%.2f", weightCount);
		map.SetValueAt(index, 4, COleVariant(str));
		sprintf(merge_start, "E%d", index + 6);
		sprintf(merge_end, "J%d", index + 6);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		sprintf(merge_start, "A%d", index + 6 - nCount); sprintf(merge_end, "A%d", index + 5);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		sprintf(merge_start, "K%d", index + 6 - nCount); sprintf(merge_end, "K%d", index + 5);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);

		index++;
	}
	char cell_end[15] = "";
	//设置左对齐
	sprintf(cell_end, "K%d", index + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)3));
	//设置表格内容
	sprintf(cell_end, "K%d", index + 5);
	CExcelOper::SetRangeValue(excel_sheet, "A6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 21.75, "A2", cell_end);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(13.0));
	CExcelOper::SetFontSize(excel_sheet, "A1", "K1", 14,true);
	CExcelOper::SetFontSize(excel_sheet, "A2", "K4", 14);
	CExcelOper::SetFontSize(excel_sheet, "J5", "J5", 13);
	//
	excel_sheet.ReleaseDispatch();
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
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//2.1 构件明细表
	AddPartSpecTblSheet(bom, pWorksheet);
	//2.3 组焊件明细表
	pWorksheet = excel_sheets.GetItem(COleVariant((short)2));
	AddPartGroupSumMatTbl(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}