// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
// ���ݷɽ�

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
//	ע�⣡
//
//		����� DLL ��̬���ӵ� MFC
//		DLL���Ӵ� DLL ������
//		���� MFC ���κκ����ں�������ǰ��
//		��������� AFX_MANAGE_STATE �ꡣ
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CBomExportApp

BEGIN_MESSAGE_MAP(CBomExportApp, CWinApp)
END_MESSAGE_MAP()


// CBomExportApp ����

CBomExportApp::CBomExportApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}
// Ψһ��һ�� CBomExportApp ����

CBomExportApp theApp;


// CBomExportApp ��ʼ��

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

static CXhChar100 GetProcessNotes(BOMPART* pBomPart, bool bIncKaiHeAngle = TRUE)
{
	CXhChar100 notes;
	long nHuoquLineCount = pBomPart->GetHuoquLineCount();
	if (pBomPart->cPartType == BOMPART::PLATE)
		nHuoquLineCount = ((PART_PLATE*)pBomPart)->m_cFaceN - 1;
	if (nHuoquLineCount > 0)	//�Ƿ���Ҫ����
		notes.Append("����", ' ');
	if (pBomPart->bWeldPart)
		notes.Append("����", ' ');
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
		if (pPlate->bNeedFillet)		//��Ҫ�¿�
			notes.Append("�¿�", ' ');
		for (LIST_NODE<BOMPROFILE_VERTEX> *pNode = pPlate->listVertex.EnumFirst(); pNode; pNode = pPlate->listVertex.EnumNext())
		{
			if (pNode->data.m_bRollEdge)
			{
				notes.Append(CXhChar16("���� ���%dmm", abs(pNode->data.manu_space)), ' ');
				break;
			}
		}
	}
	else if (pBomPart->cPartType == BOMPART::TUBE)
	{
		PART_TUBE *pTube = (PART_TUBE*)pBomPart;
		if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("�¿�", ' ');
		else if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD ||
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("�¿�", ' ');
	}
	else if (pBomPart->cPartType == BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle = (PART_ANGLE*)pBomPart;
		if (pBomAngle->bCutAngle)		//�Ƿ��н�
			notes.Append("�н�", ' ');
		if (pBomAngle->bCutBer)			//�Ƿ���Ҫ����
			notes.Append("����", ' ');
		if (pBomAngle->bCutRoot)		//�Ƿ���Ҫ���
			notes.Append("���", ' ');
		if (pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle) > 0)		//�Ƿ���Ҫ����
		{
			if (bIncKaiHeAngle)	//�������Ƕ��� wht 11-05-24
				notes.Append(CXhChar16("����(%.1f��)", pBomAngle->wing_angle - 90), ' ');
			else
				notes.Append("����", ' ');
		}
		if (pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle) > 0)			//�Ƿ���Ҫ�Ͻ�
		{
			if (bIncKaiHeAngle)	//�����ϽǶ��� wht 11-05-24
				notes.Append(CXhChar16("�Ͻ�(%.1f��)", 90 - pBomAngle->wing_angle), ' ');
			else
				notes.Append("�Ͻ�", ' ');
		}
		if (pBomAngle->nPushFlat > 0)		//�Ƿ�ѹ��
		{
			if (pBomAngle->nPushFlat == 0x02)
				notes.Append("�м�ѹ��", ' ');
			else
				notes.Append("ѹ��", ' ');
		}
	}
	return notes;
}

