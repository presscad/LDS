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
#include "SegI.h"
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

static CXhChar100 GetProcessNotes(BOMPART* pBomPart,bool bIncKaiHeAngle=FALSE)
{
	CXhChar100 notes;
	long nHuoquLineCount=pBomPart->GetHuoquLineCount();
	if(pBomPart->cPartType==BOMPART::PLATE)
		nHuoquLineCount=((PART_PLATE*)pBomPart)->m_cFaceN-1;
	if(nHuoquLineCount>0)	//是否需要制弯
		notes.Append("制弯",' ');
	if(pBomPart->bWeldPart)
		notes.Append("焊接",' ');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->bNeedFillet)		//需要坡口
			notes.Append("坡口",' ');
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				notes.Append("制弯 卷边",' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("两端坡口",' ');
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("一端坡口",' ');
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)		//是否切角
			notes.Append("切角",' ');
		if(pBomAngle->bCutBer)			//是否需要铲背
			notes.Append("铲背",' ');
		if(pBomAngle->bCutRoot)		//是否需要清根
			notes.Append("清根",' ');
		if(pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle)>0)		//是否需要开角
		{
			if(bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
				notes.Append(CXhChar16("开角%.1f°",pBomAngle->wing_angle-90),' ');
			else
				notes.Append("开角",' ');
		}
		if(pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle)>0)			//是否需要合角
		{
			if(bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
				notes.Append(CXhChar16("合角%.1f°",90-pBomAngle->wing_angle),' ');
			else
				notes.Append("合角",' ');
		}
		if(pBomAngle->nPushFlat>0)		//是否压扁
		{
			if(pBomAngle->nPushFlat==0x02)
				notes.Append("中间压扁",' ');
			else
				notes.Append("压扁",' ');
		}
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			notes.Append("带脚钉",' ');
	}
	return notes;
}

static int GetCutAngleNum(PART_ANGLE *pBomAngle)
{
	int nNum=0;
	int alfa = 0;
	if(pBomAngle->cut_wing[0]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[0]==1)		//始端X肢有切肢
			nNum++;
		else if(pBomAngle->cut_wing[0]==2)	//始端Y肢有切肢
			nNum++;
	}
	if(pBomAngle->cut_angle[0][0]>0&&pBomAngle->cut_angle[0][1]>0&&pBomAngle->cut_wing[0]!=1)
	{	//始端X肢有切角
		nNum++;
	}
	if(pBomAngle->cut_angle[1][0]>0&&pBomAngle->cut_angle[1][1]>0&&pBomAngle->cut_wing[0]!=2)
	{	//始端Y肢有切角
		nNum++;
	}
	if(pBomAngle->cut_wing[1]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[1]==1)		//终端X肢有切肢
			nNum++;
		else if(pBomAngle->cut_wing[1]==2)	//终端Y肢有切肢
			nNum++;
	}
	if(pBomAngle->cut_angle[2][0]>0&&pBomAngle->cut_angle[2][1]>0&&pBomAngle->cut_wing[1]!=1)	
	{	//终端X肢有切角
		nNum++;
	}
	if(pBomAngle->cut_angle[3][0]>0&&pBomAngle->cut_angle[3][1]>0&&pBomAngle->cut_wing[1]!=2)
	{	//终端Y肢有切角
		nNum++;
	}
	return nNum;
}

