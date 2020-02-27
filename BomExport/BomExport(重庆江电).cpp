// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
//���콭������豸���޹�˾

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
	return 3;
}

const int BOM_SHEET_PART		= 0x01;	//������ϸ��
const int BOM_SHEET_BOLT		= 0x02;	//��˨��ϸ��
const int BOM_SHEET_WELDPART	= 0x04;	//�麸����ϸ��
TOWER_BOM_EXPORTS_API DWORD GetSupportBOMType()
{
	return BOM_SHEET_BOLT;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"
//�Ƚ϶κ�
static int CompareSegiFunc(const SEGI& item1,const SEGI& item2)
{
	if(item1.iSeg>item2.iSeg)
		return 1;
	else if(item1.iSeg<item2.iSeg)
		return -1;
	else 
		return 0;
}
static CXhChar16 GetBoltTypeStr(BOMBOLT& bolt)
{
	CXhChar16 sBoltType="";
	if(!bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //��ͨ��˨
		sBoltType="��ͨ��˨";
	else if(!bolt.bFootNail&&bolt.bTwoCap&&!bolt.bAntiTheft) //˫ñ��˨
		sBoltType="˫ñ��˨";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //��ͨ�Ŷ�
		sBoltType="��ͨ�Ŷ�";
	else if(!bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //������˨
		sBoltType="������˨";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //�����Ŷ�
		sBoltType="�����Ŷ�";
	else if (bolt.IsDianQuan())
		sBoltType="��Ƭ";
	return sBoltType;
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
	if(bom.m_nBoltsN<=0)
		return 0;
	//��������
	ARRAY_LIST<SEGI> segIArr;
	for(int i=0;i<bom.m_nBoltsN;i++)
	{
		BOMBOLT bomBolt=bom.m_pBoltRecsArr[i];
		SEGI *pSegI=NULL;
		for(pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			if(pSegI->iSeg==bomBolt.iSeg)
				break;
		}
		if(pSegI==NULL)
			segIArr.append(SEGI(bomBolt.iSeg));
	}
	CBubbleSort<SEGI>::BubSort(segIArr.m_pData,segIArr.GetSize(),CompareSegiFunc);
	int nColCount=segIArr.GetSize()+4;	//��Ӷκ� ���ƣ���˨�ȼ�������޿۳���lg�� �ϼ� ����   �����Ǵ�0��ʼ�� 5-1
	CXhChar50 sMaxColCell;
	int nLen=nColCount/26;
	int nSubLen=nColCount%26;
	if(nLen>26)
		return 0;	//��ʱ��֧��A~AZ
	if(nLen>0)
		sMaxColCell.Printf("%c%c",'A'+nLen-1,'A'+nSubLen);
	else
		sMaxColCell.Printf("%c",'A'+nSubLen);
	//
	int nBoltCount=0;
	ARRAY_LIFE<BOMBOLT_BOX> arrayLife;
	BOMBOLT_BOX *boltArr=(BOMBOLT_BOX*)arrayLife.Create(bom.m_nBoltsN);
	GetBomBoltBoxArr(bom,boltArr,nBoltCount);
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
	CXhChar50 sTempCell("%s1",(char*)sMaxColCell);
	CExcelOper::MergeColRange(excel_sheet,"A1",sTempCell);
	CExcelOper::SetMainTitle(excel_sheet,"A1", sTempCell,"���콭������豸���޹�˾",30,20);
	sTempCell.Printf("%s2",(char*)sMaxColCell);
	CExcelOper::MergeColRange(excel_sheet,"A2",sTempCell);
	CExcelOper::SetMainTitle(excel_sheet,"A2", sTempCell,"�����",20,15);
	CExcelOper::SetFontBold(excel_sheet,0,2,1);
	//������
	CExcelOper::MergeColRange(excel_sheet,"A3","C3");
	CExcelOper::SetMainTitle(excel_sheet,"A3","A3","�������ƣ�",15,10,(short)2,(short)2,false);
	//
	CExcelOper::MergeColRange(excel_sheet,"D3","F3");
	CExcelOper::SetMainTitle(excel_sheet,"D3","D3","������:",15,10,(short)2,(short)2,false);

	CExcelOper::MergeColRange(excel_sheet,"G3","I3");
	CExcelOper::SetMainTitle(excel_sheet,"G3","G3","����:",15,10,(short)2,(short)2,false);
	CExcelOper::SetFontBold(excel_sheet,0,3,3);
	//������

	CXhChar100 sTowerType="����:";
	sTowerType.Append(bom.m_sTowerTypeName);
	CExcelOper::MergeColRange(excel_sheet,"A4","C4");
	CExcelOper::SetFontBold(excel_sheet,0,4,1);
	CExcelOper::SetMainTitle(excel_sheet,"A4","A4",(char*)sTowerType,15,10,(short)2);
	//
	CExcelOper::MergeColRange(excel_sheet,"D4","F4");
	CExcelOper::SetMainTitle(excel_sheet,"D4","D4","�ƻ�����:",15,10,(short)2);
	//
	CExcelOper::MergeRowRange(excel_sheet,"A5","A6");
	CExcelOper::MergeRowRange(excel_sheet,"B5","B6");
	CExcelOper::MergeRowRange(excel_sheet,"C5","C6");
	CExcelOper::MergeRowRange(excel_sheet,"D5","D6");
	CStringArray str_arr;
	str_arr.SetSize(4);
	str_arr[0]="����";str_arr[1]="��˨�ȼ�";str_arr[2]="���";str_arr[3]="�޿۳���lg��";
	double col_arr[4]={20.0,10.0,7.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,5,str_arr,col_arr,TRUE);
	//������
	CExcelOper::SetFontBold(excel_sheet,0,5,6);
	sTempCell=CExcelOper::GetCellPos(nColCount-1,5);
	CExcelOper::MergeColRange(excel_sheet,"E5",sTempCell);
	CExcelOper::SetRangeValue(excel_sheet,"E5", "E5",COleVariant("��λ"));
	sTempCell.Printf("%s5",(char*)sMaxColCell);
	CExcelOper::SetRangeValue(excel_sheet,sTempCell, sTempCell,COleVariant("�ϼ�"));
	//������
	sTempCell.Printf("%s6",(char*)sMaxColCell);
	CExcelOper::SetRangeValue(excel_sheet,sTempCell, sTempCell,COleVariant("����"));
	//
	//��Ӷκ�
	int iCol=4;
	for (SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
	{
		CXhChar50 sCel=CExcelOper::GetCellPos(iCol,6);
		CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant(pSegI->ToString()));
		CExcelOper::SetFontBold(excel_sheet,iCol,6,1);
		iCol++;
	}
	CExcelOper::SetFontBold(excel_sheet,iCol,6,1);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(nBoltCount+3,nColCount+5);
	for(int i=0;i<nBoltCount;i++)
	{
		BOMBOLT bomBolt;
		CopyBomBoltProp(boltArr[i].bomBolt,bomBolt);
		bomBolt.AddPart(boltArr[i].bomBolt.GetPartNum());

		CXhChar16 sBoltType=GetBoltTypeStr(bomBolt);
		if(!bomBolt.IsDianQuan())
		{
			map.SetValueAt(index,0,COleVariant(sBoltType));
			//��˨�ȼ�
			CXhChar16 sGrade("%.1f��",bomBolt.grade);
			map.SetValueAt(index,1,COleVariant(sGrade));
			CXhChar200	sGuiGe("M%d*%d",bomBolt.d,bomBolt.L);
			map.SetValueAt(index,2,COleVariant(sGuiGe));
			map.SetValueAt(index,3,COleVariant((long)bomBolt.nNoThreadlen));//�޿۳���lg��
		}
		else	
		{
			CExcelOper::SetFontBold(excel_sheet,0,index+7,4);
			map.SetValueAt(index,0,COleVariant("��Ƭ"));
			map.SetValueAt(index,1,COleVariant("Q235"));
			CXhChar16 str("-%d��%.1f",bomBolt.L,bomBolt.d+1.5);
			map.SetValueAt(index,2,COleVariant(str));
		}
		iCol=4;
		for(SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			long *pCount=boltArr[i].m_hashNumBySegI.GetValue(pSegI->iSeg);
			map.SetValueAt(index,iCol,COleVariant(pCount?*pCount:0));
			iCol++;
		}
		CXhChar50 sPartNum("=SUM(E%d:%s)",index+7,(char*)CExcelOper::GetCellPos(iCol-1,index+7));
		map.SetValueAt(index,iCol,COleVariant(sPartNum));
		index++;
	}
	CString sNotes("��ע:\n");
	sNotes+="        1).�޿۳���������ƫ���˨���Ȳ�����ƫ�\n";
	sNotes+="        2).�Ŷ�����45���乳������ʽ��\n";
	sNotes+="        3).��˨������������3%���䡣\n";
	sNotes+="        4).�������ͨ����������˨���Ŷ����Ѽӳ���\n";
	sNotes+="        5).��Ȧ���(M16���Ϊ3mm��M20��M24���Ϊ4mm)��\n";
	sNotes+="        6).������ĸ���ù���ʽ��\n";
	map.SetValueAt(index,0,COleVariant(sNotes));
	CXhChar50 sStartCell=CExcelOper::GetCellPos(0,index+7);
	CXhChar50 sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+4,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	Range excel_range;
	excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
	excel_range.SetRowHeight(COleVariant(100.0));
	index++;
	//
	sNotes.Empty();
	sNotes+="���ã�\n";
	sNotes+="        ��ͨ��˨��һ��ĸһ��Ȧһ����ĸ����ͨ�Ŷ�������ĸһ��Ȧһ����ĸ��˫ñ��˨������ĸһ��Ȧ��\n";
	sNotes+="        ������˨��һ��ĸһ��Ȧһ������ĸ�������Ŷ�������ĸһ��Ȧһ������ĸ��\n";
	sNotes+="        U����˨����ñ˫�沢����ͼ���мӹ���\n";
	map.SetValueAt(index,0,COleVariant(sNotes));
	sStartCell=CExcelOper::GetCellPos(0,index+7);
	sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+4,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
	excel_range.SetRowHeight(COleVariant(60.0));
	//
	index++;
	sNotes.Empty();
	sNotes+="�Ʊ�:                           У�ˣ�                               ����: ";
	sStartCell=CExcelOper::GetCellPos(0,index+7);
	sEndCell=CExcelOper::GetCellPos(segIArr.GetSize()+5,index+7);
	CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
	CExcelOper::SetFontBold(excel_sheet,0,index+7,1);
	map.SetValueAt(index,0,COleVariant(sNotes));
	//
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+7);
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A1");
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A7",sTempCell,map.var);
	//���ö���
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+6);  //
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A5",sTempCell,COleVariant((long)3));
	CXhChar16 sCell("A%d",index+5);
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+7);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,sCell,sTempCell,COleVariant((long)2));
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A5",sTempCell,COleVariant(10.5));
	//������ɫ
	index=7;
	for (int i=0;i<nBoltCount;i++)
	{
	    CExcelOper::SetFontBold(excel_sheet,0,index,4);
		if(!boltArr[i].bomBolt.IsDianQuan())
		{
			if(16==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,10,index,0,3);
			else if(20==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,6,index,0,3);
		}
		index++;
	}
	return nCount;
}
