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

//��������
const static BYTE TYPE_WELD			=1;	//����
const static BYTE TYPE_HUOQU		=2;	//����
const static BYTE TYPE_CUT_ANGLE	=3;	//�н�
const static BYTE TYPE_CUT_BER		=4;	//����
const static BYTE TYPE_CUT_ROOT		=5;	//���
const static BYTE TYPE_PUSH_FLAT	=6;	//ѹ��
const static BYTE TYPE_KAIHE_ANGLE	=7;	//���Ͻ�
const static BYTE TYPE_DRILL_HOLE	=8;	//���
const static BYTE TYPE_FILLET		=9;	//���¿�
const static BYTE TYPE_CUT_ARC		=10;//�л�

static int IsNeedProcess(BOMPART* pBomPart,BYTE process_type)
{
	if(process_type==TYPE_DRILL_HOLE && pBomPart->IsDrillHole())
		return 1;	//���
	else if(process_type==TYPE_WELD && pBomPart->bWeldPart)
		return 1;	//����
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
		if(process_type==TYPE_CUT_ARC && pBomPlate->GetArcEdgeCount()>0 )
			return 1;	//�л�
		else if(process_type==TYPE_FILLET && pBomPlate->bNeedFillet)
			return 1;	//���¿�
		else if(process_type==TYPE_HUOQU && pBomPlate->m_cFaceN>1)
			return 1;	//������
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE* pBomJg=(PART_ANGLE*)pBomPart;
		if(process_type==TYPE_CUT_BER && pBomJg->bCutBer)
			return 1;	//����
		else if(process_type==TYPE_CUT_ROOT && pBomJg->bCutRoot)
			return 1;	//���
		else if(process_type==TYPE_CUT_ANGLE && pBomJg->bCutAngle)
			return 1;	//�н�
		else if(process_type==TYPE_PUSH_FLAT && pBomJg->nPushFlat>0)
			return 1;	//ѹ��
		else if(process_type==TYPE_KAIHE_ANGLE)
		{	//���Ͻ�
			if(pBomJg->bHeJiao)
				return 2;
			else if(pBomJg->bKaiJiao)
				return 1;
		}
		else if(process_type==TYPE_HUOQU)
		{
			int nHuoquType=pBomJg->GetHuoquType();
			if(pBomJg->GetHuoquLineCount()>0&&nHuoquType>=1&&nHuoquType<=4)
				return 1;	//����Ǹ�
		}
	}
	return 0;
}
typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
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
		sprintf(sSpec,"%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart)
{
	CXhChar50 str;
	char sSpec[30]="";
	if(pBomPart->cPartType==BOMPART::ANGLE)	//��ע�Ǹֹ���
	{
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
		str.Printf("��%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetNoteParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam(pBomPart->sNotes);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		//���
		if(pPlate->siSubType=BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
		{	//������˨���
			CXhChar16 sDianBan;
			if(pPlate->m_arrBoltRecs[0].d==16)
				sDianBan.Printf("��17.5*%d",pBomPart->GetPartNum());
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Printf("��21.5*%d",pBomPart->GetPartNum());
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Printf("��25.5*%d",pBomPart->GetPartNum());
			sProcessParam.Append(sDianBan);
			//�п׵�岻��Ҫ�ٽ��к���
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		//���
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
			{
				sProcessParam.Append(CXhChar50("���%dmm",pVertex->data.manu_space),' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("���Ŷ�",' ');
	}
	return sProcessParam;
}
CXhChar100 GetTypeParam(BOMPART* pBomPart)
{
	CXhChar100 sSpecialType;

	return sSpecialType;
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
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.SetName("�Ǹ��㹹���嵥");	
	//���ʵʱ���ݱ���
	CVariant2dArray map(bom.m_nPartsN+1,25);
	int index=0;
	map.SetValueAt(index,0,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"A1");
	map.SetValueAt(index,1,COleVariant("������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.5,"C1");
	map.SetValueAt(index,3,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"D1");
	map.SetValueAt(index,4,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,9.5,"E1");
	map.SetValueAt(index,5,COleVariant("����(mm)"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"F1");
	map.SetValueAt(index,6,COleVariant("���(mm)"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"G1");
	map.SetValueAt(index,7,COleVariant("������"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"H1");
	map.SetValueAt(index,8,COleVariant("��������(kg)"));
	CExcelOper::SetRangeColWidth(excel_sheet,12.5,"I1");
	map.SetValueAt(index,9,COleVariant("������(kg)"));
	CExcelOper::SetRangeColWidth(excel_sheet,11,"J1");
	map.SetValueAt(index,10,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"K1");
	map.SetValueAt(index,11,COleVariant("��ע"));
	CExcelOper::SetRangeColWidth(excel_sheet,14,"L1");
	map.SetValueAt(index,12,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"M1");
	map.SetValueAt(index,13,COleVariant("�纸"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"N1");
	map.SetValueAt(index,14,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"O1");
	map.SetValueAt(index,15,COleVariant("�н���֫"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"P1");
	map.SetValueAt(index,16,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"Q1");
	map.SetValueAt(index,17,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"R1");
	map.SetValueAt(index,18,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"S1");
	map.SetValueAt(index,19,COleVariant("���Ͻ�"));
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"T1");
	map.SetValueAt(index,20,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"U1");
	map.SetValueAt(index,21,COleVariant("������ʽ"));
	CExcelOper::SetRangeColWidth(excel_sheet,9,"V1");
	map.SetValueAt(index,22,COleVariant("���¿�"));
	CExcelOper::SetRangeColWidth(excel_sheet,6.5,"W1");
	map.SetValueAt(index,23,COleVariant("�"));
	CExcelOper::SetRangeColWidth(excel_sheet,4,"X1");
	map.SetValueAt(index,24,COleVariant("�Ƿ�ר��"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"Y1");
	//���ʵʱ��������
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//����
		map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));				//����
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));						//���
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//����
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//���
		else
			map.SetValueAt(index,6,COleVariant("0"));
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//�ӹ�����
		map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//��������
		CXhChar16 sFormula("=H%d*I%d",index+1,index+1);
		map.SetValueAt(index,9,COleVariant(sFormula));	//ͨ����ʽ��������
		map.SetValueAt(index,10,COleVariant(pBomPart->nMSumLs));						//����
		map.SetValueAt(index,11,COleVariant(GetNoteParam(pBomPart)));				//��ע
		map.SetValueAt(index,12,COleVariant(GetTypeParam(pBomPart)));				//����
		map.SetValueAt(index,13,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_WELD))));		//�纸
		map.SetValueAt(index,14,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_HUOQU))));		//����
		map.SetValueAt(index,15,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ANGLE))));	//�н�
		map.SetValueAt(index,16,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_BER))));		//����
		map.SetValueAt(index,17,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ROOT))));	//���
		map.SetValueAt(index,18,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_PUSH_FLAT))));	//���
		map.SetValueAt(index,19,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_KAIHE_ANGLE))));	//���Ͻ�
		map.SetValueAt(index,20,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_DRILL_HOLE))));	//���
		map.SetValueAt(index,21,COleVariant(""));														//������ʽ
		map.SetValueAt(index,22,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_FILLET))));		//���¿�
		map.SetValueAt(index,23,COleVariant(CXhChar16("%d",IsNeedProcess(pBomPart,TYPE_CUT_ARC))));		//�
		map.SetValueAt(index,24,COleVariant("1"));	//�Ƿ�ר��
		//
		index++;
	}
	CXhChar16 cell_end("Y%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�������С
	//
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 0;
}