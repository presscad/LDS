// BomExport.cpp : 定义 DLL 的初始化例程。
// 湖州飞剑

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"

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
	return 1;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"
long ftol(double fVal)
{
	return (long)(fVal+0.5);
}
CXhChar16 QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else //if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	//1.获取BOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer);
	}
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
	str_arr.SetSize(29);
	str_arr[0]="段号";str_arr[1]="部件号";str_arr[2]="材质";str_arr[3]="材料";
	str_arr[4]="规格";str_arr[5]="长度";str_arr[6]="宽度";str_arr[7]="厚度";
	str_arr[8]="单基数量";str_arr[9]="单件重量";str_arr[10]="单件孔数";str_arr[11]="备注";
	str_arr[12]="电焊";str_arr[13]="火曲";str_arr[14]="切角";str_arr[15]="铲背";
	str_arr[16]="清根";str_arr[17]="打扁";str_arr[18]="开合角";str_arr[19]="钻孔";
	str_arr[20]="坡口";str_arr[21]="割相贯线";str_arr[22]="开槽形式";str_arr[23]="类型";
	str_arr[24]="边数";str_arr[25]="周长";str_arr[26]="表面积";str_arr[27]="各孔径孔数";
	str_arr[28]="焊接边mm";
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						4.0,7.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,4.0,8.0,7.5,4.0,
						4.0,7.0,12.5,3.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,29);
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		map.SetValueAt(index,0,COleVariant((long)pPart->iSeg));		//段号
		CXhChar50 sPartNoIncMat("%s",(char*)pPart->sPartNo);
		if(pPart->cMaterial!='S') 
			sprintf(sPartNoIncMat,"%s%c",(char*)pPart->sPartNo,pPart->cMaterial);
		map.SetValueAt(index,1,COleVariant(sPartNoIncMat));			//编号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//材质
		map.SetValueAt(index,3,COleVariant(pPart->GetPartTypeName()));				//材料
		map.SetValueAt(index,4,COleVariant(pPart->GetSpec()));		//规格
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));	//厚度
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//单基数量
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//单件重量
		map.SetValueAt(index,10,COleVariant(pPart->nMSumLs));		//单件孔数
		map.SetValueAt(index,11,COleVariant(pPart->sNotes));
		if(pPart->bWeldPart)		//是否电焊件
			map.SetValueAt(index,12,COleVariant("1"));
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//火曲
			map.SetValueAt(index,13,COleVariant(nHuoquLineCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			long nCutAngleCount=pAngle->CutAngleN();
			if(nCutAngleCount>0)			//切角刀数
				map.SetValueAt(index,14,COleVariant(nCutAngleCount));
			if(pAngle->bCutBer)				//铲背
				map.SetValueAt(index,15,COleVariant("1"));
			if(pAngle->bCutRoot)			//清根
				map.SetValueAt(index,16,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 表示一端压扁 2 表示两端压扁 
				map.SetValueAt(index,17,COleVariant((long)pAngle->nPushFlat));
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//开合角
				map.SetValueAt(index,18,COleVariant(nKaiHeJiaoN));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//坡口
			if(pPlate->bNeedFillet)		
				map.SetValueAt(index,20,COleVariant("1"));
			//边数
			map.SetValueAt(index,24,COleVariant(pPlate->GetEdgeCount()));
			//周长
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,25,COleVariant(str));
			//面积
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,26,COleVariant(str));
			//焊接边mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,28,COleVariant(str));
			}
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			/*if(pPart->IsTubeTransect())	//割相贯线
				map.SetValueAt(index,21,COleVariant("1"));
			//开槽类型
			char sType[100]="";
			int nType=pPart->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,22,COleVariant(sType));*/
		}
		if(pPart->thick>=14)		//钻孔
			map.SetValueAt(index,19,COleVariant("1"));
		map.SetValueAt(index,23,COleVariant(pPart->GetPartSubTypeName()));	//构件类型
		//孔数孔径
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AC%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
