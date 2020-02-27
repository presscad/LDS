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

const int COL_COUNT=8;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=5;	//���
		colIndexArr[2]=3;	//����
		colIndexArr[3]=9;	//����
		colIndexArr[4]=0;	//����
		colIndexArr[5]=6;	//����
		colIndexArr[6]=7;	//���
		colIndexArr[7]=4;	//����
		return 8;
	}
}

//////////////////////////////////////////////////////////////////////////
//��������
CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
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
	CXhChar16 materialStr=cMaterial!='0'?QuerySteelMatMark(cMaterial):"";
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
		sSpec.Printf("%.f*%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%.f*%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0||pBomPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
			sSpec.Printf("%.f",pBomPart->thick);
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
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else
		return sSpec;
	/*else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);*/
}
static CXhChar16 GetPartTypeName(BOMPART *pPart,BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
	if(pPart->cPartType==BOMPART::PLATE)		//�ְ�
	{
		sPartType=bSymbol?"-":"�ְ�";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
			sPartType=bSymbol?"C":"���Ͳ��";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U)
			sPartType=bSymbol?"U":"U�Ͳ��";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
			sPartType=bSymbol?"X":"ʮ�ֲ��";
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType=bSymbol?"F":"����";
	}
	else if(pPart->cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType=bSymbol?"��":"�Ǹ�";
	else if(pPart->cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType=bSymbol?"��":"�ֹ�";
	else if(pPart->cPartType==BOMPART::SLOT)	//�۸�
		sPartType=bSymbol?"[":"�۸�";
	else if(pPart->cPartType==BOMPART::FLAT)	//���
		sPartType=bSymbol?"-":"���";
	else if(pPart->cPartType==BOMPART::ROUND)	//Բ��
		sPartType=bSymbol?"��":"Բ��";
	else if(pPart->cPartType==BOMPART::ACCESSORY)
	{
		if(pPart->siSubType==BOMPART::SUB_TYPE_LADDER)
			sPartType=bSymbol?"T":"����";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			sPartType=bSymbol?"-":"�ָ�դ";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE)
			sPartType=bSymbol?" ":"��������";
		else
			sPartType=bSymbol?" ":"����";
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
			if(cMaterial=='D')
				cMaterial='P';
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
		}
		//�ᵼ����˨�����ʴ��� wht 18-12-30
		/*else if(pBomPart->siSubType!=0)
		{
			cMaterial='0';
		}*/
	}
	return cMaterial;
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
	//���ԭ������
	Range excel_usedRange,excel_range;
	excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
	excel_usedRange.Select();
	excel_usedRange.ClearContents();
	//
	double col_arr[11]={5.5,7.5,7.0,10.0,12.5,
						9.5,9.5,9.0,9.0,8.5,8.5};
	CStringArray str_arr;
	str_arr.SetSize(11);
	str_arr[0]="����";str_arr[1]="�������";str_arr[2]="����";str_arr[3]="��������";str_arr[4]="���";
	str_arr[5]="����";str_arr[6]="���";str_arr[7]="���";str_arr[8]="��������";str_arr[9]="����";str_arr[10]="��ע";
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	
	//
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,11);//��ȡExcel���ķ�Χ
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		CXhChar100 sSpec=GetBomPartSpec(pBomPart);
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//
		if(sSpec.EqualNoCase(pBomPart->sPartNo))
			map.SetValueAt(index,1,COleVariant(""));
		else
			map.SetValueAt(index,1,COleVariant(pBomPart->sPartNo));
		char cMaterial=GetPartMaterialChar(pBomPart);
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(cMaterial)));	//����
		map.SetValueAt(index,3,COleVariant(GetPartTypeName(pBomPart,FALSE)));//��������
		map.SetValueAt(index,4,COleVariant(sSpec));							//��ƹ��
		if(pBomPart->length>0)
		{
			if(pBomPart->cPartType==BOMPART::ACCESSORY&&pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index,5,COleVariant(CXhChar50("%.fX%.f",pBomPart->wide,pBomPart->length)));
			else
				map.SetValueAt(index,5,COleVariant(CXhChar50("%.f",pBomPart->length)));
		}
		else
			map.SetValueAt(index,5,COleVariant(""));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if(pBomPart->wide>0)
				map.SetValueAt(index,6,COleVariant(CXhChar50("%.f",pBomPart->wide)));
		}
		map.SetValueAt(index,8,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,9,COleVariant(CXhChar50("%.1f",pBomPart->fSumWeight)));
		index++;
		
	}
	CXhChar16 cell_end("K%d",index),a_cell_end("B%d",index);
	//CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A1",cell_end,COleVariant("@"));
	//CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A1",cell_end,COleVariant("G/ͨ�ø�ʽ"));
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A1",a_cell_end,COleVariant("@"));
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
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
	/*CXhChar500 sTempPath;
	::GetModuleFileName(theApp.m_hInstance,sTempPath,500);
	sTempPath.Replace("BomExport.dll","BOMģ��.xls");
	if(!PathFileExists(sTempPath))
	{
		MessageBox(NULL,CXhChar500("δ�ҵ�ģ���ļ���ģ��·����%s",(char*)sTempPath),"������ʾ",MB_OK);
		return 0;
	}
	pWorksheets=CExcelOper::OpenExcelWorksheets(sTempPath,true);
	if(pWorksheets==NULL)
		return 0;*/
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