// BomExport.cpp : 定义 DLL 的初始化例程。
//螺栓明细表(青岛东方格式基础上改进)

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "SortFunc.h"
#include "ArrayList.h"
#include "HashTable.h"
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
//version：1.第一个版本
//		   2.湖州飞剑钢管部分，版本升级为2
//		   3.修改螺栓信息，版本升级为3
TOWER_BOM_EXPORTS_API DWORD GetSupportDataBufferVersion()
{
	return 11;
}
const int BOM_SHEET_PART		= 0x01;	//构件明细表
const int BOM_SHEET_BOLT		= 0x02;	//螺栓明细表
const int BOM_SHEET_WELDPART	= 0x04;	//组焊件明细表
TOWER_BOM_EXPORTS_API DWORD GetSupportBOMType()
{
	return BOM_SHEET_BOLT;
}

#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

static CXhChar50 GetBoltConfig(BOMBOLT& bomBolt)
{
	CXhChar50 sCfg;
	BYTE nCount=bomBolt.nAntiTheftNut+bomBolt.nCommonGasket+bomBolt.nCommonNut+bomBolt.nFastenNut+
				bomBolt.nLockClasp + bomBolt.nThinNut + bomBolt.nSpringGasket;
	if(bomBolt.IsDianQuan())
		sCfg.Copy("平垫圈");
	else if(nCount>0)
	{
		if (bomBolt.bFootNail)
			sCfg.Copy("脚钉配");
		if (bomBolt.nCommonNut > 0)
			sCfg.Append(CXhChar16("%d螺母", bomBolt.nCommonNut));
		if (bomBolt.nThinNut > 0)
			sCfg.Append(CXhChar16("%d薄螺母", bomBolt.nThinNut));
		if (bomBolt.nAntiTheftNut > 0)
			sCfg.Append(CXhChar16("%d防卸螺母", bomBolt.nAntiTheftNut));
		if (bomBolt.nLockClasp > 0)
			sCfg.Append(CXhChar16("%d防松扣", bomBolt.nLockClasp));
		if (bomBolt.nSpringGasket > 0)
			sCfg.Append(CXhChar16("%d弹簧垫圈", bomBolt.nSpringGasket));
		if (bomBolt.nCommonGasket > 0)
			sCfg.Append(CXhChar16("%d平垫圈", bomBolt.nCommonGasket));
		if (bomBolt.nFastenNut > 0)
			sCfg.Append(CXhChar16("%d防松扣紧螺母", bomBolt.nFastenNut));
	}
	else
	{
		if (bomBolt.bFootNail)
		{
			if (bomBolt.bAntiTheft)
				sCfg.Copy("脚钉配2螺母1防卸螺母1平垫圈");
			else if (bomBolt.bAnitLoose)
				sCfg.Copy("脚钉配2螺母1防松扣紧螺母1平垫圈");
			else
				sCfg.Copy("脚钉配3螺母1平垫圈");
		}
		else
		{
			if (bomBolt.bAntiTheft)
				sCfg.Copy("1螺母1防卸螺母1平垫圈");
			else if (bomBolt.bAnitLoose)
				sCfg.Copy("1螺母1平垫圈1防松扣紧螺母");
			else if (bomBolt.bTwoCap)
				sCfg.Copy("2螺母1平垫圈");
			else
				sCfg.Copy("1螺母1平垫圈");
		}
	}
	return sCfg;
}
static void InitBomBoltByBoltConfig(BOMBOLT &bomBolt,const char* bolt_cfg)
{

}

CModelBOM g_xBom;
const int TOWER_BOM_EXCEL_COL_COUNT			= 10;

