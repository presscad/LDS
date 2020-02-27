// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
//��˨��ϸ��(�ൺ������ʽ�����ϸĽ�)

#include "stdafx.h"
#include "BomExport.h"
#include "ITowerBom.h"
#include "Buffer.h"
#include "SortFunc.h"
#include "ArrayList.h"
#include "HashTable.h"
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
//version��1.��һ���汾
//		   2.���ݷɽ��ֹܲ��֣��汾����Ϊ2
//		   3.�޸���˨��Ϣ���汾����Ϊ3
TOWER_BOM_EXPORTS_API DWORD GetSupportDataBufferVersion()
{
	return 11;
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

static CXhChar50 GetBoltConfig(BOMBOLT& bomBolt)
{
	CXhChar50 sCfg;
	BYTE nCount=bomBolt.nAntiTheftNut+bomBolt.nCommonGasket+bomBolt.nCommonNut+bomBolt.nFastenNut+
				bomBolt.nLockClasp + bomBolt.nThinNut + bomBolt.nSpringGasket;
	if(bomBolt.IsDianQuan())
		sCfg.Copy("ƽ��Ȧ");
	else if(nCount>0)
	{
		if (bomBolt.bFootNail)
			sCfg.Copy("�Ŷ���");
		if (bomBolt.nCommonNut > 0)
			sCfg.Append(CXhChar16("%d��ĸ", bomBolt.nCommonNut));
		if (bomBolt.nThinNut > 0)
			sCfg.Append(CXhChar16("%d����ĸ", bomBolt.nThinNut));
		if (bomBolt.nAntiTheftNut > 0)
			sCfg.Append(CXhChar16("%d��ж��ĸ", bomBolt.nAntiTheftNut));
		if (bomBolt.nLockClasp > 0)
			sCfg.Append(CXhChar16("%d���ɿ�", bomBolt.nLockClasp));
		if (bomBolt.nSpringGasket > 0)
			sCfg.Append(CXhChar16("%d���ɵ�Ȧ", bomBolt.nSpringGasket));
		if (bomBolt.nCommonGasket > 0)
			sCfg.Append(CXhChar16("%dƽ��Ȧ", bomBolt.nCommonGasket));
		if (bomBolt.nFastenNut > 0)
			sCfg.Append(CXhChar16("%d���ɿ۽���ĸ", bomBolt.nFastenNut));
	}
	else
	{
		if (bomBolt.bFootNail)
		{
			if (bomBolt.bAntiTheft)
				sCfg.Copy("�Ŷ���2��ĸ1��ж��ĸ1ƽ��Ȧ");
			else if (bomBolt.bAnitLoose)
				sCfg.Copy("�Ŷ���2��ĸ1���ɿ۽���ĸ1ƽ��Ȧ");
			else
				sCfg.Copy("�Ŷ���3��ĸ1ƽ��Ȧ");
		}
		else
		{
			if (bomBolt.bAntiTheft)
				sCfg.Copy("1��ĸ1��ж��ĸ1ƽ��Ȧ");
			else if (bomBolt.bAnitLoose)
				sCfg.Copy("1��ĸ1ƽ��Ȧ1���ɿ۽���ĸ");
			else if (bomBolt.bTwoCap)
				sCfg.Copy("2��ĸ1ƽ��Ȧ");
			else
				sCfg.Copy("1��ĸ1ƽ��Ȧ");
		}
	}
	return sCfg;
}
static void InitBomBoltByBoltConfig(BOMBOLT &bomBolt,const char* bolt_cfg)
{

}

CModelBOM g_xBom;
const int TOWER_BOM_EXCEL_COL_COUNT			= 10;

const char* TITLE_BOLT_SEGI				    = "�α�";
const char* TITLE_BOLT_GRADE				= "����";
const char* TITLE_BOLT_GUIGE				= "���";
const char* TITLE_BOLT_NO_THREAD_LEN 		= "�޿۳�";	
const char* TITLE_BOLT_NUM				    = "����";
const char* TITLE_BOLT_CFG				    = "��˨����";
const char* TITLE_BOLT_NOTES				= "��ע";
//
const char* TYPE_BOLT				        = "��˨";
const char* TYPE_TOW_CAP_BOLT				= "˫ñ��˨";
const char* TYPE_DIAN_QUAN				    = "��Ȧ";
const char* TYPE_DIAN_PIAN				    = "��Ƭ";
const char* TYPE_U_BOLT				        = "U�ͻ�";
const char* TYPE_FOOT_NAIL				    = "�Ŷ�";

static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}

