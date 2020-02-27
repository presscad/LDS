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
	return 2;
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
static CXhChar16 GetPartTypeName(BOMPART *pPart)
{
	BYTE cPartType=pPart->cPartType;
	CXhChar16 sPartType;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cPartType==BOMPART::PLATE)		//钢板
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="Flange";
		else
			sPartType="Plate";
	}
	else if(cPartType==BOMPART::ANGLE)	//角钢
		sPartType="Angle";
	else if(cPartType==BOMPART::TUBE)	//钢管
		sPartType="Tube";
	else if(cPartType==BOMPART::SLOT)	//槽钢
		sPartType="U-steel";
	else if(cPartType==BOMPART::FLAT)	//扁钢
		sPartType="Flat Steel";
#else
	if(cPartType==BOMPART::PLATE)		//钢板
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="法兰";
		else
			sPartType="钢板";
	}
	else if(cPartType==BOMPART::ANGLE)	//角钢
		sPartType="角钢";
	else if(cPartType==BOMPART::TUBE)	//钢管
		sPartType="钢管";
	else if(cPartType==BOMPART::SLOT)	//槽钢
		sPartType="槽钢";
	else if(cPartType==BOMPART::FLAT)	//扁钢
		sPartType="扁钢";
#endif
	return sPartType;
}
static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec=pPart->GetSpec();
	if(GetSupportDataBufferVersion()>=3)
	{
		if(pPart->cPartType!=BOMPART::PLATE)
			return sSpec;
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		if(pPlate->IsFL()||pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("Φ%.f/%.f",pPlate->fFlOutterD,pPlate->fFlInnerD);
	}
	return sSpec;
}
#ifdef __TMA_BOM_
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
	str_arr.SetSize(34);
	str_arr[0]="段号";str_arr[1]="部件号";str_arr[2]="材质";str_arr[3]="材料";
	str_arr[4]="规格";str_arr[5]="长度";str_arr[6]="宽度";str_arr[7]="厚度";
	str_arr[8]="单基数量";str_arr[9]="单件重量";str_arr[10]="单件孔数";str_arr[11]="备注";
	str_arr[12]="电焊";str_arr[13]="火曲";str_arr[14]="内曲";str_arr[15]="外曲";
	str_arr[16]="内曲筋";str_arr[17]="外曲筋";str_arr[18]="切角";str_arr[19]="铲背";
	str_arr[20]="清根";str_arr[21]="打扁";str_arr[22]="开合角";str_arr[23]="钻孔";
	str_arr[24]="坡口";str_arr[25]="含弧";str_arr[26]="割相贯线";str_arr[27]="开槽形式";str_arr[28]="类型";
	str_arr[29]="边数";str_arr[30]="周长";str_arr[31]="表面积";str_arr[32]="各孔径孔数";
	str_arr[33]="焊接边mm";
	double col_arr[34]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						4.0,7.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,4.0,8.0,7.5,4.0,
						4.0,7.0,12.5,3.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,34);
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
		map.SetValueAt(index,3,COleVariant(GetPartTypeName(pPart)));				//材料
		map.SetValueAt(index,4,COleVariant(GetPartSpec(pPart)));	//规格
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));	//厚度
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//单基数量
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//单件重量
		map.SetValueAt(index,10,COleVariant(pPart->nMSumLs));		//单件孔数
		CXhChar200 sNotes("%s",pPart->sNotes);
		map.SetValueAt(index,11,COleVariant(pPart->sNotes));
		if(pPart->bWeldPart)		//是否电焊件
			map.SetValueAt(index,12,COleVariant("1"));
		else
		{
			LIST_NODE<BOM_WELDPART> *pWeldNode=NULL;
			for(pWeldNode=bom.listWeldParts.EnumFirst();pWeldNode;pWeldNode=bom.listWeldParts.EnumNext())
			{
				if(pWeldNode->data.sPartNo.EqualNoCase(pPart->sPartNo))
					break;
			}
			if(pWeldNode)	//组焊件主构件也需要设置为电焊件
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//火曲
			map.SetValueAt(index,13,COleVariant(nHuoquLineCount));
		long nCutAngleCount=pPart->CutAngleN();
		if(nCutAngleCount>0)		//切角刀数
			map.SetValueAt(index,18,COleVariant(nCutAngleCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			BYTE cHuoquType=pAngle->GetHuoquType();
			if(cHuoquType==1)		//内曲
				map.SetValueAt(index,14,COleVariant("1"));
			else if(cHuoquType==2)	//外曲
				map.SetValueAt(index,15,COleVariant("1"));
			else if(cHuoquType==3)	//内曲筋
				map.SetValueAt(index,16,COleVariant("1"));
			else if(cHuoquType==4)	//外曲筋
				map.SetValueAt(index,17,COleVariant("1"));
			if(pAngle->bCutBer)				//铲背
				map.SetValueAt(index,19,COleVariant("1"));
			if(pAngle->bCutRoot)			//清根
				map.SetValueAt(index,20,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 表示一端压扁 2 表示两端压扁 
			{
				long nPushState=0;
				if(pAngle->nPushFlat==0x03||	//压扁状态：011(始、中、终)
				   pAngle->nPushFlat==0x06)		//压扁状态：110(始、中、终)
				   nPushState=5;				//5.中间+一端
				else if(pAngle->nPushFlat==0x07)//压扁状态：111(始、中、终)
					nPushState=4;				//4.中间+两端
				else if(pAngle->nPushFlat==0x02)//压扁状态：010(始、中、终)
					nPushState=3;				//3.中间压扁
				else if(pAngle->nPushFlat==0x05)//压扁状态：101(始、中、终)
					nPushState=2;				//2.两端压扁
				else if(pAngle->nPushFlat==0x01||//压扁状态：100(始、中、终)
						pAngle->nPushFlat==0x04) //压扁状态：001(始、中、终)
					nPushState=1;				//1.一端压扁

				map.SetValueAt(index,21,COleVariant((long)nPushState));
			}
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//开合角
				map.SetValueAt(index,22,COleVariant(nKaiHeJiaoN));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//坡口
			if(pPlate->bWeldPart&&pPlate->thick>=8)
				map.SetValueAt(index,24,COleVariant("1"));
			//含弧
			long nArcEdgeCount=pPlate->GetArcEdgeCount();
			if(nArcEdgeCount>0)
				map.SetValueAt(index,25,COleVariant(nArcEdgeCount));
			//边数
			map.SetValueAt(index,29,COleVariant(pPlate->GetEdgeCount()-nArcEdgeCount));
			//周长
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,30,COleVariant(str));
			//面积
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,31,COleVariant(str));
			//焊接边mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,33,COleVariant(str));
			}
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			map.SetValueAt(index,21,COleVariant(nTransect));	//割相贯线
			//开槽类型
			/*char sType[100]="";
			int nType=pPart->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,22,COleVariant(sType));*/
		}
		//if(pPart->thick>=14)		//钻孔
		//	map.SetValueAt(index,23,COleVariant("1"));
		char cMat=toupper(pPart->cMaterial);
		if( (pPart->thick>=16&&cMat=='S')||
			(pPart->thick>=14&&cMat=='H')||
			(pPart->thick>=12&&(cMat=='P'||cMat=='T')))
			map.SetValueAt(index,23,COleVariant("1"));
		map.SetValueAt(index,28,COleVariant(pPart->GetPartSubTypeName()));	//构件类型
		//孔数孔径
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,32,COleVariant(sHoleStr));
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AH%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
#else
//开槽组焊代号类型表
//nRuleType 开槽类型编号规则
//nRuleType==0 代号：LRM(左右中) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管 3.节点板和相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL 6.相贯 7.平面切割
//nRuleType==1 代号：LMR(左中右) 
//				M位: (钢管中间连接方式) 0.无连接 1.节点板 2.相贯钢管
//			  L,R位: (钢管两端开槽方式) 1.槽型插板 2.U型插板 3.T型插板 4.X型插板 5.FL
void AddSlotAndWeldCodeTblSheet(LPDISPATCH pSheet,int nRuleType)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Slot Group Weld Type Table");
#else 
	excel_sheet.SetName("开槽组焊代号类型表");