const char* TITLE_BOLT_SEGI				    = "段别";
const char* TITLE_BOLT_GRADE				= "级别";
const char* TITLE_BOLT_GUIGE				= "规格";
const char* TITLE_BOLT_NO_THREAD_LEN 		= "无扣长";	
const char* TITLE_BOLT_NUM				    = "数量";
const char* TITLE_BOLT_CFG				    = "螺栓配置";
const char* TITLE_BOLT_NOTES				= "备注";
//
const char* TYPE_BOLT				        = "螺栓";
const char* TYPE_TOW_CAP_BOLT				= "双帽螺栓";
const char* TYPE_DIAN_QUAN				    = "垫圈";
const char* TYPE_DIAN_PIAN				    = "垫片";
const char* TYPE_U_BOLT				        = "U型环";
const char* TYPE_FOOT_NAIL				    = "脚钉";

static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}

TOWER_BOM_EXPORTS_API void ImportExcelBomSegFile(const char* file_path)
{
	CVariant2dArray sheetContentMap(1,1);
	if(!CExcelOper::GetExcelContentOfSpecifySheet(file_path,sheetContentMap,1))
		return;
	CXhSimpleList<BOMBOLT> boltList;
	//获取列名
	VARIANT value;
	value.intVal=0;
	CHashStrList<DWORD> hashColIndexByColTitle;
	for(int i=1;i<=TOWER_BOM_EXCEL_COL_COUNT;i++)
	{
		sheetContentMap.GetValueAt(0,i,value);
		if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_SEGI)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_SEGI,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_GUIGE)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_GUIGE,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NO_THREAD_LEN)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NO_THREAD_LEN,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_GRADE)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_GRADE,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NUM)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NUM,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_CFG)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_CFG,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NOTES)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NOTES,i);
	}
	// 获取excel 值
	int nCount=0;
	VARIANT valueBoltGuiGe,valueBoltNoThreadLen,valueGrade;
	for(int i = 1; i < sheetContentMap.RowsCount(); i++)
	{
		BOMBOLT bomBolt;
		DWORD *pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_GUIGE);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueBoltGuiGe);
			CXhChar100 sGuiGe=VariantToString(valueBoltGuiGe);
			sGuiGe.Replace("*"," ");
			sGuiGe.Replace("x"," ");
			sGuiGe.Replace("X"," ");
			sGuiGe.Replace("×"," ");
			sGuiGe.Replace("M"," ");
			sGuiGe.Replace("φ"," ");
			sGuiGe.Replace("-"," ");
			int d=0,l=0;
			sscanf((char*)sGuiGe,"%d %d",&d,&l);
			bomBolt.d=d;
			bomBolt.L=l;
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NO_THREAD_LEN);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueBoltNoThreadLen);
			bomBolt.nNoThreadlen=atoi(VariantToString(valueBoltNoThreadLen));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_GRADE);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.grade=(float)atof(VariantToString(valueGrade));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_SEGI);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.iSeg=SEGI(VariantToString(valueGrade)).Digital();
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NUM);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_CFG);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			//bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NOTES);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			//bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		boltList.AttachObject(bomBolt);
		nCount++;
	}
	g_xBom.m_nBoltsN=nCount;
	if(g_xBom.m_nBoltsN>0)
	{
		int i=0;
		g_xBom.m_pBoltRecsArr = new BOMBOLT[g_xBom.m_nBoltsN];
		for(BOMBOLT* pBomBolt=boltList.EnumObjectFirst();pBomBolt;pBomBolt=boltList.EnumObjectNext(),i++)
			g_xBom.m_pBoltRecsArr[i]=*pBomBolt;
	}
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	//1.获取BOM
	CModelBOM bom, *pBOM = &bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	else
		pBOM = &g_xBom;
	CBubbleSort<BOMBOLT>::BubSort(pBOM->m_pBoltRecsArr, pBOM->m_nBoltsN, CompareBomBoltIncSegI);
	//2.生成ＥＸＣＥＬ材料表
	//2.1 创建Excel工作薄
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	if (pWorksheet)
	{
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet, FALSE);
		excel_sheet.Select();
		excel_sheet.SetName("搭配表");
	}
	pWorksheet = excel_sheets.GetItem(COleVariant((short)2));
	if(pWorksheet)
	{
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet, FALSE);
		excel_sheet.Select();
		excel_sheet.SetName("螺栓明细表");
		/*bom.m_sTaAlias含有特殊字符(空格,-,\)，导致excel_sheet.SetName死机
		if (bom.m_sTaAlias.GetLength() > 0)
			excel_sheet.SetName(CXhChar200("%s螺栓明细",(char*)bom.m_sTaAlias));
		else
			excel_sheet.SetName("螺栓分段明细表");
		*/
		//2.2 输出材料表
		CStringArray str_arr;
		str_arr.SetSize(9);
		str_arr[0] = "序号"; str_arr[1] = "段号"; str_arr[2] = "级别"; str_arr[3] = "材质";
		str_arr[4] = "规格"; str_arr[5] = "无扣长"; str_arr[6] = "数量"; str_arr[7] = "螺栓配置";
		str_arr[8] = "备注";
		double col_arr[9] = { 5.0,5.0,5.0,5.0,8.0,7.0,5.0,28.0,10.0 };
		CExcelOper::AddRowToExcelSheet(excel_sheet, 1, str_arr, col_arr, TRUE);
		//2.2.2 输出材料表
		int i = 0, nCount = 1;
		int index = 0;
		CString str;
		CVariant2dArray map(pBOM->m_nBoltsN, 9);
		for (i = 0; i < pBOM->m_nBoltsN; i++)
		{
			BOMBOLT bolt = pBOM->m_pBoltRecsArr[i];
			map.SetValueAt(index, 0, COleVariant((long)(i + 1)));
			//段号
			map.SetValueAt(index, 1, COleVariant((long)bolt.iSeg));
			//螺栓等级
			CXhChar16 sGrade(bolt.grade);
			map.SetValueAt(index, 2, COleVariant(sGrade));
			//材质
			map.SetValueAt(index, 3, COleVariant(""));
			// 规格
			CXhChar200	sGuiGe("M");
			sGuiGe.Append(CXhChar16(bolt.d));
			if (bolt.IsDianQuan())
				sGuiGe.Append("垫圈");
			else
			{
				sGuiGe.Append("*");
				sGuiGe.Append(CXhChar16(bolt.L));
			}
			map.SetValueAt(index, 4, COleVariant(sGuiGe));
			//无扣长（lg）
			if (!bolt.IsDianQuan())
				map.SetValueAt(index, 5, COleVariant((long)bolt.nNoThreadlen));
			//数量
			map.SetValueAt(index, 6, COleVariant((long)bolt.GetPartNum()));
			//螺栓配置
			map.SetValueAt(index, 7, COleVariant(GetBoltConfig(bolt)));
			index++;
		}
		CXhChar16 sCell = "I";
		sCell.Append(CXhChar16(index + 1));
		//设置右对齐
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A1", sCell, COleVariant((long)3));
		//设置表格内容
		CExcelOper::SetRangeValue(excel_sheet, "A2", sCell, map.var);
		//设置边框
		CExcelOper::SetRangeBorders(excel_sheet, "A1", sCell, COleVariant(10.5));
		excel_sheet.ReleaseDispatch();
	}
	return 0;
}

int CreateExcelBoltSummaryBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	/*CXhChar500 sTempPath;
	::GetModuleFileName(theApp.m_hInstance, sTempPath, 500);
	sTempPath.Replace("BomExport.dll", "青岛东方螺栓质量计算标准.xlsx");
	BOOL bBoltLib = TRUE;
	CBoltLibSet boltLibSet;
	if (!PathFileExists(sTempPath))
	{
		MessageBox(NULL, CXhChar500("未找到模板文件,将按默认重量生成表格。模板路径：%s", (char*)sTempPath), "错误提示", MB_OK);
		bBoltLib = FALSE;
	}
	else
		boltLibSet.ImportBoltLibByExcelFile(sTempPath);*/
	//1.获取BOM
	CModelBOM bom, *pBOM = &bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	else
		pBOM = &g_xBom;
	CBubbleSort<BOMBOLT>::BubSort(pBOM->m_pBoltRecsArr, pBOM->m_nBoltsN, CompareBomBoltIncSegI);
	//2.生成ＥＸＣＥＬ材料表
	//2.1 创建Excel工作薄
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short) 2));
	if(pWorksheet2)
	{
		//重新汇总螺栓不考虑分段
		CHashStrList<BOMBOLT> hashBoltByCombinedKey; //通过类型
		ARRAY_LIST<BOMBOLT> boltArr;
		for (int i=0;i<pBOM->m_nBoltsN;i++)
		{
			BOMBOLT bolt=pBOM->m_pBoltRecsArr[i];
			CXhChar50 sKey=bolt.CombinedKey();
			BOMBOLT* pSumBolt=hashBoltByCombinedKey.GetValue(sKey);
			if(pSumBolt==NULL)
			{
				pSumBolt=hashBoltByCombinedKey.Add(sKey);
				CopyBomBoltProp(bolt,*pSumBolt);
			}
			pSumBolt->AddPart(bolt.GetPartNum());
		}
		for(BOMBOLT *pBolt=hashBoltByCombinedKey.GetFirst();pBolt;pBolt=hashBoltByCombinedKey.GetNext())
		{
			BOMBOLT *pNewBolt=boltArr.append();
			CopyBomBoltProp(*pBolt,*pNewBolt);
			pNewBolt->AddPart(pBolt->GetPartNum());
		}
		CBubbleSort<BOMBOLT>::BubSort(boltArr.m_pData,boltArr.GetSize(),CompareBomBolt);

		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet2,FALSE);
		excel_sheet.Select();
		if(bom.m_sTaAlias.GetLength()>0)
			excel_sheet.SetName(CXhChar200("%s汇总",bom.m_sTaAlias));
		else
			excel_sheet.SetName("汇总表");
		//2.2 输出材料表
		CExcelOper::MergeColRange(excel_sheet,"A1","N1");
		CExcelOper::SetMainTitle(excel_sheet,"A1", "N1","青岛东方铁塔股份有限公司",14.5,14);
		//
		CExcelOper::MergeColRange(excel_sheet,"A2","N2");
		CExcelOper::SetMainTitle(excel_sheet,"A2", "N2",CXhChar200("%s工程 螺栓汇总表",(char*)bom.m_sPrjName),14.5,14);
		
		CStringArray str_arr;
		str_arr.SetSize(16);
		str_arr[0]="序号";str_arr[1]="分区名称";str_arr[2]="分区代号";str_arr[3]="级别";str_arr[4]="材质";
		str_arr[5]="规格型号";str_arr[6]="无扣长";str_arr[7]="数量";str_arr[8]="理重(kg)";str_arr[9]="螺栓配置";
		str_arr[10]="执行标准";str_arr[11]="防腐形式";str_arr[12]="基数";str_arr[13]="备注";str_arr[14]="";str_arr[15]="单重";
		double col_arr[16]={4.5,18.5,8.5,4.5,4.5,7.5,6.5,5.0,7.5,24.5,13.0,7.5,12.5,6.5,8.5,8.5};
		CExcelOper::AddRowToExcelSheet(excel_sheet,3,str_arr,col_arr,TRUE);
		CExcelOper::SetFontSize(excel_sheet,"A3","N3",10.5,TRUE);
		//2.2.2 输出材料表
		int i=0,nCount=0;
		int index=0;
		CVariant2dArray map(boltArr.GetSize()+20,16);
		//
		CHashStrList<BOMBOLT> hashDianQuanByCombinedKey;
		CHashStrList<BOMBOLT> hashLuoMuByCombinedKey;
		for (int i=0;i<boltArr.GetSize();i++)
		{
			BOMBOLT bolt=boltArr[i];
			if(!bolt.IsDianQuan()&&(bolt.bAnitLoose||bolt.bAntiTheft))
			{
				CXhChar50 sKey("%d*%.1f",bolt.d,bolt.grade);
				if(bolt.bAntiTheft)
					sKey.Append("T");
				else
					sKey.Append("L");
				BOMBOLT* pSumBolt=hashLuoMuByCombinedKey.GetValue(sKey);
				if(pSumBolt==NULL)
				{
					pSumBolt=hashLuoMuByCombinedKey.Add(sKey);
					CopyBomBoltProp(bolt,*pSumBolt);
				}
				pSumBolt->AddPart(bolt.GetPartNum());
			}
			if(!bolt.IsDianQuan())
			{
				CXhChar50 sKey("%d*%.1f",bolt.d,bolt.grade);
				BOMBOLT* pSumBolt=hashDianQuanByCombinedKey.GetValue(sKey);
				if(pSumBolt==NULL)
				{
					pSumBolt=hashDianQuanByCombinedKey.Add(sKey);
					CopyBomBoltProp(bolt,*pSumBolt);
				}
				pSumBolt->AddPart(bolt.GetPartNum());
			}
			//序号
			map.SetValueAt(index,0,COleVariant((long)i+1));
			//分区名称
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			//分却代号
			map.SetValueAt(index,2,COleVariant("A1110"));
			//螺栓等级
			CXhChar16 sGrade(bolt.grade);
			map.SetValueAt(index,3,COleVariant(sGrade));
			//材质
			map.SetValueAt(index,4,COleVariant(""));
			// 规格
			CXhChar200 sGuiGe("M");
			sGuiGe.Append(CXhChar16(bolt.d));
			if(bolt.IsDianQuan())
				sGuiGe.Append("垫圈");
			else
			{
				sGuiGe.Append("*");
				sGuiGe.Append(CXhChar16(bolt.L));
			}
			map.SetValueAt(index,5,COleVariant(sGuiGe));
			//
			map.SetValueAt(index,6,COleVariant((long)bolt.nNoThreadlen));//无扣长（lg）
			//数量
			map.SetValueAt(index,7,COleVariant((long)bolt.GetPartNum()));
			// 理重（公斤）
			double fUnitWeight = bolt.fUnitWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.4f",fUnitWeight*bolt.GetPartNum())));
			//
			map.SetValueAt(index,9,COleVariant(GetBoltConfig(bolt)));
			//执行标准
			if(bolt.IsDianQuan())
				map.SetValueAt(index,10,COleVariant("GB/T 95-2000"));
			else
				map.SetValueAt(index,10,COleVariant("GB/T 5780-2000"));
			//防腐形式
			map.SetValueAt(index,11,COleVariant("热镀锌"));
			//基数
			map.SetValueAt(index,12,COleVariant("此为1基，共1基"));
			//单重
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		//添加螺栓
		for (BOMBOLT* pSumBolt=hashDianQuanByCombinedKey.GetFirst();pSumBolt;pSumBolt=hashDianQuanByCombinedKey.GetNext())
		{
			map.SetValueAt(index,0,COleVariant(CXhChar16(index+1)));
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			map.SetValueAt(index,2,COleVariant("A1110"));
			//级别
			map.SetValueAt(index,3,COleVariant(CXhChar16(pSumBolt->grade)));
			//规格
			map.SetValueAt(index,5,COleVariant(CXhChar16("M%d",pSumBolt->d)));
			//
			map.SetValueAt(index,7,COleVariant(long(pSumBolt->GetPartNum())));
			//总重
			double fUnitWeight = pSumBolt->fUnitWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.4f",fUnitWeight*pSumBolt->GetPartNum())));
			map.SetValueAt(index,9,COleVariant("平垫圈"));
			map.SetValueAt(index,10,COleVariant("GB/T 95-2000"));
			map.SetValueAt(index,11,COleVariant("热镀锌"));
			//基数
			map.SetValueAt(index,12,COleVariant("此为1基，共1基"));
			map.SetValueAt(index,13,COleVariant("汇出"));
			//单重
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		for (BOMBOLT* pSumBolt=hashLuoMuByCombinedKey.GetFirst();pSumBolt;pSumBolt=hashLuoMuByCombinedKey.GetNext())
		{
			map.SetValueAt(index,0,COleVariant(CXhChar16(index+1)));
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			map.SetValueAt(index,2,COleVariant("A1110"));
			//级别
			map.SetValueAt(index,3,COleVariant(CXhChar16(pSumBolt->grade)));
			//规格
			map.SetValueAt(index,5,COleVariant(CXhChar16("M%d",pSumBolt->d)));
			//
			map.SetValueAt(index,7,COleVariant(long(pSumBolt->GetPartNum())));
			if(pSumBolt->bAntiTheft)
				map.SetValueAt(index,9,COleVariant("防卸螺母"));
			else if(pSumBolt->bAnitLoose)
				map.SetValueAt(index,9,COleVariant("防松扣紧螺母"));
			if(pSumBolt->bAnitLoose)
				map.SetValueAt(index,10,COleVariant("GB\\T805-88"));
			else
				map.SetValueAt(index,10,COleVariant(""));
			map.SetValueAt(index,11,COleVariant("热镀锌"));
			map.SetValueAt(index,12,COleVariant("此为1基，共1基"));
			map.SetValueAt(index,13,COleVariant("汇出"));
			//单重
			double fUnitWeight=pSumBolt->fUnitWeight;
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		if(index<=0)
			return 0;
		
		//添加最后一行
		CXhChar50 sStartCell("A%d",index+4),sEndCell("B%d",index+4);
		CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
		map.SetValueAt(index,0,COleVariant("合计："));
		map.SetValueAt(index,5,COleVariant("总计"));
		//总数
		map.SetValueAt(index,7,COleVariant(CXhChar16("=SUM(H4:H%d)",index+4-1)));
		//总重
		map.SetValueAt(index,8,COleVariant(CXhChar16("=SUM(I4:I%d)",index+4-1)));
		//
		index++;
		CString sNotes("备注:\n");
		sNotes+="1、防盗螺栓及防盗脚钉需螺栓厂家自行加长，自行加长的长度应同防盗螺母厚度。\n";
		sNotes+="2、防盗螺母厚度等同普通螺母的厚度。\n";
		sNotes+="3、防盗螺栓采用六角滚珠式防盗螺栓。\n";
		sNotes+="4、脚钉形式采用六角墩头直杆全防滑式。\n";
		sNotes+="5、所有螺栓（包括防盗螺栓）的强度等级为热镀锌后的强度值。\n";
		sNotes+="6、防盗螺栓无扣长应和普通螺栓一致，能复紧，同时具有防松性能。\n";
		sNotes+="7、螺栓及脚钉需在其螺头上打印级别字样：6.8级的打印“6.8”，8.8级的打印“8.8”。\n";
		sNotes+="8、螺栓、螺母、扣紧螺母应符合的标准为《六角头螺栓C级》〔GB/T 5780-2000〕、《I型六角螺母》〔GB/T 6170-2000〕、《扣紧螺母》〔GB 805-88〕、垫圈标准应符合《平垫圈C级》〔GB/T 95-2002〕。\n";
		sNotes+="9、螺栓请于    年  月  日前购回\n";
		map.SetValueAt(index,0,COleVariant(sNotes));
		sStartCell.Printf("A%d",index+4),sEndCell.Printf("N%d",index+4);
		CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
		Range excel_range;
		excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
		excel_range.SetRowHeight(COleVariant(175.0));
		index++;
		map.SetValueAt(index,1,COleVariant("编 制："));
		map.SetValueAt(index,4,COleVariant("校 对："));
		map.SetValueAt(index,7,COleVariant("审 核："));
		//
		sEndCell.Printf("N%d",index+4-2);
		//设置居中对齐
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",sEndCell,COleVariant((long)3));
		CXhChar16 sCell1("A%d",index+4-1),sCell2("N%d",index+4);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,sCell1,sCell2,COleVariant((long)2));
		//设置表格内容
		CXhChar16 sContentEndCell("P%d",index+4);
		CExcelOper::SetRangeValue(excel_sheet,"A4",sContentEndCell,map.var);
		//设置边框
		sEndCell.Printf("N%d",index+4-1);
		CExcelOper::SetRangeBorders(excel_sheet,"A1",sEndCell,COleVariant(10.5));
		excel_sheet.ReleaseDispatch();
	}
	return 0;
}