TOWER_BOM_EXPORTS_API void ImportExcelBomSegFile(const char* file_path)
{
	CVariant2dArray sheetContentMap(1,1);
	if(!CExcelOper::GetExcelContentOfSpecifySheet(file_path,sheetContentMap,1))
		return;
	CXhSimpleList<BOMBOLT> boltList;
	//��ȡ����
	VARIANT value;
	value.intVal=0;
	CHashStrList<DWORD> hashColIndexByColTitle;
	for(int i=1;i<=TOWER_BOM_EXCEL_COL_COUNT;i++)
	{
		sheetContentMap.GetValueAt(0,i,value);
		if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_SEGI)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_SEGI,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_GUIGE)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_GUIGE,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NO_THREAD_LEN)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NO_THREAD_LEN,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_GRADE)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_GRADE,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NUM)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NUM,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_CFG)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_CFG,i);
		else if( CString(value.bstrVal).CompareNoCase(TITLE_BOLT_NOTES)==0)
			hashColIndexByColTitle.SetValue(TITLE_BOLT_NOTES,i);
	}
	// ��ȡexcel ֵ
	int nCount=0;
	VARIANT valueBoltGuiGe,valueBoltNoThreadLen,valueGrade;
	for(int i = 1; i < sheetContentMap.RowsCount(); i++)
	{
		BOMBOLT bomBolt;
		DWORD *pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_GUIGE);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueBoltGuiGe);
			CXhChar100 sGuiGe=VariantToString(valueBoltGuiGe);
			sGuiGe.Replace("*"," ");
			sGuiGe.Replace("x"," ");
			sGuiGe.Replace("X"," ");
			sGuiGe.Replace("��"," ");
			sGuiGe.Replace("M"," ");
			sGuiGe.Replace("��"," ");
			sGuiGe.Replace("-"," ");
			int d=0,l=0;
			sscanf((char*)sGuiGe,"%d %d",&d,&l);
			bomBolt.d=d;
			bomBolt.L=l;
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NO_THREAD_LEN);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueBoltNoThreadLen);
			bomBolt.nNoThreadlen=atoi(VariantToString(valueBoltNoThreadLen));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_GRADE);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.grade=(float)atof(VariantToString(valueGrade));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_SEGI);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.iSeg=SEGI(VariantToString(valueGrade)).Digital();
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NUM);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_CFG);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			//bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_NOTES);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			//bomBolt.AddPart(atoi(VariantToString(valueGrade)));
		}
		boltList.AttachObject(bomBolt);
		nCount++;
	}
	g_xBom.m_nBoltsN=nCount;
	if(g_xBom.m_nBoltsN>0)
	{
		int i=0;
		g_xBom.m_pBoltRecsArr = new BOMBOLT[g_xBom.m_nBoltsN];
		for(BOMBOLT* pBomBolt=boltList.EnumObjectFirst();pBomBolt;pBomBolt=boltList.EnumObjectNext(),i++)
			g_xBom.m_pBoltRecsArr[i]=*pBomBolt;
	}
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	//1.��ȡBOM
	CModelBOM bom, *pBOM = &bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	else
		pBOM = &g_xBom;
	CBubbleSort<BOMBOLT>::BubSort(pBOM->m_pBoltRecsArr, pBOM->m_nBoltsN, CompareBomBoltIncSegI);
	//2.���ɣţأãţ̲��ϱ�
	//2.1 ����Excel������
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	if (pWorksheet)
	{
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet, FALSE);
		excel_sheet.Select();
		excel_sheet.SetName("�����");
	}
	pWorksheet = excel_sheets.GetItem(COleVariant((short)2));
	if(pWorksheet)
	{
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet, FALSE);
		excel_sheet.Select();
		excel_sheet.SetName("��˨��ϸ��");
		/*bom.m_sTaAlias���������ַ�(�ո�,-,\)������excel_sheet.SetName����
		if (bom.m_sTaAlias.GetLength() > 0)
			excel_sheet.SetName(CXhChar200("%s��˨��ϸ",(char*)bom.m_sTaAlias));
		else
			excel_sheet.SetName("��˨�ֶ���ϸ��");
		*/
		//2.2 ������ϱ�
		CStringArray str_arr;
		str_arr.SetSize(9);
		str_arr[0] = "���"; str_arr[1] = "�κ�"; str_arr[2] = "����"; str_arr[3] = "����";
		str_arr[4] = "���"; str_arr[5] = "�޿۳�"; str_arr[6] = "����"; str_arr[7] = "��˨����";
		str_arr[8] = "��ע";
		double col_arr[9] = { 5.0,5.0,5.0,5.0,8.0,7.0,5.0,28.0,10.0 };
		CExcelOper::AddRowToExcelSheet(excel_sheet, 1, str_arr, col_arr, TRUE);
		//2.2.2 ������ϱ�
		int i = 0, nCount = 1;
		int index = 0;
		CString str;
		CVariant2dArray map(pBOM->m_nBoltsN, 9);
		for (i = 0; i < pBOM->m_nBoltsN; i++)
		{
			BOMBOLT bolt = pBOM->m_pBoltRecsArr[i];
			map.SetValueAt(index, 0, COleVariant((long)(i + 1)));
			//�κ�
			map.SetValueAt(index, 1, COleVariant((long)bolt.iSeg));
			//��˨�ȼ�
			CXhChar16 sGrade(bolt.grade);
			map.SetValueAt(index, 2, COleVariant(sGrade));
			//����
			map.SetValueAt(index, 3, COleVariant(""));
			// ���
			CXhChar200	sGuiGe("M");
			sGuiGe.Append(CXhChar16(bolt.d));
			if (bolt.IsDianQuan())
				sGuiGe.Append("��Ȧ");
			else
			{
				sGuiGe.Append("*");
				sGuiGe.Append(CXhChar16(bolt.L));
			}
			map.SetValueAt(index, 4, COleVariant(sGuiGe));
			//�޿۳���lg��
			if (!bolt.IsDianQuan())
				map.SetValueAt(index, 5, COleVariant((long)bolt.nNoThreadlen));
			//����
			map.SetValueAt(index, 6, COleVariant((long)bolt.GetPartNum()));
			//��˨����
			map.SetValueAt(index, 7, COleVariant(GetBoltConfig(bolt)));
			index++;
		}
		CXhChar16 sCell = "I";
		sCell.Append(CXhChar16(index + 1));
		//�����Ҷ���
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet, "A1", sCell, COleVariant((long)3));
		//���ñ������
		CExcelOper::SetRangeValue(excel_sheet, "A2", sCell, map.var);
		//���ñ߿�
		CExcelOper::SetRangeBorders(excel_sheet, "A1", sCell, COleVariant(10.5));
		excel_sheet.ReleaseDispatch();
	}
	return 0;
}

