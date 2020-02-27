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

//��˨���ַ���
typedef struct tagHOLE_INFO
{
	int d;					//��˨ֱ��
	double hole_d_increment;//������
	int nCount;				//��ֱ����˨����
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
			sCurHole.Printf("%d��%smm", pNode->data.nCount, (char*)sD);
		else
			sCurHole.Printf("%d��%s", pNode->data.nCount, (char*)sD);
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
	if (pBomPart->cPartType != BOMPART::PLATE && nHuoquLineCount > 0)	//�Ƿ���Ҫ����
	{
		if(strstr(notes,"����")==NULL)
			notes.Append("����", ' ');	//����Ǹְ�����
	}
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		PART_PLATE *pPlate = (PART_PLATE*)pBomPart;
		if (pPlate->bNeedFillet)		//��Ҫ�¿�
		{
			if (strstr(notes, "�¿�") == NULL)
				notes.Append("�¿�", ' ');
		}
		BOOL bRollEdge = FALSE;
		for (LIST_NODE<BOMPROFILE_VERTEX> *pNode = pPlate->listVertex.EnumFirst(); pNode; pNode = pPlate->listVertex.EnumNext())
		{
			if (pNode->data.m_bRollEdge)
			{
				bRollEdge = TRUE;
				notes.Append(CXhChar16(" ���� ���%dmm", abs(pNode->data.manu_space)), ' ');
				break;
			}
		}
		if (!bRollEdge && nHuoquLineCount>0)
		{	//����ְ����������ȡ�۽� wht 19-09-29
			notes.Append("����", ' ');
			for (int i = 0; i < nHuoquLineCount; i++)
			{
				double fDegree = pPlate->GetHuoquDegree(i);
				if (fDegree < 0)
					fDegree *= -1;
				if (fDegree < 90)
					fDegree = 180 - fDegree;
				CXhChar16 sDegree(" %.1f��", fDegree);
				__SimplifiedNumString(sDegree);
				notes.Append(sDegree, ' ');
			}
		}
		if (pPlate->siSubType == BOMPART::SUB_TYPE_BOLT_PAD)
		{
			notes.Append("���", ' ');
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
			notes.Append("�����¿�", ' ');
		else if (pTube->startProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD ||
			pTube->endProcess.type == PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("һ���¿�", ' ');
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
		if (pBomAngle->bHasFootNail)
		{
			if (strstr(notes, "���Ŷ�") == NULL)
				notes.Append("���Ŷ�", ' ');
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
	excel_sheet.SetName("��������");
	//
	int index = 0, nRow = 0;
	nRow = bom.m_nPartsN;
	CVariant2dArray map(nRow + 1, 24);

	map.SetValueAt(index, 0, COleVariant("����(ֻ��)"));	//�κ�
	map.SetValueAt(index, 1, COleVariant("������"));		//�������
	map.SetValueAt(index, 2, COleVariant("����"));			//����
	map.SetValueAt(index, 3, COleVariant("���"));			//��ƹ��
	map.SetValueAt(index, 4, COleVariant("����(mm)"));		//����
	map.SetValueAt(index, 5, COleVariant("���"));			//���
	map.SetValueAt(index, 6, COleVariant("������"));		//������
	map.SetValueAt(index, 7, COleVariant("��������(kg)"));//��������
	map.SetValueAt(index, 8, COleVariant("������(kg)"));	//��������
	map.SetValueAt(index, 9, COleVariant("��������(��)"));//��������
	map.SetValueAt(index, 10, COleVariant("��ע"));		//��ע
	map.SetValueAt(index, 11, COleVariant("�纸"));		//����
	map.SetValueAt(index, 12, COleVariant("����"));		//����
	map.SetValueAt(index, 13, COleVariant("�н���֫"));	//��֫
	map.SetValueAt(index, 14, COleVariant("����"));		//����
	map.SetValueAt(index, 15, COleVariant("���"));		//�ٽ�
	map.SetValueAt(index, 16, COleVariant("���"));		//ѹ��
	map.SetValueAt(index, 17, COleVariant("���Ͻ�"));		//����
	map.SetValueAt(index, 18, COleVariant("����"));
	map.SetValueAt(index, 19, COleVariant("����"));
	map.SetValueAt(index, 20, COleVariant("�ܳ�"));
	map.SetValueAt(index, 21, COleVariant("�׾�28~38mm(A)"));
	map.SetValueAt(index, 22, COleVariant("�׾�39~50mm(B)"));
	map.SetValueAt(index, 23, COleVariant("�׾�50mm����(C)"));
	//�����п�
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
		map.SetValueAt(index, 0, COleVariant(SEGI(pRec->iSeg).ToString()));	//�κ�
		map.SetValueAt(index, 1, COleVariant(pRec->GetPartNo(bom.m_biMatCharPosType, bom.m_cMatCharSeparator)));	//����
		CXhChar16 sMark=_QuerySteelMatMark(pRec);
		if (sMark.EqualNoCase("Q235") || sMark.Equal("Q345"))
			sMark.Append("B");
		map.SetValueAt(index, 2, COleVariant((char*)sMark));	//����
		CXhChar16 sSpec("%s", (char*)pRec->sSpec);
		if (pRec->cPartType == BOMPART::PLATE)
			sSpec.Printf("-%.0f", pRec->thick);
		map.SetValueAt(index, 3, COleVariant(sSpec));					//��ƹ��
		map.SetValueAt(index, 4, COleVariant(ftol(pRec->length)));		//����
		map.SetValueAt(index, 5, COleVariant(ftol(pRec->wide)));		//���
		map.SetValueAt(index, 6, COleVariant((long)pRec->GetPartNum()));	//��������
		if (bom.m_iPlankWeightStatType <= 1)
			map.SetValueAt(index, 7, COleVariant(ftol(10 * pRec->fPieceWeight) / 10.0));	//��������
		else
			map.SetValueAt(index, 7, COleVariant(ftol(10 * pRec->fPieceWeight) / 10.0));	//��������
		double djzl = 0;
		if (bom.m_iPlankWeightStatType <= 1)
			djzl = pRec->fPieceWeight*pRec->GetPartNum();
		else
			djzl = pRec->fPieceWeight*pRec->GetPartNum();
		map.SetValueAt(index, 8, COleVariant(ftol(10 * djzl) / 10.0));	//��������
		map.SetValueAt(index, 9, COleVariant((long)pRec->nMSumLs));	//��������
		CXhChar100 sNotes = GetProcessNotes(pRec, TRUE);
		map.SetValueAt(index, 10, COleVariant((char*)sNotes));		//��ע
		if (pRec->bWeldPart)
			map.SetValueAt(index, 11, COleVariant("1"));	//����
		long nHuoquLineCount = pRec->GetHuoquLineCount();
		if (pRec->cPartType == BOMPART::PLATE)
			nHuoquLineCount = ((PART_PLATE*)pRec)->m_cFaceN - 1;
		if (nHuoquLineCount > 0)	//�Ƿ���Ҫ����
			map.SetValueAt(index, 12, COleVariant("1"));	//����
		if (pRec->cPartType == BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle = (PART_ANGLE*)pRec;
			if (pAngle->bCutAngle == 2)
				map.SetValueAt(index, 13, COleVariant("2"));	//�н���֫
			else if (pAngle->bCutAngle != 0)
				map.SetValueAt(index, 13, COleVariant("1"));	//�н���֫
			//else
			//	map.SetValueAt(index, 13, COleVariant("0"));	//�н���֫
			if (pAngle->bCutBer)
				map.SetValueAt(index, 14, COleVariant("1"));	//����
			if (pAngle->bCutRoot)
				map.SetValueAt(index, 15, COleVariant("1"));	//���
			if (pAngle->nPushFlat > 0)
				map.SetValueAt(index, 16, COleVariant("1"));	//ѹ��
			if (pAngle->bKaiJiao || pAngle->bHeJiao)
				map.SetValueAt(index, 17, COleVariant("1"));	//���ǺϽ�
		}
		else if (pRec->cPartType == BOMPART::PLATE)
		{
			PART_PLATE *pPlate = (PART_PLATE*)pRec;
			int nEdgeNum = pPlate->GetEdgeCount();
			if (nEdgeNum > 0)
				map.SetValueAt(index, 19, COleVariant((char*)CXhChar16("%d", nEdgeNum)));				//����
			if (pPlate->fPerimeter > 0)
				map.SetValueAt(index, 20, COleVariant((char*)CXhChar16("%.0f", pPlate->fPerimeter)));	//�ܳ�
		}
		int nCount = pRec->GetBoltOrHoleNum(28, 38);
		if(nCount>0)
			map.SetValueAt(index, 21, COleVariant((char*)CXhChar16("%d", nCount)));	//�׾�28~38mm(A)
		nCount = pRec->GetBoltOrHoleNum(39, 50);
		if(nCount>0)
			map.SetValueAt(index, 22, COleVariant((char*)CXhChar16("%d", nCount)));	//�׾�39~50mm(B)
		nCount = pRec->GetBoltOrHoleNum(51);
		if (nCount > 0)
			map.SetValueAt(index, 23, COleVariant((char*)CXhChar16("%d", nCount)));	//�׾�50mm����(C)
		index++;
	}
	char cell_end[15] = "";
	//���ñ�Ÿ�ʽ
	sprintf(cell_end, "B%d", index - 1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet, "B2", cell_end, COleVariant("@"));
	//���ñ������
	sprintf(cell_end, "X%d", index);
	CExcelOper::SetRangeValue(excel_sheet, "A1", cell_end, map.var);
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
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	//������ϸ��
	AddPartSpecTblSheet(bom, pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}