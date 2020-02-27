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

static int GetProcessNotes(BOMPART* pBomPart,char *notes,bool bIncKaiHeAngle=FALSE)
{
	char ss[300]="";
	long nHuoquLineCount=pBomPart->GetHuoquLineCount();
	if(pBomPart->cPartType==BOMPART::PLATE)
		nHuoquLineCount=((PART_PLATE*)pBomPart)->m_cFaceN-1;
	if(nHuoquLineCount>0)	//是否需要制弯
		sprintf(ss,"%s 制弯",ss);
	if(pBomPart->bWeldPart&&pBomPart->cPartType!=BOMPART::PLATE)
		sprintf(ss,"%s 焊接",ss);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->bWeldPart)		//是否需要焊接
			sprintf(ss,"%s 焊接",ss);
		if(pPlate->bNeedFillet)		//需要坡口
			sprintf(ss,"%s 坡口",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s 卷边%dmm",ss,abs(pNode->data.manu_space));
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s 两端坡口",ss);
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s 一端坡口",ss);
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		//if(pBomAngle->bCutAngle)		//是否切角
		//	sprintf(ss,"%s 切角",ss);
		//if(pBomAngle->bCutBer)			//是否需要铲背
		//	sprintf(ss,"%s 铲背",ss);
		//if(pBomAngle->bCutRoot)		//是否需要清根
		//	sprintf(ss,"%s 清根",ss);
		if(pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle)>0)		//是否需要开角
		{
			if(bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
				sprintf(ss,"%s 开角(%.1f°)",ss,pBomAngle->wing_angle-90);
			else 
				sprintf(ss,"%s 开角",ss);
		}
		if(pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle)>0)			//是否需要合角
		{
			if(bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
				sprintf(ss,"%s 合角(%.1f°)",ss,90-pBomAngle->wing_angle);
			else 
				sprintf(ss,"%s 合角",ss);
		}
		if(pBomAngle->nPushFlat>0)		//是否压扁
		{
			if(pBomAngle->nPushFlat==0x02)
				sprintf(ss,"%s 中间压扁",ss);
			else
				sprintf(ss,"%s 压扁",ss);
		}
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			sprintf(ss,"%s 带脚钉",ss);
	}
	if(notes)
		strcpy(notes,ss);
	return strlen(ss);
}
static BOOL IsZhiWan(BOMPART* pBomPart)
{
	if(pBomPart==NULL)
		return FALSE;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
				return TRUE;
		}
		int nHuoquLineCount=pPlate->m_cFaceN-1;
		if(nHuoquLineCount>0)
			return TRUE;
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if((pBomAngle->bKaiJiao||pBomAngle->bHeJiao)&&GetKaiHeJiaoN(pBomAngle)>0)
			return TRUE;
		if(pBomAngle->nPushFlat>0)		//是否压扁
			return TRUE;
		if(pBomAngle->GetHuoquType()>0)	//火曲
			return TRUE;
	}
	return FALSE;
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
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
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
	//从buffer中获取构件明细
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
	//设置特定标题
	CXhChar50 sTitle("鞍山铁塔制造总厂放样原始材料表");
	CExcelOper::SetMainTitle(excel_sheet,"A1","N1",sTitle,20,16,(short)3,(short)2,false);
	CExcelOper::SetRangeBorders(excel_sheet,"A1","N1",COleVariant(16.0));
	sTitle.Copy("放样任务单号码:0000");
	CExcelOper::SetMainTitle(excel_sheet,"A2","N2",sTitle,15,10,(short)4,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","E3","工程名称:",15,10,(short)2,(short)2,false);
	CExcelOper::MergeColRange(excel_sheet,"F3","M3");
	CExcelOper::SetMainTitle(excel_sheet,"N3","N3","填注者:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A4","E4","塔型:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"F4","F4","塔高m:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"M4","M4","合计:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A5","E5","施工图号:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"F5","F5","数量G:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N5","N5","审核者:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A6","E6","防腐处理：       镀锌:",15,10,(short)2,(short)2,false);
	CExcelOper::SetRangeBorders(excel_sheet,"A2","N6",COleVariant(11.0));
	//添加实时数据的列标题和实时数据
	CVariant2dArray map(bom.m_nPartsN+2,15);
	int index=0;
	//序号
	map.SetValueAt(index,0,COleVariant("序号"));
	CExcelOper::MergeRowRange(excel_sheet,"A7","A8");
	CExcelOper::SetRangeColWidth(excel_sheet,4,"A7",NULL);
	//构件编号
	map.SetValueAt(index,1,COleVariant("构件编号"));
	CExcelOper::MergeRowRange(excel_sheet,"B7","B8");
	//材质
	map.SetValueAt(index,2,COleVariant("材质"));
	CExcelOper::MergeRowRange(excel_sheet,"C7","C8");
	//材料规格
	map.SetValueAt(index,3,COleVariant("材质规格"));
	CExcelOper::MergeColRange(excel_sheet,"D7","E7");
	map.SetValueAt(index+1,3,COleVariant("设计规格"));
	map.SetValueAt(index+1,4,COleVariant("代用规格"));
	//长度（面积）
	map.SetValueAt(index,5,COleVariant("长度mm\n面积mm^2"));
	CExcelOper::MergeRowRange(excel_sheet,"F7","F8");
	//单基数量
	map.SetValueAt(index,6,COleVariant("单基数量"));
	CExcelOper::MergeRowRange(excel_sheet,"G7","G8");
	//电焊
	map.SetValueAt(index,7,COleVariant("电焊"));
	CExcelOper::MergeRowRange(excel_sheet,"H7","H8"); 
	//工作量
	map.SetValueAt(index,8,COleVariant("工作量(单件)"));
	CExcelOper::MergeColRange(excel_sheet,"I7","M7");
	map.SetValueAt(index+1,8,COleVariant("孔数"));
	map.SetValueAt(index+1,9,COleVariant("弯曲"));
	map.SetValueAt(index+1,10,COleVariant("切角"));
	map.SetValueAt(index+1,11,COleVariant("铲背"));
	map.SetValueAt(index+1,12,COleVariant("刨根"));
	//备注
	map.SetValueAt(index,13,COleVariant("备注"));
	CExcelOper::MergeRowRange(excel_sheet,"N7","N8");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"N7",NULL);
	//添加构件明细数据
	index+=2;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(CXhChar16("%d",index-1)));	//序号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));			//编号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//材质
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//输出钢板规格为：厚度X长度
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			double length=pBomPlate->length;
			if(bom.m_iPlankWeightStatType==2)
				length=pBomPlate->real_area/pBomPlate->wide;	//净面积输出时应根据钢板净面积与宽反推钢板长度
			map.SetValueAt(index,3,COleVariant(CXhChar50("-%.0fX%.0f",pBomPlate->thick,length)));			//设计规格
		}
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->sSpec));			//设计规格
		//map.SetValueAt(index,4,COleVariant(pBomPart->sSpec));			//代用规格
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,5,COleVariant(CXhChar50("%.1f",pBomPart->wide)));
		else
			map.SetValueAt(index,5,COleVariant(CXhChar16("%.1f",pBomPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(pBomPart->GetPartNum()));	//单基数
		if(pBomPart->bWeldPart)
			map.SetValueAt(index,7,COleVariant(CXhChar16("√")));		//电焊
		map.SetValueAt(index,8,COleVariant(CXhChar16("%d",pBomPart->nMSumLs)));		//孔数
		if(IsZhiWan(pBomPart))
			map.SetValueAt(index,9,COleVariant(CXhChar16("√")));		//弯曲
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,10,COleVariant(CXhChar16("√")));	//切角
			if(pBomAngle->bCutBer)
				map.SetValueAt(index,11,COleVariant(CXhChar16("√")));	//铲背
			if(pBomAngle->bCutRoot)
				map.SetValueAt(index,12,COleVariant(CXhChar16("√")));	//刨根
		}
		CXhChar50 sNote;
		GetProcessNotes(pBomPart,sNote);
		map.SetValueAt(index,13,COleVariant(sNote));			//备注
		index++;
	}

	CXhChar16 cell_end("N%d",index+6);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A7",cell_end,COleVariant((long)3));
	CExcelOper::SetRangeValue(excel_sheet,"A7",cell_end,map.var);				//设置表格内容
	CExcelOper::SetRangeBorders(excel_sheet,"A7",cell_end,COleVariant(10.5));	//设置边框及字高
	return 1;
}