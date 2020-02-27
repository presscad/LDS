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
#include "SegI.h"

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
const int COL_COUNT=8;
//�����Ŵ�1��ʼ��0��ʾû�ж�Ӧ��
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=6;
		colIndexArr[0]=1;	//����
		colIndexArr[1]=3;	//���
		colIndexArr[2]=2;	//����
		colIndexArr[3]=5;	//����
		colIndexArr[4]=0;	//����
		colIndexArr[5]=4;	//����
		colIndexArr[6]=0;	//���
		colIndexArr[7]=0;	//����
		return 8;
	}
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

//////////////////////////////////////////////////////////////////////////
//��������
static void GetAngleSimpleSpec(char *simple_spec,double x_wing_wide,double y_wing_wide,double wide_thick)
{
	char sSpec[200]="";
	char sWide[100]="",sThick[100]="";
	sprintf(sWide,"%s",(char*)CXhChar16(x_wing_wide));
	sprintf(sThick,"%s",(char*)CXhChar16(wide_thick));
	if(IsUnequalAngle(x_wing_wide,y_wing_wide,wide_thick))
	{
		char sWideY[100]="";
		sprintf(sWideY,"%s",(char*)CXhChar16(y_wing_wide));
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%.fx%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Remove('[');
		sSpec.Remove('C');
	}
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if( pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C||	//���Ͳ��
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||	//U�Ͳ��
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||	//ʮ�ֲ��
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||	//�Ժ�����
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||	//ƽ������
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||	//���Է���
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)	//���Է���
			sSpec.Copy("");		//��׼��������Ҫ��ʾ���
		else //if(pBomPart->siSubType==0)
		{
			if(bNeedPlateW)
				sSpec.Printf("%.fx%.f",pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("%.f",pBomPart->thick);
		}
	}
	else 
		sSpec.Printf("%s",(char*)pBomPart->sSpec);
	return sSpec;
}

static char GetPartMaterialChar(BOMPART *pBomPart)
{
	if (pBomPart == NULL)
		return '0';
	char cMaterial = pBomPart->cMaterial;
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD || pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 9) ? pBomPart->sPartNo.At(8) : '0';
			if (cMaterial == 'D')
				cMaterial = 'P';
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 6) ? pBomPart->sPartNo.At(5) : '0';
		}
		//�ᵼ����˨�����ʴ��� wht 18-12-30
		/*else if(pBomPart->siSubType!=0)
		{
			cMaterial='0';
		}*/
	}
	return cMaterial;
}

CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
{
	CXhChar100 sSpec, sSpecialPartSpec;
	char cMaterial = pBomPart->cMaterial;
	if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLD || pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 9) ? pBomPart->sPartNo.At(8) : '0';
			if (cMaterial == 'D')
				cMaterial = 'P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_U ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_X ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial = (strlen(pBomPart->sPartNo) >= 6) ? pBomPart->sPartNo.At(5) : '0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if (pBomPart->siSubType == BOMPART::SUB_TYPE_STEEL_GRATING ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_STAY_ROPE ||
			pBomPart->siSubType == BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//�ָ�դ
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//��������
			//const static BYTE SUB_TYPE_LADDER		= 20;	//����
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial = '0';
		}
	}
	CXhChar16 materialStr = cMaterial != '0' ? QuerySteelMatMark(cMaterial) : "";
	if (pBomPart->sPartNo.EqualNoCase("T2105") ||
		pBomPart->sPartNo.EqualNoCase("T2605") ||
		pBomPart->sPartNo.EqualNoCase("T3605") ||
		pBomPart->sPartNo.EqualNoCase("T3980") ||
		pBomPart->sPartNo.EqualNoCase("T5855") ||
		pBomPart->sPartNo.EqualNoCase("T6355") ||
		pBomPart->cPartType == BOMPART::ACCESSORY)
	{
		materialStr = "";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if (pBomPart->cPartType == BOMPART::ANGLE)
		sSpec.Printf("L%.f*%.f", pBomPart->wide, pBomPart->thick);
	else if (pBomPart->cPartType == BOMPART::TUBE)
		sSpec.Printf("��%.f*%.f", pBomPart->wide, pBomPart->thick);
	else if (pBomPart->cPartType == BOMPART::PLATE)
	{
		if (pBomPart->siSubType == 0 || pBomPart->siSubType == BOMPART::SUB_TYPE_BOLT_PAD)
		{
			if (pBomPart->wide > pBomPart->length)
			{
				double len = pBomPart->length;
				pBomPart->length = pBomPart->wide;
				pBomPart->wide = len;
			}
			sSpec.Printf("-%.f*%.f", pBomPart->thick,pBomPart->wide);
		}
		else
			sSpec.Printf("%s", (char*)pBomPart->sSpec);
	}
	else if (pBomPart->cPartType == BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C', '[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits = sSpec.Replace("��", " ");
		hits += sSpec.Replace("��", " ");
		sSpec.Replace("/", " ");
		sSpec.Replace("\\", " ");
		if (hits == 2)
		{
			int D = 0, d = 0;
			sscanf(sSpec, "%d%d", &D, &d);
			sSpec.Printf("��%d/%d", D, d);
		}
		else
			sSpec.Copy(pBomPart->sSpec);
	}
	if (sSpecialPartSpec.GetLength() > 0)
		return sSpecialPartSpec;
	else
		return sSpec;
	/*else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
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
	CStringArray str_arr;
	const int COL_COUNT=6;
	str_arr.SetSize(COL_COUNT);
	str_arr[0] = "�κ�"; str_arr[1]="����";str_arr[2]="����";
	str_arr[3]="���";str_arr[4]="����";str_arr[5]="����";
	double col_arr[6]={8.5,8.5,8.5,8.5,8.5,8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,COL_COUNT);//��ȡExcel���ķ�Χ
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if (pBomPart->iSeg == -1)
		{
			SEGI segI;
			if (ParsePartNo(pBomPart->sPartNo, &segI, NULL))
				pBomPart->iSeg = segI;
		}
		char cMaterial = GetPartMaterialChar(pBomPart);
		CXhChar50 sSpec = GetBomPartSpec(pBomPart);
		map.SetValueAt(index, 0, COleVariant(SEGI(pBomPart->iSeg).ToString()));
		map.SetValueAt(index, 1, COleVariant(pBomPart->sPartNo));
		map.SetValueAt(index, 2, COleVariant(QuerySteelMatMark(cMaterial)));	//����
		map.SetValueAt(index, 3, COleVariant(sSpec));
		map.SetValueAt(index, 4, COleVariant(CXhChar50("%.f", pBomPart->length)));
		map.SetValueAt(index, 5, COleVariant((long)pBomPart->GetPartNum()));
		index++;
	}
	CXhChar16 cell_end("F%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"B2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"B2",cell_end,COleVariant(10.5));
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