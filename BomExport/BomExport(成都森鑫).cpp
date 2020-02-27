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
	return 7;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

static int CompareSegiFunc(const SUPERLIST_NODE<BOMPART>& item1,const SUPERLIST_NODE<BOMPART>& item2)
{
	BOMPART* pBomPart1= (BOMPART*)item1.pDataObj;
	BOMPART* pBomPart2= (BOMPART*)item2.pDataObj;
	SEGI seg1(pBomPart1->iSeg),seg2(pBomPart2->iSeg);
	if(seg1.iSeg>seg2.iSeg)
		return 1;
	else if(seg1.iSeg<seg2.iSeg)
		return -1;
	else	//if(seg1.iSeg==seg2.iSeg)
		return ComparePartNoString(pBomPart1->sPartNo,pBomPart2->sPartNo);
}
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
		sprintf(sSpec,"%sx%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sX%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart)
{
	CXhChar50 str;
	if(pBomPart->cPartType==BOMPART::ANGLE)
		GetAngleSimpleSpec(str,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("%.fX%.f",pBomPart->thick,pBomPart->wide);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
static CXhChar16 GetMatName(BOMPART* pBomPart)
{
	CXhChar16 sMatType;
	if(pBomPart->cMaterial!='S')
		sMatType.Printf("%c",pBomPart->cMaterial);
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sMatType.Append("∟");
	else if(pBomPart->cPartType==BOMPART::PLATE)
		sMatType.Append("—");
	return sMatType;
}
static BOOL IsHasHuoqu(BOMPART* pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->m_cFaceN>1)
			return TRUE;
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)
			return TRUE;
	}
	return FALSE;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessNotes(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam="";
	if(pBomPart->bWeldPart)
		sProcessParam.Append("焊接",' ');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
			sProcessParam.Append("制弯",' ');
		if(pPlate->bNeedFillet)		//需要坡口
			sProcessParam.Append("坡口",' ');
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sProcessParam.Append("卷边",' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("带脚钉",' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("切角",' ');
		//制弯
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)
			sProcessParam.Append("制弯",' ');
		//压扁
		if(pBomAngle->nPushFlat>0)		//是否压扁
			sProcessParam.Append("压扁",' ');
		//开合角
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append("开角",' ');
		if(pBomAngle->bHeJiao)
			sProcessParam.Append("合角",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("铲背",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("清根",' ');
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("两端坡口",' ');
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("一端坡口",' ');
	}
	return sProcessParam;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	if(bom.m_nBoltsN<=0)
		return 0;
	//计算列数
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		if(segTable.GetNodeNum()==0||segTable.GetValue(pBomPart->iSeg)!=NULL)	//段号过滤
			BomArr.append(*pNode);
	}
	CBubbleSort<SUPERLIST_NODE<BOMPART>>::BubSort(BomArr.m_pData,BomArr.GetSize(),CompareSegiFunc);
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("放样原始材料表");
	CStringArray str_arr;
	str_arr.SetSize(14);
	str_arr[0]="塔型";str_arr[1]="零件编号";str_arr[2]="材料名称";str_arr[3]="设计规格";
	str_arr[4]="代用规格";str_arr[5]="材质";str_arr[6]="长度mm";str_arr[7]="每基数量";										
	str_arr[8]="眼数";str_arr[9]="电焊";str_arr[10]="火曲";str_arr[11]="切角";str_arr[12]="铲背";str_arr[13]="备注";
	double col_arr[14]={8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeBorders(excel_sheet,"A1","N1",COleVariant(10.5));
	//添加数据内容
	CVariant2dArray map(bom.m_nPartsN,14);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		//塔型
		map.SetValueAt(index,0,COleVariant(bom.m_sTowerTypeName));
		//零件编号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));
		//材料名称
		map.SetValueAt(index,2,COleVariant(GetMatName(pBomPart)));
		//设计规格
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));
		//代用规格
		map.SetValueAt(index,4,COleVariant(""));
		//材质
		map.SetValueAt(index,5,COleVariant(""));
		//长度
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.1f",pBomPart->length)));
		//每基数量
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));
		//眼数
		map.SetValueAt(index,8,COleVariant(pBomPart->m_arrBoltRecs.GetSize()));
		//电焊
		if(pBomPart->bWeldPart)
			map.SetValueAt(index,9,COleVariant("√"));
		//火曲
		if(IsHasHuoqu(pBomPart))
			map.SetValueAt(index,10,COleVariant("√"));
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
			//切角
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,11,COleVariant("√"));
			//铲背
			if(pBomAngle->bCutBer)
				map.SetValueAt(index,12,COleVariant("√"));
		}
		//备注
		map.SetValueAt(index,13,COleVariant(GetProcessNotes(pBomPart)));
		index++;
	}
	//设置右对齐
	char cell_end[30]={};
	sprintf(cell_end,"N%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	return 1;
}