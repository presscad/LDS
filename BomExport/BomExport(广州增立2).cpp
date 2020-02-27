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
//////////////////////////////////////////////////////////////////////////
//螺栓处理
class CBoltRecord{
public:
	short d,L;
	float grade;
	long nNoThreadlen;	//无扣长
	CXhChar50 sName;	//螺栓系列名称
	CXhChar50 sNote;	//装配备注
	//
	struct SEG_LS_ITEM{
		long iSeg;
		long nCount;
	};
	ATOM_LIST<SEG_LS_ITEM> m_xSegLsList;
public:
	CBoltRecord(){d=L=0;nNoThreadlen=0;}
	~CBoltRecord(){;}
	//
	void Clone(CBoltRecord* pSrcLsRec)
	{
		sName=pSrcLsRec->sName;
		sNote=pSrcLsRec->sNote;
		d=pSrcLsRec->d;
		L=pSrcLsRec->L;
		grade=pSrcLsRec->grade;
		nNoThreadlen=pSrcLsRec->nNoThreadlen;
		//
		m_xSegLsList.Empty();
		for(int i=0;i<pSrcLsRec->m_xSegLsList.GetNodeNum();i++)
			m_xSegLsList.append(pSrcLsRec->m_xSegLsList[i]);
	}
};
typedef CBoltRecord* BOLTRECPTR;
//
static const BYTE TYPE_COMM_BOLT		=1;
static const BYTE TYPE_SAFE_BOLT		=2;
static const BYTE TYPE_TWOCAP_BOLT		=3;
static const BYTE TYPE_COMM_FOOT_NAIL	=4;
static const BYTE TYPE_SAFE_FOOT_NAIL	=5;
void GetBoltDesc(BYTE ciLsType,CXhChar50& sName,CXhChar50& sNote)
{
	if(ciLsType==TYPE_COMM_BOLT)
	{
		sName.Copy("普通螺栓");
		sNote.Copy("单帽单垫");
	}
	if(ciLsType==TYPE_SAFE_BOLT)
	{
		sName.Copy("防盗螺栓");
		sNote.Copy("单帽单垫+防盗帽");
	}
	if(ciLsType==3)
	{
		sName.Copy("双帽螺栓");
		sNote.Copy("双帽单垫");
	}
	if(ciLsType==4)
	{
		sName.Copy("普通脚钉");
		sNote.Copy("双帽双垫");
	}
	if(ciLsType==5)
	{
		sName.Copy("防盗脚钉");
		sNote.Copy("双帽双垫+防盗帽");
	}
}
static int CompareFunc1(const BOLTRECPTR& item1,const BOLTRECPTR& item2)
{
	if(item1->grade>item2->grade)
		return 1;
	else if(item1->grade<item2->grade)
		return -1;
	else
	{
		if(item1->d>item2->d)
			return 1;
		else if(item1->d<item2->d)
			return -1;
		else
		{
			if(item1->L>item2->L)
				return 1;
			else
				return -1;
		}
	}
}
BOOL ExtractBoltInfo(CModelBOM &bom,const BYTE bolt_series,ARRAY_LIST<CBoltRecord> &xBoltRecArr)
{
	CHashStrList<CBoltRecord> xBoltHash;
	for(int i=0;i<bom.m_nBoltsN;i++)
	{
		BOMBOLT bolt=bom.m_pBoltRecsArr[i];
		if(bolt_series==1&&(bolt.bFootNail||bolt.bTwoCap||bolt.bAntiTheft))
			continue;	//普通螺栓
		if(bolt_series==2&&(!bolt.bAntiTheft||bolt.bFootNail||bolt.bTwoCap))	
			continue;	//防盗螺栓
		if(bolt_series==3&&(!bolt.bTwoCap||bolt.bFootNail||bolt.bAntiTheft))	
			continue;	//双帽螺栓
		if(bolt_series==4&&(!bolt.bFootNail||bolt.bTwoCap||bolt.bAntiTheft))	
			continue;	//普通脚钉
		if(bolt_series==5&&(!bolt.bFootNail||!bolt.bAntiTheft||bolt.bTwoCap))
			continue;	//防盗脚钉
		CXhChar50 sName,sNote;
		GetBoltDesc(bolt_series,sName,sNote);
		CXhChar16 sKey("M%dX%d",bolt.d,bolt.L);
		CBoltRecord* pBoltRec=xBoltHash.GetValue(sKey);
		if(pBoltRec==NULL)
		{	
			pBoltRec=xBoltHash.Add(sKey);
			pBoltRec->d=bolt.d;
			pBoltRec->L=bolt.L;
			pBoltRec->sName=sName;
			pBoltRec->sNote=sNote;
			pBoltRec->grade=bolt.grade;
			pBoltRec->nNoThreadlen=bolt.nNoThreadlen;
		}
		CBoltRecord::SEG_LS_ITEM* pSegLsItem=pBoltRec->m_xSegLsList.append();
		pSegLsItem->iSeg=bolt.iSeg;
		pSegLsItem->nCount=bolt.GetPartNum();
	}
	//根据螺栓直径和长度进行排序
	ARRAY_LIST<BOLTRECPTR> boltRecPtrList(0,xBoltHash.GetNodeNum());
	for(CBoltRecord* pBoltRec=xBoltHash.GetFirst();pBoltRec;pBoltRec=xBoltHash.GetNext())
		boltRecPtrList.append(pBoltRec);
	CQuickSort<BOLTRECPTR>::QuickSort(boltRecPtrList.m_pData,boltRecPtrList.GetSize(),CompareFunc1);
	if(boltRecPtrList.GetSize()>0)
	{
		xBoltRecArr.Empty();
		xBoltRecArr.SetSize(0,xBoltHash.GetNodeNum());
		for(int i=0;i<boltRecPtrList.GetSize();i++)
		{
			CBoltRecord* pLsRec=xBoltRecArr.append();
			pLsRec->Clone(boltRecPtrList[i]);
		}
		return TRUE;
	}
	return FALSE;
}
void AddBoltTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	CHashList<SEGI> segTable;
	int nSegNum=GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("螺栓表");
	//添加标题
	CHashStrList<int>	hashSegList;
	int nCount=nSegNum>0?nSegNum+5:6;
	CXhChar16 sCell("%C1",'A'+nCount-1);
	CVariant2dArray map(bom.m_nBoltsN+2,nCount);
	map.SetValueAt(0,0,COleVariant("塔型"));
	map.SetValueAt(0,1,COleVariant(bom.m_sTowerTypeName));
	CExcelOper::MergeColRange(excel_sheet,"B1","D1");
	CExcelOper::SetRangeColWidth(excel_sheet,8,"A1","D1");
	map.SetValueAt(0,4,COleVariant("配置"));
	CExcelOper::MergeRowRange(excel_sheet,"E1","E2");
	CExcelOper::SetRangeColWidth(excel_sheet,20,"E1");
	map.SetValueAt(0,5,COleVariant("配段及数量"));
	CExcelOper::MergeRowRange(excel_sheet,"F1",sCell);
	CExcelOper::SetRangeColWidth(excel_sheet,4,"F1",sCell);
	map.SetValueAt(1,0,COleVariant("名称"));
	map.SetValueAt(1,1,COleVariant("质量等级"));
	map.SetValueAt(1,2,COleVariant("规格"));
	map.SetValueAt(1,3,COleVariant("无扣"));
	int index=0;
	for(SEGI* pSeg=segTable.GetFirst();pSeg;pSeg=segTable.GetNext())
	{
		int iCol=index+5;
		map.SetValueAt(1,iCol,COleVariant(pSeg->ToString()));
		hashSegList.SetValue(pSeg->ToString(),iCol);
		index++;
	}
	//添加数据
	int iRow=2;
	ARRAY_LIST<CBoltRecord> xBoltRecArr;
	for(int i=1;i<6;i++)
	{
		if(!ExtractBoltInfo(bom,i,xBoltRecArr))
			continue;
		for(CBoltRecord* pLsRec=xBoltRecArr.GetFirst();pLsRec;pLsRec=xBoltRecArr.GetNext())
		{
			map.SetValueAt(iRow,0,COleVariant(pLsRec->sName));
			map.SetValueAt(iRow,1,COleVariant(CXhChar16("%.1f",pLsRec->grade)));
			map.SetValueAt(iRow,2,COleVariant(CXhChar16("M%dX%d",pLsRec->d,pLsRec->L)));
			map.SetValueAt(iRow,3,COleVariant(pLsRec->nNoThreadlen));
			map.SetValueAt(iRow,4,COleVariant(pLsRec->sNote));
			for(CBoltRecord::SEG_LS_ITEM* pSegLsItem=pLsRec->m_xSegLsList.GetFirst();pSegLsItem;pSegLsItem=pLsRec->m_xSegLsList.GetNext())
			{
				int* pValue=hashSegList.GetValue(SEGI(pSegLsItem->iSeg).ToString());
				if(pValue)
					map.SetValueAt(iRow,*pValue,COleVariant(pSegLsItem->nCount));
			}
			iRow++;
		}
	}
	CXhChar16 cell_end("%C%d",'A'+nCount-1,iRow);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);	//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)3));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框
	//
	excel_sheet.ReleaseDispatch();
}
//////////////////////////////////////////////////////////////////////////
//构件处理
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
		str.Printf("L%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
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
			CXhChar16 sDianBan;
			if(pPlate->m_arrBoltRecs[0].d==16)
				sDianBan.Copy("∮17.5");
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Copy("∮21.5");
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Copy("∮25.5");
			sProcessParam.Append(CXhChar16("垫板%s",(char*)sDianBan));
			//有孔垫板不需要再进行焊接
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		if(pBomPart->bWeldPart)
			sProcessParam.Append("电焊",' ');
		if(pPlate->bNeedFillet)
			sProcessParam.Append("坡口",' ');
		//火曲
		if(pPlate->m_cFaceN>1)
			sProcessParam.Append("火曲",' ');
		//卷边
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge&&pPlate->m_cFaceN==1)
			{
				sProcessParam.Append("火曲",' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		//焊接
		if(pBomPart->bWeldPart)
			sProcessParam.Append("电焊",' ');
		if(pBomAngle->nPushFlat>0)		//是否压扁
			sProcessParam.Append("打扁",' ');
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append("开角",' ');
		if(pBomAngle->bHeJiao) 
			sProcessParam.Append("合角",' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("切角",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("铲背",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("清根",' ');
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("带钉",' ');
	}
	return sProcessParam;
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
	excel_sheet.SetName("放样原始材料表");	
	//添加实时数据标题
	CVariant2dArray map(bom.m_nPartsN+1,19);
	int index=0;
	map.SetValueAt(index,0,COleVariant("段名"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"A1");
	map.SetValueAt(index,1,COleVariant("编号"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("材质"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"C1");
	map.SetValueAt(index,3,COleVariant("规格"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"D1");
	map.SetValueAt(index,4,COleVariant("宽度"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"E1");
	map.SetValueAt(index,5,COleVariant("长度"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"F1");
	map.SetValueAt(index,6,COleVariant("单基数"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"G1");
	map.SetValueAt(index,7,COleVariant("备注"));
	CExcelOper::SetRangeColWidth(excel_sheet,20,"H1");
	//添加实时数据内容
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//段名
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//部件编号
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//材质
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));						//设计规格
		map.SetValueAt(index,4,COleVariant(""));	//宽度
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,4,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//长度
		map.SetValueAt(index,6,COleVariant(pBomPart->GetPartNum()));					//单基件数
		map.SetValueAt(index,7,COleVariant(GetProcessParam(pBomPart)));					//工艺要求
		//
		index++;
	}
	CXhChar16 cell_end("H%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框和字体大小
	//
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
	LPDISPATCH pWorksheets=NULL,pWorksheet=NULL;
	pWorksheets=CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//1 构件明细表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//2 螺栓表
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddBoltTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}