static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec = pPart->GetSpec();
	if (GetSupportDataBufferVersion() >= 2)
	{
		if (pPart->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%d",(int)pPart->thick);
		else if (pPart->cPartType == BOMPART::ANGLE)
			sSpec.Printf("L%.fx%.f", pPart->wide, pPart->thick);
		PART_PLATE *pPlate = (PART_PLATE*)pPart;
		if (pPlate->IsFL() || pPlate->siSubType == BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("��%.f/%.f", pPlate->fFlOutterD, pPlate->fFlInnerD);
	}
	return sSpec;
}


void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("�ܱ�");
	CHashList<SEGI> segNoHashList;
	for (SUPERLIST_NODE<BOMPART> *pBomPart = bom.listParts.EnumFirst(); pBomPart; pBomPart = bom.listParts.EnumNext())
	{
		if (segNoHashList.GetValue(pBomPart->pDataObj->iSeg)==NULL)
			segNoHashList.SetValue(pBomPart->pDataObj->iSeg,SEGI(pBomPart->pDataObj->iSeg));
	}
	//���ʵʱ���ݵ��б���
	CExcelOper::SetMainTitle(excel_sheet, "A1", "L1", "���乤��ӹ���-�ܱ�", 28.5);
	char strTile[500] = "";
	CString strTile1;
	sprintf(strTile, "�������ƣ�%s", (char*)bom.m_sPrjName);
	CExcelOper::SetMainTitle(excel_sheet, "A2", "L2", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "���ͣ�%s", (char*)bom.m_sTowerTypeName);
	CExcelOper::SetMainTitle(excel_sheet, "A3", "C3", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "������%d��", bom.m_nTaNum);
	CExcelOper::SetMainTitle(excel_sheet, "D3", "H3", strTile, 20, 14, 3, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I3", "L3", "���ϱ�׼��������׼", 20, 14, 3, 2, false);
	sprintf(strTile, "�ӹ�������%s", (char*)SegNoHashListToString(segNoHashList));
	CExcelOper::SetMainTitle(excel_sheet, "A4", "L4", strTile, 20, 14, 2, 2, false);
	//2.2 ������ϱ�
	//2.2.1 ��ӱ�����
	const int COL_COUNT = 12;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "����";  str_arr[1] = "����"; str_arr[2] = "���"; str_arr[3] = "����(mm)"; str_arr[4] = "���";
	str_arr[5] = "������"; str_arr[6] = "������"; str_arr[7] = "����"; str_arr[8] = "����"; str_arr[9] = "����(kg)";	str_arr[10] = "����(kg)";  str_arr[11] = "��ע";
	double col_arr[COL_COUNT] = { 8.0,7.0,8.5,9.0,5.63,
								8,8,5.0,5.0,9.0,9.0,15.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet, 33, "A5", "L5");
	//2.2.2 ������ϱ�
	int i = 0, nCount = 0;
	int index = 0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN, COL_COUNT);
	for (SUPERLIST_NODE<BOMPART> *pNode = bom.listParts.EnumFirst(); pNode; pNode = bom.listParts.EnumNext())
	{
		BOMPART *pPart = pNode->pDataObj;
		if (pPart == NULL)
			continue;
		i++;
		map.SetValueAt(index, 0, COleVariant(pPart->sPartNo));		//����
		CXhChar16 sMat = QuerySteelMatMark(pPart->cMaterial);
		if (pPart->cMaterial == 'S')
			sMat.Empty();
		else if (sMat.GetLength() == 0)
			sMat.Copy(pPart->sMaterial);
		map.SetValueAt(index, 1, COleVariant(sMat));				//����
		map.SetValueAt(index, 2, COleVariant(GetPartSpec(pPart)));	//���
		map.SetValueAt(index, 3, COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index, 4, COleVariant(ftol(pPart->wide)));	//���
		map.SetValueAt(index, 5, COleVariant(pPart->GetPartNum()));	//��������
		map.SetValueAt(index, 6, COleVariant(pPart->GetPartNum()));	//��������

		map.SetValueAt(index, 7, COleVariant((long)bom.m_nTaNum));//����
		map.SetValueAt(index, 8, COleVariant(CXhChar16("=G%d*H%d", index + 6, index + 6)));	//����
		double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
		str.Format("%.2f", weight);
		map.SetValueAt(index, 9, COleVariant(str));					//��������
		double sum_weight = ftol(pPart->fPieceWeight*pPart->GetPartNum() * 100) / 100.0;
		str.Format("%.2f", sum_weight);
		map.SetValueAt(index, 10, COleVariant(str));		//����
		
		map.SetValueAt(index, 11, COleVariant(GetProcessNotes(pPart)));
		index++;
	}
	char cell_end[15] = " ";
	//�����Ҷ���
	sprintf(cell_end, "L%d", i + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)3));
	//���ñ������
	sprintf(cell_end, "L%d", i + 5);
	CExcelOper::SetRangeValue(excel_sheet, "A6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 21.75, "A2", cell_end);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(13.0));
	CExcelOper::SetFontSize(excel_sheet, "A1", "L1", 14,true);
	CExcelOper::SetFontSize(excel_sheet, "A2", "L4", 14);
	excel_sheet.DetachDispatch();
}

