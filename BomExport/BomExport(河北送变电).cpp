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
				sprintf(ss,"%s 卷边%d",ss,abs(pNode->data.manu_space));
				sprintf(ss,"%s 制弯",ss);
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
		if(pBomAngle->bCutAngle)		//是否切角
			sprintf(ss,"%s 切角",ss);
		if(pBomAngle->bCutBer)			//是否需要铲背
			sprintf(ss,"%s 铲背",ss);
		if(pBomAngle->bCutRoot)		//是否需要清根
			sprintf(ss,"%s 清根",ss);
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
	
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("放样原始材料表");
	int index=0;
	CVariant2dArray map(bom.m_nPartsN+2,18);
	char cell_start[8];
	char cell_end[8];
	CXhChar50 sTitle;
	strcpy(sTitle,"XXX公司铁塔原始材料表");
	CExcelOper::SetMainTitle(excel_sheet,"A1","Q1",sTitle);	
	//第二行
	//塔型
	map.SetValueAt(index,0,COleVariant("塔型"));
	strcpy(cell_start,"B2");
	strcpy(cell_end,"F2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
	map.SetValueAt(index,1,COleVariant("XT")); 
	// 工程名称
	map.SetValueAt(index,6,COleVariant("工程名称"));
	// 填写工程名称数据
	strcpy(cell_start,"H2");
	strcpy(cell_end,"K2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
	map.SetValueAt(index,7,COleVariant(bom.m_sPrjName));
	// 书写基数项
	map.SetValueAt(index,11,COleVariant("基数"));
	map.SetValueAt(index,12,COleVariant((long)bom.m_nTaNum));
	// 书写螺栓孔数量项
	strcpy(cell_start,"O2");
	strcpy(cell_end,"R2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end); 
	map.SetValueAt(index,14,COleVariant("螺栓孔数量"));
	index++;
	//第二行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(18);
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("编号"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A3","B3");
	map.SetValueAt(index,2,COleVariant("规格"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"C3","C3");
	map.SetValueAt(index,3,COleVariant("特殊标记"));
	map.SetValueAt(index,4,COleVariant("单件净重"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"E3",NULL);
	map.SetValueAt(index,5,COleVariant("材质"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"F3",NULL);
	map.SetValueAt(index,6,COleVariant("长度"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"G3",NULL);
	map.SetValueAt(index,7,COleVariant("单基数"));
	map.SetValueAt(index,8,COleVariant("总件数"));
	map.SetValueAt(index,9,COleVariant("单根重"));
	map.SetValueAt(index,10,COleVariant("总重"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.5,"H3","K3");
	map.SetValueAt(index,11,COleVariant("特殊工艺"));
	CExcelOper::SetRangeColWidth(excel_sheet,13,"L3",NULL);
	map.SetValueAt(index,12,COleVariant("焊缝边长"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.2,"M3",NULL);
	map.SetValueAt(index,13,COleVariant("边数"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"N3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,5,"O2","R2");
	map.SetValueAt(index,14,COleVariant("M16"));
	map.SetValueAt(index,15,COleVariant("M20"));
	map.SetValueAt(index,16,COleVariant("M24"));
	map.SetValueAt(index,17,COleVariant("M_X"));
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet,2.5,"O3","R3");
	index++;
	//合并单元格
	if(BomArr.GetSize()<=0)
		return FALSE;
	int nTempIndex=index;
	ARRAY_LIST<int> indexList;
	SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();
	BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
	SEGI preSeg(pBomPart->iSeg);
	indexList.append(index);
	index++;
	for(pNode=BomArr.GetNext();pNode;pNode=BomArr.GetNext())
	{
		pBomPart=(BOMPART*)pNode->pDataObj;
		if (preSeg.iSeg!=pBomPart->iSeg)
		{
			indexList.append(index);
			preSeg.iSeg=pBomPart->iSeg;
		}
		index++;
	}
	if(indexList.GetSize()>0)
		indexList.append(index);
	if(indexList.GetSize()!=1)
	{
		int * pPreIndex=indexList.GetFirst();
		for (int *pIndex=indexList.GetNext();pIndex;pIndex=indexList.GetNext())
		{
			char cell_start[16]="";
			char cell_end[16]="";
			_snprintf(cell_start,15,"A%d",(*pPreIndex)+2);
			_snprintf(cell_end,15,"A%d",(*pIndex)+1);
			(*pPreIndex)=(*pIndex);
			//合并行
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
		}
	}
	index=nTempIndex;
	for(pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//编号
		map.SetValueAt(index,2,COleVariant(pBomPart->sSpec));
		//特殊标记
		//单件净重
		double real_weight=0,min_rect_weight=0;
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			real_weight=ftol(CalSteelPlankWei(pPlate->real_area*pPlate->thick)*100)/100.0;
			min_rect_weight=ftol(CalSteelPlankWei(pPlate->min_area*pPlate->thick)*100)/100.0;
		}
		else
			min_rect_weight=real_weight=ftol(pBomPart->fPieceWeight*100)/100.0;
		map.SetValueAt(index,4,COleVariant(real_weight));

		//材质
		map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));
		//长度
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.1f",pBomPart->length)));
		//单基数
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));
		//总件数
		map.SetValueAt(index,8,COleVariant(bom.m_nTaNum*pBomPart->GetPartNum()));
		//单根重
		map.SetValueAt(index,9,COleVariant(CXhChar16(min_rect_weight)));
		//总重
		if(strcmp(pBomPart->sPartNo,"207")==0)
		{
			int i=0;
			i++;
		}
		CXhChar50 str="";
		_snprintf(str,15,"=E%d*I%d",index+2,index+2);
		map.SetValueAt(index,10,COleVariant(str));
		char ss[200]="";
		GetProcessNotes(pBomPart,ss);
		//特殊工艺
		map.SetValueAt(index,11,COleVariant(ss));
		//焊缝边长
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			CString st="";
			st.Format("%.1f",((PART_PLATE*)pBomPart)->fWeldEdgeLen);
			map.SetValueAt(index,12,COleVariant(st));
			//边数
			map.SetValueAt(index,13,COleVariant(((PART_PLATE*)pBomPart)->GetEdgeCount()));
		}
		else
		{
			map.SetValueAt(index,12,COleVariant((long)0));
			map.SetValueAt(index,13,COleVariant((long)0));
		}
		//M16
		if(pBomPart->nM16Ls!=0)
			map.SetValueAt(index,14,COleVariant(pBomPart->nM16Ls));
		//M20
		if(pBomPart->nM20Ls!=0)
			map.SetValueAt(index,15,COleVariant(pBomPart->nM20Ls));
		//M14
		if(pBomPart->nM24Ls!=0)
			map.SetValueAt(index,16,COleVariant(pBomPart->nM24Ls));
		//M_X
		if(pBomPart->nMXLs!=0)
			map.SetValueAt(index,17,COleVariant(pBomPart->nMXLs));
		index++;
	}
	//设置右对齐
	sprintf(cell_end,"R%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	
	return 1;
}