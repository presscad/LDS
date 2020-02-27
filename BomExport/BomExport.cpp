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
	return 2;
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
static CXhChar16 GetPartTypeName(BOMPART *pPart)
{
	BYTE cPartType=pPart->cPartType;
	CXhChar16 sPartType;
#ifdef AFX_TARG_ENU_ENGLISH
	if(cPartType==BOMPART::PLATE)		//�ְ�
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="Flange";
		else
			sPartType="Plate";
	}
	else if(cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType="Angle";
	else if(cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType="Tube";
	else if(cPartType==BOMPART::SLOT)	//�۸�
		sPartType="U-steel";
	else if(cPartType==BOMPART::FLAT)	//���
		sPartType="Flat Steel";
#else
	if(cPartType==BOMPART::PLATE)		//�ְ�
	{
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType="����";
		else
			sPartType="�ְ�";
	}
	else if(cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType="�Ǹ�";
	else if(cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType="�ֹ�";
	else if(cPartType==BOMPART::SLOT)	//�۸�
		sPartType="�۸�";
	else if(cPartType==BOMPART::FLAT)	//���
		sPartType="���";
#endif
	return sPartType;
}
static CXhChar16 GetPartSpec(BOMPART *pPart)
{
	CXhChar16 sSpec=pPart->GetSpec();
	if(GetSupportDataBufferVersion()>=3)
	{
		if(pPart->cPartType!=BOMPART::PLATE)
			return sSpec;
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		if(pPlate->IsFL()||pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
			sSpec.Printf("��%.f/%.f",pPlate->fFlOutterD,pPlate->fFlInnerD);
	}
	return sSpec;
}
#ifdef __TMA_BOM_
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
	str_arr.SetSize(34);
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="����";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="���";str_arr[7]="���";
	str_arr[8]="��������";str_arr[9]="��������";str_arr[10]="��������";str_arr[11]="��ע";
	str_arr[12]="�纸";str_arr[13]="����";str_arr[14]="����";str_arr[15]="����";
	str_arr[16]="������";str_arr[17]="������";str_arr[18]="�н�";str_arr[19]="����";
	str_arr[20]="���";str_arr[21]="���";str_arr[22]="���Ͻ�";str_arr[23]="���";
	str_arr[24]="�¿�";str_arr[25]="����";str_arr[26]="�������";str_arr[27]="������ʽ";str_arr[28]="����";
	str_arr[29]="����";str_arr[30]="�ܳ�";str_arr[31]="�����";str_arr[32]="���׾�����";
	str_arr[33]="���ӱ�mm";
	double col_arr[34]={3.0,7.5,3.5,4.5,8.5,
						5.5,4.5,3.0,5.0,5.0,
						4.0,7.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,4.0,8.0,7.5,4.0,
						4.0,7.0,12.5,3.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,34);
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
		map.SetValueAt(index,3,COleVariant(GetPartTypeName(pPart)));				//����
		map.SetValueAt(index,4,COleVariant(GetPartSpec(pPart)));	//���
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//���
		map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));	//���
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//��������
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//��������
		map.SetValueAt(index,10,COleVariant(pPart->nMSumLs));		//��������
		CXhChar200 sNotes("%s",pPart->sNotes);
		map.SetValueAt(index,11,COleVariant(pPart->sNotes));
		if(pPart->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,12,COleVariant("1"));
		else
		{
			LIST_NODE<BOM_WELDPART> *pWeldNode=NULL;
			for(pWeldNode=bom.listWeldParts.EnumFirst();pWeldNode;pWeldNode=bom.listWeldParts.EnumNext())
			{
				if(pWeldNode->data.sPartNo.EqualNoCase(pPart->sPartNo))
					break;
			}
			if(pWeldNode)	//�麸��������Ҳ��Ҫ����Ϊ�纸��
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//����
			map.SetValueAt(index,13,COleVariant(nHuoquLineCount));
		long nCutAngleCount=pPart->CutAngleN();
		if(nCutAngleCount>0)		//�нǵ���
			map.SetValueAt(index,18,COleVariant(nCutAngleCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			BYTE cHuoquType=pAngle->GetHuoquType();
			if(cHuoquType==1)		//����
				map.SetValueAt(index,14,COleVariant("1"));
			else if(cHuoquType==2)	//����
				map.SetValueAt(index,15,COleVariant("1"));
			else if(cHuoquType==3)	//������
				map.SetValueAt(index,16,COleVariant("1"));
			else if(cHuoquType==4)	//������
				map.SetValueAt(index,17,COleVariant("1"));
			if(pAngle->bCutBer)				//����
				map.SetValueAt(index,19,COleVariant("1"));
			if(pAngle->bCutRoot)			//���
				map.SetValueAt(index,20,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
			{
				long nPushState=0;
				if(pAngle->nPushFlat==0x03||	//ѹ��״̬��011(ʼ���С���)
				   pAngle->nPushFlat==0x06)		//ѹ��״̬��110(ʼ���С���)
				   nPushState=5;				//5.�м�+һ��
				else if(pAngle->nPushFlat==0x07)//ѹ��״̬��111(ʼ���С���)
					nPushState=4;				//4.�м�+����
				else if(pAngle->nPushFlat==0x02)//ѹ��״̬��010(ʼ���С���)
					nPushState=3;				//3.�м�ѹ��
				else if(pAngle->nPushFlat==0x05)//ѹ��״̬��101(ʼ���С���)
					nPushState=2;				//2.����ѹ��
				else if(pAngle->nPushFlat==0x01||//ѹ��״̬��100(ʼ���С���)
						pAngle->nPushFlat==0x04) //ѹ��״̬��001(ʼ���С���)
					nPushState=1;				//1.һ��ѹ��

				map.SetValueAt(index,21,COleVariant((long)nPushState));
			}
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//���Ͻ�
				map.SetValueAt(index,22,COleVariant(nKaiHeJiaoN));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//�¿�
			if(pPlate->bWeldPart&&pPlate->thick>=8)
				map.SetValueAt(index,24,COleVariant("1"));
			//����
			long nArcEdgeCount=pPlate->GetArcEdgeCount();
			if(nArcEdgeCount>0)
				map.SetValueAt(index,25,COleVariant(nArcEdgeCount));
			//����
			map.SetValueAt(index,29,COleVariant(pPlate->GetEdgeCount()-nArcEdgeCount));
			//�ܳ�
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,30,COleVariant(str));
			//���
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,31,COleVariant(str));
			//���ӱ�mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,33,COleVariant(str));
			}
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			map.SetValueAt(index,21,COleVariant(nTransect));	//�������
			//��������
			/*char sType[100]="";
			int nType=pPart->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,22,COleVariant(sType));*/
		}
		//if(pPart->thick>=14)		//���
		//	map.SetValueAt(index,23,COleVariant("1"));
		char cMat=toupper(pPart->cMaterial);
		if( (pPart->thick>=16&&cMat=='S')||
			(pPart->thick>=14&&cMat=='H')||
			(pPart->thick>=12&&(cMat=='P'||cMat=='T')))
			map.SetValueAt(index,23,COleVariant("1"));
		map.SetValueAt(index,28,COleVariant(pPart->GetPartSubTypeName()));	//��������
		//�����׾�
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,32,COleVariant(sHoleStr));
		index++;
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"AH%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
#else
//�����麸�������ͱ�
//nRuleType �������ͱ�Ź���
//nRuleType==0 ���ţ�LRM(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ� 3.�ڵ������ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL 6.��� 7.ƽ���и�
//nRuleType==1 ���ţ�LMR(������) 
//				Mλ: (�ֹ��м����ӷ�ʽ) 0.������ 1.�ڵ�� 2.���ֹ�
//			  L,Rλ: (�ֹ����˿��۷�ʽ) 1.���Ͳ�� 2.U�Ͳ�� 3.T�Ͳ�� 4.X�Ͳ�� 5.FL
void AddSlotAndWeldCodeTblSheet(LPDISPATCH pSheet,int nRuleType)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Slot Group Weld Type Table");
#else 
	excel_sheet.SetName("�����麸�������ͱ�");
#endif
	//
	int nMidType=4,nType=7,nRowNum=0;
	int i=0,j=0,k=0,index=0;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//�м�������ʽ
		nType=5;	//����������ʽ
		nRowNum++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
				nRowNum++;
		}
	}
	nRowNum+=7;
	//
	CVariant2dArray map(nRowNum+1,3);
	//��һ������������У���Ҫ��ʽ���
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Sequence Number"));
	map.SetValueAt(index,1,COleVariant("Code Name"));
	map.SetValueAt(index,2,COleVariant("Instruction"));
