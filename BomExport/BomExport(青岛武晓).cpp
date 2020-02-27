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
	return 8;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

//工艺类型
const static BYTE TYPE_WELD			=1;	//焊接
const static BYTE TYPE_HUOQU		=2;	//火曲
const static BYTE TYPE_CUT_ANGLE	=3;	//切角
const static BYTE TYPE_CUT_BER		=4;	//铲背
const static BYTE TYPE_CUT_ROOT		=5;	//清根
const static BYTE TYPE_PUSH_FLAT	=6;	//压扁
const static BYTE TYPE_KAIHE_ANGLE	=7;	//开合角
const static BYTE TYPE_DRILL_HOLE	=8;	//钻孔
const static BYTE TYPE_FILLET		=9;	//打坡口
const static BYTE TYPE_CUT_ARC		=10;//切弧

static int IsNeedProcess(BOMPART* pBomPart,BYTE process_type)
{
	if(process_type==TYPE_DRILL_HOLE && pBomPart->IsDrillHole())
		return 1;	//钻孔
	else if(process_type==TYPE_WELD && pBomPart->bWeldPart)
		return 1;	//焊接
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
		if(process_type==TYPE_CUT_ARC && pBomPlate->GetArcEdgeCount()>0 )
			return 1;	//切弧
		else if(process_type==TYPE_FILLET && pBomPlate->bNeedFillet)
			return 1;	//打坡口
		else if(process_type==TYPE_HUOQU && pBomPlate->m_cFaceN>1)
			return 1;	//火曲板
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE* pBomJg=(PART_ANGLE*)pBomPart;
		if(process_type==TYPE_CUT_BER && pBomJg->bCutBer)
			return 1;	//铲背
		else if(process_type==TYPE_CUT_ROOT && pBomJg->bCutRoot)
			return 1;	//清根
		else if(process_type==TYPE_CUT_ANGLE && pBomJg->bCutAngle)
			return 1;	//切角
		else if(process_type==TYPE_PUSH_FLAT && pBomJg->nPushFlat>0)
			return 1;	//压扁
		else if(process_type==TYPE_KAIHE_ANGLE)
		{	//开合角
			if(pBomJg->bHeJiao)
				return 2;
			else if(pBomJg->bKaiJiao)
				return 1;
		}
		else if(process_type==TYPE_HUOQU)
		{
			int nHuoquType=pBomJg->GetHuoquType();
			if(pBomJg->GetHuoquLineCount()>0&&nHuoquType>=1&&nHuoquType<=4)
				return 1;	//制弯角钢
		}
	}
	return 0;
}
typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
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
		sprintf(sSpec,"%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart)
{
	CXhChar50 str;
	char sSpec[30]="";
	if(pBomPart->cPartType==BOMPART::ANGLE)	//标注角钢规格∠
	{
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
		str.Printf("∠%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetNoteParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam(pBomPart->sNotes);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		//垫板
		if(pPlate->siSubType=BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
		{	//单孔螺栓垫板
			CXhChar16 sDianBan;
			if(pPlate->m_arrBoltRecs[0].d==16)
				sDianBan.Printf("φ17.5*%d",pBomPart->GetPartNum());
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Printf("φ21.5*%d",pBomPart->GetPartNum());
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Printf("φ25.5*%d",pBomPart->GetPartNum());
			sProcessParam.Append(sDianBan);
			//有孔垫板不需要再进行焊接
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		//卷边
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
			{
				sProcessParam.Append(CXhChar50("卷边%dmm",pVertex->data.manu_space),' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("带脚钉",' ');
	}
	return sProcessParam;
}
CXhChar100 GetTypeParam(BOMPART* pBomPart)
{
	CXhChar100 sSpecialType;

	return sSpecialType;
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
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.SetName("角钢零构件清单");	
	//添加实时数据标题
	CVariant2dArray map(bom.m_nPartsN+1,25);
	int index=0;
	map.SetValueAt(index,0,COleVariant("段名"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"A1");
	map.SetValueAt(index,1,COleVariant("部件号"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("材质"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"C1");
	map.SetValueAt(index,3,COleVariant("材料"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"D1");
	map.SetValueAt(index,4,COleVariant("规格"));
	CExcelOper::SetRangeColWidth(excel_sheet,9.5,"E1");
	map.SetValueAt(index,5,COleVariant("长度(mm)"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"F1");
	map.SetValueAt(index,6,COleVariant("宽度(mm)"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"G1");
	map.SetValueAt(index,7,COleVariant("单段数"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"H1");
	map.SetValueAt(index,8,COleVariant("单件重量(kg)"));
	CExcelOper::SetRangeColWidth(excel_sheet,12.5,"I1");
	map.SetValueAt(index,9,COleVariant("总重量(kg)"));
	CExcelOper::SetRangeColWidth(excel_sheet,11,"J1");
	map.SetValueAt(index,10,COleVariant("单件孔数"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"K1");
	map.SetValueAt(index,11,COleVariant("备注"));
	CExcelOper::SetRangeColWidth(excel_sheet,14,"L1");
	map.SetValueAt(index,12,COleVariant("类型"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"M1");
	map.SetValueAt(index,13,COleVariant("电焊"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"N1");
	map.SetValueAt(index,14,COleVariant("火曲"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"O1");
	map.SetValueAt(index,15,COleVariant("切角切肢"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"P1");
	map.SetValueAt(index,16,COleVariant("铲背"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"Q1");
	map.SetValueAt(index,17,COleVariant("清根"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"R1");
	map.SetValueAt(index,18,COleVariant("打扁"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"S1");
	map.SetValueAt(index,19,COleVariant("开合角"));
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"T1");
	map.SetValueAt(index,20,COleVariant("钻孔"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"U1");
	map.SetValueAt(index,21,COleVariant("开槽形式"));
	CExcelOper::SetRangeColWidth(excel_sheet,9,"V1");
	map.SetValueAt(index,22,COleVariant("打坡口"));
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"W1");
	map.SetValueAt(index,23,COleVariant("割弧"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"X1");
	map.SetValueAt(index,24,COleVariant("是否专用"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"Y1");
	//添加实时数据内容
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//部件号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//材质
		map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));				//材料
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));						//规格
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//长度
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//宽度
		else
			map.SetValueAt(index,6,COleVariant("0"));
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//加工数量
		map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单件重量
		CXhChar16 sFormula("=H%d*I%d",index+1,index+1);
		map.SetValueAt(index,9,COleVariant(sFormula));	//通过公式计算总重
		map.SetValueAt(index,10,COleVariant(pBomPart->nMSumLs));						//孔数
		map.SetValueAt(index,11,COleVariant(GetNoteParam(pBomPart)));				//备注
		map.SetValueAt(index,12,COleVariant(GetTypeParam(pBomPart)));				//类型
		map.SetValueAt(index,13,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_WELD))));		//电焊
		map.SetValueAt(index,14,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_HUOQU))));		//火曲
		map.SetValueAt(index,15,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ANGLE))));	//切角
		map.SetValueAt(index,16,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_BER))));		//铲背
		map.SetValueAt(index,17,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ROOT))));	//清根
		map.SetValueAt(index,18,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_PUSH_FLAT))));	//打扁
		map.SetValueAt(index,19,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_KAIHE_ANGLE))));	//开合角
		map.SetValueAt(index,20,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_DRILL_HOLE))));	//钻孔
		map.SetValueAt(index,21,COleVariant(""));														//开槽形式
		map.SetValueAt(index,22,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_FILLET))));		//打坡口
		map.SetValueAt(index,23,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ARC))));		//割弧
		map.SetValueAt(index,24,COleVariant("1"));	//是否专用
		//
		index++;
	}
	CXhChar16 cell_end("Y%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框和字体大小
	//
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 0;
}