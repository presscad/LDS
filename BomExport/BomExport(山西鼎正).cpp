// BomExport.cpp : 定义 DLL 的初始化例程。
// 湖州飞剑

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "ToolKit.h"
#include "HashTable.h"
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
	return 10;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

//螺栓孔字符串
typedef struct tagHOLE_INFO
{
	int d;					//螺栓直径
	double hole_d_increment;//孔增量
	int nCount;				//该直径螺栓个数
	tagHOLE_INFO() { d = nCount = 0; hole_d_increment = 0; }
}HOLE_INFO;
CXhChar500 GetLsHoleStr(PART_PLATE *pPlate, BOOL bIncUnit/*=TRUE*/)
{
	CXhChar500 sHoleStr;
	if (pPlate->nMSumLs <= 0)
		return sHoleStr;
	int nHoleCount = 0;
	CXhSimpleList<HOLE_INFO> holeList;
	LIST_NODE<HOLE_INFO> *pNode = NULL;
	for (int i = 0; i < pPlate->m_arrBoltRecs.GetSize(); i++)
	{
		for (pNode = holeList.EnumFirst(); pNode; pNode = holeList.EnumNext())
		{
			double dd = pNode->data.d + pNode->data.hole_d_increment - pPlate->m_arrBoltRecs[i].d - pPlate->m_arrBoltRecs[i].hole_d_increment;
			if (dd < 0)
				dd *= -1.0;
			if (dd < 0.0000009)
			{
				pNode->data.nCount++;
				break;
			}
		}
		if (pNode == NULL)
		{
			pNode = holeList.AttachNode();
			pNode->data.d = pPlate->m_arrBoltRecs[i].d;
			pNode->data.hole_d_increment = pPlate->m_arrBoltRecs[i].hole_d_increment;
			pNode->data.nCount = 1;
			nHoleCount++;
		}
	}
	int nCount = 0;
	for (pNode = holeList.EnumFirst(); pNode; pNode = holeList.EnumNext())
	{
		CXhChar50 sCurHole, sD(pNode->data.d + pNode->data.hole_d_increment);
		if (bIncUnit)
			sCurHole.Printf("%dΦ%smm", pNode->data.nCount, (char*)sD);
		else
			sCurHole.Printf("%dΦ%s", pNode->data.nCount, (char*)sD);
		sHoleStr.Append(sCurHole, ' ');
	}
	return sHoleStr;
}

