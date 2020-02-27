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
#include "XhCharString.h"

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
	return 13;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}

const int COL_COUNT=7;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=1;
		colIndexArr[0]=2;	//件号
		colIndexArr[1]=3;	//规格
		colIndexArr[2]=0;	//材质
		colIndexArr[3]=5;	//数量
		colIndexArr[4]=6;	//单重
		colIndexArr[5]=4;	//长度
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//构件处理
CXhChar16 _QuerySteelMatMark(char cMat)
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
	else if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}

CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
{
	CXhChar100 sSpec,sSpecialPartSpec;
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			if(cMaterial=='D')
				cMaterial='P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING||
				pBomPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE||
				pBomPart->siSubType==BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//钢格栅
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//拉索构件
			//const static BYTE SUB_TYPE_LADDER		= 20;	//爬梯
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'?_QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("∠%.f×%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("φ%.f×%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
			sSpec.Printf("-%.f",pBomPart->thick);
		else 
			sSpec.Printf("%s",(char*)pBomPart->sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C','[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits=sSpec.Replace("φ"," ");
		hits+=sSpec.Replace("Φ"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		if(hits==2)
		{
			int D=0,d=0;
			sscanf(sSpec,"%d%d",&D,&d);
			sSpec.Printf("φ%d/%d",D,d);
		}
		else 
			sSpec.Copy(pBomPart->sSpec);
	}
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);
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
		//if(pPlate->bNeedFillet||pPlate->thick>=8)	
		//	sNotes.Append("坡口 ");
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
		//if(nHoleCount<=0)
		//	sNotes.Append("无孔 ");
		//else 
		if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
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
void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("图纸构件");
	//CExcelOper::SetRangeColWidth(excel_sheet,12.0,"C1");
	//清空原有内容
	Range excel_usedRange,excel_range;
	excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
	excel_usedRange.Select();
	excel_usedRange.ClearContents();
	
	CXhChar16 cell_start,cell_end;
	int index=0;
	CVariant2dArray map(BomArr.GetSize()+2,17);
	//标题行
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("编号"));
	map.SetValueAt(index,2,COleVariant("材质"));
	map.SetValueAt(index,3,COleVariant("规格"));
	map.SetValueAt(index,4,COleVariant("长度"));
	map.SetValueAt(index,5,COleVariant("单基数"));
	map.SetValueAt(index,6,COleVariant("特殊工艺"));
	index++;
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"B1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,18.0,"D1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.5,"E1","F1");
	CExcelOper::SetRangeColWidth(excel_sheet,30.0,"G1",NULL);
	//
	long n=BomArr.GetSize();
	CXhChar16 cell;
	BYTE part_type=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		SEGI segI(pBomPart->iSeg);
		map.SetValueAt(index,0,COleVariant(segI.ToString()));
		CXhChar16 sPartNo(pBomPart->sPartNo);	//pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)
		map.SetValueAt(index,1,COleVariant(sPartNo));
		//材质
		char cMat=pBomPart->cMaterial;
		//if('P'==cMat)
		//	cMat='D';
		if(cMat!='S')
			map.SetValueAt(index,2,COleVariant(CXhChar16("%C",cMat)));
		map.SetValueAt(index,3,COleVariant(GetBomPartSpec(pBomPart)));	//规格
		//长度
		char str[30];
		double length=pBomPart->length;
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)
			length=((PART_PLATE*)pBomPart)->real_area/pBomPart->wide;	//净面积输出时应根据钢板净面积与宽反推钢板长度
		if(pBomPart->cPartType==BOMPART::PLATE)
			sprintf(str,"%.f*%.f",pBomPart->wide,length);
		else
			sprintf(str,"%.f",length);
		map.SetValueAt(index,4,COleVariant(str));
		map.SetValueAt(index,5,COleVariant((long)pBomPart->GetPartNum()));	//单基数
		//CXhChar200 ss =GetPartNotes(pBomPart);
		//strcat(ss,pBomPart->sNotes);
		//map.SetValueAt(index,6,COleVariant(ss));
		index++;
	}
	//设置构件编号格式
	strcpy(cell_start,"B2");
	sprintf(cell_end,"B%d",index+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//设置段号水平对齐方式 居中对齐 wht 13-03-11
	strcpy(cell_start,"A1");
	sprintf(cell_end,"A%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)3));
	//设置表格内容
	strcpy(cell_start,"A1");
	sprintf(cell_end,"G%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//设置边框
	//CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	CExcelOper::SetRangeRowHeight(excel_sheet,18.75,cell_start,cell_end);
	CExcelOper::SetFontSize(excel_sheet,cell_start,cell_end,14.0);
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
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}