#else 
	map.SetValueAt(index,0,COleVariant("���"));
	map.SetValueAt(index,1,COleVariant("����"));
	map.SetValueAt(index,2,COleVariant("˵��"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,60.0,"C1",NULL);
	//
	int nCurRow=2;
#ifdef AFX_TARG_ENU_ENGLISH
	CString sMidTypeName[4]={"","Center Node Plate","Center Intersect Tube","Center Intersect Tube And Node Plate"};
	CString sTypeName[7]={"1-type Insert Plate","U Insert Plate","T Insert Plate","Cross Insert Plate","Flange","Intersect","Plane Cut"};
#else 
	CString sMidTypeName[4]={"","�м�ڵ��","�м�����","�м����ܼ��ڵ��"};
	CString sTypeName[7]={"һ���Ͳ��","U�Ͳ��","T�Ͳ��","ʮ�ֲ��","����","���","ƽ���и�"};
#endif
	nMidType=4;
	nType=7;
	if(nRuleType==1)
	{	//LMR
		nMidType=3;	//�м�������ʽ
		nType=5;	//����������ʽ
		map.SetValueAt(index,0,COleVariant("1"));
		map.SetValueAt(index,1,COleVariant("0"));
#ifdef AFX_TARG_ENU_ENGLISH
		map.SetValueAt(index,2,COleVariant("Main Tube Weld"));
#else 
		map.SetValueAt(index,2,COleVariant("���ܺ���"));
#endif
		index++;
		nCurRow++;
	}
	for(i=1;i<nType+1;i++)
	{
		for(j=i;j<nType+1;j++)
		{
			for(k=0;k<nMidType;k++)
			{
				char sID[50]="";
				sprintf(sID,"%d",nCurRow-1);
				map.SetValueAt(index,0,COleVariant(sID));
				char sCode[50]="";
				//���
				if(nRuleType==0)
				{	//LRM
					if(k==0)
						sprintf(sCode,"%d%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,j,k);
				}
				else if(nRuleType==1)
				{	//LMR
					if(k==0)
						sprintf(sCode,"%d0%d",i,j);
					else 
						sprintf(sCode,"%d%d%d",i,k,j);
				}
				map.SetValueAt(index,1,COleVariant(sCode));
				char sHelpStr[200]="";
				if(i==j)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sHelpStr,"Two End %s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"One End %s Anoter End %s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#else 
					sprintf(sHelpStr,"����%s%s",sTypeName[i-1],sMidTypeName[k]);
				else 
					sprintf(sHelpStr,"һ��%s��һ��%s%s",sTypeName[i-1],sTypeName[j-1],sMidTypeName[k]);
#endif
				map.SetValueAt(index,2,COleVariant(sHelpStr));
				index++;
				nCurRow++;
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	CString sHelpStr[7]={"Angle Weld Part","Angle Rib Plate","Only Plate","Tower Foot","Handrail Weld Part","Ladder Weld Part","Other Weld Part"};
#else 
	CString sHelpStr[7]={"����Ǹֺ���","�ǸּӾ���","���庸��","���ź���","���ֺ���","���ݺ���","��������"};
#endif
	for(i=0;i<7;i++)
	{
		char sCode[50]="";
		//���
		if(nRuleType==0)
		{	//LRM
			if(i==5)
				strcpy(sCode,"A");
			else if(i==6)
				strcpy(sCode,"B");
			else if(i==0||i==1)
				sprintf(sCode,"%d",i+8);
			else
				continue;	//��ʽһ����ӣ�"�ǸּӾ���","���庸��","���ź���"����
		}
		else if(nRuleType==1)	//LMR
		{
			if(i<4)
				sprintf(sCode,"60%d",i);
			else
				sprintf(sCode,"%d00",i+3);
		}
		char sID[50]="";
		sprintf(sID,"%d",nCurRow-1);
		map.SetValueAt(index,0,COleVariant(sID));
		map.SetValueAt(index,1,COleVariant(sCode));
		map.SetValueAt(index,2,COleVariant(sHelpStr[i]));
		nCurRow++;
		index++;
	}
	char cell_end[15]="";
	//���ñ������
	sprintf(cell_end,"C%d",nCurRow-1);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.DetachDispatch();
}
void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorksheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("����ԭʼ���ϱ�");
	//2.2 ������ϱ�
	//2.2.1 ��ӱ�����
	const int COL_COUNT=38;
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="���";str_arr[1]="�κ�";str_arr[2]="�����";str_arr[3]="����";str_arr[4]="����";
	str_arr[5]="���";str_arr[6]="���";str_arr[7]="����";str_arr[8]="������";str_arr[9]="����";
	str_arr[10]="����";str_arr[11]="ѹ��";str_arr[12]="�纸";str_arr[13]="�н�";str_arr[14]="����";
	str_arr[15]="�ٸ�";str_arr[16]="���";str_arr[17]="���Ͻ�";str_arr[18]="���";str_arr[19]="����";
	str_arr[20]="����";str_arr[21]="�����";str_arr[22]="���";str_arr[23]="���ӳ���";str_arr[24]="���ܳ�";
	str_arr[25]="�·���";str_arr[26]="�������";str_arr[27]="�¿�";str_arr[28]="����";str_arr[29]="��λԲ";
	str_arr[30]="�Ƿ��׽�";str_arr[31]="����";str_arr[32]="���׾�����";str_arr[33]="��ע";str_arr[34]="����";
	str_arr[35]="����";str_arr[36]="����";str_arr[37]="�����";
	double col_arr[COL_COUNT]={ 3.0,3.0,7.5,3.5,4.5,
								8.5,4.5,4.5,2.5,5.0,
								5.0,2.5,2.5,2.5,2.5,
								2.5,2.5,2.5,2.5,2.5,
								2.5,2.5,2.5,5.0,5.0,
								2.5,2.5,2.5,3.5,7.5,
								2.5,7.5,8.0,7.5,2.5,
								2.5,2.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,COL_COUNT);
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		map.SetValueAt(index,0,COleVariant((long)i));				//���
		map.SetValueAt(index,1,COleVariant((long)pPart->iSeg));		//�κ�
		CXhChar50 sPartNoIncMat("%s",(char*)pPart->sPartNo);
		if(pPart->cMaterial!='S') 
			sprintf(sPartNoIncMat,"%s%c",(char*)pPart->sPartNo,pPart->cMaterial);
		map.SetValueAt(index,2,COleVariant(sPartNoIncMat));			//���
		map.SetValueAt(index,3,COleVariant(GetPartTypeName(pPart)));				//����
		map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//����
		map.SetValueAt(index,5,COleVariant(GetPartSpec(pPart)));	//���
		map.SetValueAt(index,6,COleVariant(ftol(pPart->wide)));		//���
		map.SetValueAt(index,7,COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index,8,COleVariant(pPart->GetPartNum()));	//��������
		double weight=ftol(pPart->fPieceWeight*100)/100.0;
		str.Format("%.2f",weight);			
		map.SetValueAt(index,9,COleVariant(str));					//��������
		double sum_weight=ftol(pPart->fPieceWeight*pPart->GetPartNum()*100)/100.0;
		str.Format("%.2f",weight);
		map.SetValueAt(index,10,COleVariant(str));		//����
		if(pPart->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,12,COleVariant("1"));
		else
		{
			LIST_NODE<BOM_WELDPART> *pWeldNode=NULL;
			for(pWeldNode=bom.listWeldParts.EnumFirst();pWeldNode;pWeldNode=bom.listWeldParts.EnumNext())
			{
				if(pWeldNode->data.sPartNo.EqualNoCase(pPart->sPartNo))
					break;
			}
			if(pWeldNode)	//�麸��������Ҳ��Ҫ����Ϊ�纸��
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nCutAngleCount=pPart->CutAngleN();
		if(nCutAngleCount>0)		//�нǵ���
			map.SetValueAt(index,13,COleVariant(nCutAngleCount));
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//����
			map.SetValueAt(index,19,COleVariant(nHuoquLineCount));
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			if(pAngle->bCutBer)				//����
				map.SetValueAt(index,14,COleVariant("1"));
			if(pAngle->bCutRoot)			//���
				map.SetValueAt(index,15,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
			{
				long nPushState=0;
				if(pAngle->nPushFlat==0x03||	//ѹ��״̬��011(ʼ���С���)
				   pAngle->nPushFlat==0x06)		//ѹ��״̬��110(ʼ���С���)
				   nPushState=5;				//5.�м�+һ��
				else if(pAngle->nPushFlat==0x07)//ѹ��״̬��111(ʼ���С���)
					nPushState=4;				//4.�м�+����
				else if(pAngle->nPushFlat==0x02)//ѹ��״̬��010(ʼ���С���)
					nPushState=3;				//3.�м�ѹ��
				else if(pAngle->nPushFlat==0x05)//ѹ��״̬��101(ʼ���С���)
					nPushState=2;				//2.����ѹ��
				else if(pAngle->nPushFlat==0x01||//ѹ��״̬��100(ʼ���С���)
						pAngle->nPushFlat==0x04) //ѹ��״̬��001(ʼ���С���)
					nPushState=1;				//1.һ��ѹ��

				map.SetValueAt(index,16,COleVariant((long)nPushState));
			}
			long nKaiHeJiaoN=pAngle->KaiHeJiaoN();
			if(nKaiHeJiaoN>0)				//���Ͻ�
				map.SetValueAt(index,17,COleVariant(nKaiHeJiaoN));
			BYTE cHuoquType=pAngle->GetHuoquType();
			if(cHuoquType==1||cHuoquType==3)		//����
				map.SetValueAt(index,35,COleVariant("1"));
			else if(cHuoquType==2||cHuoquType==4)	//����
				map.SetValueAt(index,36,COleVariant("1"));
		}
		else if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//���
			char cMat=toupper(pPart->cMaterial);
			if( (pPart->thick>=16&&cMat=='S')||
				(pPart->thick>=14&&cMat=='H')||
				(pPart->thick>=12&&(cMat=='P'||cMat=='T')))
				map.SetValueAt(index,22,COleVariant("1"));			//���
			//���ӱ�mm
			if(pPlate->fWeldEdgeLen>0)
			{
				str.Format("%.1f",pPlate->fWeldEdgeLen);
				map.SetValueAt(index,23,COleVariant(str));
			}
			//�ܳ�
			str.Format("%.1f",pPlate->fPerimeter);
			map.SetValueAt(index,24,COleVariant(str));
			//�¿�
			if(pPlate->bNeedFillet||(pPlate->bWeldPart&&pPlate->thick>=8))
			{
				if(pPlate->thick>=8&&pPlate->thick<=12)
					map.SetValueAt(index,27,COleVariant("1"));
				else if(pPlate->thick>=14)
					map.SetValueAt(index,27,COleVariant("2"));
			}
			//����
			long nArcEdgeCount=pPlate->GetArcEdgeCount();
			map.SetValueAt(index,28,COleVariant(pPlate->GetEdgeCount()-nArcEdgeCount));
			if(pPlate->IsFL()||pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
			{
				map.SetValueAt(index,7,COleVariant(ftol(pPart->thick)));			//�������
				map.SetValueAt(index,29,COleVariant(ftol(pPlate->fFlBoltLayoutD)));	//������˨��λԲ
			}
			//�׽�
			if(pPlate->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
				map.SetValueAt(index,30,COleVariant("1"));
			//����
			if(nArcEdgeCount>0)
				map.SetValueAt(index,34,COleVariant(nArcEdgeCount));
			//���
			str.Format("%.1f",pPlate->real_area);
			map.SetValueAt(index,37,COleVariant(str));
		}
		else if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			//��������
			char sType[100]="";
			int nType=pBomTube->GetNotchType(sType,1);
			if(nType>0)
				map.SetValueAt(index,20,COleVariant(sType));
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,21,COleVariant(nTransect));	//�������
		}
		map.SetValueAt(index,31,COleVariant(pPart->GetPartSubTypeName()));	//��������
		//�����׾�
		char sHoleStr[500]="";
		if(pPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,32,COleVariant(sHoleStr));
		CXhChar200 sNotes("%s",pPart->sNotes);
		if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			if( pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
				pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
				sNotes.Append(" �����¿�");
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
					pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
				sNotes.Append(" һ���¿�");
		}
		map.SetValueAt(index,33,COleVariant(pPart->sNotes));
		index++;
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"E%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"AL%d",i+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
//Ϋ�������麸��ϸ�� wht 10-09-30
void AddPartGroupSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
#ifdef AFX_TARG_ENU_ENGLISH
	excel_sheet.SetName("Group Weld Part Table");
#else 
	excel_sheet.SetName("�麸����ϸ��");
#endif
	//ͳ������
	int nRowNum=1;
	LIST_NODE<BOM_WELDPART> *pNode=NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode=NULL;
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext())
			nRowNum++;
	}
	int index=0;
	CVariant2dArray map(nRowNum,4);
#ifdef AFX_TARG_ENU_ENGLISH
	map.SetValueAt(index,0,COleVariant("Segment Number"));
	map.SetValueAt(index,1,COleVariant("Group Part No."));
	map.SetValueAt(index,2,COleVariant("Piece Part"));
	map.SetValueAt(index,3,COleVariant("Single Group Amount"));
#else 
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("�����"));
	map.SetValueAt(index,2,COleVariant("���"));
	map.SetValueAt(index,3,COleVariant("�������"));
#endif
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","B1");
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"D1",NULL);
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext(),index++)
		{
			//PART *pPart=mat_tbl.GetPartFromPartNo(pPartRef->sPartNo);
			//if(pPart==NULL)
			//	continue;
			map.SetValueAt(index,0,COleVariant((long)pNode->data.iSeg));			//�κ�
			map.SetValueAt(index,1,COleVariant(pNode->data.sPartNo));
			map.SetValueAt(index,2,COleVariant(pPartRefNode->data.sPartNo));		//���
			map.SetValueAt(index,3,COleVariant((long)pPartRefNode->data.nPartNum));	//��������
		}
	}
	char cell_end[15]="";
	//���������
	sprintf(cell_end,"C%d",nRowNum);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//���ñ������
	sprintf(cell_end,"D%d",nRowNum);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	//
	excel_sheet.ReleaseDispatch();	
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
	//2.���ɣţأãţ̲��ϱ�
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(3);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	//2.1 ������ϸ��
	AddPartSpecTblSheet(bom,pWorksheet);
	//2.2 ���۴��ű�
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddSlotAndWeldCodeTblSheet(pWorksheet,1);
	//2.3 �麸����ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 3));
	AddPartGroupSumMatTbl(bom,pWorksheet);
	excel_sheets.DetachDispatch();
	return 0;
}
#endif