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
#include "XhCharString.h"

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

char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}

const int COL_COUNT=11;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=5;	//件号
		colIndexArr[1]=2;	//规格
		colIndexArr[2]=1;	//材质
		colIndexArr[3]=4;	//数量
		colIndexArr[4]=6;	//单重
		colIndexArr[5]=3;	//长度
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//构件处理
CXhChar16 __QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if ('H' == cMat)
		sMatMark.Copy("Q345");
	else if ('G' == cMat)
		sMatMark.Copy("Q390");
	else if ('P' == cMat)
		sMatMark.Copy("Q420");
	else if ('T' == cMat)
		sMatMark.Copy("Q460");
	else if ('S' == cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
CXhChar100 GetBomPartSpec(BOMPART *pBomPart, char* material=NULL)
{
	CXhChar100 sSpec,sSpecialPartSpec;
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			if(cMaterial=='D')
				cMaterial='P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING||
				pBomPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE||
				pBomPart->siSubType==BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//钢格栅
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//拉索构件
			//const static BYTE SUB_TYPE_LADDER		= 20;	//爬梯
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'? __QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("L%.fX%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("Φ%.fX%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
			sSpec.Printf("-%.fX%.f",pBomPart->thick,pBomPart->wide);
		else 
			sSpec.Printf("%s",(char*)pBomPart->sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C','[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits=sSpec.Replace("φ"," ");
		hits+=sSpec.Replace("Φ"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		if(hits==2)
		{
			int D=0,d=0;
			sscanf(sSpec,"%d%d",&D,&d);
			sSpec.Printf("φ%d/%d",D,d);
		}
		else 
			sSpec.Copy(pBomPart->sSpec);
	}
	if (material != NULL)
		strcpy(material, materialStr);
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else //if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	/*else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);*/
}
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
	//
	CStringArray str_arr;
	const int COL_COUNT = 7;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "材质"; str_arr[1] = "规格"; str_arr[2] = "长度"; str_arr[3] = "数量";
	str_arr[4] = "编号"; str_arr[5] = "单重"; str_arr[6] = "总重"; 
	double col_arr[7] = { 10.5,11.5,11.5,9.5,
						  10.0,10.0,12.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 1, str_arr, col_arr, TRUE);
	//
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//获取Excel表格的范围
	ARRAY_LIST<int> Q345RowIndexArr, Q420RowIndexArr;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		CXhChar16 sMaterial;
		CXhChar100 sSpec=GetBomPartSpec(pBomPart,sMaterial);
		map.SetValueAt(index, 0, COleVariant(sMaterial));
		map.SetValueAt(index, 1, COleVariant(sSpec));
		if (pBomPart->length > 0)
		{
			if (pBomPart->cPartType == BOMPART::ACCESSORY&&pBomPart->siSubType == BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index, 2, COleVariant(CXhChar50("%.fX%.f", pBomPart->wide, pBomPart->length)));
			else
				map.SetValueAt(index, 2, COleVariant(CXhChar50("%.f", pBomPart->length)));
		}
		else
			map.SetValueAt(index, 2, COleVariant(""));
		map.SetValueAt(index, 3, COleVariant((long)pBomPart->GetPartNum()));
		if(sSpec.EqualNoCase(pBomPart->sPartNo))
			map.SetValueAt(index,4,COleVariant(""));
		else
			map.SetValueAt(index,4,COleVariant(pBomPart->sPartNo));
		map.SetValueAt(index, 5, COleVariant(CXhChar50("%.2f", pBomPart->fPieceWeight)));
		map.SetValueAt(index, 6, COleVariant(CXhChar50("%.1f", pBomPart->fPieceWeight*pBomPart->GetPartNum())));
		if (sMaterial.EqualNoCase("Q345"))
			Q345RowIndexArr.append(index + 2);
		if (sMaterial.EqualNoCase("Q420"))
			Q420RowIndexArr.append(index + 2);
		index++;
		
	}
	CXhChar16 cell_end("G%d", index+1);
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(14.0));
	CExcelOper::SetRangeValue(excel_sheet, "A2", cell_end, map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A1", cell_end, COleVariant((long)3));

	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A2", CXhChar16("A%d",index), COleVariant((long)2));
	for (int i = 0; i < Q345RowIndexArr.GetSize(); i++)
	{
		int rowIndex = Q345RowIndexArr[i];
		CXhChar16 curCell("A%d", rowIndex);
		//CExcelOper::SetFontSize(excel_sheet, curCell, curCell, 0, true);
		//CExcelOper::SetRangeRowHeight(excel_sheet, 25.5, curCell, NULL, TRUE);
		CExcelOper::SetFontColor(excel_sheet, curCell, NULL, CExcelOper::COLOR_ID_RED);
		CExcelOper::SetBoldFont(excel_sheet, curCell, NULL);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, curCell, NULL, COleVariant((long)4));
	}
	for (int i = 0; i < Q420RowIndexArr.GetSize(); i++)
	{
		int rowIndex = Q420RowIndexArr[i];
		CXhChar16 curCell("A%d", rowIndex);
		//CExcelOper::SetFontSize(excel_sheet, curCell, curCell, 0, true);
		//CExcelOper::SetRangeRowHeight(excel_sheet, 25.5, curCell, NULL, TRUE);
		CExcelOper::SetFontColor(excel_sheet, curCell, NULL, CExcelOper::COLOR_ID_BLUE);
		CExcelOper::SetBoldFont(excel_sheet, curCell, NULL);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, curCell, NULL, COleVariant((long)4));
	}
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