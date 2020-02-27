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

static CXhChar16 LocalQuerySteelMatMark(BOMPART *pPart)
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
		sMatMark.Copy("");
	else
		sMatMark.Copy(pPart->sMaterial);
	return sMatMark;
}
static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec = pPart->GetSpec();
	CXhChar16 sMaterial = LocalQuerySteelMatMark(pPart);
	if (GetSupportDataBufferVersion() >= 2)
	{
		if (pPart->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%.fX%.f", pPart->thick, pPart->wide);
		else if (pPart->cPartType == BOMPART::ANGLE)
			sSpec.Printf("L%.fX%.f", pPart->wide, pPart->thick);
		PART_PLATE *pPlate = (PART_PLATE*)pPart;
		if (pPlate->IsFL() || pPlate->siSubType == BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("��%.f/%.f", pPlate->fFlOutterD, pPlate->fFlInnerD);
	}
	return CXhChar16("%s%s",(char*)sMaterial, (char*)sSpec);
}

void AddPartSpecTblSheet(CModelBOM &bom, LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("sheet");
	//���ʵʱ���ݵ��б���
	//2.2 ������ϱ�
	//2.2.1 ��ӱ�����
	const int COL_COUNT = 8;
	CStringArray str_arr;
	str_arr.SetSize(11);
	str_arr[0] = ""; str_arr[1] = ""; str_arr[2] = ""; str_arr[3] = "";  
	str_arr[4] = "���"; str_arr[5] = "���"; str_arr[6] = "����\n(mm)"; str_arr[7] = "��\n��";
	str_arr[8] = "һ��"; str_arr[9] = "С��"; str_arr[10] = "��ע"; 
	double col_arr[11] = { 8.38,8.38,8.38,8.38,8.38,12.38,8.38,8.38,
								8.38,8.38,8.38 };
	CExcelOper::AddRowToExcelSheet(excel_sheet, 5, str_arr, col_arr, TRUE);
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
		map.SetValueAt(index, 0, COleVariant(SEGI(pPart->iSeg).ToString()));	//�κ�
		map.SetValueAt(index, 1, COleVariant(pPart->sPartNo));		//���
		map.SetValueAt(index, 2, COleVariant(GetPartSpec(pPart)));	//���
		map.SetValueAt(index, 3, COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index, 4, COleVariant(pPart->GetPartNum()));	//��������

		double weight = ftol(pPart->fPieceWeight * 100) / 100.0;
		str.Format("%.2f", weight);
		map.SetValueAt(index, 5, COleVariant(str));					//��������
		double sum_weight = ftol(pPart->fPieceWeight*pPart->GetPartNum() * 100) / 100.0;
		str.Format("%.2f", sum_weight);
		map.SetValueAt(index, 6, COleVariant(str));		//����

		map.SetValueAt(index, 7, COleVariant(pPart->sNotes));//��ע
		index++;
	}
	CExcelOper::MergeRowRange(excel_sheet, "E4", "E5");
	CExcelOper::MergeRowRange(excel_sheet, "F4", "F5");
	CExcelOper::MergeRowRange(excel_sheet, "G4", "G5");
	CExcelOper::MergeRowRange(excel_sheet, "H4", "H5");
	CExcelOper::SetMainTitle(excel_sheet, "I4", "J4", "����(kg)", 20, 11, 2, 2, false);
	CExcelOper::MergeRowRange(excel_sheet, "K4", "K5");
	char cell_end[15] = " ";
	//�����Ҷ���
	sprintf(cell_end, "K%d", i + 5);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A5", cell_end, COleVariant((long)4));
	//���ñ������
	sprintf(cell_end, "K%d", i + 5);
	CExcelOper::SetRangeValue(excel_sheet, "D6", cell_end, map.var);
	CExcelOper::SetRangeRowHeight(excel_sheet, 13.50, "D6", cell_end);
	//���ñ߿�
	//CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(11.0));
	//CExcelOper::SetFontSize(excel_sheet, "A1", "L1", 14, true);
	CExcelOper::SetFontSize(excel_sheet, "D4", cell_end, 11);
	excel_sheet.DetachDispatch();
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
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//2.1 ������ϸ��
	AddPartSpecTblSheet(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}