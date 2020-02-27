// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
// ���ݷɽ�

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"

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
	return 1;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"
long ftol(double fVal)
{
	return (long)(fVal+0.5);
}
CXhChar16 QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else //if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{	
	//1.��ȡBOM
	CModelBOM bom;
	CBuffer buffer(buf_len);
	if(buf_len>0)
	{
		buffer.AttachMemory(data_buf,buf_len);
		bom.FromBuffer(buffer);
	}
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
	str_arr.SetSize(29);
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="����";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="���";str_arr[7]="���";
	str_arr[8]="��������";str_arr[9]="��������";str_arr[10]="��������";str_arr[11]="��ע";
	str_arr[12]="�纸";str_arr[13]="����";str_arr[14]="�н�";str_arr[15]="����";
	str_arr[16]="���";str_arr[17]="���";str_arr[18]="���Ͻ�";str_arr[19]="���";
	str_arr[20]="�¿�";str_arr[21]="�������";str_arr[22]="������ʽ";str_arr[23]="����";
	str_arr[24]="����";str_arr[25]="�ܳ�";str_arr[26]="�����";str_arr[27]="���׾�����";
	str_arr[28]="���ӱ�mm";
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						4.0,7.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,4.0,8.0,7.5,4.0,
						4.0,7.0,12.5,3.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,29);
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		map.SetValueAt(index,0,COleVariant((long)pPart->iSeg));		//�κ�
		CXhChar50 sPartNoIncMat("%s",(char*)pPart->sPartNo);
		if(pPart->cMaterial!='S') 
			sprintf(sPartNoIncMat,"%s%c",(char*)pPart->sPartNo,pPart->cMaterial);
		map.SetValueAt(index,1,COleVariant(sPartNoIncMat));			//���
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//����
		map.SetValueAt(index,3,COleVariant(pPart->GetPartTypeName()));				//����
		map.SetValueAt(index,4,COleVariant(pPart->GetSpec()));		//���
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//���
		map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));	//���
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//��������
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//��������
		map.SetValueAt(index,10,COleVariant(pPart->nMSumLs));		//��������
		map.SetValueAt(index,11,COleVariant(pPart->sNotes));
		if(pPart->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,12,COleVariant("1"));
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//����
			map.SetValueAt(index,13,COleVariant(nHuoquLineCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			long nCutAngleCount=pAngle->CutAngleN();
			if(nCutAngleCount>0)			//�нǵ���
				map.SetValueAt(index,14,COleVariant(nCutAngleCount));
			if(pAngle->bCutBer)				//����
				map.SetValueAt(index,15,COleVariant("1"));
			if(pAngle->bCutRoot)			//���
				map.SetValueAt(index,16,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
				map.SetValueAt(index,17,COleVariant((long)pAngle->nPushFlat));
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//���Ͻ�
				map.SetValueAt(index,18,COleVariant(nKaiHeJiaoN));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//�¿�
			if(pPlate->bNeedFillet)		
				map.SetValueAt(index,20,COleVariant("1"));
			//����
			map.SetValueAt(index,24,COleVariant(pPlate->GetEdgeCount()));
			//�ܳ�
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,25,COleVariant(str));
			//���
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,26,COleVariant(str));
			//���ӱ�mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,28,COleVariant(str));
			}
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			/*if(pPart->IsTubeTransect())	//�������
				map.SetValueAt(index,21,COleVariant("1"));
			//��������
			char sType[100]="";
			int nType=pPart->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,22,COleVariant(sType));*/
		}
		if(pPart->thick>=14)		//���
			map.SetValueAt(index,19,COleVariant("1"));
		map.SetValueAt(index,23,COleVariant(pPart->GetPartSubTypeName()));	//��������
		//�����׾�
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		index++;
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"AC%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