void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{	//计算列数
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
	//插入放样原始材料表
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("放样原始材料表");
	//第一行数据项标题行，需要格式输出
	CStringArray str_arr;
	str_arr.SetSize(29);
	str_arr[0]="段号";str_arr[1]="部件号";str_arr[2]="材质";str_arr[3]="材料";str_arr[4]="规格";
	str_arr[5]="长度";str_arr[6]="宽度";str_arr[7]="厚度";str_arr[8]="单件数量";str_arr[9]="单件重量";	//新增 10-10-12
	str_arr[10]="单件孔数";str_arr[11]="备注";str_arr[12]="电焊";str_arr[13]="火曲";str_arr[14]="切角";
	str_arr[15]="铲背";str_arr[16]="清根";str_arr[17]="打扁";str_arr[18]="开合角";str_arr[19]="钻孔";
	str_arr[20]="坡口";str_arr[21]="割相贯线";str_arr[22]="开槽形式";str_arr[23]="类型";str_arr[24]="边数";
	str_arr[25]="周长";str_arr[26]="表面积㎡";str_arr[27]="各孔径孔数";str_arr[28]="焊接边mm";//新增 10-10-12
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,5.5,4.5,3.0,5.0,5.0,5.0,17.5,2.5,2.5,2.5,2.5,2.5,2.5,4.0,2.5,2.5,4.0,4.0,4.5,2.5,8.0,8.0,18.5,5.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1","AC1",COleVariant((long)3));
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+1,29);
	int index =0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//编号
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);
		map.SetValueAt(index,2,COleVariant(sMat));				//材质
		CExcelOper::SetRangeBackColor(excel_sheet,GetMatColorIndex(sMat),CXhChar16("B%d",index+2),CXhChar16("C%d",index+2));
		map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));		//材料
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));				//规格
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)	//长安提出钢板宽度应根据<净面积/长度>计算所得 wjh-2014.4.09
			map.SetValueAt(index,5,COleVariant(ftol(((PART_PLATE*)pBomPart)->real_area/pBomPart->wide)));		//宽度
		else
			map.SetValueAt(index,5,COleVariant(ftol(pBomPart->length)));	//长度
		map.SetValueAt(index,6,COleVariant(ftol(pBomPart->wide)));		//宽度
		map.SetValueAt(index,7,COleVariant(ftol(pBomPart->thick)));		//厚度
		map.SetValueAt(index,8,COleVariant((long)(pBomPart->GetPartNum())));	//单基数量
		double weight=0;
		if(bom.m_iPlankWeightStatType<=1&&pBomPart->cPartType==BOMPART::PLATE)	//
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			double fMinWeight=CalSteelPlankWei(pPlate->min_area*pPlate->thick);
			weight=ftol(fMinWeight*100)/100.0;
		}
		else
			weight=ftol(pBomPart->fPieceWeight*100)/100.0;
		map.SetValueAt(index,9,COleVariant(CXhChar16("%.2f",weight)));	//单件重量
		map.SetValueAt(index,10,COleVariant((long)pBomPart->nMSumLs));	//单件孔数
		map.SetValueAt(index,11,COleVariant(GetProcessNotes(pBomPart,TRUE)));	//备注
		map.SetValueAt(index,12,pBomPart->bWeldPart?COleVariant("1"):COleVariant(""));	//是否电焊件
		map.SetValueAt(index,13,pBomPart->GetHuoquLineCount()>0?COleVariant("1"):COleVariant(""));	//火曲
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAnge = (PART_ANGLE *)pBomPart;
			map.SetValueAt(index,14,pBomAnge->bCutAngle?COleVariant((long)GetCutAngleNum(pBomAnge)):COleVariant(""));	//切角
			map.SetValueAt(index,15,pBomAnge->bCutBer?COleVariant("1"):COleVariant(""));	//铲背
			map.SetValueAt(index,16,pBomAnge->bCutRoot?COleVariant("1"):COleVariant(""));	//清根
			map.SetValueAt(index,17,pBomAnge->nPushFlat>0?COleVariant((long)pBomAnge->nPushFlat):COleVariant(""));	//压扁
			map.SetValueAt(index,18,pBomAnge->bKaiJiao?COleVariant("1"):COleVariant(""));	//开角
			map.SetValueAt(index,18,pBomAnge->bHeJiao?COleVariant("2"):COleVariant(""));	//合角
			map.SetValueAt(index,19,pBomAnge->thick>=14?COleVariant("1"):COleVariant(""));	//钻孔
		}
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//坡口
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,20,pBomPlate->bNeedFillet?COleVariant("1"):COleVariant(""));
		}
		if(pBomPart->cPartType==BOMPART::TUBE)
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pBomPart;
			//割相贯线
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,21,COleVariant("1"));	//割相贯线
			//开槽类型
			long nType=0;
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
				map.SetValueAt(index,22,COleVariant(nType));
		}
		map.SetValueAt(index,23,COleVariant(pBomPart->GetPartSubTypeName()));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE * pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,24,COleVariant((long)pBomPlate->GetEdgeCount()));			//边数
			map.SetValueAt(index,25,COleVariant(CXhChar16("%g",pBomPlate->fPerimeter)));	//周长
		}
		double fArea=GetPartZincCoatedArea(pBomPart);
		if(fArea>0)
			map.SetValueAt(index,26,COleVariant(CXhChar16("%0.3f",fArea)));	//面积
		//孔数孔径
		char sHoleStr[500]="";
		if(pBomPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		//焊缝长度
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			if(pBomPlate->fWeldLineLen>0)
				map.SetValueAt(index,28,COleVariant(CXhChar16(pBomPlate->fWeldLineLen)));
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBoltAngle=(PART_ANGLE*)pBomPart;
			if(pBoltAngle->fWeldLineLen>0)
				map.SetValueAt(index,28,COleVariant(CXhChar16(pBoltAngle->fWeldLineLen)));
		}
		index++;
	}
	CXhChar16 cell_end("AC%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);	//设置表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",CXhChar16("E%d",index+1),COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框和字体大小
	excel_sheet.ReleaseDispatch();
}

void AddPartGroupSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	//统计行数
	int nRowNum=3;
	LIST_NODE<BOM_WELDPART> *pNode=NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode=NULL;
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext())
			nRowNum++;
	}
	CHashStrList<BOMPART*> hashBomPartPtrByLabel;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
		hashBomPartPtrByLabel.SetValue(pNode->pDataObj->sPartNo,pNode->pDataObj);
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("组焊件明细表");
	//
	CExcelOper::SetMainTitle(excel_sheet,"A1","J1","电焊组合表",20,14,3,2,false);
	CVariant2dArray map(nRowNum,10);
	int index=0;
	map.SetValueAt(index,0,COleVariant("工程名称:"));
	map.SetValueAt(index,6,COleVariant("塔型:"));
	index++;
	map.SetValueAt(index,0,COleVariant("段号"));
	map.SetValueAt(index,1,COleVariant("组件号"));
	map.SetValueAt(index,2,COleVariant("加工组数"));
	map.SetValueAt(index,3,COleVariant("零件号"));
	map.SetValueAt(index,4,COleVariant("材料"));
	map.SetValueAt(index,5,COleVariant("材质"));
	map.SetValueAt(index,6,COleVariant("规格"));
	map.SetValueAt(index,7,COleVariant("长"));
	map.SetValueAt(index,8,COleVariant("宽"));
	map.SetValueAt(index,9,COleVariant("单组件数"));
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","J1");
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		map.SetValueAt(index,0,COleVariant((long)pNode->data.iSeg));		//段号
		map.SetValueAt(index,1,COleVariant(pNode->data.sPartNo));
		map.SetValueAt(index,2,COleVariant((long)pNode->data.nWeldPartNum));//加工组
		int startIndex = index;
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext(),index++)
		{
			BOMPART **ppPart=hashBomPartPtrByLabel.GetValue(pPartRefNode->data.sPartNo);
			if(ppPart==NULL)
				continue;
			BOMPART *pPart=*ppPart;
			map.SetValueAt(index,3,COleVariant(pPart->sPartNo));
			map.SetValueAt(index,4,COleVariant(pPart->GetPartTypeName()));
			map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pPart->cMaterial)));
			map.SetValueAt(index,6,COleVariant(pPart->sSpec));
			map.SetValueAt(index,7,COleVariant(CXhChar16("%.f",pPart->length)));
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.f",pPart->wide)));
			map.SetValueAt(index,9,COleVariant((long)pPartRefNode->data.nPartNum));
		}
		for(int i=0;i<3;i++)
			CExcelOper::MergeRowRange(excel_sheet,CExcelOper::GetCellPos(i,startIndex+2),CExcelOper::GetCellPos(i,index+1));
	}
	CXhChar16 cell_end("J%d",nRowNum);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2",CXhChar16("C%d",nRowNum),COleVariant((long)-4160));
	CExcelOper::SetRangeBorders(excel_sheet,"A3",cell_end,COleVariant(10.5));	//设置边框和字体大小
	excel_sheet.ReleaseDispatch();	
}

