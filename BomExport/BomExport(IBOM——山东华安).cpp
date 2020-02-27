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

const int COL_COUNT=6;
//�����Ŵ�1��ʼ��0��ʾû�ж�Ӧ��
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=2;
		colIndexArr[0]=7;	//����
		colIndexArr[1]=10;	//���
		colIndexArr[2]=9;	//����
		colIndexArr[3]=12;	//����
		colIndexArr[4]=13;	//����
		colIndexArr[5]=11;	//����
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//��������
CXhChar100 GetBomPartSpec(BOMPART *pBomPart,bool bPlateSpecIncWidth,char* material=NULL)
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
		else if(pBomPart->siSubType!=0)
		{
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'?QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3805")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T4480")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("L%.f*%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("��%.f*%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
		{
			if(bPlateSpecIncWidth)
				sSpec.Printf("-%.f*%.f",pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("-%.f",pBomPart->thick);
		}
		else 
			sSpec.Printf("%s",pBomPart->sSpec);
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
	{
		if(material!=NULL)
			strcpy(material,materialStr);
		return sSpecialPartSpec;
	}
	else 
	{
		if(material!=NULL)
		{	//�����������
			strcpy(material,materialStr);
			return sSpec;
		}
		else if(materialStr.EqualNoCase("Q235"))
			return sSpec;
		else
			return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);
	}
}
void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	BOOL bTubeTower=FALSE;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		BomArr.append(*pNode);
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		if( !bTubeTower&&
			pBomPart->cPartType==BOMPART::PLATE&&
			(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			 pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR))
			bTubeTower=TRUE;
		else if(!bTubeTower&&pBomPart->cPartType==BOMPART::TUBE&&pBomPart->siSubType==0)
			bTubeTower=TRUE;
	}
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("erp���벿��");	
	CStringArray str_arr;
	const int COL_COUNT=15;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="�Ǽ�����";str_arr[1]="¼��Ա";str_arr[2]="��Ʒ����";str_arr[3]="��Ʒ���";str_arr[4]="��Ʒ����(����)";
	str_arr[5]="�κ�";str_arr[6]="�����";str_arr[7]="��������"; str_arr[8]="����";str_arr[9]="���ȫ��";
	str_arr[10]="�ߴ�";str_arr[11]="��������";str_arr[12]="����";str_arr[13]="�ߴ綨��?";str_arr[14]="��ע";
	double col_arr[15]={16.0,8.5,8.5,8.5,14.0,
						 8.5,8.5,8.5,8.5,8.5,
						 8.5,8.5,8.5,8.5,8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//��ȡExcel���ķ�Χ
	CTime t=CTime::GetCurrentTime();
	CString sTime=t.Format("%Y-%m-%d 00:00:00");
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->iSeg==-1)
		{
			SEGI segI;
			if(ParsePartNo(pBomPart->sPartNo,&segI,NULL))
				pBomPart->iSeg=segI;
		}
		map.SetValueAt(index,0,COleVariant(sTime));
		map.SetValueAt(index,1,COleVariant(bom.m_sOperator));
		if(bTubeTower)
		{
			map.SetValueAt(index,2,COleVariant("�Ǹ���"));
			map.SetValueAt(index,3,COleVariant("001"));
		}	
		else
		{
			map.SetValueAt(index,2,COleVariant("�ֹ���"));
			map.SetValueAt(index,3,COleVariant("002"));
		}
		map.SetValueAt(index,4,COleVariant(bom.m_sTowerTypeName));
		map.SetValueAt(index,5,COleVariant(SEGI(pBomPart->iSeg).ToString()));
		CXhChar16 sMaterial;
		CXhChar100 sSpec=GetBomPartSpec(pBomPart,false,sMaterial);
		map.SetValueAt(index,6,COleVariant(pBomPart->sPartNo));
		if(pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,7,COleVariant("�ֹ�"));
		else
			map.SetValueAt(index,7,COleVariant(pBomPart->GetPartTypeName()));
		map.SetValueAt(index,8,COleVariant(sMaterial));
		if(sSpec.EqualNoCase(pBomPart->sPartNo))
			map.SetValueAt(index,9,COleVariant(""));
		else
			map.SetValueAt(index,9,COleVariant(sSpec));
		if(pBomPart->length>0)
		{
			if(pBomPart->cPartType==BOMPART::ACCESSORY&&pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
				map.SetValueAt(index,10,COleVariant(CXhChar50("%.f*%.f",pBomPart->wide,pBomPart->length)));
			else if(pBomPart->cPartType==BOMPART::PLATE)
				map.SetValueAt(index,10,COleVariant(CXhChar50("0")));
			else
				map.SetValueAt(index,10,COleVariant(CXhChar50("%.f",pBomPart->length)));
		}
		else
			map.SetValueAt(index,10,COleVariant(""));
		map.SetValueAt(index,11,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,12,COleVariant(CXhChar50("%.2f",pBomPart->fPieceWeight)));
		map.SetValueAt(index,13,COleVariant("0"));
		index++;
	}
	CXhChar16 cell_end("O%d",index+1);
	//CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"B2",cell_end,COleVariant("G/ͨ�ø�ʽ"));
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"A1",cell_end,COleVariant("@"));
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"F1",CXhChar16("F%d",index+1),COleVariant((long)3));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"G1",CXhChar16("G%d",index+1),COleVariant((long)4));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"K1",CXhChar16("O%d",index+1),COleVariant((long)4));
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