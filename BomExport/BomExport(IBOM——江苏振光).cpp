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
#include "XhCharString.h"

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

char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}

const int COL_COUNT=11;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=5;	//����
		colIndexArr[1]=2;	//���
		colIndexArr[2]=1;	//����
		colIndexArr[3]=4;	//����
		colIndexArr[4]=6;	//����
		colIndexArr[5]=3;	//����
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//��������
CXhChar16 __QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if ('H' == cMat)
		sMatMark.Copy("Q345");
	else if ('G' == cMat)
		sMatMark.Copy("Q390");
	else if ('P' == cMat)
		sMatMark.Copy("Q420");
	else if ('T' == cMat)
		sMatMark.Copy("Q460");
	else if ('S' == cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
CXhChar100 GetBomPartSpec(BOMPART *pBomPart, char* material=NULL)
{
	CXhChar100 sSpec,sSpecialPartSpec;
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			if(cMaterial=='D')
				cMaterial='P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING||
				pBomPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE||
				pBomPart->siSubType==BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//�ָ�դ
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//��������
			//const static BYTE SUB_TYPE_LADDER		= 20;	//����
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'? __QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("L%.fX%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("��%.fX%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
			sSpec.Printf("-%.fX%.f",pBomPart->thick,pBomPart->wide);
		else 
			sSpec.Printf("%s",(char*)pBomPart->sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C','[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits=sSpec.Replace("��"," ");
		hits+=sSpec.Replace("��"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		if(hits==2)
		{
			int D=0,d=0;
			sscanf(sSpec,"%d%d",&D,&d);
			sSpec.Printf("��%d/%d",D,d);
		}
		else 
			sSpec.Copy(pBomPart->sSpec);
	}
	if (material != NULL)
		strcpy(material, materialStr);
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else //if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	/*else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);*/
}
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
	//
	CStringArray str_arr;
	const int COL_COUNT = 7;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "����"; str_arr[1] = "���"; str_arr[2] = "����"; str_arr[3] = "����";
	str_arr[4] = "���"; str_arr[5] = "����"; str_arr[6] = "����"; 
	double col_arr[7] = { 10.5,11.5,11.5,9.5,
						  10.0,10.0,12.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet, 1, str_arr, col_arr, TRUE);
	//
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//��ȡExcel���ķ�Χ
	ARRAY_LIST<int> Q345RowIndexArr, Q420RowIndexArr;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		CXhChar16 sMaterial;
		CXhChar100 sSpec=GetBomPartSpec(pBomPart,sMaterial);
		map.SetValueAt(index, 0, COleVariant(sMaterial));
		map.SetValueAt(index, 1, COleVariant(sSpec));
		if (pBomPart->length > 0)
		{
			if (pBomPart->cPartType == BOMPART::ACCESSORY&&pBomPart->siSubType == BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index, 2, COleVariant(CXhChar50("%.fX%.f", pBomPart->wide, pBomPart->length)));
			else
				map.SetValueAt(index, 2, COleVariant(CXhChar50("%.f", pBomPart->length)));
		}
		else
			map.SetValueAt(index, 2, COleVariant(""));
		map.SetValueAt(index, 3, COleVariant((long)pBomPart->GetPartNum()));
		if(sSpec.EqualNoCase(pBomPart->sPartNo))
			map.SetValueAt(index,4,COleVariant(""));
		else
			map.SetValueAt(index,4,COleVariant(pBomPart->sPartNo));
		map.SetValueAt(index, 5, COleVariant(CXhChar50("%.2f", pBomPart->fPieceWeight)));
		map.SetValueAt(index, 6, COleVariant(CXhChar50("%.1f", pBomPart->fPieceWeight*pBomPart->GetPartNum())));
		if (sMaterial.EqualNoCase("Q345"))
			Q345RowIndexArr.append(index + 2);
		if (sMaterial.EqualNoCase("Q420"))
			Q420RowIndexArr.append(index + 2);
		index++;
		
	}
	CXhChar16 cell_end("G%d", index+1);
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(14.0));
	CExcelOper::SetRangeValue(excel_sheet, "A2", cell_end, map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A1", cell_end, COleVariant((long)3));

	CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A2", CXhChar16("A%d",index), COleVariant((long)2));
	for (int i = 0; i < Q345RowIndexArr.GetSize(); i++)
	{
		int rowIndex = Q345RowIndexArr[i];
		CXhChar16 curCell("A%d", rowIndex);
		//CExcelOper::SetFontSize(excel_sheet, curCell, curCell, 0, true);
		//CExcelOper::SetRangeRowHeight(excel_sheet, 25.5, curCell, NULL, TRUE);
		CExcelOper::SetFontColor(excel_sheet, curCell, NULL, CExcelOper::COLOR_ID_RED);
		CExcelOper::SetBoldFont(excel_sheet, curCell, NULL);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, curCell, NULL, COleVariant((long)4));
	}
	for (int i = 0; i < Q420RowIndexArr.GetSize(); i++)
	{
		int rowIndex = Q420RowIndexArr[i];
		CXhChar16 curCell("A%d", rowIndex);
		//CExcelOper::SetFontSize(excel_sheet, curCell, curCell, 0, true);
		//CExcelOper::SetRangeRowHeight(excel_sheet, 25.5, curCell, NULL, TRUE);
		CExcelOper::SetFontColor(excel_sheet, curCell, NULL, CExcelOper::COLOR_ID_BLUE);
		CExcelOper::SetBoldFont(excel_sheet, curCell, NULL);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, curCell, NULL, COleVariant((long)4));
	}
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