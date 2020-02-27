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
		colIndexArr[1]=4;	//���
		colIndexArr[2]=0;	//����
		colIndexArr[3]=6;	//����
		colIndexArr[4]=7;	//����
		colIndexArr[5]=5;	//����
		colIndexArr[6]=0;	//���
		return 7;
	}
}

CXhChar16 GetLocalPartTypeName(BOMPART *pPart)
{
	BYTE cPartType=pPart->cPartType;
	CXhChar16 sPartType;
	if(cPartType==BOMPART::PLATE)		//�ְ�
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="����";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
				pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
			sPartType="��������";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)	//���Ͳ��
			sPartType="C�Ͳ��";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U)	//U�Ͳ��
			sPartType="U�Ͳ��";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)	//ʮ�ֲ��
			sPartType="X�Ͳ��";
		else
			sPartType="�ְ�";
	}
	else if(cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType="�Ǹ�";
	else if(cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType="����";
	else if(cPartType==BOMPART::SLOT)	//�۸�
		sPartType="�۸�";
	else if(cPartType==BOMPART::FLAT)	//���
		sPartType="���";
	else if(cPartType==BOMPART::ROUND)	//Բ��
	{
		sPartType="Բ��";
		if(pPart->sPartNo.GetLength()>0)
		{
			if(pPart->sPartNo.At(0)=='T')
				sPartType="����";
			else if(strstr(pPart->sPartNo,"GJ-")||
				strstr(pPart->sPartNo,"NUT-")||
				strstr(pPart->sPartNo,"U-")||
				strstr(pPart->sPartNo,"NX-")||
				strstr(pPart->sPartNo,"JK-"))
				sPartType="��������";
		}
	}
	CXhChar50 sSpec(pPart->sSpec);
	int hits=sSpec.Replace("��"," ");
	hits+=sSpec.Replace("��"," ");
	if(hits>=1)
	{
		if( sSpec.Replace("/"," ")||sSpec.Replace("\\"," ")||
			sSpec.Replace("X"," ")||sSpec.Replace("x"," ")||
			sSpec.Replace("*"," ")||sSpec.Replace("��"," "))
			sPartType="����";
		else
			sPartType="Բ��";
	}
	return sPartType;
}
CXhChar16 LocalQuerySteelMatMark(char cMat)
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
static CXhChar16 GetMaterialStr(BOMPART* pBomPart,BOOL bIncQuality)
{
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
	CXhChar16 materialStr=cMaterial!='0'? LocalQuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
		materialStr="";
	if(materialStr.GetLength()>0)
	{
		if(bIncQuality)
			materialStr.Append("B");
	}
	return materialStr;
}

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
		sprintf(sSpec,"��%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"��%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE,BOOL bIncMat=FALSE)
{
	CXhChar16 sMat=bIncMat?GetMaterialStr(pBomPart,true):CXhChar16();
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
	{
		CXhChar50 ss;
		GetAngleSimpleSpec(ss,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
		sSpec.Append(sMat);
		sSpec.Append(ss);
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%s��%s*%s",(char*)sMat,(char*)CXhChar16(pBomPart->wide),(char*)CXhChar16(pBomPart->thick));
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		CXhChar50 ss(pBomPart->sSpec);
		ss.Replace('C','[');
		if(pBomPart->sSpec.StartWith('Q'))
			sSpec.Copy(ss);
		else
			sSpec.Printf("%s%s",(char*)sMat,(char*)ss);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0||(pBomPart->wide>0&&pBomPart->length>0&&pBomPart->thick>0))
		{
			if(bNeedPlateW)
				sSpec.Printf("%s-%.f*%.f",(char*)sMat,pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("%s-%.f",(char*)sMat,pBomPart->thick);
		}
		else	//��塢���������������������ű���һ��
			sSpec.Copy(pBomPart->sPartNo);
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
			sSpec.Printf("%s��%d/%d",(char*)sMat,D,d);
		}
		else 
		{
			if(pBomPart->sSpec.StartWith('Q'))
				sSpec.Copy(pBomPart->sSpec);
			else
				sSpec.Printf("%s%s",(char*)sMat,(char*)pBomPart->sSpec);
		}
	}
	return sSpec;
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
	//2.2.1 ��ӱ�����
	CStringArray str_arr;
	str_arr.SetSize(9);
	str_arr[0]="����";str_arr[1]="���";str_arr[2]="����";str_arr[3]="���";str_arr[4]="����";
	str_arr[5]="��������";str_arr[6]="һ��";str_arr[7]="С��";str_arr[8]="��ע";
	double col_arr[9]={9.75,9.75,9.75,16.75,9.75,
					   9.75,9.75,9.75,9.75};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//
	int index=0;
	CVariant2dArray map(BomArr.Size()*2,9);//��ȡExcel���ķ�Χ
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		CorrectBomPartSegI(pBomPart);
		if(pBomPart->iSeg>0)
			map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//
		map.SetValueAt(index,1,COleVariant(pBomPart->sPartNo));//
		//��������
		CXhChar16 sTypeName=GetLocalPartTypeName(pBomPart);
		map.SetValueAt(index,2,COleVariant(sTypeName));
		//
		if( pBomPart->cPartType==BOMPART::PLATE&&
			pBomPart->length>0&&pBomPart->wide>0&&
			pBomPart->wide>pBomPart->length)
		{
			double wide=pBomPart->wide;
			pBomPart->wide=pBomPart->length;
			pBomPart->length=wide;
		}
		CXhChar50 sSpec=GetSpecStr(pBomPart,TRUE,TRUE);
		map.SetValueAt(index,3,COleVariant(sSpec));
		if(pBomPart->length>0)
			map.SetValueAt(index,4,COleVariant(CXhChar50("%.f",pBomPart->length)));
		map.SetValueAt(index,5,COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index,6,COleVariant(CXhChar50("%.2f",pBomPart->fPieceWeight)));
		map.SetValueAt(index,7,COleVariant(CXhChar50("%.2f",pBomPart->fSumWeight)));
		index++;
	}
	CXhChar16 cell_end("I%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(11.0));
	CExcelOper::SetRangeRowHeight(excel_sheet,19.5,"A1",cell_end,TRUE);
	CExcelOper::SetBoldFont(excel_sheet,"A1","I1");
	CExcelOper::SetFontColor(excel_sheet,"A1","I1",CExcelOper::COLOR_ID_RED);
	excel_sheet.ReleaseDispatch();
}

void AddPartTblSheet2(CModelBOM &bom, LPDISPATCH pWorkSheet)
{
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for (SUPERLIST_NODE<BOMPART> *pNode = bom.listParts.EnumFirst(); pNode; pNode = bom.listParts.EnumNext())
	{
		BOMPART* pBomPart = (BOMPART*)pNode->pDataObj;
		BomArr.append(*pNode);
	}
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet, FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("ͼֽ����");
	//2.2.1 ��ӱ�����
	CStringArray str_arr;
	str_arr.SetSize(10);
	str_arr[0] = "����"; str_arr[1] = "���"; str_arr[2] = "����"; str_arr[3] = "����"; str_arr[4] = "���"; 
	str_arr[5] = "����";str_arr[6] = "��������"; str_arr[7] = "һ��"; str_arr[8] = "С��"; str_arr[9] = "��ע";
	double col_arr[10] = { 9.75,9.75,9.75,16.75,9.75,
					      9.75,9.75,9.75,9.75,9.75 };
	CExcelOper::AddRowToExcelSheet(excel_sheet, 1, str_arr, col_arr, TRUE);
	//
	int index = 0;
	CVariant2dArray map(BomArr.Size() * 2, 10);//��ȡExcel���ķ�Χ
	for (SUPERLIST_NODE<BOMPART>*pNode = BomArr.GetFirst(); pNode; pNode = BomArr.GetNext())
	{
		BOMPART* pBomPart = (BOMPART*)pNode->pDataObj;
		CorrectBomPartSegI(pBomPart);
		if (pBomPart->iSeg > 0)
			map.SetValueAt(index, 0, COleVariant(SEGI(pBomPart->iSeg).ToString()));//
		map.SetValueAt(index, 1, COleVariant(pBomPart->sPartNo));//
		//��������
		CXhChar16 sTypeName = GetLocalPartTypeName(pBomPart);
		map.SetValueAt(index, 2, COleVariant(sTypeName));
		//
		if (pBomPart->cPartType == BOMPART::PLATE&&
			pBomPart->length > 0 && pBomPart->wide > 0 &&
			pBomPart->wide > pBomPart->length)
		{
			double wide = pBomPart->wide;
			pBomPart->wide = pBomPart->length;
			pBomPart->length = wide;
		}
		CXhChar16 sMat = GetMaterialStr(pBomPart, FALSE);
		map.SetValueAt(index, 3, COleVariant(sMat));
		CXhChar50 sSpec = GetSpecStr(pBomPart, TRUE,FALSE);
		map.SetValueAt(index, 4, COleVariant(sSpec));
		if (pBomPart->length > 0)
			map.SetValueAt(index, 5, COleVariant(CXhChar50("%.f", pBomPart->length)));
		map.SetValueAt(index, 6, COleVariant((long)pBomPart->GetPartNum()));
		map.SetValueAt(index, 7, COleVariant(CXhChar50("%.2f", pBomPart->fPieceWeight)));
		map.SetValueAt(index, 8, COleVariant(CXhChar50("%.2f", pBomPart->fSumWeight)));
		index++;
	}
	CXhChar16 cell_end("J%d", index + 1);
	CExcelOper::SetRangeValue(excel_sheet, "A2", cell_end, map.var);
	CExcelOper::SetRangeBorders(excel_sheet, "A1", cell_end, COleVariant(11.0));
	CExcelOper::SetRangeRowHeight(excel_sheet, 19.5, "A1", cell_end, TRUE);
	CExcelOper::SetBoldFont(excel_sheet, "A1", "J1");
	CExcelOper::SetFontColor(excel_sheet, "A1", "J1", CExcelOper::COLOR_ID_RED);
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