int CreateExcelBoltSummaryBomFile(char* data_buf, int buf_len, const char* file_name, DWORD dwFlag)
{
	/*CXhChar500 sTempPath;
	::GetModuleFileName(theApp.m_hInstance, sTempPath, 500);
	sTempPath.Replace("BomExport.dll", "�ൺ������˨���������׼.xlsx");
	BOOL bBoltLib = TRUE;
	CBoltLibSet boltLibSet;
	if (!PathFileExists(sTempPath))
	{
		MessageBox(NULL, CXhChar500("δ�ҵ�ģ���ļ�,����Ĭ���������ɱ��ģ��·����%s", (char*)sTempPath), "������ʾ", MB_OK);
		bBoltLib = FALSE;
	}
	else
		boltLibSet.ImportBoltLibByExcelFile(sTempPath);*/
	//1.��ȡBOM
	CModelBOM bom, *pBOM = &bom;
	CBuffer buffer(buf_len);
	if (buf_len > 0)
	{
		buffer.AttachMemory(data_buf, buf_len);
		bom.FromBuffer(buffer, GetSupportDataBufferVersion());
	}
	else
		pBOM = &g_xBom;
	CBubbleSort<BOMBOLT>::BubSort(pBOM->m_pBoltRecsArr, pBOM->m_nBoltsN, CompareBomBoltIncSegI);
	//2.���ɣţأãţ̲��ϱ�
	//2.1 ����Excel������
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short) 2));
	if(pWorksheet2)
	{
		//���»�����˨�����Ƿֶ�
		CHashStrList<BOMBOLT> hashBoltByCombinedKey; //ͨ������
		ARRAY_LIST<BOMBOLT> boltArr;
		for (int i=0;i<pBOM->m_nBoltsN;i++)
		{
			BOMBOLT bolt=pBOM->m_pBoltRecsArr[i];
			CXhChar50 sKey=bolt.CombinedKey();
			BOMBOLT* pSumBolt=hashBoltByCombinedKey.GetValue(sKey);
			if(pSumBolt==NULL)
			{
				pSumBolt=hashBoltByCombinedKey.Add(sKey);
				CopyBomBoltProp(bolt,*pSumBolt);
			}
			pSumBolt->AddPart(bolt.GetPartNum());
		}
		for(BOMBOLT *pBolt=hashBoltByCombinedKey.GetFirst();pBolt;pBolt=hashBoltByCombinedKey.GetNext())
		{
			BOMBOLT *pNewBolt=boltArr.append();
			CopyBomBoltProp(*pBolt,*pNewBolt);
			pNewBolt->AddPart(pBolt->GetPartNum());
		}
		CBubbleSort<BOMBOLT>::BubSort(boltArr.m_pData,boltArr.GetSize(),CompareBomBolt);

		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet2,FALSE);
		excel_sheet.Select();
		if(bom.m_sTaAlias.GetLength()>0)
			excel_sheet.SetName(CXhChar200("%s����",bom.m_sTaAlias));
		else
			excel_sheet.SetName("���ܱ�");
		//2.2 ������ϱ�
		CExcelOper::MergeColRange(excel_sheet,"A1","N1");
		CExcelOper::SetMainTitle(excel_sheet,"A1", "N1","�ൺ���������ɷ����޹�˾",14.5,14);
		//
		CExcelOper::MergeColRange(excel_sheet,"A2","N2");
		CExcelOper::SetMainTitle(excel_sheet,"A2", "N2",CXhChar200("%s���� ��˨���ܱ�",(char*)bom.m_sPrjName),14.5,14);
		
		CStringArray str_arr;
		str_arr.SetSize(16);
		str_arr[0]="���";str_arr[1]="��������";str_arr[2]="��������";str_arr[3]="����";str_arr[4]="����";
		str_arr[5]="����ͺ�";str_arr[6]="�޿۳�";str_arr[7]="����";str_arr[8]="����(kg)";str_arr[9]="��˨����";
		str_arr[10]="ִ�б�׼";str_arr[11]="������ʽ";str_arr[12]="����";str_arr[13]="��ע";str_arr[14]="";str_arr[15]="����";
		double col_arr[16]={4.5,18.5,8.5,4.5,4.5,7.5,6.5,5.0,7.5,24.5,13.0,7.5,12.5,6.5,8.5,8.5};
		CExcelOper::AddRowToExcelSheet(excel_sheet,3,str_arr,col_arr,TRUE);
		CExcelOper::SetFontSize(excel_sheet,"A3","N3",10.5,TRUE);
		//2.2.2 ������ϱ�
		int i=0,nCount=0;
		int index=0;
		CVariant2dArray map(boltArr.GetSize()+20,16);
		//
		CHashStrList<BOMBOLT> hashDianQuanByCombinedKey;
		CHashStrList<BOMBOLT> hashLuoMuByCombinedKey;
		for (int i=0;i<boltArr.GetSize();i++)
		{
			BOMBOLT bolt=boltArr[i];
			if(!bolt.IsDianQuan()&&(bolt.bAnitLoose||bolt.bAntiTheft))
			{
				CXhChar50 sKey("%d*%.1f",bolt.d,bolt.grade);
				if(bolt.bAntiTheft)
					sKey.Append("T");
				else
					sKey.Append("L");
				BOMBOLT* pSumBolt=hashLuoMuByCombinedKey.GetValue(sKey);
				if(pSumBolt==NULL)
				{
					pSumBolt=hashLuoMuByCombinedKey.Add(sKey);
					CopyBomBoltProp(bolt,*pSumBolt);
				}
				pSumBolt->AddPart(bolt.GetPartNum());
			}
			if(!bolt.IsDianQuan())
			{
				CXhChar50 sKey("%d*%.1f",bolt.d,bolt.grade);
				BOMBOLT* pSumBolt=hashDianQuanByCombinedKey.GetValue(sKey);
				if(pSumBolt==NULL)
				{
					pSumBolt=hashDianQuanByCombinedKey.Add(sKey);
					CopyBomBoltProp(bolt,*pSumBolt);
				}
				pSumBolt->AddPart(bolt.GetPartNum());
			}
			//���
			map.SetValueAt(index,0,COleVariant((long)i+1));
			//��������
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			//��ȴ����
			map.SetValueAt(index,2,COleVariant("A1110"));
			//��˨�ȼ�
			CXhChar16 sGrade(bolt.grade);
			map.SetValueAt(index,3,COleVariant(sGrade));
			//����
			map.SetValueAt(index,4,COleVariant(""));
			// ���
			CXhChar200 sGuiGe("M");
			sGuiGe.Append(CXhChar16(bolt.d));
			if(bolt.IsDianQuan())
				sGuiGe.Append("��Ȧ");
			else
			{
				sGuiGe.Append("*");
				sGuiGe.Append(CXhChar16(bolt.L));
			}
			map.SetValueAt(index,5,COleVariant(sGuiGe));
			//
			map.SetValueAt(index,6,COleVariant((long)bolt.nNoThreadlen));//�޿۳���lg��
			//����
			map.SetValueAt(index,7,COleVariant((long)bolt.GetPartNum()));
			// ���أ����
			double fUnitWeight = bolt.fUnitWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.4f",fUnitWeight*bolt.GetPartNum())));
			//
			map.SetValueAt(index,9,COleVariant(GetBoltConfig(bolt)));
			//ִ�б�׼
			if(bolt.IsDianQuan())
				map.SetValueAt(index,10,COleVariant("GB/T 95-2000"));
			else
				map.SetValueAt(index,10,COleVariant("GB/T 5780-2000"));
			//������ʽ
			map.SetValueAt(index,11,COleVariant("�ȶ�п"));
			//����
			map.SetValueAt(index,12,COleVariant("��Ϊ1������1��"));
			//����
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		//�����˨
		for (BOMBOLT* pSumBolt=hashDianQuanByCombinedKey.GetFirst();pSumBolt;pSumBolt=hashDianQuanByCombinedKey.GetNext())
		{
			map.SetValueAt(index,0,COleVariant(CXhChar16(index+1)));
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			map.SetValueAt(index,2,COleVariant("A1110"));
			//����
			map.SetValueAt(index,3,COleVariant(CXhChar16(pSumBolt->grade)));
			//���
			map.SetValueAt(index,5,COleVariant(CXhChar16("M%d",pSumBolt->d)));
			//
			map.SetValueAt(index,7,COleVariant(long(pSumBolt->GetPartNum())));
			//����
			double fUnitWeight = pSumBolt->fUnitWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.4f",fUnitWeight*pSumBolt->GetPartNum())));
			map.SetValueAt(index,9,COleVariant("ƽ��Ȧ"));
			map.SetValueAt(index,10,COleVariant("GB/T 95-2000"));
			map.SetValueAt(index,11,COleVariant("�ȶ�п"));
			//����
			map.SetValueAt(index,12,COleVariant("��Ϊ1������1��"));
			map.SetValueAt(index,13,COleVariant("���"));
			//����
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		for (BOMBOLT* pSumBolt=hashLuoMuByCombinedKey.GetFirst();pSumBolt;pSumBolt=hashLuoMuByCombinedKey.GetNext())
		{
			map.SetValueAt(index,0,COleVariant(CXhChar16(index+1)));
			map.SetValueAt(index,1,COleVariant(bom.m_sPrjCode));
			map.SetValueAt(index,2,COleVariant("A1110"));
			//����
			map.SetValueAt(index,3,COleVariant(CXhChar16(pSumBolt->grade)));
			//���
			map.SetValueAt(index,5,COleVariant(CXhChar16("M%d",pSumBolt->d)));
			//
			map.SetValueAt(index,7,COleVariant(long(pSumBolt->GetPartNum())));
			if(pSumBolt->bAntiTheft)
				map.SetValueAt(index,9,COleVariant("��ж��ĸ"));
			else if(pSumBolt->bAnitLoose)
				map.SetValueAt(index,9,COleVariant("���ɿ۽���ĸ"));
			if(pSumBolt->bAnitLoose)
				map.SetValueAt(index,10,COleVariant("GB\\T805-88"));
			else
				map.SetValueAt(index,10,COleVariant(""));
			map.SetValueAt(index,11,COleVariant("�ȶ�п"));
			map.SetValueAt(index,12,COleVariant("��Ϊ1������1��"));
			map.SetValueAt(index,13,COleVariant("���"));
			//����
			double fUnitWeight=pSumBolt->fUnitWeight;
			map.SetValueAt(index,15,COleVariant(fUnitWeight));
			index++;
		}
		if(index<=0)
			return 0;
		
		//������һ��
		CXhChar50 sStartCell("A%d",index+4),sEndCell("B%d",index+4);
		CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
		map.SetValueAt(index,0,COleVariant("�ϼƣ�"));
		map.SetValueAt(index,5,COleVariant("�ܼ�"));
		//����
		map.SetValueAt(index,7,COleVariant(CXhChar16("=SUM(H4:H%d)",index+4-1)));
		//����
		map.SetValueAt(index,8,COleVariant(CXhChar16("=SUM(I4:I%d)",index+4-1)));
		//
		index++;
		CString sNotes("��ע:\n");
		sNotes+="1��������˨�������Ŷ�����˨�������мӳ������мӳ��ĳ���Ӧͬ������ĸ��ȡ�\n";
		sNotes+="2��������ĸ��ȵ�ͬ��ͨ��ĸ�ĺ�ȡ�\n";
		sNotes+="3��������˨�������ǹ���ʽ������˨��\n";
		sNotes+="4���Ŷ���ʽ�������Ƕ�ͷֱ��ȫ����ʽ��\n";
		sNotes+="5��������˨������������˨����ǿ�ȵȼ�Ϊ�ȶ�п���ǿ��ֵ��\n";
		sNotes+="6��������˨�޿۳�Ӧ����ͨ��˨һ�£��ܸ�����ͬʱ���з������ܡ�\n";
		sNotes+="7����˨���Ŷ���������ͷ�ϴ�ӡ����������6.8���Ĵ�ӡ��6.8����8.8���Ĵ�ӡ��8.8����\n";
		sNotes+="8����˨����ĸ���۽���ĸӦ���ϵı�׼Ϊ������ͷ��˨C������GB/T 5780-2000������I��������ĸ����GB/T 6170-2000�������۽���ĸ����GB 805-88������Ȧ��׼Ӧ���ϡ�ƽ��ȦC������GB/T 95-2002����\n";
		sNotes+="9����˨����    ��  ��  ��ǰ����\n";
		map.SetValueAt(index,0,COleVariant(sNotes));
		sStartCell.Printf("A%d",index+4),sEndCell.Printf("N%d",index+4);
		CExcelOper::MergeColRange(excel_sheet,sStartCell,sEndCell);
		Range excel_range;
		excel_range.AttachDispatch(excel_sheet.GetRange(COleVariant(sStartCell),COleVariant(sEndCell)));
		excel_range.SetRowHeight(COleVariant(175.0));
		index++;
		map.SetValueAt(index,1,COleVariant("�� �ƣ�"));
		map.SetValueAt(index,4,COleVariant("У �ԣ�"));
		map.SetValueAt(index,7,COleVariant("�� �ˣ�"));
		//
		sEndCell.Printf("N%d",index+4-2);
		//���þ��ж���
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",sEndCell,COleVariant((long)3));
		CXhChar16 sCell1("A%d",index+4-1),sCell2("N%d",index+4);
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,sCell1,sCell2,COleVariant((long)2));
		//���ñ������
		CXhChar16 sContentEndCell("P%d",index+4);
		CExcelOper::SetRangeValue(excel_sheet,"A4",sContentEndCell,map.var);
		//���ñ߿�
		sEndCell.Printf("N%d",index+4-1);
		CExcelOper::SetRangeBorders(excel_sheet,"A1",sEndCell,COleVariant(10.5));
		excel_sheet.ReleaseDispatch();
	}
	return 0;
}