#endif
	//
	int nMidType=4,nType=7,nRowNum=0;
	int i=0,j=0,k=0,index=0;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//中间连接样式
		nType=5;	//两端连接样式
		nRowNum++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
				nRowNum++;
		}
	}
	nRowNum+=7;
	//
	CVariant2dArray map(nRowNum+1,3);
	//第一行数据项标题行，需要格式输出
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Sequence Number"));
	map.SetValueAt(index,1,COleVariant("Code Name"));
	map.SetValueAt(index,2,COleVariant("Instruction"));
#else 
	map.SetValueAt(index,0,COleVariant("序号"));
	map.SetValueAt(index,1,COleVariant("代号"));
	map.SetValueAt(index,2,COleVariant("说明"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,60.0,"C1",NULL);
	//
	int nCurRow=2;
#ifdef AFX_TARG_ENU_ENGLISH
	CString sMidTypeName[4]={"","Center Node Plate","Center Intersect Tube","Center Intersect Tube And Node Plate"};
	CString sTypeName[7]={"1-type Insert Plate","U Insert Plate","T Insert Plate","Cross Insert Plate","Flange","Intersect","Plane Cut"};
#else 
	CString sMidTypeName[4]={"","中间节点板","中间相贯管","中间相贯管及节点板"};
	CString sTypeName[7]={"一字型插板","U型插板","T型插板","十字插板","法兰","相贯","平面切割"};
#endif
	nMidType=4;
	nType=7;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//中间连接样式
		nType=5;	//两端连接样式
		map.SetValueAt(index,0,COleVariant("1"));
		map.SetValueAt(index,1,COleVariant("0"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,2,COleVariant("Main Tube Weld"));
#else 
		map.SetValueAt(index,2,COleVariant("主管焊接"));
#endif
		index++;
		nCurRow++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
			{
				char sID[50]="";
				sprintf(sID,"%d",nCurRow-1);
				map.SetValueAt(index,0,COleVariant(sID));
				char sCode[50]="";
				//编号
				if(nRuleType==0)
				{	//LRM
					if(k==0)
						sprintf(sCode,"%d%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,j,k);
				}
				else if(nRuleType==1)
				{	//LMR
					if(k==0)
						sprintf(sCode,"%d0%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,k,j);
				}
				map.SetValueAt(index,1,COleVariant(sCode));
				char sHelpStr[200]="";
				if(i==j)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sHelpStr,"Two End %s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"One End %s Anoter End %s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#else 
					sprintf(sHelpStr,"两端%s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"一端%s另一端%s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#endif
				map.SetValueAt(index,2,COleVariant(sHelpStr));
				index++;
				nCurRow++;
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	CString sHelpStr[7]={"Angle Weld Part","Angle Rib Plate","Only Plate","Tower Foot","Handrail Weld Part","Ladder Weld Part","Other Weld Part"};
#else 
	CString sHelpStr[7]={"连板角钢焊件","角钢加劲板","纯板焊件","塔脚焊件","扶手焊件","爬梯焊件","其他焊件"};
#endif
	for(i=0;i<7;i++)
	{
		char sCode[50]="";
		//编号
		if(nRuleType==0)
		{	//LRM
			if(i==5)
				strcpy(sCode,"A");
			else if(i==6)
				strcpy(sCode,"B");
			else if(i==0||i==1)
				sprintf(sCode,"%d",i+8);
			else
				continue;	//样式一不添加："角钢加劲板","纯板焊件","塔脚焊件"三行
		}
		else if(nRuleType==1)	//LMR
		{
			if(i<4)
				sprintf(sCode,"60%d",i);
			else
				sprintf(sCode,"%d00",i+3);
		}
		char sID[50]="";
		sprintf(sID,"%d",nCurRow-1);
		map.SetValueAt(index,0,COleVariant(sID));
		map.SetValueAt(index,1,COleVariant(sCode));
		map.SetValueAt(index,2,COleVariant(sHelpStr[i]));
		nCurRow++;
		index++;
	}
	char cell_end[15]="";
	//设置表格内容
	sprintf(cell_end,"C%d",nCurRow-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("放样原始材料表");
	//2.2 输出材料表
	//2.2.1 添加标题栏
	const int COL_COUNT=38;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="序号";str_arr[1]="段号";str_arr[2]="零件号";str_arr[3]="材料";str_arr[4]="材质";
	str_arr[5]="规格";str_arr[6]="宽度";str_arr[7]="长度";str_arr[8]="单基数";str_arr[9]="单量";
	str_arr[10]="计重";str_arr[11]="压制";str_arr[12]="电焊";str_arr[13]="切角";str_arr[14]="铲背";
	str_arr[15]="刨根";str_arr[16]="打扁";str_arr[17]="开合角";str_arr[18]="卷边";str_arr[19]="火曲";
	str_arr[20]="开槽";str_arr[21]="相贯线";str_arr[22]="钻孔";str_arr[23]="焊接长度";str_arr[24]="板周长";
	str_arr[25]="新放样";str_arr[26]="特殊件号";str_arr[27]="坡口";str_arr[28]="边数";str_arr[29]="定位圆";
	str_arr[30]="是否套接";str_arr[31]="类型";str_arr[32]="各孔径孔数";str_arr[33]="备注";str_arr[34]="含弧";
	str_arr[35]="内曲";str_arr[36]="外曲";str_arr[37]="表面积";
	double col_arr[COL_COUNT]={ 3.0,3.0,7.5,3.5,4.5,
								8.5,4.5,4.5,2.5,5.0,
								5.0,2.5,2.5,2.5,2.5,
								2.5,2.5,2.5,2.5,2.5,
								2.5,2.5,2.5,5.0,5.0,
								2.5,2.5,2.5,3.5,7.5,
								2.5,7.5,8.0,7.5,2.5,
								2.5,2.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,COL_COUNT);
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		map.SetValueAt(index,0,COleVariant((long)i));				//序号
		map.SetValueAt(index,1,COleVariant((long)pPart->iSeg));		//段号
		CXhChar50 sPartNoIncMat("%s",(char*)pPart->sPartNo);
		if(pPart->cMaterial!='S') 
			sprintf(sPartNoIncMat,"%s%c",(char*)pPart->sPartNo,pPart->cMaterial);
		map.SetValueAt(index,2,COleVariant(sPartNoIncMat));			//编号
		map.SetValueAt(index,3,COleVariant(GetPartTypeName(pPart)));				//材料
		map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//材质
		map.SetValueAt(index,5,COleVariant(GetPartSpec(pPart)));	//规格
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pPart->length)));	//长度
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//单基数量
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//单件重量
		double sum_weight=ftol(pPart->fPieceWeight*pPart->GetPartNum()*100)/100.0;
		str.Format("%.2f",weight);
		map.SetValueAt(index,10,COleVariant(str));		//计重
		if(pPart->bWeldPart)		//是否电焊件
			map.SetValueAt(index,12,COleVariant("1"));
		else
		{
			LIST_NODE<BOM_WELDPART> *pWeldNode=NULL;
			for(pWeldNode=bom.listWeldParts.EnumFirst();pWeldNode;pWeldNode=bom.listWeldParts.EnumNext())
			{
				if(pWeldNode->data.sPartNo.EqualNoCase(pPart->sPartNo))
					break;
			}
			if(pWeldNode)	//组焊件主构件也需要设置为电焊件
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nCutAngleCount=pPart->CutAngleN();
		if(nCutAngleCount>0)		//切角刀数
			map.SetValueAt(index,13,COleVariant(nCutAngleCount));
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//火曲
			map.SetValueAt(index,19,COleVariant(nHuoquLineCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			if(pAngle->bCutBer)				//铲背
				map.SetValueAt(index,14,COleVariant("1"));
			if(pAngle->bCutRoot)			//清根
				map.SetValueAt(index,15,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 表示一端压扁 2 表示两端压扁 
			{
				long nPushState=0;
				if(pAngle->nPushFlat==0x03||	//压扁状态：011(始、中、终)
				   pAngle->nPushFlat==0x06)		//压扁状态：110(始、中、终)
				   nPushState=5;				//5.中间+一端
				else if(pAngle->nPushFlat==0x07)//压扁状态：111(始、中、终)
					nPushState=4;				//4.中间+两端
				else if(pAngle->nPushFlat==0x02)//压扁状态：010(始、中、终)
					nPushState=3;				//3.中间压扁
				else if(pAngle->nPushFlat==0x05)//压扁状态：101(始、中、终)
					nPushState=2;				//2.两端压扁
				else if(pAngle->nPushFlat==0x01||//压扁状态：100(始、中、终)
						pAngle->nPushFlat==0x04) //压扁状态：001(始、中、终)
					nPushState=1;				//1.一端压扁

				map.SetValueAt(index,16,COleVariant((long)nPushState));
			}
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//开合角
				map.SetValueAt(index,17,COleVariant(nKaiHeJiaoN));
			BYTE cHuoquType=pAngle->GetHuoquType();
			if(cHuoquType==1||cHuoquType==3)		//内曲
				map.SetValueAt(index,35,COleVariant("1"));
			else if(cHuoquType==2||cHuoquType==4)	//外曲
				map.SetValueAt(index,36,COleVariant("1"));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//钻孔
			char cMat=toupper(pPart->cMaterial);
			if( (pPart->thick>=16&&cMat=='S')||
				(pPart->thick>=14&&cMat=='H')||
				(pPart->thick>=12&&(cMat=='P'||cMat=='T')))
				map.SetValueAt(index,22,COleVariant("1"));			//钻孔
			//焊接边mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,23,COleVariant(str));
			}
			//周长
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,24,COleVariant(str));
			//坡口
			if(pPlate->bNeedFillet||(pPlate->bWeldPart&&pPlate->thick>=8))
			{
				if(pPlate->thick>=8&&pPlate->thick<=12)
					map.SetValueAt(index,27,COleVariant("1"));
				else if(pPlate->thick>=14)
					map.SetValueAt(index,27,COleVariant("2"));
			}
			//边数
			long nArcEdgeCount=pPlate->GetArcEdgeCount();
			map.SetValueAt(index,28,COleVariant(pPlate->GetEdgeCount()-nArcEdgeCount));
			if(pPlate->IsFL()||pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
			{
				map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));			//法兰厚度
				map.SetValueAt(index,29,COleVariant(ftol(pPlate->fFlBoltLayoutD)));	//法兰螺栓定位圆
			}
			//套接
			if(pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
				map.SetValueAt(index,30,COleVariant("1"));
			//含弧
			if(nArcEdgeCount>0)
				map.SetValueAt(index,34,COleVariant(nArcEdgeCount));
			//面积
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,37,COleVariant(str));
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			//开槽类型
			char sType[100]="";
			int nType=pBomTube->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,20,COleVariant(sType));
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,21,COleVariant(nTransect));	//割相贯线
		}
		map.SetValueAt(index,31,COleVariant(pPart->GetPartSubTypeName()));	//构件类型
		//孔数孔径
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,32,COleVariant(sHoleStr));
		CXhChar200 sNotes("%s",pPart->sNotes);
		if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			if( pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
				pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
				sNotes.Append(" 两端坡口");
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
					pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
				sNotes.Append(" 一端坡口");
		}
		map.SetValueAt(index,33,COleVariant(pPart->sNotes));
		index++;
	}
	char cell_end[15]=" ";
	//设置右对齐
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"AL%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
//潍坊长安组焊明细表 wht 10-09-30
void AddPartGroupSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("组焊件明细表");
#endif
	//统计行数
	int nRowNum=1;
	LIST_NODE<BOM_WELDPART> *pNode=NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode=NULL;
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext())
			nRowNum++;
	}
	int index=0;
	CVariant2dArray map(nRowNum,4);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Group Part No."));
	map.SetValueAt(index,2,COleVariant("Piece Part"));
	map.SetValueAt(index,3,COleVariant("Single Group Amount"));
