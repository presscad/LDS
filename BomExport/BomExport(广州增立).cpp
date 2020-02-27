// BomExport.cpp : 定义 DLL 的初始化例程。
//增立

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
	str_arr.SetSize(22);
	str_arr[0]="段位";str_arr[1]="编号";str_arr[2]="材质";str_arr[3]="规格";
	str_arr[4]="宽度";str_arr[5]="长度";str_arr[6]="单基数";str_arr[7]="切角";
	str_arr[8]="打扁";str_arr[9]="清根";str_arr[10]="铲背";str_arr[11]="开角";
	str_arr[12]="合角";str_arr[13]="火曲";str_arr[14]="电焊";str_arr[15]="坡口";
	str_arr[16]="钻孔";str_arr[17]="车工";str_arr[18]="开槽";str_arr[19]="相贯线";
	str_arr[20]="备注";str_arr[21]="打字码";
	double col_arr[22]={5.0,5.0,5.0,7.5,5.0,
						5.5,5.0,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,2.5,2.5,5.0,5.0,
						5.0,7.5,};
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
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			BOOL bCutAngle=pAngle->bCutAngle;
			if(bCutAngle)				//切角
				map.SetValueAt(index,7,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 表示一端压扁 2 表示两端压扁 
				map.SetValueAt(index,8,COleVariant("1"));
			if(pAngle->bCutRoot)			//清根
				map.SetValueAt(index,9,COleVariant("1"));
			if(pAngle->bCutBer)				//铲背
				map.SetValueAt(index,10,COleVariant("1"));
			if(pAngle->bKaiJiao)			//开角
				map.SetValueAt(index,11,COleVariant("1"));
			if(pAngle->bHeJiao)				//合角
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//火曲
			map.SetValueAt(index,13,COleVariant("1"));
		if(pPart->bWeldPart)		//是否电焊件
			map.SetValueAt(index,14,COleVariant("1"));
	    if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//坡口
			if(pPlate->bNeedFillet||pPlate->thick>=8)	
				map.SetValueAt(index,15,COleVariant("1"));
			/*{
				if(pPlate->thick>=8&&pPlate->thick<=12)
					map.SetValueAt(index,15,COleVariant("1"));
				else if(pPlate->thick>=14)
					map.SetValueAt(index,15,COleVariant("2"));
			}*/
		}
		if((pPart->cMaterial=='S'&&pPart->thick>16)||			//钻孔
			(pPart->cMaterial=='H'&&pPart->thick>14)||
			(pPart->cMaterial=='P'&&pPart->thick>12)||
			pPart->cMaterial=='T'||pPart->IsDrillHole())
				map.SetValueAt(index,16,COleVariant("1"));
		if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			//开槽类型
			int nType=0;
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//支管 两头C型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//支管 两头U型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//支管 两头T型插板
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//支管 两头X型插板
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//支管 两头C型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//支管 两头U型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//支管 两头T型插板
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//支管 两头X型插板
			if(nType>0)
				map.SetValueAt(index,18,COleVariant("1"));
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,19,COleVariant("1"));	//割相贯线
		}
		if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="垫板";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="∮M16*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="∮M20*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="∮M24*"+str;
			}
			map.SetValueAt(index,20,COleVariant(sNote));
		}
		else
			map.SetValueAt(index,20,COleVariant(pPart->sNotes));
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"V%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)3));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