static CXhChar16 GetBoltTypeStr(BOMBOLT& bolt)
{
	CXhChar16 sBoltType="";
	if(!bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //普通螺栓
		sBoltType="螺栓";	//"普通螺栓";
	else if(!bolt.bFootNail&&bolt.bTwoCap&&!bolt.bAntiTheft) //双帽螺栓
		sBoltType="螺栓";//"双帽螺栓";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //普通脚钉
		sBoltType="脚钉";	//普通脚钉
	else if(!bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //防盗螺栓
		sBoltType="螺栓";	//"防盗螺栓";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //防盗脚钉
		sBoltType="脚钉";	//"防盗脚钉";
	else if (bolt.IsDianQuan())
		sBoltType="垫片";
	return sBoltType;
}

void AddBomBoltSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{	
	//1.获取BOM
	if(bom.m_nBoltsN<=0)
		return;
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
		return;	//暂时仅支持A~AZ
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("螺栓明细表");
	//2.2 输出材料表
	//2.2.1 添加标题栏
	CStringArray str_arr;
	str_arr.SetSize(4);
	str_arr[0]="名称";str_arr[1]="规格";str_arr[2]="无扣长",str_arr[3]="等级";
	double col_arr[4]={10.0,17.0,7.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//添加段号
	int iCol=4;
	for (SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
	{
		CXhChar50 sCel=CExcelOper::GetCellPos(iCol,1);
		CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant(pSegI->ToString()));
		CExcelOper::SetFontBold(excel_sheet,iCol,1,1);
		iCol++;
	}
	CXhChar50 sCel=CExcelOper::GetCellPos(iCol,1);
	CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant("总计"));
	CExcelOper::SetFontBold(excel_sheet,iCol,1,1);
	//2.2.2 输出材料表
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(nBoltCount,nColCount+4);
	for(int i=0;i<nBoltCount;i++)
	{
		BOMBOLT bomBolt;
		CopyBomBoltProp(boltArr[i].bomBolt,bomBolt);
		bomBolt.AddPart(boltArr[i].bomBolt.GetPartNum());

		CXhChar16 sBoltType=GetBoltTypeStr(bomBolt);
		if(!bomBolt.IsDianQuan())
		{
			map.SetValueAt(index,0,COleVariant(sBoltType));
			CXhChar200	sGuiGe("M%d*%d",bomBolt.d,bomBolt.L);
			if(bomBolt.bFootNail||bomBolt.bTwoCap)
				sGuiGe.Append("双帽一垫");
			else
				sGuiGe.Append("一帽一垫");
			map.SetValueAt(index,1,COleVariant(sGuiGe));
			map.SetValueAt(index,2,COleVariant((long)bomBolt.nNoThreadlen));//无扣长（lg）
			CXhChar16 sGrade("%.1f",bomBolt.grade);
			map.SetValueAt(index,3,COleVariant(sGrade));	//螺栓等级
		}
		else	
		{
			CExcelOper::SetFontBold(excel_sheet,0,index+7,4);
			map.SetValueAt(index,0,COleVariant("垫片"));
			CXhChar16 str("-%d(%.1f)",bomBolt.L,bomBolt.d+1.5);
			map.SetValueAt(index,1,COleVariant(str));
			map.SetValueAt(index,3,COleVariant("Q235"));
		}
		iCol=4;
		for(SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			long *pCount=boltArr[i].m_hashNumBySegI.GetValue(pSegI->iSeg);
			if(pCount)
				map.SetValueAt(index,iCol,COleVariant(*pCount));
			iCol++;
		}
		CXhChar50 sPartNum("=SUM(E%d:%s)",index+2,(char*)CExcelOper::GetCellPos(iCol-1,index+2));
		map.SetValueAt(index,iCol,COleVariant(sPartNum));
		index++;
	}
	//
	CXhChar16 sTempCell("%s%d",(char*)sMaxColCell,index+1);
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A1");
	//设置表格内容
	CExcelOper::SetRangeValue(excel_sheet,"A2",sTempCell,map.var);
	//设置对齐
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+1);  //
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",sTempCell,COleVariant((long)3));
	//设置边框
	CExcelOper::SetRangeBorders(excel_sheet,"A1",sTempCell,COleVariant(10.5));
	//设置颜色
	index=2;
	for (int i=0;i<nBoltCount;i++)
	{
		CExcelOper::SetFontBold(excel_sheet,0,index,4);
		if(!boltArr[i].bomBolt.IsDianQuan())
		{
			if(16==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,42,index,0,3);
			else if(20==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,43,index,0,3);
		}
		index++;
	}
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
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(3);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	//1 构件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartSpecTblSheet(bom,pWorksheet);
	//2 螺栓明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddBomBoltSumMatTbl(bom,pWorksheet);
	//3 组焊件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 3));
	AddPartGroupSumMatTbl(bom,pWorksheet);

	excel_sheets.DetachDispatch();
	return 0;
}