#else 
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("组件号"));
	map.SetValueAt(index,2,COleVariant("零件"));
	map.SetValueAt(index,3,COleVariant("单组件数"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"D1",NULL);
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext(),index++)
		{
			//PART *pPart=mat_tbl.GetPartFromPartNo(pPartRef->sPartNo);
			//if(pPart==NULL)
			//	continue;
			map.SetValueAt(index,0,COleVariant((long)pNode->data.iSeg));			//段号
			map.SetValueAt(index,1,COleVariant(pNode->data.sPartNo));
			map.SetValueAt(index,2,COleVariant(pPartRefNode->data.sPartNo));		//零件
			map.SetValueAt(index,3,COleVariant((long)pPartRefNode->data.nPartNum));	//单组数量
		}
	}
	char cell_end[15]="";
	//设置左对齐
	sprintf(cell_end,"C%d",nRowNum);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//设置表格内容
	sprintf(cell_end,"D%d",nRowNum);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();	
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
	//2.生成ＥＸＣＥＬ材料表
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(3);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	//2.1 构件明细表
	AddPartSpecTblSheet(bom,pWorksheet);
	//2.2 开槽代号表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddSlotAndWeldCodeTblSheet(pWorksheet,1);
	//2.3 组焊件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 3));
	AddPartGroupSumMatTbl(bom,pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}
#endif