// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
// ɽ������
//TODO û�жԺ�����

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
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

const int COL_COUNT=6;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=4;	//���
		colIndexArr[2]=5;	//����
		colIndexArr[3]=7;	//����
		colIndexArr[4]=8;	//����
		colIndexArr[5]=6;	//����
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//��������
void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		BomArr.append(*pNode);
	}
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("ͼֽ����");	
	CStringArray str_arr;
	const int COL_COUNT=10;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="����";str_arr[1]="�κ�";str_arr[2]="�������";
	str_arr[3]="��������";str_arr[4]="��ƹ��";str_arr[5]="����";
	str_arr[6]="����";str_arr[7]="��������";str_arr[8]="��������";str_arr[9]="С������";
	double col_arr[10]={8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,10.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//��ȡExcel���ķ�Χ
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(""));
		map.SetValueAt(index,1,COleVariant(SEGI(pBomPart->iSeg).ToString()));
		map.SetValueAt(index,2,COleVariant(pBomPart->sPartNo));
		if(pBomPart->cPartType==1)
			map.SetValueAt(index,4,COleVariant(CXhChar50("��%d*%d",(int)pBomPart->wide),(int)pBomPart->thick));
		else if(pBomPart->cPartType==3)
			map.SetValueAt(index,4,COleVariant(CXhChar50("-%d",(int)pBomPart->thick)));
		if(pBomPart->cPartType==1)
			map.SetValueAt(index,3,COleVariant("�Ǹ�"));
		else if(pBomPart->cPartType==3)
			map.SetValueAt(index,3,COleVariant("���"));
		else if(pBomPart->cPartType==4)
			map.SetValueAt(index,3,COleVariant("�ֹ�"));
		else 
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		CXhChar16 materialStr=QuerySteelMatMark(pBomPart->cMaterial);
		if(stricmp(materialStr,"Q235")==0)
			map.SetValueAt(index,5,COleVariant("A3"));
		else
			map.SetValueAt(index,5,COleVariant(materialStr));
		if(pBomPart->cPartType==3)
		{
			map.SetValueAt(index,4,COleVariant(CXhChar50("-%d",(int)pBomPart->thick)));
			map.SetValueAt(index,6,COleVariant(CXhChar50("%dx%.f",(int)pBomPart->wide,pBomPart->length)));
		}
		else
		{
			map.SetValueAt(index,4,COleVariant(pBomPart->sSpec));
			if(pBomPart->cPartType==BOMPART::ACCESSORY&&pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index,6,COleVariant(CXhChar50("%.fX%.f",pBomPart->wide,pBomPart->length)));
			else
				map.SetValueAt(index,6,COleVariant(CXhChar50("%.f",pBomPart->length)));
		}
		map.SetValueAt(index,7,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,8,COleVariant(CXhChar50("%.2f",pBomPart->fPieceWeight)));
		map.SetValueAt(index,9,COleVariant(CXhChar50("=H%d*I%d",index+2,index+2)));
		index++;
	}
	CXhChar16 cell_end("J%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
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
	pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//1 ������ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}