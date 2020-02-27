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
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(bNeedPlateW)
			sSpec.Printf("%.fx%.f",pBomPart->thick,pBomPart->wide);
		else
			sSpec.Printf("%.f",pBomPart->thick);
	}
	else 
		sSpec.Printf("%s",(char*)pBomPart->sSpec);
	return sSpec;
}
static BOOL IsZhiWan(BOMPART* pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
			return TRUE;	//火曲板
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				return TRUE;	//卷边板
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHeJiao||pBomAngle->bKaiJiao)
			return TRUE;	//有开合角
		int nHuoquType=pBomAngle->GetHuoquType();
		if(pBomPart->GetHuoquLineCount()>0&&nHuoquType>=1&&nHuoquType<=4)
			return TRUE;	//制弯角钢
	}
	return FALSE;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam(pBomPart->sNotes);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		//垫板
		if(pPlate->siSubType=BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
		{	//单孔螺栓垫板
			CXhChar16 sDianBan("垫板%.f",pBomPart->thick);
			if(pPlate->m_arrBoltRecs[0].d==16)
				sDianBan.Append("A");
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Append("B");
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Append("C");
			sProcessParam.Append(sDianBan,' ');
			//有孔垫板不需要再进行焊接
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		if(pBomPart->bWeldPart)
			sProcessParam.Append("焊接",' ');
		if(pPlate->bNeedFillet)
			sProcessParam.Append("坡口",' ');
		//火曲
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
		{
			double fHuoquAngle1=fabs(pPlate->huoQuAngleArr[0]);
			double fHuoquAngle2=fabs(pPlate->huoQuAngleArr[1]);
			CXhChar50 sAngle("火曲%0.1f",fHuoquAngle1>fHuoquAngle2?fHuoquAngle1:fHuoquAngle2);
			sProcessParam.Append(sAngle,' ');
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
		//焊接
		if(pBomPart->bWeldPart)
			sProcessParam.Append("焊接",' ');
		//制弯
		int nHuoquType=pBomAngle->GetHuoquType();
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		CXhChar16 zhiwanArr[4]={"内曲面","外曲面","内曲筋","外曲筋"};
		if(nHuoquLineCount>0&&nHuoquType>=1&&nHuoquType<=4)
		{
			BOMPROFILE_VERTEX::COORD3D prev_len_vec;
			prev_len_vec.z=1;
			double fMinCosa=1;
			pBomAngle->listPolyAngle.EnumFirst();
			for(LIST_NODE<PART_ANGLE::POLY_ANGLE_INFO> *pNode=pBomAngle->listPolyAngle.EnumNext();pNode;pNode=pBomAngle->listPolyAngle.EnumNext())
			{
				BOMPROFILE_VERTEX::COORD3D len_vec(pNode->data.len_vec);
				double cosa=prev_len_vec.x*len_vec.x+prev_len_vec.y*len_vec.y+prev_len_vec.z*len_vec.z;
				if(cosa<fMinCosa)
					fMinCosa=cosa;
				prev_len_vec.x=len_vec.x;
				prev_len_vec.y=len_vec.y;
				prev_len_vec.z=len_vec.z;
			}
			double fAngle=acos(fMinCosa)*DEGTORAD_COEF;
			sProcessParam.Append(CXhChar16("%s%0.1f",(char*)zhiwanArr[nHuoquType-1],fAngle),' ');
		}
		if(pBomAngle->nPushFlat>0)		//是否压扁
			sProcessParam.Append(CXhChar16("压扁"),' ');
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append(CXhChar16("开角%0.1f",pBomAngle->wing_angle-90),' ');
		if(pBomAngle->bHeJiao) 
			sProcessParam.Append(CXhChar16("合角%0.1f",90-pBomAngle->wing_angle),' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("切角",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("铲背",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("清根",' ');
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("带脚钉",' ');
	}
	return sProcessParam;
}

void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
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
	excel_sheet.SetName("放样原始材料表");
	//添加标题行
	CExcelOper::SetMainTitle(excel_sheet,"A1","S1","成都铁塔厂",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A2","S2","铁塔原始材料表",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","B3","工程名:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"C3","K3",bom.m_sPrjName,15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"L3","M3","塔型:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N3","S3",bom.m_sTowerTypeName,15,12,2,2,false);
	//合并行与列
	CExcelOper::MergeRowRange(excel_sheet,"A4","A5");	//段号
	CExcelOper::MergeRowRange(excel_sheet,"B4","B5");	//零件编号
	CExcelOper::MergeRowRange(excel_sheet,"C4","C5");	//材料
	CExcelOper::MergeColRange(excel_sheet,"D4","E4");	//材料规格
	CExcelOper::MergeRowRange(excel_sheet,"F4","F5");	//长度
	CExcelOper::MergeRowRange(excel_sheet,"G4","G5");	//宽度
	CExcelOper::MergeRowRange(excel_sheet,"H4","H5");	//加工数量
	CExcelOper::MergeColRange(excel_sheet,"I4","P4");	//工作量
	CExcelOper::MergeRowRange(excel_sheet,"Q4","Q5");	//加工说明
	CExcelOper::MergeRowRange(excel_sheet,"R4","R5");	//单件重量
	CExcelOper::MergeRowRange(excel_sheet,"S4","S5");	//小计重量
	//添加实时数据的列标题
	double col_arr[19]={4.0,5.0,4.0,7.0,4.5,5.0,5.0,4.0,2.5,4.0,2.5,2.5,2.5,2.5,2.5,2.5,17.0,5.0,5.0};
	CStringArray str_arr;
	str_arr.SetSize(19);
	str_arr[4]=str_arr[9]=str_arr[10]=str_arr[11]=str_arr[12]=str_arr[13]=str_arr[14]=str_arr[15]="";
	str_arr[0]="段号";str_arr[1]="零件编号";str_arr[2]="材料";str_arr[3]="材料规格";
	str_arr[5]="长度(mm)";str_arr[6]="宽度(mm)";str_arr[7]="加工数量";str_arr[8]="工作量(单件)";
	str_arr[16]="加工说明";str_arr[17]="单件重量";str_arr[18]="小计重量";
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,15,"A4","S4");	//设置行高
	str_arr[0]=str_arr[1]=str_arr[2]=str_arr[5]=str_arr[6]=str_arr[7]=str_arr[16]=str_arr[17]=str_arr[18]="";
	str_arr[3]="设计规格";str_arr[4]="材质";
	str_arr[8]="阻焊";str_arr[9]="孔数";str_arr[10]="弯曲";str_arr[11]="切角";
	str_arr[12]="打扁";str_arr[13]="铲背";str_arr[14]="刨角";str_arr[15]="钻孔";
	CExcelOper::AddRowToExcelSheet(excel_sheet,5,str_arr,NULL,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,25.5,"A5","S5");	//设置行高
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4","S5",COleVariant((long)3));
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+2,19);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//段号
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//零件编号
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartTypeName(TRUE)));			//材料
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));						//设计规格
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);
		if(pBomPart->cMaterial!='S')
			map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//材质
		if(pBomPart->cMaterial=='P')	//Q420字体进行加粗
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+6),NULL);	//字体加粗
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));		//长度
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));		//宽度
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//加工数量
		map.SetValueAt(index,8,pBomPart->bWeldPart?COleVariant("*"):COleVariant(""));	//阻焊
		map.SetValueAt(index,9,COleVariant(pBomPart->nMSumLs));							//孔数
		map.SetValueAt(index,10,IsZhiWan(pBomPart)?COleVariant("*"):COleVariant(""));	//弯曲
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			map.SetValueAt(index,11,pBomAngle->bCutAngle?COleVariant("*"):COleVariant(""));	 //切角
			map.SetValueAt(index,12,pBomAngle->nPushFlat>0?COleVariant("*"):COleVariant(""));//打扁
			map.SetValueAt(index,13,pBomAngle->bCutBer?COleVariant("*"):COleVariant(""));	 //铲背
			map.SetValueAt(index,14,pBomAngle->bCutRoot?COleVariant("*"):COleVariant(""));	 //刨角
		}
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			map.SetValueAt(index,14,pBomPlate->bNeedFillet?COleVariant("*"):COleVariant(""));	 //刨角
		}
		map.SetValueAt(index,15,pBomPart->IsDrillHole()?COleVariant("*"):COleVariant(""));	//钻孔
		map.SetValueAt(index,16,COleVariant(GetProcessParam(pBomPart)));				//加工说明
		if(pBomPart->cPartType==BOMPART::PLATE)	//钢板考虑净面积重量
			map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",((PART_PLATE*)pBomPart)->fPieceNetWeight)));	//单件重量
		else
			map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单件重量
		CXhChar16 sFormula("=R%d*H%d",index+6,index+6);
		map.SetValueAt(index,18,COleVariant(sFormula));	//通过公式计算总重
		index++;
	}
	CXhChar16 cell_end("S%d",index+6);
	CExcelOper::SetRangeValue(excel_sheet,"A6",cell_end,map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A6",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	//设置边框和字体大小
	excel_sheet.ReleaseDispatch();
}
void AddMatEstimateTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
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
	excel_sheet.SetName("材料预估");
	//添加标题行
	CExcelOper::SetMainTitle(excel_sheet,"A1","R1","成都铁塔厂",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A2","R2","材料预估表",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","B3","工程名称:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"C3","K3",bom.m_sPrjName,15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"L3","M3","塔型:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N3","R3",bom.m_sTowerTypeName,15,12,2,2,false);
	//添加实时数据的列标题
	CStringArray str_arr;
	str_arr.SetSize(18);
	str_arr[0]="段位";str_arr[1]="零件编号";str_arr[2]="材料名称";str_arr[3]="规格";str_arr[4]="材质";
	str_arr[5]="长度";str_arr[6]="宽度";str_arr[7]="单基件数";str_arr[8]="电焊";str_arr[9]="单件孔数";
	str_arr[10]="火曲";str_arr[11]="切角";str_arr[12]="打扁";str_arr[13]="铲背";str_arr[14]="刨角";
	str_arr[15]="钻孔";str_arr[16]="加工说明";str_arr[17]="单件重量";
	double col_arr[18]={4.0,5.0,4.5,7.0,4.5,5.0,5.0,4.0,2.5,4.0,2.5,2.5,2.5,2.5,2.5,2.5,17.0,5.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,30,"A4","R4");	//设置行高
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4","R4",COleVariant((long)3));	//水平居左
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+2,19);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//段位
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//零件编号
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartTypeName(TRUE)));			//材料名称
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart,TRUE)));					//设计规格
		map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//材质
		if(pBomPart->cMaterial=='P')	//Q420字体进行加粗
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+5),NULL);	//字体加粗
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//长度
		map.SetValueAt(index,6,COleVariant(""));										//宽度
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//单基件数
		map.SetValueAt(index,8,pBomPart->bWeldPart?COleVariant("*"):COleVariant(""));	//电焊
		map.SetValueAt(index,9,COleVariant("0"));										//单件
		map.SetValueAt(index,10,IsZhiWan(pBomPart)?COleVariant("*"):COleVariant(""));	//火曲
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			map.SetValueAt(index,11,pBomAngle->bCutAngle?COleVariant("*"):COleVariant(""));	 //切角
			map.SetValueAt(index,12,pBomAngle->nPushFlat>0?COleVariant("*"):COleVariant(""));//打扁
			map.SetValueAt(index,13,pBomAngle->bCutBer?COleVariant("*"):COleVariant(""));	 //铲背
			map.SetValueAt(index,14,pBomAngle->bCutRoot?COleVariant("*"):COleVariant(""));	 //刨角
		}
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			map.SetValueAt(index,14,pBomPlate->bNeedFillet?COleVariant("*"):COleVariant(""));//刨角
		}
		map.SetValueAt(index,15,pBomPart->IsDrillHole()?COleVariant("*"):COleVariant(""));	//钻孔
		map.SetValueAt(index,16,COleVariant(GetProcessParam(pBomPart)));				//加工说明
		map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单件重量(矩形面积重量)
		index++;
	}
	CXhChar16 cell_end("R%d",index+5);
	CExcelOper::SetRangeValue(excel_sheet,"A5",cell_end,map.var);	//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A5",cell_end,COleVariant((long)2));//水平左对齐
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	//设置边框
	excel_sheet.ReleaseDispatch();
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
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	//1 构件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartSpecTblSheet(bom,pWorksheet);
	//2 材料预估表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddMatEstimateTbl(bom,pWorksheet);
	//
	excel_sheets.ReleaseDispatch();
	return 0;
}