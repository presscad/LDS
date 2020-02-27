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
	return 9;
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
		sprintf(sSpec,"%sX%sX%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sX%s",sWide,sThick);
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
struct WELD_RELATION{
	CXhChar16 sKeyPartNo;
	struct WELD_SON_PART{
		int index;
		CXhChar16 sPartNo;
		WELD_SON_PART(){index=0;}
	};
	ATOM_LIST<WELD_SON_PART> m_xWeldSonPartArr;
public:
	int GetCount(){return m_xWeldSonPartArr.GetNodeNum();}
	void Append(const char* sPartNo,int iNO)
	{
		WELD_SON_PART* pWeldSonPart=NULL;
		for(pWeldSonPart=m_xWeldSonPartArr.GetFirst();pWeldSonPart;pWeldSonPart=m_xWeldSonPartArr.GetNext())
		{
			if(pWeldSonPart->sPartNo.Equal(sPartNo))
				break;
		}
		if(pWeldSonPart==NULL)
		{
			pWeldSonPart=m_xWeldSonPartArr.append();
			pWeldSonPart->sPartNo.Copy(sPartNo);
			pWeldSonPart->index=iNO;
		}
	}
	int GetWeldPartIndex(const char* sPartNo)
	{
		for(WELD_SON_PART* pWeldPart=m_xWeldSonPartArr.GetFirst();pWeldPart;pWeldPart=m_xWeldSonPartArr.GetNext())
		{
			if(pWeldPart->sPartNo.Equal(sPartNo))
				return pWeldPart->index;
		}
		return 0;
	}
};
void InitWeldPartRelation(ARRAY_LIST<SUPERLIST_NODE<BOMPART>> &BomArr,CHashStrList<WELD_RELATION>& xWeldRelHash)
{
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->cPartType!=BOMPART::PLATE)
			continue;
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(!pVertex->data.m_bWeldEdge || pVertex->data.sWeldMotherPart.GetLength()<=0)
				continue;
			WELD_RELATION* pWeldRel=xWeldRelHash.GetValue(pVertex->data.sWeldMotherPart);
			if(pWeldRel==NULL)
			{
				pWeldRel=xWeldRelHash.Add(pVertex->data.sWeldMotherPart);
				pWeldRel->sKeyPartNo=pVertex->data.sWeldMotherPart;
				pWeldRel->Append(pVertex->data.sWeldMotherPart,1);
			}
			pWeldRel->Append(pPlate->sPartNo,pWeldRel->GetCount()+1);
		}
	}
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart,CHashStrList<WELD_RELATION>& xWeldRelHash)
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
		//焊接子构件
		CXhChar100 sWeldRel;
		WELD_RELATION* pWeldRel=xWeldRelHash.GetValue(pPlate->sPartNo);
		if(pWeldRel)
			sWeldRel.Printf("%s-%d-1",(char*)pPlate->sPartNo,pWeldRel->GetCount());
		//
		CHashStrList<BOOL> hashVertexState;
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(!pVertex->data.m_bWeldEdge||pVertex->data.sWeldMotherPart.GetLength()<=0)
				continue;
			pWeldRel=xWeldRelHash.GetValue(pVertex->data.sWeldMotherPart);
			if(pWeldRel && hashVertexState.GetValue(pVertex->data.sWeldMotherPart)==NULL)
			{
				sWeldRel.Append(CXhChar50("%s-%d-%d",(char*)pVertex->data.sWeldMotherPart,pWeldRel->GetCount(),pWeldRel->GetWeldPartIndex(pPlate->sPartNo)),' ');
				hashVertexState.SetValue(pVertex->data.sWeldMotherPart,TRUE);
			}
		}
		if(sWeldRel.GetLength()>0)
			sProcessParam.Append(sWeldRel,' ');
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

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	CHashStrList<WELD_RELATION> xWeldRelHash;
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
	//初始化钢板之间的焊接关系
	InitWeldPartRelation(BomArr,xWeldRelHash);
	//获取系统当前时间
	CXhChar100 timeStr;
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	timeStr.Printf("%d.%02d.%02d",st.wYear,st.wMonth,st.wDay);
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.SetName("构件明细表");	
	//添加实时数据标题
	CVariant2dArray map(bom.m_nPartsN+1,19);
	int index=0;
	map.SetValueAt(index,0,COleVariant("塔型"));
	CExcelOper::SetRangeColWidth(excel_sheet,9,"A1");
	map.SetValueAt(index,1,COleVariant("段名"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("部件编号"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"C1");
	map.SetValueAt(index,3,COleVariant("材料名称"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"D1");
	map.SetValueAt(index,4,COleVariant("设计规格"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"E1");
	map.SetValueAt(index,5,COleVariant("材质"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"F1");
	map.SetValueAt(index,6,COleVariant("B"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"G1");
	map.SetValueAt(index,7,COleVariant("D"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"H1");
	map.SetValueAt(index,8,COleVariant("L"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"I1");
	map.SetValueAt(index,9,COleVariant("长度"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"J1");
	map.SetValueAt(index,10,COleVariant("单基件数"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"K1");
	map.SetValueAt(index,11,COleVariant("单基孔数"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"L1");
	map.SetValueAt(index,12,COleVariant("单件重量"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"M1");
	map.SetValueAt(index,13,COleVariant("小计重量"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"N1");
	map.SetValueAt(index,14,COleVariant("工艺要求"));
	CExcelOper::SetRangeColWidth(excel_sheet,16,"O1");
	map.SetValueAt(index,15,COleVariant("创建日期"));
	CExcelOper::SetRangeColWidth(excel_sheet,11,"P1");
	map.SetValueAt(index,16,COleVariant("电压等级"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"Q1");
	//添加实时数据内容
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(bom.m_sTowerTypeName));			//塔型
		map.SetValueAt(index,1,COleVariant(SEGI(pBomPart->iSeg).ToString()));//段名
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//部件编号
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,3,COleVariant("板材"));					//材料名称
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));						//设计规格
		if(pBomPart->cMaterial=='S')
			map.SetValueAt(index,5,COleVariant("A3"));	//材质
		else
			map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//材质
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->thick))));			//B
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));			//D
			map.SetValueAt(index,8,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));		//L
			map.SetValueAt(index,9,COleVariant(CXhChar16("%dx%d",ftol(pBomPart->wide),ftol(pBomPart->length))));	//长度
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));		//B
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->thick))));		//D
			map.SetValueAt(index,8,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//L
			map.SetValueAt(index,9,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//长度
		}
		map.SetValueAt(index,10,COleVariant(pBomPart->GetPartNum()));					//单基件数
		map.SetValueAt(index,11,COleVariant(pBomPart->nMSumLs));						//单基孔数
		map.SetValueAt(index,12,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//单件重量
		CXhChar16 sFormula("=K%d*M%d",index+1,index+1);
		map.SetValueAt(index,13,COleVariant(sFormula));									//通过公式计算总重
		map.SetValueAt(index,14,COleVariant(GetProcessParam(pBomPart,xWeldRelHash)));	//工艺要求
		map.SetValueAt(index,15,COleVariant(timeStr));									//创建日期
		map.SetValueAt(index,16,COleVariant(bom.m_sPrjName));							//电压等级
		//
		index++;
	}
	CXhChar16 cell_end("Q%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//填充表格内容
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//设置边框和字体大小
	//
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 0;
}