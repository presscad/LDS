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

short CutAngleN(PART_ANGLE *pBomAngle)
{
	short nCutCount=0;
	for(int i=0;i<2;i++)
	{	//0.始端 1.终端
		if(pBomAngle->cut_wing[i]>0&&pBomAngle->cut_wing_para[i][0]>0)
		{	//切肢
			if(pBomAngle->cut_wing_para[i][1]==0)
				nCutCount+=2;		//切空肢(记2刀)
			else 
				nCutCount+=3;		//切肢(记3刀)
		}
		if(pBomAngle->cut_angle[i*2][0]>0)		//X肢切角(记1刀)
			nCutCount++;
		if(pBomAngle->cut_angle[i*2+1][0]>0)	//Y肢切角(记1刀)
			nCutCount++;
	}
	return nCutCount;
}
static int GetProcessNotes(BOMPART* pBomPart,char *notes,int nTaNum,bool bIncKaiHeAngle=FALSE)
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
		if (pBomPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{	
			int nBoltNum=pBomPart->nM16Ls+pBomPart->nM20Ls+pBomPart->nM24Ls;
			
			if(pBomPart->nM16Ls>0)
				sprintf(ss,"%s %d-%.1f垫片",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
			if(pBomPart->nM20Ls>0)
				sprintf(ss,"%s %d-%.1f垫片",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
			if(pBomPart->nM24Ls>0)
				sprintf(ss,"%s %d-%.1f垫片",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
		}
		if(pBomPart->bWeldPart)
			sprintf(ss,"%s 焊接",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s 卷边",ss);
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
	if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)//是否切角
		{
			if(pBomAngle->cut_wing[0]!=0||pBomAngle->cut_wing[1]!=0)
				sprintf(ss,"%s 切肢角",ss);
			for (int i=0;i<4;i++)
			{
				if (pBomAngle->cut_angle[i][0]!=0||pBomAngle->cut_angle[i][1]!=0)
				{
					sprintf(ss,"%s 切角",ss);
					break;
				}
			}
		}
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
	char cell_end[8];
	CExcelOper::MergeColRange(excel_sheet,"A1","Q1");
	CExcelOper::SetMainTitle(excel_sheet,"A1","Q1","综合清单");
	//第二行
	CXhChar100 sPrjName="工程名称: ";
	sPrjName.Append(bom.m_sPrjName);
	CExcelOper::MergeColRange(excel_sheet,"A2","Q2");
	CExcelOper::SetRangeValue(excel_sheet,"A2","A2",COleVariant(sPrjName));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2","Q2",COleVariant((long)1));
	//第三行
	//塔型
	CExcelOper::MergeColRange(excel_sheet,"A3","Q3");
	CXhChar100 sTowerType="塔型：";
	sTowerType.Append(bom.m_sTowerTypeName);
	CExcelOper::SetRangeValue(excel_sheet,"A3","A3",COleVariant(sTowerType));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A3","Q3",COleVariant((long)1));
	
	CStringArray str_arr;
	str_arr.SetSize(17);
	str_arr[0]="编号";str_arr[1]="材质";str_arr[2]="规格";str_arr[3]="长度";
	str_arr[4]="数量";str_arr[5]="单重";str_arr[6]="总重";str_arr[7]="基数";
	str_arr[8]="加工总量";str_arr[9]="焊";str_arr[10]="M16";str_arr[11]="M20";
	str_arr[12]="M24";str_arr[13]="M_X";str_arr[14]="开合";str_arr[15]="切角";
	str_arr[16]="特殊工艺";
	double col_arr[17]={5.0,5.0,5.0,7.5,5.0,
						5.5,5.0,5.0,5.0,3,
						3,3,3.5,3.5,5.0,
						5.0,13};
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CVariant2dArray map(bom.m_nPartsN,22);
	int index=0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		//编号
		map.SetValueAt(index,0,COleVariant(pPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));
		//材质
		CXhChar16 sMaterial=QuerySteelMatMark(pPart->cMaterial);
		if(strcmp(sMaterial,"Q235")!=0) 
			map.SetValueAt(index,1,COleVariant(QuerySteelMatMark(pPart->cMaterial)));
		//规格
		map.SetValueAt(index,2,COleVariant(pPart->sSpec));
		//长度
		map.SetValueAt(index,3,COleVariant(CXhChar16("%.2f",pPart->length)));
		//数量
		map.SetValueAt(index,4,COleVariant((long)pPart->GetPartNum()));
		
		double real_weight=0,min_rect_weight=0;
		if(pPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			real_weight=ftol(CalSteelPlankWei(pPlate->real_area*pPlate->thick)*100)/100.0;
			min_rect_weight=ftol(CalSteelPlankWei(pPlate->min_area*pPlate->thick)*100)/100.0;
		}
		else
			min_rect_weight=real_weight=ftol(pPart->fPieceWeight*100)/100.0;
		//单重
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.2f",min_rect_weight)));
		//总重
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.2f",pPart->GetPartNum()*min_rect_weight)));
		//基数
		map.SetValueAt(index,7,COleVariant((long)bom.m_nTaNum));
		CXhChar50 sStartCell=CExcelOper::GetCellPos(4,index+5);
		CXhChar50 sEndCell=CExcelOper::GetCellPos(7,index+5);
		CXhChar100 str="=";
		str.Append(sStartCell);
		str.Append("*");
		str.Append(sEndCell);
		//加工总量
		map.SetValueAt(index,8,COleVariant(str));
		//焊
		if(pPart->bWeldPart)
			map.SetValueAt(index,9,COleVariant("√"));
		//M16
		if(pPart->nM16Ls!=0)
			map.SetValueAt(index,10,COleVariant(pPart->nM16Ls));
		//M20
		if(pPart->nM20Ls!=0)
			map.SetValueAt(index,11,COleVariant(pPart->nM20Ls));
		//M24
		if(pPart->nM24Ls!=0)
			map.SetValueAt(index,12,COleVariant(pPart->nM24Ls));
		//M_X
		if(pPart->nMXLs!=0)
			map.SetValueAt(index,13,COleVariant(pPart->nMXLs));
		if (pPart->cPartType==BOMPART::ANGLE)
		{	//开合
			PART_ANGLE *pBomAngle=(PART_ANGLE*)pPart;
			short siKaiHeJiaoCount=GetKaiHeJiaoN(pBomAngle);
			if(siKaiHeJiaoCount>0)
				map.SetValueAt(index,14,COleVariant((long)siKaiHeJiaoCount));
			//切角
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,15,COleVariant(CutAngleN(pBomAngle)));
		}
		//特殊供工艺
		CXhChar100 sNots="";
		GetProcessNotes(pPart,sNots,bom.m_nTaNum);
		map.SetValueAt(index,16,COleVariant(sNots));
		index++;
	}
	//设置右对齐
	sprintf(cell_end,"Q%d",index+4);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",cell_end,COleVariant((long)2));
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A5",cell_end,map.var);
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	
	return 1;
}