bool __SimplifiedNumString(char *num_str)
{
	if (num_str == NULL)
		return false;
	int str_len = strlen(num_str);
	char *point = NULL;
	for (int i = str_len - 1; i >= 0; i--)
	{
		if (num_str[i] == '.')
		{
			point = &num_str[i];
			break;
		}
	}
	if (point == NULL)
		return true;
	char* cursor = &num_str[str_len - 1];
	while (cursor >= point)
	{
		if (*cursor == '0' || *cursor == '.')
		{
			*cursor = '\0';
			cursor--;
		}
		else
			break;
	}
	if (*num_str == '-'&&*(num_str + 1) == '0'&&*(num_str + 2) == 0)
	{
		*num_str = '0';
		*(num_str + 1) = 0;
	}
	return true;
}
static CXhChar100 GetProcessNotes(BOMPART* pBomPart, bool bIncKaiHeAngle = TRUE)
{
	CXhChar100 notes;
	if (pBomPart == NULL)
		return notes;
	notes.Append(pBomPart->sNotes, ' ');
	long nHuoquLineCount = pBomPart->GetHuoquLineCount();
	if (pBomPart->cPartType == BOMPART::PLATE)
		nHuoquLineCount = ((PART_PLATE*)pBomPart)->m_cFaceN - 1;
	if (pBomPart->cPartType != BOMPART::PLATE && nHuoquLineCount > 0)	//是否需要制弯
	{
		if(strstr(notes,"制弯")==NULL)
			notes.Append("制弯", ' ');	//输出非钢板制弯
	}
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
		if (pPlate->bNeedFillet)		//需要坡口
		{
			if (strstr(notes, "坡口") == NULL)
				notes.Append("坡口", ' ');
		}
		BOOL bRollEdge = FALSE;
		for (LIST_NODE<BOMPROFILE_VERTEX> *pNode = pPlate->listVertex.EnumFirst(); pNode; pNode = pPlate->listVertex.EnumNext())
		{
			if (pNode->data.m_bRollEdge)
			{
				bRollEdge = TRUE;
				notes.Append(CXhChar16(" 制弯 卷边%dmm", abs(pNode->data.manu_space)), ' ');
				break;
			}
		}
		if (!bRollEdge && nHuoquLineCount>0)
		{	//输出钢板制弯度数，取钝角 wht 19-09-29
			notes.Append("制弯", ' ');
			for (int i = 0; i < nHuoquLineCount; i++)
			{
				double fDegree = pPlate->GetHuoquDegree(i);
				if (fDegree < 0)
					fDegree *= -1;
				if (fDegree < 90)
					fDegree = 180 - fDegree;
				CXhChar16 sDegree(" %.1f度", fDegree);
				__SimplifiedNumString(sDegree);
				notes.Append(sDegree, ' ');
			}
		}
		if (pPlate->siSubType == BOMPART::SUB_TYPE_BOLT_PAD)
		{
			notes.Append("垫块", ' ');
			CXhChar500 sHole = GetLsHoleStr(pPlate, FALSE);
			if (sHole.GetLength() > 0)
				notes.Append(sHole, ' ');
		}
	}
	else if (pBomPart->cPartType == BOMPART::TUBE)
	{
		PART_TUBE *pTube = (PART_TUBE*)pBomPart;
		if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("两端坡口", ' ');
		else if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD ||
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("一端坡口", ' ');
	}
	else if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
		if (pBomAngle->bCutAngle)		//是否切角
			notes.Append("切角", ' ');
		if (pBomAngle->bCutBer)			//是否需要铲背
			notes.Append("铲背", ' ');
		if (pBomAngle->bCutRoot)		//是否需要清根
			notes.Append("清根", ' ');
		if (pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle) > 0)		//是否需要开角
		{
			if (bIncKaiHeAngle)	//包括开角度数 wht 11-05-24
				notes.Append(CXhChar16("开角(%.1f°)", pBomAngle->wing_angle - 90), ' ');
			else
				notes.Append("开角", ' ');
		}
		if (pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle) > 0)			//是否需要合角
		{
			if (bIncKaiHeAngle)	//包括合角度数 wht 11-05-24
				notes.Append(CXhChar16("合角(%.1f°)", 90 - pBomAngle->wing_angle), ' ');
			else
				notes.Append("合角", ' ');
		}
		if (pBomAngle->nPushFlat > 0)		//是否压扁
		{
			if (pBomAngle->nPushFlat == 0x02)
				notes.Append("中间压扁", ' ');
			else
				notes.Append("压扁", ' ');
		}
		if (pBomAngle->bHasFootNail)
		{
			if (strstr(notes, "带脚钉") == NULL)
				notes.Append("带脚钉", ' ');
		}
	}
	return notes;
}

CXhChar16 _QuerySteelMatMark(BOMPART *pPart)
{
	CXhChar16 sMatMark;
	if ('H' == pPart->cMaterial)
		sMatMark.Copy("Q345");
	else if ('G' == pPart->cMaterial)
		sMatMark.Copy("Q390");
	else if ('P' == pPart->cMaterial)
		sMatMark.Copy("Q420");
	else if ('T' == pPart->cMaterial)
		sMatMark.Copy("Q460");
	else if ('S' == pPart->cMaterial)
		sMatMark.Copy("Q235");
	else
		sMatMark.Copy(pPart->sMaterial);
	return sMatMark;
}


