// BomExport.cpp : 定义 DLL 的初始化例程。
//增立-TMA

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
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
const int COL_COUNT=7;
//索引号从1开始，0表示没有对应列
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=1;
		colIndexArr[0]=2;	//件号
		colIndexArr[1]=4;	//规格
		colIndexArr[2]=5;	//材质
		colIndexArr[3]=8;	//数量
		colIndexArr[4]=18;	//单重
		colIndexArr[5]=6;	//长度
		colIndexArr[6]=7;	//宽度
		return 7;
	}
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
CXhChar200 GetPartNotes(BOMPART *pPart)
{
	CXhChar200 sNotes;
	bool bDrillHole=false;
	if( (pPart->cMaterial=='S'&&pPart->thick>16)||			//钻孔
		(pPart->cMaterial=='H'&&pPart->thick>14)||
		(pPart->cMaterial=='P'&&pPart->thick>12)||
		pPart->cMaterial=='T'||pPart->IsDrillHole())
		bDrillHole=true;
	if(pPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
		//带脚钉
		if(pAngle->bHasFootNail)
			sNotes.Append("带脚钉 ");
		//钻孔
		if(bDrillHole)
			sNotes.Append("钻孔 ");
		//切角
		if(pAngle->IsCutAngleOrWing())				
			sNotes.Append("切角 ");
		//压扁
		if(pAngle->nPushFlat==0x05)			//0x01:始端压扁 0x02:中间压扁 0x04:终端压扁
			sNotes.Append("两端压扁 ");
		else if((pAngle->nPushFlat&0x01)||(pAngle->nPushFlat&0x04))
			sNotes.Append("一端压扁 ");
		if(pAngle->nPushFlat&0x02)
			sNotes.Append("中间压扁 ");
		if(pAngle->bCutRoot)			//清根
			sNotes.Append("清根 ");
		if(pAngle->bCutBer)				//铲背
			sNotes.Append("铲背 ");
		if(pAngle->bKaiJiao)			//开角
			sNotes.Append("开角 ");
		if(pAngle->bHeJiao)				//合角
			sNotes.Append("合角 ");
	}
	else if(pPart->cPartType==BOMPART::PLATE)	
	{	
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		//钻孔
		if(bDrillHole)
			sNotes.Append("钻孔 ");
		//坡口
		if(pPlate->bNeedFillet||pPlate->thick>=8)	
			sNotes.Append("坡口 ");
		//卷边
		int nRollEdgeCount=0;
		for(LIST_NODE<BOMPROFILE_VERTEX>* pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				nRollEdgeCount++;
		}
		if(nRollEdgeCount>0)
			sNotes.Append("卷边 ");
		//割口
		//无孔
		int nHoleCount=pPlate->m_arrBoltRecs.GetSize();
		if(nHoleCount<=0)
			sNotes.Append("无孔 ");
		else if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="垫板";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="Φ17.5*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="Φ21.5*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="Φ25.5*"+str;
			}
			sNotes.Append(sNote);
			sNotes.Append(" ");
		}
	}
	long nHuoquLineCount=pPart->GetHuoquLineCount();
	if(nHuoquLineCount>0)		//火曲
		sNotes.Append("制弯 ");
	if(pPart->bWeldPart)		//是否电焊件
		sNotes.Append("组焊 ");
	return sNotes;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	//1.获取BOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	//进行过滤、排序
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
	//2.生成ＥＸＣＥＬ材料表
	//2.1 创建Excel工作薄
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("放样原始材料表");
	//2.2 输出材料表
	//2.2.1 添加标题栏
	CStringArray str_arr;
	//
	str_arr.SetSize(9);
	str_arr[0]="段名";str_arr[1]="编号";str_arr[2]="材质";str_arr[3]="规格";
	str_arr[4]="宽度";str_arr[5]="长度";str_arr[6]="单基数";str_arr[7]="";str_arr[8]="备注";
	double col_arr[9]={5.0,5.0,5.0,7.5,5.0,
						5.5,7.0,5.0,25};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,22);
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		SEGI segI(pPart->iSeg);
		map.SetValueAt(index,0,COleVariant(segI.ToString()));		//段位
		map.SetValueAt(index,1,COleVariant(pPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//编号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//材质
		CXhChar16 sGuiGe=GetPartSpec(pPart);
		sGuiGe.Replace('x','*');
		sGuiGe.Replace("Φ","∮");
		sGuiGe.Replace("∠","L");
		map.SetValueAt(index,3,COleVariant(sGuiGe));	//规格
		map.SetValueAt(index,4,COleVariant(ftol(pPart->wide)));		//宽度
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(pPart->GetPartNum()));	//单基数量
		map.SetValueAt(index,7,COleVariant(""));
		map.SetValueAt(index,8,COleVariant(GetPartNotes(pPart)));	//备注
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"I%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)3));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
