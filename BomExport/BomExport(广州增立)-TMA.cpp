// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
//����-TMA

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "SortFunc.h"
#include "ToolKit.h"

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
	return 8;
}
const int COL_COUNT=7;
//�����Ŵ�1��ʼ��0��ʾû�ж�Ӧ��
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=1;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=4;	//���
		colIndexArr[2]=5;	//����
		colIndexArr[3]=8;	//����
		colIndexArr[4]=18;	//����
		colIndexArr[5]=6;	//����
		colIndexArr[6]=7;	//���
		return 7;
	}
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

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
CXhChar200 GetPartNotes(BOMPART *pPart)
{
	CXhChar200 sNotes;
	bool bDrillHole=false;
	if( (pPart->cMaterial=='S'&&pPart->thick>16)||			//���
		(pPart->cMaterial=='H'&&pPart->thick>14)||
		(pPart->cMaterial=='P'&&pPart->thick>12)||
		pPart->cMaterial=='T'||pPart->IsDrillHole())
		bDrillHole=true;
	if(pPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
		//���Ŷ�
		if(pAngle->bHasFootNail)
			sNotes.Append("���Ŷ� ");
		//���
		if(bDrillHole)
			sNotes.Append("��� ");
		//�н�
		if(pAngle->IsCutAngleOrWing())				
			sNotes.Append("�н� ");
		//ѹ��
		if(pAngle->nPushFlat==0x05)			//0x01:ʼ��ѹ�� 0x02:�м�ѹ�� 0x04:�ն�ѹ��
			sNotes.Append("����ѹ�� ");
		else if((pAngle->nPushFlat&0x01)||(pAngle->nPushFlat&0x04))
			sNotes.Append("һ��ѹ�� ");
		if(pAngle->nPushFlat&0x02)
			sNotes.Append("�м�ѹ�� ");
		if(pAngle->bCutRoot)			//���
			sNotes.Append("��� ");
		if(pAngle->bCutBer)				//����
			sNotes.Append("���� ");
		if(pAngle->bKaiJiao)			//����
			sNotes.Append("���� ");
		if(pAngle->bHeJiao)				//�Ͻ�
			sNotes.Append("�Ͻ� ");
	}
	else if(pPart->cPartType==BOMPART::PLATE)	
	{	
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		//���
		if(bDrillHole)
			sNotes.Append("��� ");
		//�¿�
		if(pPlate->bNeedFillet||pPlate->thick>=8)	
			sNotes.Append("�¿� ");
		//���
		int nRollEdgeCount=0;
		for(LIST_NODE<BOMPROFILE_VERTEX>* pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				nRollEdgeCount++;
		}
		if(nRollEdgeCount>0)
			sNotes.Append("��� ");
		//���
		//�޿�
		int nHoleCount=pPlate->m_arrBoltRecs.GetSize();
		if(nHoleCount<=0)
			sNotes.Append("�޿� ");
		else if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="���";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="��17.5*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="��21.5*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="��25.5*"+str;
			}
			sNotes.Append(sNote);
			sNotes.Append(" ");
		}
	}
	long nHuoquLineCount=pPart->GetHuoquLineCount();
	if(nHuoquLineCount>0)		//����
		sNotes.Append("���� ");
	if(pPart->bWeldPart)		//�Ƿ�纸��
		sNotes.Append("�麸 ");
	return sNotes;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	//1.��ȡBOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer,GetSupportDataBufferVersion());
	}
	//���й��ˡ�����
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		if(segTable.GetNodeNum()==0||segTable.GetValue(pBomPart->iSeg)!=NULL)	//�κŹ���
			BomArr.append(*pNode);
	}
	CBubbleSort<SUPERLIST_NODE<BOMPART>>::BubSort(BomArr.m_pData,BomArr.GetSize(),CompareSegiFunc);
	//2.���ɣţأãţ̲��ϱ�
	//2.1 ����Excel������
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("����ԭʼ���ϱ�");
	//2.2 ������ϱ�
	//2.2.1 ��ӱ�����
	CStringArray str_arr;
	//
	str_arr.SetSize(9);
	str_arr[0]="����";str_arr[1]="���";str_arr[2]="����";str_arr[3]="���";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="������";str_arr[7]="";str_arr[8]="��ע";
	double col_arr[9]={5.0,5.0,5.0,7.5,5.0,
						5.5,7.0,5.0,25};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,22);
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		SEGI segI(pPart->iSeg);
		map.SetValueAt(index,0,COleVariant(segI.ToString()));		//��λ
		map.SetValueAt(index,1,COleVariant(pPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//���
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//����
		CXhChar16 sGuiGe=GetPartSpec(pPart);
		sGuiGe.Replace('x','*');
		sGuiGe.Replace("��","��");
		sGuiGe.Replace("��","L");
		map.SetValueAt(index,3,COleVariant(sGuiGe));	//���
		map.SetValueAt(index,4,COleVariant(ftol(pPart->wide)));		//���
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(pPart->GetPartNum()));	//��������
		map.SetValueAt(index,7,COleVariant(""));
		map.SetValueAt(index,8,COleVariant(GetPartNotes(pPart)));	//��ע
		index++;
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"I%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)3));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
