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

static int GetProcessNotes(BOMPART* pBomPart,char *notes,bool bIncKaiHeAngle=FALSE)
{
	char ss[300]="";
	long nHuoquLineCount=pBomPart->GetHuoquLineCount();
	if(pBomPart->cPartType==BOMPART::PLATE)
		nHuoquLineCount=((PART_PLATE*)pBomPart)->m_cFaceN-1;
	if(nHuoquLineCount>0)	//�Ƿ���Ҫ����
		sprintf(ss,"%s ����",ss);
	if(pBomPart->bWeldPart&&pBomPart->cPartType!=BOMPART::PLATE)
		sprintf(ss,"%s ����",ss);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->bWeldPart)		//�Ƿ���Ҫ����
			sprintf(ss,"%s ����",ss);
		if(pPlate->bNeedFillet)		//��Ҫ�¿�
			sprintf(ss,"%s �¿�",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s ���%dmm",ss,abs(pNode->data.manu_space));
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s �����¿�",ss);
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sprintf(ss,"%s һ���¿�",ss);
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		//if(pBomAngle->bCutAngle)		//�Ƿ��н�
		//	sprintf(ss,"%s �н�",ss);
		//if(pBomAngle->bCutBer)			//�Ƿ���Ҫ����
		//	sprintf(ss,"%s ����",ss);
		//if(pBomAngle->bCutRoot)		//�Ƿ���Ҫ���
		//	sprintf(ss,"%s ���",ss);
		if(pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle)>0)		//�Ƿ���Ҫ����
		{
			if(bIncKaiHeAngle)	//�������Ƕ��� wht 11-05-24
				sprintf(ss,"%s ����(%.1f��)",ss,pBomAngle->wing_angle-90);
			else 
				sprintf(ss,"%s ����",ss);
		}
		if(pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle)>0)			//�Ƿ���Ҫ�Ͻ�
		{
			if(bIncKaiHeAngle)	//�����ϽǶ��� wht 11-05-24
				sprintf(ss,"%s �Ͻ�(%.1f��)",ss,90-pBomAngle->wing_angle);
			else 
				sprintf(ss,"%s �Ͻ�",ss);
		}
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
		{
			if(pBomAngle->nPushFlat==0x02)
				sprintf(ss,"%s �м�ѹ��",ss);
			else
				sprintf(ss,"%s ѹ��",ss);
		}
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			sprintf(ss,"%s ���Ŷ�",ss);
	}
	if(notes)
		strcpy(notes,ss);
	return strlen(ss);
}
static BOOL IsZhiWan(BOMPART* pBomPart)
{
	if(pBomPart==NULL)
		return FALSE;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
				return TRUE;
		}
		int nHuoquLineCount=pPlate->m_cFaceN-1;
		if(nHuoquLineCount>0)
			return TRUE;
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if((pBomAngle->bKaiJiao||pBomAngle->bHeJiao)&&GetKaiHeJiaoN(pBomAngle)>0)
			return TRUE;
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
			return TRUE;
		if(pBomAngle->GetHuoquType()>0)	//����
			return TRUE;
	}
	return FALSE;
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
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
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
	//��buffer�л�ȡ������ϸ
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
	//�����ض�����
	CXhChar50 sTitle("��ɽ���������ܳ�����ԭʼ���ϱ�");
	CExcelOper::SetMainTitle(excel_sheet,"A1","N1",sTitle,20,16,(short)3,(short)2,false);
	CExcelOper::SetRangeBorders(excel_sheet,"A1","N1",COleVariant(16.0));
	sTitle.Copy("�������񵥺���:0000");
	CExcelOper::SetMainTitle(excel_sheet,"A2","N2",sTitle,15,10,(short)4,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","E3","��������:",15,10,(short)2,(short)2,false);
	CExcelOper::MergeColRange(excel_sheet,"F3","M3");
	CExcelOper::SetMainTitle(excel_sheet,"N3","N3","��ע��:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A4","E4","����:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"F4","F4","����m:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"M4","M4","�ϼ�:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A5","E5","ʩ��ͼ��:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"F5","F5","����G:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N5","N5","�����:",15,10,(short)2,(short)2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A6","E6","��������       ��п:",15,10,(short)2,(short)2,false);
	CExcelOper::SetRangeBorders(excel_sheet,"A2","N6",COleVariant(11.0));
	//���ʵʱ���ݵ��б����ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+2,15);
	int index=0;
	//���
	map.SetValueAt(index,0,COleVariant("���"));
	CExcelOper::MergeRowRange(excel_sheet,"A7","A8");
	CExcelOper::SetRangeColWidth(excel_sheet,4,"A7",NULL);
	//�������
	map.SetValueAt(index,1,COleVariant("�������"));
	CExcelOper::MergeRowRange(excel_sheet,"B7","B8");
	//����
	map.SetValueAt(index,2,COleVariant("����"));
	CExcelOper::MergeRowRange(excel_sheet,"C7","C8");
	//���Ϲ��
	map.SetValueAt(index,3,COleVariant("���ʹ��"));
	CExcelOper::MergeColRange(excel_sheet,"D7","E7");
	map.SetValueAt(index+1,3,COleVariant("��ƹ��"));
	map.SetValueAt(index+1,4,COleVariant("���ù��"));
	//���ȣ������
	map.SetValueAt(index,5,COleVariant("����mm\n���mm^2"));
	CExcelOper::MergeRowRange(excel_sheet,"F7","F8");
	//��������
	map.SetValueAt(index,6,COleVariant("��������"));
	CExcelOper::MergeRowRange(excel_sheet,"G7","G8");
	//�纸
	map.SetValueAt(index,7,COleVariant("�纸"));
	CExcelOper::MergeRowRange(excel_sheet,"H7","H8"); 
	//������
	map.SetValueAt(index,8,COleVariant("������(����)"));
	CExcelOper::MergeColRange(excel_sheet,"I7","M7");
	map.SetValueAt(index+1,8,COleVariant("����"));
	map.SetValueAt(index+1,9,COleVariant("����"));
	map.SetValueAt(index+1,10,COleVariant("�н�"));
	map.SetValueAt(index+1,11,COleVariant("����"));
	map.SetValueAt(index+1,12,COleVariant("�ٸ�"));
	//��ע
	map.SetValueAt(index,13,COleVariant("��ע"));
	CExcelOper::MergeRowRange(excel_sheet,"N7","N8");
	CExcelOper::SetRangeColWidth(excel_sheet,15,"N7",NULL);
	//��ӹ�����ϸ����
	index+=2;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(CXhChar16("%d",index-1)));	//���
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));			//���
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//����
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//����ְ���Ϊ�����X����
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			double length=pBomPlate->length;
			if(bom.m_iPlankWeightStatType==2)
				length=pBomPlate->real_area/pBomPlate->wide;	//��������ʱӦ���ݸְ徻�������Ƹְ峤��
			map.SetValueAt(index,3,COleVariant(CXhChar50("-%.0fX%.0f",pBomPlate->thick,length)));			//��ƹ��
		}
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->sSpec));			//��ƹ��
		//map.SetValueAt(index,4,COleVariant(pBomPart->sSpec));			//���ù��
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,5,COleVariant(CXhChar50("%.1f",pBomPart->wide)));
		else
			map.SetValueAt(index,5,COleVariant(CXhChar16("%.1f",pBomPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(pBomPart->GetPartNum()));	//������
		if(pBomPart->bWeldPart)
			map.SetValueAt(index,7,COleVariant(CXhChar16("��")));		//�纸
		map.SetValueAt(index,8,COleVariant(CXhChar16("%d",pBomPart->nMSumLs)));		//����
		if(IsZhiWan(pBomPart))
			map.SetValueAt(index,9,COleVariant(CXhChar16("��")));		//����
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,10,COleVariant(CXhChar16("��")));	//�н�
			if(pBomAngle->bCutBer)
				map.SetValueAt(index,11,COleVariant(CXhChar16("��")));	//����
			if(pBomAngle->bCutRoot)
				map.SetValueAt(index,12,COleVariant(CXhChar16("��")));	//�ٸ�
		}
		CXhChar50 sNote;
		GetProcessNotes(pBomPart,sNote);
		map.SetValueAt(index,13,COleVariant(sNote));			//��ע
		index++;
	}

	CXhChar16 cell_end("N%d",index+6);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A7",cell_end,COleVariant((long)3));
	CExcelOper::SetRangeValue(excel_sheet,"A7",cell_end,map.var);				//���ñ������
	CExcelOper::SetRangeBorders(excel_sheet,"A7",cell_end,COleVariant(10.5));	//���ñ߿��ָ�
	return 1;
}