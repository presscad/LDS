// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
//����

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
	return 7;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

static int CompareSegiFunc(const SUPERLIST_NODE<BOMPART>& item1,const SUPERLIST_NODE<BOMPART>& item2)
{
	BOMPART* pBomPart1= (BOMPART*)item1.pDataObj;
	BOMPART* pBomPart2= (BOMPART*)item2.pDataObj;
	SEGI seg1(pBomPart1->iSeg),seg2(pBomPart2->iSeg);
	if(seg1.iSeg>seg2.iSeg)
		return 1;
	else if(seg1.iSeg<seg2.iSeg)
		return -1;
	else	//if(seg1.iSeg==seg2.iSeg)
		return ComparePartNoString(pBomPart1->sPartNo,pBomPart2->sPartNo);
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
	//���й��ˡ�����
	CHashList<SEGI> segTable;
	GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		if(segTable.GetNodeNum()==0||segTable.GetValue(pBomPart->iSeg)!=NULL)	//�κŹ���
			BomArr.append(*pNode);
	}
	CBubbleSort<SUPERLIST_NODE<BOMPART>>::BubSort(BomArr.m_pData,BomArr.GetSize(),CompareSegiFunc);
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
	//
	str_arr.SetSize(22);
	str_arr[0]="��λ";str_arr[1]="���";str_arr[2]="����";str_arr[3]="���";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="������";str_arr[7]="�н�";
	str_arr[8]="���";str_arr[9]="���";str_arr[10]="����";str_arr[11]="����";
	str_arr[12]="�Ͻ�";str_arr[13]="����";str_arr[14]="�纸";str_arr[15]="�¿�";
	str_arr[16]="���";str_arr[17]="����";str_arr[18]="����";str_arr[19]="�����";
	str_arr[20]="��ע";str_arr[21]="������";
	double col_arr[22]={5.0,5.0,5.0,7.5,5.0,
						5.5,5.0,2.5,2.5,2.5,
						2.5,2.5,2.5,2.5,2.5,
						2.5,2.5,2.5,5.0,5.0,
						5.0,7.5,};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(bom.m_nPartsN,22);
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		i++;
		SEGI segI(pPart->iSeg);
		map.SetValueAt(index,0,COleVariant(segI.ToString()));		//��λ
		map.SetValueAt(index,1,COleVariant(pPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//���
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pPart->cMaterial)));	//����
		CXhChar16 sGuiGe=GetPartSpec(pPart);
		sGuiGe.Replace('x','*');
		sGuiGe.Replace("��","��");
		sGuiGe.Replace("��","L");
		map.SetValueAt(index,3,COleVariant(sGuiGe));	//���
		map.SetValueAt(index,4,COleVariant(ftol(pPart->wide)));		//���
		map.SetValueAt(index,5,COleVariant(ftol(pPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(pPart->GetPartNum()));	//��������
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
			BOOL bCutAngle=pAngle->bCutAngle;
			if(bCutAngle)				//�н�
				map.SetValueAt(index,7,COleVariant("1"));
			if(pAngle->nPushFlat>0)			//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
				map.SetValueAt(index,8,COleVariant("1"));
			if(pAngle->bCutRoot)			//���
				map.SetValueAt(index,9,COleVariant("1"));
			if(pAngle->bCutBer)				//����
				map.SetValueAt(index,10,COleVariant("1"));
			if(pAngle->bKaiJiao)			//����
				map.SetValueAt(index,11,COleVariant("1"));
			if(pAngle->bHeJiao)				//�Ͻ�
				map.SetValueAt(index,12,COleVariant("1"));
		}
		long nHuoquLineCount=pPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//����
			map.SetValueAt(index,13,COleVariant("1"));
		if(pPart->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,14,COleVariant("1"));
	    if(pPart->cPartType==BOMPART::PLATE)	
		{	
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			//�¿�
			if(pPlate->bNeedFillet||pPlate->thick>=8)	
				map.SetValueAt(index,15,COleVariant("1"));
			/*{
				if(pPlate->thick>=8&&pPlate->thick<=12)
					map.SetValueAt(index,15,COleVariant("1"));
				else if(pPlate->thick>=14)
					map.SetValueAt(index,15,COleVariant("2"));
			}*/
		}
		if((pPart->cMaterial=='S'&&pPart->thick>16)||			//���
			(pPart->cMaterial=='H'&&pPart->thick>14)||
			(pPart->cMaterial=='P'&&pPart->thick>12)||
			pPart->cMaterial=='T'||pPart->IsDrillHole())
				map.SetValueAt(index,16,COleVariant("1"));
		if(pPart->cPartType==BOMPART::TUBE)	
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pPart;
			//��������
			int nType=0;
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//֧�� ��ͷC�Ͳ��
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//֧�� ��ͷU�Ͳ��
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//֧�� ��ͷT�Ͳ��
			else if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//֧�� ��ͷX�Ͳ��
			if(pBomTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				nType=11;	//֧�� ��ͷC�Ͳ��
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				nType=22;	//֧�� ��ͷU�Ͳ��
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				nType=33;	//֧�� ��ͷT�Ͳ��
			else if(pBomTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				nType=44;	//֧�� ��ͷX�Ͳ��
			if(nType>0)
				map.SetValueAt(index,18,COleVariant("1"));
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,19,COleVariant("1"));	//�������
		}
		if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="���";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="��M16*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="��M20*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="��M24*"+str;
			}
			map.SetValueAt(index,20,COleVariant(sNote));
		}
		else
			map.SetValueAt(index,20,COleVariant(pPart->sNotes));
		index++;
	}
	char cell_end[15]=" ";
	//�����Ҷ���
	sprintf(cell_end,"V%d",i+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)3));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return nCount;
}
