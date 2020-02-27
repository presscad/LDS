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
		sprintf(sSpec,"%sx%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sX%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart)
{
	CXhChar50 str;
	if(pBomPart->cPartType==BOMPART::ANGLE)
		GetAngleSimpleSpec(str,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("%.fX%.f",pBomPart->thick,pBomPart->wide);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
static CXhChar16 GetMatName(BOMPART* pBomPart)
{
	CXhChar16 sMatType;
	if(pBomPart->cMaterial!='S')
		sMatType.Printf("%c",pBomPart->cMaterial);
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sMatType.Append("�N");
	else if(pBomPart->cPartType==BOMPART::PLATE)
		sMatType.Append("��");
	return sMatType;
}
static BOOL IsHasHuoqu(BOMPART* pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->m_cFaceN>1)
			return TRUE;
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)
			return TRUE;
	}
	return FALSE;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessNotes(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam="";
	if(pBomPart->bWeldPart)
		sProcessParam.Append("����",' ');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
			sProcessParam.Append("����",' ');
		if(pPlate->bNeedFillet)		//��Ҫ�¿�
			sProcessParam.Append("�¿�",' ');
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sProcessParam.Append("���",' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("���Ŷ�",' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("�н�",' ');
		//����
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)
			sProcessParam.Append("����",' ');
		//ѹ��
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
			sProcessParam.Append("ѹ��",' ');
		//���Ͻ�
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append("����",' ');
		if(pBomAngle->bHeJiao)
			sProcessParam.Append("�Ͻ�",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("����",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("���",' ');
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("�����¿�",' ');
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("һ���¿�",' ');
	}
	return sProcessParam;
}
TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{
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
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("����ԭʼ���ϱ�");
	CStringArray str_arr;
	str_arr.SetSize(14);
	str_arr[0]="����";str_arr[1]="������";str_arr[2]="��������";str_arr[3]="��ƹ��";
	str_arr[4]="���ù��";str_arr[5]="����";str_arr[6]="����mm";str_arr[7]="ÿ������";										
	str_arr[8]="����";str_arr[9]="�纸";str_arr[10]="����";str_arr[11]="�н�";str_arr[12]="����";str_arr[13]="��ע";
	double col_arr[14]={8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5,8.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeBorders(excel_sheet,"A1","N1",COleVariant(10.5));
	//�����������
	CVariant2dArray map(bom.m_nPartsN,14);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		//����
		map.SetValueAt(index,0,COleVariant(bom.m_sTowerTypeName));
		//������
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));
		//��������
		map.SetValueAt(index,2,COleVariant(GetMatName(pBomPart)));
		//��ƹ��
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));
		//���ù��
		map.SetValueAt(index,4,COleVariant(""));
		//����
		map.SetValueAt(index,5,COleVariant(""));
		//����
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.1f",pBomPart->length)));
		//ÿ������
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));
		//����
		map.SetValueAt(index,8,COleVariant(pBomPart->m_arrBoltRecs.GetSize()));
		//�纸
		if(pBomPart->bWeldPart)
			map.SetValueAt(index,9,COleVariant("��"));
		//����
		if(IsHasHuoqu(pBomPart))
			map.SetValueAt(index,10,COleVariant("��"));
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
			//�н�
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,11,COleVariant("��"));
			//����
			if(pBomAngle->bCutBer)
				map.SetValueAt(index,12,COleVariant("��"));
		}
		//��ע
		map.SetValueAt(index,13,COleVariant(GetProcessNotes(pBomPart)));
		index++;
	}
	//�����Ҷ���
	char cell_end[30]={};
	sprintf(cell_end,"N%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	return 1;
}