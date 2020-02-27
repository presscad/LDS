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
#include "SegI.h"

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
const int COL_COUNT=8;
//索引号从1开始，0表示没有对应列
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=6;
		colIndexArr[0]=1;	//件号
		colIndexArr[1]=3;	//规格
		colIndexArr[2]=2;	//材质
		colIndexArr[3]=5;	//数量
		colIndexArr[4]=0;	//单重
		colIndexArr[5]=4;	//长度
		colIndexArr[6]=0;	//宽度
		colIndexArr[7]=0;	//类型
		return 8;
	}
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

//////////////////////////////////////////////////////////////////////////
//构件处理
static void GetAngleSimpleSpec(char *simple_spec,double x_wing_wide,double y_wing_wide,double wide_thick)
{
	char sSpec[200]="";
	char sWide[100]="",sThick[100]="";
	sprintf(sWide,"%s",(char*)CXhChar16(x_wing_wide));
	sprintf(sThick,"%s",(char*)CXhChar16(wide_thick));
	if(IsUnequalAngle(x_wing_wide,y_wing_wide,wide_thick))
	{
		char sWideY[100]="";
		sprintf(sWideY,"%s",(char*)CXhChar16(y_wing_wide));
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%.fx%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Remove('[');
		sSpec.Remove('C');
	}
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if( pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C||	//槽型插板
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||	//U型插板
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||	//十字插板
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||	//对焊法兰
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||	//平焊法兰
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||	//刚性法兰
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)	//柔性法兰
			sSpec.Copy("");		//标准构件不需要显示规格
		else //if(pBomPart->siSubType==0)
		{
			if(bNeedPlateW)
				sSpec.Printf("%.fx%.f",pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("%.f",pBomPart->thick);
		}
	}
	else 
		sSpec.Printf("%s",(char*)pBomPart->sSpec);
	return sSpec;
}

static char GetPartMaterialChar(BOMPART *pBomPart)
{
	if (pBomPart == NULL)
		return '0';
	char cMaterial = pBomPart->cMaterial;
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD || pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 9) ? pBomPart->sPartNo.At(8) : '0';
			if (cMaterial == 'D')
				cMaterial = 'P';
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 6) ? pBomPart->sPartNo.At(5) : '0';
		}
		//会导致螺栓垫板材质错误 wht 18-12-30
		/*else if(pBomPart->siSubType!=0)
		{
			cMaterial='0';
		}*/
	}
	return cMaterial;
}

CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
{
	CXhChar100 sSpec, sSpecialPartSpec;
	char cMaterial = pBomPart->cMaterial;
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD || pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 9) ? pBomPart->sPartNo.At(8) : '0';
			if (cMaterial == 'D')
				cMaterial = 'P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 6) ? pBomPart->sPartNo.At(5) : '0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_STEEL_GRATING ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_STAY_ROPE ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//钢格栅
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//拉索构件
			//const static BYTE SUB_TYPE_LADDER		= 20;	//爬梯
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial = '0';
		}
	}
	CXhChar16 materialStr = cMaterial != '0' ? QuerySteelMatMark(cMaterial) : "";
	if (pBomPart->sPartNo.EqualNoCase("T2105") ||
		pBomPart->sPartNo.EqualNoCase("T2605") ||
		pBomPart->sPartNo.EqualNoCase("T3605") ||
		pBomPart->sPartNo.EqualNoCase("T3980") ||
		pBomPart->sPartNo.EqualNoCase("T5855") ||
		pBomPart->sPartNo.EqualNoCase("T6355") ||
		pBomPart->cPartType == BOMPART::ACCESSORY)
	{
		materialStr = "";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if (pBomPart->cPartType == BOMPART::ANGLE)
		sSpec.Printf("L%.f*%.f", pBomPart->wide, pBomPart->thick);
	else if (pBomPart->cPartType == BOMPART::TUBE)
		sSpec.Printf("Φ%.f*%.f", pBomPart->wide, pBomPart->thick);
	else if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == 0 || pBomPart->siSubType == BOMPART::SUB_TYPE_BOLT_PAD)
		{
			if (pBomPart->wide > pBomPart->length)
			{
				double len = pBomPart->length;
				pBomPart->length = pBomPart->wide;
				pBomPart->wide = len;
			}
			sSpec.Printf("-%.f*%.f", pBomPart->thick,pBomPart->wide);
		}
		else
			sSpec.Printf("%s", (char*)pBomPart->sSpec);
	}
	else if (pBomPart->cPartType == BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C', '[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits = sSpec.Replace("φ", " ");
		hits += sSpec.Replace("Φ", " ");
		sSpec.Replace("/", " ");
		sSpec.Replace("\\", " ");
		if (hits == 2)
		{
			int D = 0, d = 0;
			sscanf(sSpec, "%d%d", &D, &d);
			sSpec.Printf("φ%d/%d", D, d);
		}
		else
			sSpec.Copy(pBomPart->sSpec);
	}
	if (sSpecialPartSpec.GetLength() > 0)
		return sSpecialPartSpec;
	else
		return sSpec;
	/*else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
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
	CStringArray str_arr;
	const int COL_COUNT=6;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "段号"; str_arr[1]="件号";str_arr[2]="材质";
	str_arr[3]="规格";str_arr[4]="长度";str_arr[5]="件数";
	double col_arr[6]={8.5,8.5,8.5,8.5,8.5,8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//获取Excel表格的范围
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if (pBomPart->iSeg == -1)
		{
			SEGI segI;
			if (ParsePartNo(pBomPart->sPartNo, &segI, NULL))
				pBomPart->iSeg = segI;
		}
		char cMaterial = GetPartMaterialChar(pBomPart);
		CXhChar50 sSpec = GetBomPartSpec(pBomPart);
		map.SetValueAt(index, 0, COleVariant(SEGI(pBomPart->iSeg).ToString()));
		map.SetValueAt(index, 1, COleVariant(pBomPart->sPartNo));
		map.SetValueAt(index, 2, COleVariant(QuerySteelMatMark(cMaterial)));	//材质
		map.SetValueAt(index, 3, COleVariant(sSpec));
		map.SetValueAt(index, 4, COleVariant(CXhChar50("%.f", pBomPart->length)));
		map.SetValueAt(index, 5, COleVariant((long)pBomPart->GetPartNum()));
		index++;
	}
	CXhChar16 cell_end("F%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"B2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"B2",cell_end,COleVariant(10.5));
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