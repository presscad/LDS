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

const int COL_COUNT=7;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=1;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=5;	//���
		colIndexArr[2]=4;	//����
		colIndexArr[3]=8;	//����
		colIndexArr[4]=9;	//����
		colIndexArr[5]=7;	//����
		colIndexArr[6]=6;	//���
		return 7;
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
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//��ȡExcel���ķ�Χ
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//
		
		map.SetValueAt(index,1,COleVariant(pBomPart->sPartNo));//
		//��������
		CXhChar16 sTypeName=GetPartTypeName(pBomPart);
		map.SetValueAt(index,2,COleVariant(sTypeName));
		//
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
				cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			else if(pBomPart->siSubType!=0)
				cMaterial='0';
		}
		CXhChar16 materialStr=cMaterial!='0'?QuerySteelMatMark(cMaterial):"";
		if( pBomPart->sPartNo.EqualNoCase("T2105")||
			pBomPart->sPartNo.EqualNoCase("T2605")||
			pBomPart->sPartNo.EqualNoCase("T3605")||
			pBomPart->sPartNo.EqualNoCase("T3980")||
			pBomPart->sPartNo.EqualNoCase("T5855")||
			pBomPart->sPartNo.EqualNoCase("T6355")||
			pBomPart->cPartType==BOMPART::ACCESSORY)
			materialStr="";
		if(materialStr.GetLength()>0)
			map.SetValueAt(index,3,COleVariant(CXhChar50("%sB",(char*)materialStr)));
		else
			map.SetValueAt(index,3,COleVariant(""));

		if(pBomPart->cPartType==BOMPART::ANGLE)
			map.SetValueAt(index,4,COleVariant(CXhChar50("��%.f*%.f",pBomPart->wide,pBomPart->thick)));
		else if(pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,4,COleVariant(CXhChar50("��%.f*%.f",pBomPart->wide,pBomPart->thick)));
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if(pBomPart->siSubType==0)
				map.SetValueAt(index,4,COleVariant(CXhChar50("-%.f",pBomPart->thick)));
			else 
				map.SetValueAt(index,4,COleVariant(pBomPart->sSpec));
		}
		else if(pBomPart->cPartType==BOMPART::SLOT)
		{
			CXhChar50 sSpec(pBomPart->sSpec);
			sSpec.Replace('C','[');
			map.SetValueAt(index,4,COleVariant(sSpec));
		}
		else
		{
			CXhChar50 sSpec(pBomPart->sSpec);
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
			map.SetValueAt(index,4,COleVariant(sSpec));
		}
		if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->wide>0)
			map.SetValueAt(index,5,COleVariant(CXhChar50("%.f",pBomPart->wide)));
		else
			map.SetValueAt(index,5,COleVariant(""));
		if(pBomPart->length>0)
			map.SetValueAt(index,6,COleVariant(CXhChar50("%.f",pBomPart->length)));
		else
			map.SetValueAt(index,6,COleVariant(""));
		map.SetValueAt(index,7,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,8,COleVariant(CXhChar50("%.2f",pBomPart->fPieceWeight)));
		
		index++;
		
	}
	CXhChar16 cell_end("I%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
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