void AddPartGroupSumMatTbl(CModelBOM &bom, LPDISPATCH pWorkSheet)
{

	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet, FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("�麸��ϸ��(������");
#endif

	CHashList<SEGI> segNoHashList;
	CHashStrList<BOMPART*>hashPartPtrByPartNo;
	for (SUPERLIST_NODE<BOMPART> *pBomPart = bom.listParts.EnumFirst(); pBomPart; pBomPart = bom.listParts.EnumNext())
	{
		if (segNoHashList.GetValue(pBomPart->pDataObj->iSeg) == NULL)
			segNoHashList.SetValue(pBomPart->pDataObj->iSeg, SEGI(pBomPart->pDataObj->iSeg));
		hashPartPtrByPartNo.SetValue(pBomPart->pDataObj->sPartNo, pBomPart->pDataObj);
	}
	//���ʵʱ���ݵ��б���
	CExcelOper::SetMainTitle(excel_sheet, "A1", "K1", "���乤��ӹ���-�麸��ϸ��", 28.5);
	char strTile[500]="";
	CString strTile1;
	//sprintf(strTile, "%.*s%.*s", sizeof("�������ƣ�"), "�������ƣ�", sizeof(bom.m_sPrjName), bom.m_sPrjName);
	sprintf(strTile, "�������ƣ�%s", (char*)bom.m_sPrjName);
	CExcelOper::SetMainTitle(excel_sheet, "A2", "K2", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "���ͣ�%s", (char*)bom.m_sTowerTypeName);
	CExcelOper::SetMainTitle(excel_sheet, "A3", "C3", strTile, 20, 14, 2, 2, false);
	sprintf(strTile, "������%d��", bom.m_nTaNum);
	CExcelOper::SetMainTitle(excel_sheet, "D3", "H3", strTile, 20, 14, 3, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I3", "K3", "���ϱ�׼��������׼", 20, 14, 3, 2, false);
	sprintf(strTile, "�ӹ�������%s", (char*)SegNoHashListToString(segNoHashList));
	CExcelOper::SetMainTitle(excel_sheet, "A4", "K4", strTile, 20, 14, 2, 2, false);
	CExcelOper::SetMainTitle(excel_sheet, "I5", "J5", "", 33.0);
	//ͳ������
	int nRowNum = 5;
	LIST_NODE<BOM_WELDPART> *pWeldPart = NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode = NULL;
	for (BOM_WELDPART *pWeldPart = bom.listWeldParts.EnumObjectFirst(); pWeldPart; pWeldPart = bom.listWeldParts.EnumObjectNext())
	{
		for (BOM_PARTREF *pPartRef = pWeldPart->listPartRef.EnumObjectFirst(); pPartRef; pPartRef = pWeldPart->listPartRef.EnumObjectNext())
			nRowNum++;
	}
	const int COL_COUNT = 11;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "�麸���";  str_arr[1] = "����"; str_arr[2] = "����"; str_arr[3] = "���"; str_arr[4] = "�ϳ�";
	str_arr[5] = "����"; str_arr[6] = "����"; str_arr[7] = "����"; str_arr[8] = "��ע"; 	str_arr[10] = "�麸����"; 
	double col_arr[COL_COUNT] = { 8.5,7.5,7.5,10.75,8.38,
								8.38,8.38,8.38,8.38,5.0,
								8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
	//2.2.2 ������ϱ�
	int i = 0;
	int index = 0;
	CString str;
	CVariant2dArray map(nRowNum, COL_COUNT);
	for (BOM_WELDPART *pWeldPart = bom.listWeldParts.EnumObjectFirst(); pWeldPart; pWeldPart = bom.listWeldParts.EnumObjectNext())
	{
		char merge_start[15] = "", merge_end[15] = "";
		int nCount = 0;
		double weightCount=0;
		map.SetValueAt(index, 0, COleVariant(pWeldPart->sPartNo));			//�麸���
		map.SetValueAt(index, 10, COleVariant((long)pWeldPart->nWeldPartNum));			//�麸����
		for (BOM_PARTREF *pPartRef = pWeldPart->listPartRef.EnumObjectFirst(); pPartRef; pPartRef = pWeldPart->listPartRef.EnumObjectNext(), index++)
		{
			nCount++;
			BOMPART **ppPart = hashPartPtrByPartNo.GetValue(pPartRef->sPartNo);
			BOMPART *pPart = ppPart ? *ppPart : NULL;
			if (pPart==NULL)
				continue;
			map.SetValueAt(index, 1, COleVariant(pPart->sPartNo));		//����
			CXhChar16 sMat = QuerySteelMatMark(pPart->cMaterial);
			if (pPart->cMaterial == 'S')
				sMat.Empty();
			else if (sMat.GetLength() == 0)
				sMat.Copy(pPart->sMaterial);
			map.SetValueAt(index, 2, COleVariant(sMat));				//����
			map.SetValueAt(index, 3, COleVariant(GetPartSpec(pPart)));	//���
			map.SetValueAt(index, 4, COleVariant(ftol(pPart->length)));	//����
			int nPartRefNum = pPartRef->nPartNum;
			map.SetValueAt(index, 5, COleVariant((long)nPartRefNum));	//����
			double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
			str.Format("%.2f", weight);
			map.SetValueAt(index, 6, COleVariant(str));					//��������
			double sum_weight = ftol(pPart->fPieceWeight*nPartRefNum * 100) / 100.0;
			str.Format("%.2f", sum_weight);
			map.SetValueAt(index, 7, COleVariant(str));		//����
			weightCount += sum_weight;
			sprintf(merge_start, "I%d", index + 6);
			sprintf(merge_end, "J%d", index + 6);
			CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		}
		map.SetValueAt(index, 1, COleVariant("�ܼ�"));
		sprintf(merge_start, "B%d", index + 6);
		sprintf(merge_end, "D%d", index + 6);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		str.Format("%.2f", weightCount);
		map.SetValueAt(index, 4, COleVariant(str));
		sprintf(merge_start, "E%d", index + 6);
		sprintf(merge_end, "J%d", index + 6);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		sprintf(merge_start, "A%d", index + 6 - nCount); sprintf(merge_end, "A%d", index + 5);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);
		sprintf(merge_start, "K%d", index + 6 - nCount); sprintf(merge_end, "K%d", index + 5);
		CExcelOper::MergeRowRange(excel_sheet, merge_start, merge_end);

		index++;
	}
	char cell_end[15] = "";
	//���������
	sprintf(cell_end, "K%d", index + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)3));
	//���ñ������
	sprintf(cell_end, "K%d", index + 5);
	CExcelOper::SetRangeValue(excel_sheet, "A6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 21.75, "A2", cell_end);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(13.0));
	CExcelOper::SetFontSize(excel_sheet, "A1", "K1", 14,true);
	CExcelOper::SetFontSize(excel_sheet, "A2", "K4", 14);
	CExcelOper::SetFontSize(excel_sheet, "J5", "J5", 13);
	//
	excel_sheet.ReleaseDispatch();
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	//1.��ȡBOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	//2.���ɣţأãţ̲��ϱ�
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//2.1 ������ϸ��
	AddPartSpecTblSheet(bom, pWorksheet);
	//2.3 �麸����ϸ��
	pWorksheet = excel_sheets.GetItem(COleVariant((short)2));
	AddPartGroupSumMatTbl(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}