void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("工件数据");
	//
	int index = 0, nRow = 0;
	nRow = bom.m_nPartsN;
	CVariant2dArray map(nRow + 1, 24);

	map.SetValueAt(index, 0, COleVariant("段名(只读)"));	//段号
	map.SetValueAt(index, 1, COleVariant("部件号"));		//部件编号
	map.SetValueAt(index, 2, COleVariant("材质"));			//材质
	map.SetValueAt(index, 3, COleVariant("规格"));			//设计规格
	map.SetValueAt(index, 4, COleVariant("长度(mm)"));		//长度
	map.SetValueAt(index, 5, COleVariant("宽度"));			//宽度
	map.SetValueAt(index, 6, COleVariant("单段数"));		//单段数
	map.SetValueAt(index, 7, COleVariant("单件重量(kg)"));//单件重量
	map.SetValueAt(index, 8, COleVariant("总重量(kg)"));	//单基重量
	map.SetValueAt(index, 9, COleVariant("单件孔数(个)"));//单件孔数
	map.SetValueAt(index, 10, COleVariant("备注"));		//备注
	map.SetValueAt(index, 11, COleVariant("电焊"));		//焊接
	map.SetValueAt(index, 12, COleVariant("火曲"));		//弯曲
	map.SetValueAt(index, 13, COleVariant("切角切肢"));	//切肢
	map.SetValueAt(index, 14, COleVariant("铲背"));		//铲背
	map.SetValueAt(index, 15, COleVariant("清根"));		//刨角
	map.SetValueAt(index, 16, COleVariant("打扁"));		//压扁
	map.SetValueAt(index, 17, COleVariant("开合角"));		//开角
	map.SetValueAt(index, 18, COleVariant("类型"));
	map.SetValueAt(index, 19, COleVariant("边数"));
	map.SetValueAt(index, 20, COleVariant("周长"));
	map.SetValueAt(index, 21, COleVariant("孔径28~38mm(A)"));
	map.SetValueAt(index, 22, COleVariant("孔径39~50mm(B)"));
	map.SetValueAt(index, 23, COleVariant("孔径50mm以上(C)"));
	//设置列宽
	CExcelOper::SetRangeColWidth(excel_sheet, 11.0, "A1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 9.0, "E1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 13.5, "H1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 11.0, "I1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 13.5, "J1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 55.0, "K1", NULL);
	CExcelOper::SetRangeColWidth(excel_sheet, 15.5, "V1", "X1");
	index++;
	SUPERLIST_NODE<BOMPART> *pPartNode = NULL;
	for(pPartNode=bom.listParts.EnumFirst();pPartNode;pPartNode=bom.listParts.EnumNext())
	{
		BOMPART *pRec = pPartNode->pDataObj;
		map.SetValueAt(index, 0, COleVariant(SEGI(pRec->iSeg).ToString()));	//段号
		map.SetValueAt(index, 1, COleVariant(pRec->GetPartNo(bom.m_biMatCharPosType, bom.m_cMatCharSeparator)));	//件号
		CXhChar16 sMark=_QuerySteelMatMark(pRec);
		if (sMark.EqualNoCase("Q235") || sMark.Equal("Q345"))
			sMark.Append("B");
		map.SetValueAt(index, 2, COleVariant((char*)sMark));	//材质
		CXhChar16 sSpec("%s", (char*)pRec->sSpec);
		if (pRec->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%.0f", pRec->thick);
		map.SetValueAt(index, 3, COleVariant(sSpec));					//设计规格
		map.SetValueAt(index, 4, COleVariant(ftol(pRec->length)));		//长度
		map.SetValueAt(index, 5, COleVariant(ftol(pRec->wide)));		//宽度
		map.SetValueAt(index, 6, COleVariant((long)pRec->GetPartNum()));	//单基数量
		if (bom.m_iPlankWeightStatType <= 1)
			map.SetValueAt(index, 7, COleVariant(ftol(10 * pRec->fPieceWeight) / 10.0));	//单件重量
		else
			map.SetValueAt(index, 7, COleVariant(ftol(10 * pRec->fPieceWeight) / 10.0));	//单件重量
		double djzl = 0;
		if (bom.m_iPlankWeightStatType <= 1)
			djzl = pRec->fPieceWeight*pRec->GetPartNum();
		else
			djzl = pRec->fPieceWeight*pRec->GetPartNum();
		map.SetValueAt(index, 8, COleVariant(ftol(10 * djzl) / 10.0));	//单基重量
		map.SetValueAt(index, 9, COleVariant((long)pRec->nMSumLs));	//单件孔数
		CXhChar100 sNotes = GetProcessNotes(pRec, TRUE);
		map.SetValueAt(index, 10, COleVariant((char*)sNotes));		//备注
		if (pRec->bWeldPart)
			map.SetValueAt(index, 11, COleVariant("1"));	//焊接
		long nHuoquLineCount = pRec->GetHuoquLineCount();
		if (pRec->cPartType == BOMPART::PLATE)
			nHuoquLineCount = ((PART_PLATE*)pRec)->m_cFaceN - 1;
		if (nHuoquLineCount > 0)	//是否需要制弯
			map.SetValueAt(index, 12, COleVariant("1"));	//弯曲
		if (pRec->cPartType == BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle = (PART_ANGLE*)pRec;
			if (pAngle->bCutAngle == 2)
				map.SetValueAt(index, 13, COleVariant("2"));	//切角切肢
			else if (pAngle->bCutAngle != 0)
				map.SetValueAt(index, 13, COleVariant("1"));	//切角切肢
			//else
			//	map.SetValueAt(index, 13, COleVariant("0"));	//切角切肢
			if (pAngle->bCutBer)
				map.SetValueAt(index, 14, COleVariant("1"));	//铲背
			if (pAngle->bCutRoot)
				map.SetValueAt(index, 15, COleVariant("1"));	//清根
			if (pAngle->nPushFlat > 0)
				map.SetValueAt(index, 16, COleVariant("1"));	//压扁
			if (pAngle->bKaiJiao || pAngle->bHeJiao)
				map.SetValueAt(index, 17, COleVariant("1"));	//开角合角
		}
		else if (pRec->cPartType == BOMPART::PLATE)
		{
			PART_PLATE *pPlate = (PART_PLATE*)pRec;
			int nEdgeNum = pPlate->GetEdgeCount();
			if (nEdgeNum > 0)
				map.SetValueAt(index, 19, COleVariant((char*)CXhChar16("%d", nEdgeNum)));				//边数
			if (pPlate->fPerimeter > 0)
				map.SetValueAt(index, 20, COleVariant((char*)CXhChar16("%.0f", pPlate->fPerimeter)));	//周长
		}
		int nCount = pRec->GetBoltOrHoleNum(28, 38);
		if(nCount>0)
			map.SetValueAt(index, 21, COleVariant((char*)CXhChar16("%d", nCount)));	//孔径28~38mm(A)
		nCount = pRec->GetBoltOrHoleNum(39, 50);
		if(nCount>0)
			map.SetValueAt(index, 22, COleVariant((char*)CXhChar16("%d", nCount)));	//孔径39~50mm(B)
		nCount = pRec->GetBoltOrHoleNum(51);
		if (nCount > 0)
			map.SetValueAt(index, 23, COleVariant((char*)CXhChar16("%d", nCount)));	//孔径50mm以上(C)
		index++;
	}
	char cell_end[15] = "";
	//设置编号格式
	sprintf(cell_end, "B%d", index - 1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet, "B2", cell_end, COleVariant("@"));
	//设置表格内容
	sprintf(cell_end, "X%d", index);
	CExcelOper::SetRangeValue(excel_sheet, "A1", cell_end, map.var);
	//
	excel_sheet.ReleaseDispatch();
}

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	//1.获取BOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	//2.生成ＥＸＣＥＬ材料表
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//构件明细表
	AddPartSpecTblSheet(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}