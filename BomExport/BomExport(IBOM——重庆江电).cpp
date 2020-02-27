// BomExport.cpp : 定义 DLL 的初始化例程。
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

const int COL_COUNT=6;
//索引号从1开始，0表示没有对应列
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=6;
		colIndexArr[0]=2;	//件号
		colIndexArr[1]=4;	//规格
		colIndexArr[2]=5;	//材质
		colIndexArr[3]=8;	//数量
		colIndexArr[4]=18;	//单重
		colIndexArr[5]=6;	//长度
		return 6;
	}
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
		sprintf(sSpec,"%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%s*%s",(char*)CXhChar16(pBomPart->wide),(char*)CXhChar16(pBomPart->thick));
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Remove('[');
		sSpec.Remove('C');
	}
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
		{
			if(bNeedPlateW)
				sSpec.Printf("%.f*%.f",pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("%.f",pBomPart->thick);
		}
		else
			sSpec.Copy("");
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
		if(pPlate->siSubType==BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
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

static CXhChar16 GetPartTypeName(BOMPART *pPart,BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
	if(pPart->cPartType==BOMPART::PLATE)		//钢板
	{
		sPartType=bSymbol?"-":"钢板";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
			sPartType=bSymbol?"":"槽型插板";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U)
			sPartType=bSymbol?"":"U型插板";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
			sPartType=bSymbol?"":"十字插板";
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType=bSymbol?"":"法兰";
	}
	else if(pPart->cPartType==BOMPART::ANGLE)	//角钢
		sPartType=bSymbol?"L":"角钢";
	else if(pPart->cPartType==BOMPART::TUBE)	//钢管
		sPartType=bSymbol?"φ":"钢管";
	else if(pPart->cPartType==BOMPART::SLOT)	//槽钢
		sPartType=bSymbol?"[":"槽钢";
	else if(pPart->cPartType==BOMPART::FLAT)	//扁钢
		sPartType=bSymbol?"-":"扁钢";
	else if(pPart->cPartType==BOMPART::ROUND)	//圆钢
		sPartType=bSymbol?"φ":"圆钢";
	else if(pPart->cPartType==BOMPART::ACCESSORY)
	{
		if(pPart->siSubType==BOMPART::SUB_TYPE_LADDER)
			sPartType=bSymbol?" ":"爬梯";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			sPartType=bSymbol?"-":"钢格栅";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE)
			sPartType=bSymbol?" ":"拉索构件";
		else
			sPartType=bSymbol?" ":"附件";
	}
	return sPartType;
}

static char GetPartMaterialChar(BOMPART *pBomPart)
{
	if(pBomPart==NULL)
		return '0';
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			//if(cMaterial=='D')
			//	cMaterial='P';
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
		}
		else if(pBomPart->siSubType!=0)
		{
			cMaterial='0';
		}
		char cRealMat=cMaterial;
		if(cRealMat=='D')
			cRealMat='P';
		if(QuerySteelMatMark(cMaterial).GetLength()<=0)
			cMaterial='0';
	}
	return cMaterial;
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
	//添加实时数据的列标题
	int COL_COUNT=10;
	double col_arr[10]={8.0,12.0,10.0,18.0,10.0,
						8.0,10.0,10.0,10.5,10.0};
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="段名";str_arr[1]="构件编号";str_arr[2]="材料名称";str_arr[3]="规格";str_arr[4]="材质";
	str_arr[5]="单基件数";str_arr[6]="长度";str_arr[7]="宽度";str_arr[8]="重量";str_arr[9]="备注";
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+2,COL_COUNT);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		CXhChar50 sSpecialSpec; 
		SEGI segI=SEGI(pBomPart->iSeg);
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if( pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
			{
				sSpecialSpec.Copy(pBomPart->sPartNo);
				if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
					pBomPart->iSeg=0;
			}
		}
		else if(pBomPart->cPartType==BOMPART::ACCESSORY||
				pBomPart->cPartType==BOMPART::SUB_TYPE_STAY_ROPE)	//错误将设置构件类型
		{
			sSpecialSpec.Copy(pBomPart->sPartNo);
			if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
				pBomPart->iSeg=0;
			//if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			//	continue;	//钢格栅单独出一张表
		}
		if(pBomPart->iSeg!=0)
			map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//段号
		if(sSpecialSpec.GetLength()<=0)
			map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//零件编号
		map.SetValueAt(index,2,COleVariant(GetPartTypeName(pBomPart,TRUE)));	//材料
		if(sSpecialSpec.GetLength()>0)
			map.SetValueAt(index,3,COleVariant(sSpecialSpec));					//设计规格
		else
			map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));			//设计规格
		char cMaterial=GetPartMaterialChar(pBomPart);
		if(cMaterial!='0')
			map.SetValueAt(index,4,COleVariant(CXhChar16("%c",cMaterial)));		//材质
		if(cMaterial=='P'||cMaterial=='D')	//Q420字体进行加粗
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+2),NULL);	//字体加粗
		if(cMaterial=='H')
			CExcelOper::SetRangeBackColor(excel_sheet,CExcelOper::COLOR_ID_YELLOW,CXhChar16("E%d",index+2));
		map.SetValueAt(index,5,COleVariant(pBomPart->GetPartNum()));					//加工数量
		if(pBomPart->length>0)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));//长度
		if((pBomPart->cPartType!=BOMPART::ANGLE&&pBomPart->cPartType!=BOMPART::TUBE)&&pBomPart->wide>0)
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//宽度
		if(pBomPart->cPartType==BOMPART::PLATE)	//钢板考虑净面积重量
		{
			double fWeight=((PART_PLATE*)pBomPart)->fPieceNetWeight;
			if(fWeight<=0)
				fWeight=pBomPart->fPieceWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",fWeight)));				//单件重量
		}
		else
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单件重量
		index++;
	}
	CXhChar16 cell_end("J%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(16.0));	//设置边框和字体大小
	CExcelOper::SetRangeRowHeight(excel_sheet,20.25,"A1",cell_end);
	excel_sheet.ReleaseDispatch();
}

void AddSteelGratingTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("钢格板");
	excel_sheet.ReleaseDispatch();
	return;
	/*
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
	
	//添加实时数据的列标题
	int COL_COUNT=6;
	double col_arr[6]={8.0,12.0,10.0,18.0,10.0,8.0};
	//添加实时数据
	CVariant2dArray map(bom.m_nPartsN+2,COL_COUNT);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->cPartType!=BOMPART::ACCESSORY&&pBomPart->siSubType!=BOMPART::SUB_TYPE_STEEL_GRATING)
			continue;
		map.SetValueAt(index,0,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//零件编号
		map.SetValueAt(index,1,COleVariant(GetSpecStr(pBomPart)));						//设计规格
		map.SetValueAt(index,2,COleVariant(CXhChar16("%d*%d",ftol(pBomPart->wide),ftol(pBomPart->length))));
		map.SetValueAt(index,3,COleVariant(CXhChar16("%d",pBomPart->GetPartNum())));	//数量
		map.SetValueAt(index,4,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单重
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.1f",pBomPart->fPieceWeight*pBomPart->GetPartNum())));	//总重
		index++;
	}
	if(index>0)
	{
		CXhChar16 cell_end("F%d",index);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//填充表格内容
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
		CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(16.0));	//设置边框和字体大小
		CExcelOper::SetRangeRowHeight(excel_sheet,20.25,"A1",cell_end);
	}
	excel_sheet.ReleaseDispatch();*/
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
	//2.钢格栅
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddSteelGratingTblSheet(bom,pWorksheet);
	excel_sheets.ReleaseDispatch();
	return 0;
}