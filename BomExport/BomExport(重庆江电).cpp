// BomExport.cpp : 定义 DLL 的初始化例程。
//重庆江电电力设备有限公司

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
	return 3;
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
//比较段号
static int CompareSegiFunc(const SEGI& item1,const SEGI& item2)
{
	if(item1.iSeg>item2.iSeg)
		return 1;
	else if(item1.iSeg<item2.iSeg)
		return -1;
	else 
		return 0;
}
static CXhChar16 GetBoltTypeStr(BOMBOLT& bolt)
{
	CXhChar16 sBoltType="";
	if(!bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //普通螺栓
		sBoltType="普通螺栓";
	else if(!bolt.bFootNail&&bolt.bTwoCap&&!bolt.bAntiTheft) //双帽螺栓
		sBoltType="双帽螺栓";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //普通脚钉
		sBoltType="普通脚钉";
	else if(!bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //防盗螺栓
		sBoltType="防盗螺栓";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //防盗脚钉
		sBoltType="防盗脚钉";
	else if (bolt.IsDianQuan())
		sBoltType="垫片";
	return sBoltType;
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
	if(bom.m_nBoltsN<=0)
		return 0;
	//计算列数
	ARRAY_LIST<SEGI> segIArr;
	for(int i=0;i<bom.m_nBoltsN;i++)
	{
		BOMBOLT bomBolt=bom.m_pBoltRecsArr[i];
		SEGI *pSegI=NULL;
		for(pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			if(pSegI->iSeg==bomBolt.iSeg)
				break;
		}
		if(pSegI==NULL)
			segIArr.append(SEGI(bomBolt.iSeg));
	}
	CBubbleSort<SEGI>::BubSort(segIArr.m_pData,segIArr.GetSize(),CompareSegiFunc);
	int nColCount=segIArr.GetSize()+4;	//添加段号 名称，螺栓等级，规格，无扣长（lg） 合计 五列   索引是从0开始的 5-1
	CXhChar50 sMaxColCell;
	int nLen=nColCount/26;
	int nSubLen=nColCount%26;
	if(nLen>26)
		return 0;	//暂时仅支持A~AZ
	if(nLen>0)
		sMaxColCell.Printf("%c%c",'A'+nLen-1,'A'+nSubLen);
	else
		sMaxColCell.Printf("%c",'A'+nSubLen);
	//
	int nBoltCount=0;
	ARRAY_LIFE<BOMBOLT_BOX> arrayLife;
	BOMBOLT_BOX *boltArr=(BOMBOLT_BOX*)arrayLife.Create(bom.m_nBoltsN);
	GetBomBoltBoxArr(bom,boltArr,nBoltCount);
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
	CXhChar50 sTempCell("%s1",(char*)sMaxColCell);
	CExcelOper::MergeColRange(excel_sheet,"A1",sTempCell);
	CExcelOper::SetMainTitle(excel_sheet,"A1", sTempCell,"重庆江电电力设备有限公司",30,20);
	sTempCell.Printf("%s2",(char*)sMaxColCell);
	CExcelOper::MergeColRange(excel_sheet,"A2",sTempCell);
	CExcelOper::SetMainTitle(excel_sheet,"A2", sTempCell,"标件表",20,15);
	CExcelOper::SetFontBold(excel_sheet,0,2,1);
	//第三行
	CExcelOper::MergeColRange(excel_sheet,"A3","C3");
	CExcelOper::SetMainTitle(excel_sheet,"A3","A3","工程名称：",15,10,(short)2,(short)2,false);
	//
	CExcelOper::MergeColRange(excel_sheet,"D3","F3");
	CExcelOper::SetMainTitle(excel_sheet,"D3","D3","杆塔号:",15,10,(short)2,(short)2,false);

	CExcelOper::MergeColRange(excel_sheet,"G3","I3");
	CExcelOper::SetMainTitle(excel_sheet,"G3","G3","呼高:",15,10,(short)2,(short)2,false);
	CExcelOper::SetFontBold(excel_sheet,0,3,3);
	//第四行

	CXhChar100 sTowerType="塔型:";
	sTowerType.Append(bom.m_sTowerTypeName);
	CExcelOper::MergeColRange(excel_sheet,"A4","C4");
	CExcelOper::SetFontBold(excel_sheet,0,4,1);
	CExcelOper::SetMainTitle(excel_sheet,"A4","A4",(char*)sTowerType,15,10,(short)2);
	//
	CExcelOper::MergeColRange(excel_sheet,"D4","F4");
	CExcelOper::SetMainTitle(excel_sheet,"D4","D4","计划单号:",15,10,(short)2);
	//
	CExcelOper::MergeRowRange(excel_sheet,"A5","A6");
	CExcelOper::MergeRowRange(excel_sheet,"B5","B6");
	CExcelOper::MergeRowRange(excel_sheet,"C5","C6");
	CExcelOper::MergeRowRange(excel_sheet,"D5","D6");
	CStringArray str_arr;
	str_arr.SetSize(4);
	str_arr[0]="名称";str_arr[1]="螺栓等级";str_arr[2]="规格";str_arr[3]="无扣长（lg）";
	double col_arr[4]={20.0,10.0,7.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,5,str_arr,col_arr,TRUE);
	//第五行
	CExcelOper::SetFontBold(excel_sheet,0,5,6);
	sTempCell=CExcelOper::GetCellPos(nColCount-1,5);
	CExcelOper::MergeColRange(excel_sheet,"E5",sTempCell);
	CExcelOper::SetRangeValue(excel_sheet,"E5", "E5",COleVariant("段位"));
	sTempCell.Printf("%s5",(char*)sMaxColCell);
	CExcelOper::SetRangeValue(excel_sheet,sTempCell, sTempCell,COleVariant("合计"));
	//第六行
	sTempCell.Printf("%s6",(char*)sMaxColCell);
	CExcelOper::SetRangeValue(excel_sheet,sTempCell, sTempCell,COleVariant("数量"));
	//
	//添加段号
	int iCol=4;
	for (SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
	{
		CXhChar50 sCel=CExcelOper::GetCellPos(iCol,6);
		CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant(pSegI->ToString()));
		CExcelOper::SetFontBold(excel_sheet,iCol,6,1);
		iCol++;
	}
	CExcelOper::SetFontBold(excel_sheet,iCol,6,1);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(nBoltCount+3,nColCount+5);
	for(int i=0;i<nBoltCount;i++)
	{
		BOMBOLT bomBolt;
		CopyBomBoltProp(boltArr[i].bomBolt,bomBolt);
		bomBolt.AddPart(boltArr[i].bomBolt.GetPartNum());

		CXhChar16 sBoltType=GetBoltTypeStr(bomBolt);
		if(!bomBolt.IsDianQuan())
		{
			map.SetValueAt(index,0,COleVariant(sBoltType));
			//螺栓等级
			CXhChar16 sGrade("%.1f级",bomBolt.grade);
			map.SetValueAt(index,1,COleVariant(sGrade));
			CXhChar200	sGuiGe("M%d*%d",bomBolt.d,bomBolt.L);
			map.SetValueAt(index,2,COleVariant(sGuiGe));
			map.SetValueAt(index,3,COleVariant((long)bomBolt.nNoThreadlen));//无扣长（lg）
		}
		else	
		{
			CExcelOper::SetFontBold(excel_sheet,0,index+7,4);
			map.SetValueAt(index,0,COleVariant("垫片"));
			map.SetValueAt(index,1,COleVariant("Q235"));
			CXhChar16 str("-%dΦ%.1f",bomBolt.L,bomBolt.d+1.5);
			map.SetValueAt(index,2,COleVariant(str));
		}
		iCol=4;
		for(SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			long *pCount=boltArr[i].m_hashNumBySegI.GetValue(pSegI->iSeg);
			map.SetValueAt(index,iCol,COleVariant(pCount?*pCount:0));
			iCol++;
		}
		CXhChar50 sPartNum("=SUM(E%d:%s)",index+7,(char*)CExcelOper::GetCellPos(iCol-1,index+7));
		map.SetValueAt(index,iCol,COleVariant(sPartNum));
		index++;
	}
	CString sNotes("备注:\n");
	sNotes+="        1).无扣长不允许正偏差，螺栓长度不允许负偏差。\n";
	sNotes+="        2).脚钉采用45度弯钩防滑形式。\n";
	sNotes+="        3).螺栓余量按总数的3%增配。\n";
	sNotes+="        4).表格中普通及防盗的螺栓、脚钉均已加长。\n";
	sNotes+="        5).垫圈厚度(M16厚度为3mm，M20、M24厚度为4mm)。\n";
	sNotes+="        6).防盗螺母采用滚珠式。\n";
	map.SetValueAt(index,0,COleVariant(sNotes));
	CXhChar50 sStartCell=CExcelOper::GetCellPos(0,index+7);
	CXhChar50 sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+4,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	Range excel_range;
	excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
	excel_range.SetRowHeight(COleVariant(100.0));
	index++;
	//
	sNotes.Empty();
	sNotes+="配置：\n";
	sNotes+="        普通螺栓配一螺母一垫圈一薄螺母，普通脚钉配两螺母一垫圈一薄螺母，双帽螺栓配两螺母一垫圈。\n";
	sNotes+="        防盗螺栓配一螺母一垫圈一防盗螺母，防盗脚钉配两螺母一垫圈一防盗螺母。\n";
	sNotes+="        U型螺栓配四帽双垫并按附图进行加工。\n";
	map.SetValueAt(index,0,COleVariant(sNotes));
	sStartCell=CExcelOper::GetCellPos(0,index+7);
	sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+4,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
	excel_range.SetRowHeight(COleVariant(60.0));
	//
	index++;
	sNotes.Empty();
	sNotes+="制表:                           校核：                               日期: ";
	sStartCell=CExcelOper::GetCellPos(0,index+7);
	sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+5,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	map.SetValueAt(index,0,COleVariant(sNotes));
	//
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+7);
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A1");
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A7",sTempCell,map.var);
	//设置对齐
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+6);  //
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A5",sTempCell,COleVariant((long)3));
	CXhChar16 sCell("A%d",index+5);
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+7);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,sCell,sTempCell,COleVariant((long)2));
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A5",sTempCell,COleVariant(10.5));
	//设置颜色
	index=7;
	for (int i=0;i<nBoltCount;i++)
	{
	    CExcelOper::SetFontBold(excel_sheet,0,index,4);
		if(!boltArr[i].bomBolt.IsDianQuan())
		{
			if(16==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,10,index,0,3);
			else if(20==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,6,index,0,3);
		}
		index++;
	}
	return nCount;
}
