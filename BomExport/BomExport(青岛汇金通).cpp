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
#include "SegI.h"
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

static CXhChar100 GetProcessNotes(BOMPART* pBomPart,bool bIncKaiHeAngle=FALSE)
{
	CXhChar100 notes;
	long nHuoquLineCount=pBomPart->GetHuoquLineCount();
	if(pBomPart->cPartType==BOMPART::PLATE)
		nHuoquLineCount=((PART_PLATE*)pBomPart)->m_cFaceN-1;
	if(nHuoquLineCount>0)	//�Ƿ���Ҫ����
		notes.Append("����",' ');
	if(pBomPart->bWeldPart)
		notes.Append("����",' ');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->bNeedFillet)		//��Ҫ�¿�
			notes.Append("�¿�",' ');
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				notes.Append("���� ���",' ');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("�����¿�",' ');
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			notes.Append("һ���¿�",' ');
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)		//�Ƿ��н�
			notes.Append("�н�",' ');
		if(pBomAngle->bCutBer)			//�Ƿ���Ҫ����
			notes.Append("����",' ');
		if(pBomAngle->bCutRoot)		//�Ƿ���Ҫ���
			notes.Append("���",' ');
		if(pBomAngle->bKaiJiao&&GetKaiHeJiaoN(pBomAngle)>0)		//�Ƿ���Ҫ����
		{
			if(bIncKaiHeAngle)	//�������Ƕ��� wht 11-05-24
				notes.Append(CXhChar16("����%.1f��",pBomAngle->wing_angle-90),' ');
			else
				notes.Append("����",' ');
		}
		if(pBomAngle->bHeJiao&&GetKaiHeJiaoN(pBomAngle)>0)			//�Ƿ���Ҫ�Ͻ�
		{
			if(bIncKaiHeAngle)	//�����ϽǶ��� wht 11-05-24
				notes.Append(CXhChar16("�Ͻ�%.1f��",90-pBomAngle->wing_angle),' ');
			else
				notes.Append("�Ͻ�",' ');
		}
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
		{
			if(pBomAngle->nPushFlat==0x02)
				notes.Append("�м�ѹ��",' ');
			else
				notes.Append("ѹ��",' ');
		}
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			notes.Append("���Ŷ�",' ');
	}
	return notes;
}

static int GetCutAngleNum(PART_ANGLE *pBomAngle)
{
	int nNum=0;
	int alfa = 0;
	if(pBomAngle->cut_wing[0]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[0]==1)		//ʼ��X֫����֫
			nNum++;
		else if(pBomAngle->cut_wing[0]==2)	//ʼ��Y֫����֫
			nNum++;
	}
	if(pBomAngle->cut_angle[0][0]>0&&pBomAngle->cut_angle[0][1]>0&&pBomAngle->cut_wing[0]!=1)
	{	//ʼ��X֫���н�
		nNum++;
	}
	if(pBomAngle->cut_angle[1][0]>0&&pBomAngle->cut_angle[1][1]>0&&pBomAngle->cut_wing[0]!=2)
	{	//ʼ��Y֫���н�
		nNum++;
	}
	if(pBomAngle->cut_wing[1]>0)
	{
		CXhChar16 alfa_str;
		if(pBomAngle->cut_wing[1]==1)		//�ն�X֫����֫
			nNum++;
		else if(pBomAngle->cut_wing[1]==2)	//�ն�Y֫����֫
			nNum++;
	}
	if(pBomAngle->cut_angle[2][0]>0&&pBomAngle->cut_angle[2][1]>0&&pBomAngle->cut_wing[1]!=1)	
	{	//�ն�X֫���н�
		nNum++;
	}
	if(pBomAngle->cut_angle[3][0]>0&&pBomAngle->cut_angle[3][1]>0&&pBomAngle->cut_wing[1]!=2)
	{	//�ն�Y֫���н�
		nNum++;
	}
	return nNum;
}

void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{	//��������
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
	//�������ԭʼ���ϱ�
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("����ԭʼ���ϱ�");
	//��һ������������У���Ҫ��ʽ���
	CStringArray str_arr;
	str_arr.SetSize(29);
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="����";str_arr[4]="���";
	str_arr[5]="����";str_arr[6]="���";str_arr[7]="���";str_arr[8]="��������";str_arr[9]="��������";	//���� 10-10-12
	str_arr[10]="��������";str_arr[11]="��ע";str_arr[12]="�纸";str_arr[13]="����";str_arr[14]="�н�";
	str_arr[15]="����";str_arr[16]="���";str_arr[17]="���";str_arr[18]="���Ͻ�";str_arr[19]="���";
	str_arr[20]="�¿�";str_arr[21]="�������";str_arr[22]="������ʽ";str_arr[23]="����";str_arr[24]="����";
	str_arr[25]="�ܳ�";str_arr[26]="������O";str_arr[27]="���׾�����";str_arr[28]="���ӱ�mm";//���� 10-10-12
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,5.5,4.5,3.0,5.0,5.0,5.0,17.5,2.5,2.5,2.5,2.5,2.5,2.5,4.0,2.5,2.5,4.0,4.0,4.5,2.5,8.0,8.0,18.5,5.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1","AC1",COleVariant((long)3));
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+1,29);
	int index =0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//���
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);
		map.SetValueAt(index,2,COleVariant(sMat));				//����
		CExcelOper::SetRangeBackColor(excel_sheet,GetMatColorIndex(sMat),CXhChar16("B%d",index+2),CXhChar16("C%d",index+2));
		map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));		//����
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));				//���
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)	//��������ְ���Ӧ����<�����/����>�������� wjh-2014.4.09
			map.SetValueAt(index,5,COleVariant(ftol(((PART_PLATE*)pBomPart)->real_area/pBomPart->wide)));		//���
		else
			map.SetValueAt(index,5,COleVariant(ftol(pBomPart->length)));	//����
		map.SetValueAt(index,6,COleVariant(ftol(pBomPart->wide)));		//���
		map.SetValueAt(index,7,COleVariant(ftol(pBomPart->thick)));		//���
		map.SetValueAt(index,8,COleVariant((long)(pBomPart->GetPartNum())));	//��������
		double weight=0;
		if(bom.m_iPlankWeightStatType<=1&&pBomPart->cPartType==BOMPART::PLATE)	//
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			double fMinWeight=CalSteelPlankWei(pPlate->min_area*pPlate->thick);
			weight=ftol(fMinWeight*100)/100.0;
		}
		else
			weight=ftol(pBomPart->fPieceWeight*100)/100.0;
		map.SetValueAt(index,9,COleVariant(CXhChar16("%.2f",weight)));	//��������
		map.SetValueAt(index,10,COleVariant((long)pBomPart->nMSumLs));	//��������
		map.SetValueAt(index,11,COleVariant(GetProcessNotes(pBomPart,TRUE)));	//��ע
		map.SetValueAt(index,12,pBomPart->bWeldPart?COleVariant("1"):COleVariant(""));	//�Ƿ�纸��
		map.SetValueAt(index,13,pBomPart->GetHuoquLineCount()>0?COleVariant("1"):COleVariant(""));	//����
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAnge = (PART_ANGLE *)pBomPart;
			map.SetValueAt(index,14,pBomAnge->bCutAngle?COleVariant((long)GetCutAngleNum(pBomAnge)):COleVariant(""));	//�н�
			map.SetValueAt(index,15,pBomAnge->bCutBer?COleVariant("1"):COleVariant(""));	//����
			map.SetValueAt(index,16,pBomAnge->bCutRoot?COleVariant("1"):COleVariant(""));	//���
			map.SetValueAt(index,17,pBomAnge->nPushFlat>0?COleVariant((long)pBomAnge->nPushFlat):COleVariant(""));	//ѹ��
			map.SetValueAt(index,18,pBomAnge->bKaiJiao?COleVariant("1"):COleVariant(""));	//����
			map.SetValueAt(index,18,pBomAnge->bHeJiao?COleVariant("2"):COleVariant(""));	//�Ͻ�
			map.SetValueAt(index,19,pBomAnge->thick>=14?COleVariant("1"):COleVariant(""));	//���
		}
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//�¿�
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,20,pBomPlate->bNeedFillet?COleVariant("1"):COleVariant(""));
		}
		if(pBomPart->cPartType==BOMPART::TUBE)
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pBomPart;
			//�������
			long nTransect=0;
			if( pBomTube->startProcess.type==1||pBomTube->startProcess.type==2)
				nTransect++;
			if(pBomTube->endProcess.type==1||pBomTube->endProcess.type==2)
				nTransect++;
			if(nTransect>0)
				map.SetValueAt(index,21,COleVariant("1"));	//�������
			//��������
			long nType=0;
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
				map.SetValueAt(index,22,COleVariant(nType));
		}
		map.SetValueAt(index,23,COleVariant(pBomPart->GetPartSubTypeName()));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE * pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,24,COleVariant((long)pBomPlate->GetEdgeCount()));			//����
			map.SetValueAt(index,25,COleVariant(CXhChar16("%g",pBomPlate->fPerimeter)));	//�ܳ�
		}
		double fArea=GetPartZincCoatedArea(pBomPart);
		if(fArea>0)
			map.SetValueAt(index,26,COleVariant(CXhChar16("%0.3f",fArea)));	//���
		//�����׾�
		char sHoleStr[500]="";
		if(pBomPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		//���쳤��
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			if(pBomPlate->fWeldLineLen>0)
				map.SetValueAt(index,28,COleVariant(CXhChar16(pBomPlate->fWeldLineLen)));
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBoltAngle=(PART_ANGLE*)pBomPart;
			if(pBoltAngle->fWeldLineLen>0)
				map.SetValueAt(index,28,COleVariant(CXhChar16(pBoltAngle->fWeldLineLen)));
		}
		index++;
	}
	CXhChar16 cell_end("AC%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);	//���ñ������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",CXhChar16("E%d",index+1),COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�������С
	excel_sheet.ReleaseDispatch();
}

void AddPartGroupSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	//ͳ������
	int nRowNum=3;
	LIST_NODE<BOM_WELDPART> *pNode=NULL;
	LIST_NODE<BOM_PARTREF> *pPartRefNode=NULL;
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext())
			nRowNum++;
	}
	CHashStrList<BOMPART*> hashBomPartPtrByLabel;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
		hashBomPartPtrByLabel.SetValue(pNode->pDataObj->sPartNo,pNode->pDataObj);
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("�麸����ϸ��");
	//
	CExcelOper::SetMainTitle(excel_sheet,"A1","J1","�纸��ϱ�",20,14,3,2,false);
	CVariant2dArray map(nRowNum,10);
	int index=0;
	map.SetValueAt(index,0,COleVariant("��������:"));
	map.SetValueAt(index,6,COleVariant("����:"));
	index++;
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("�����"));
	map.SetValueAt(index,2,COleVariant("�ӹ�����"));
	map.SetValueAt(index,3,COleVariant("�����"));
	map.SetValueAt(index,4,COleVariant("����"));
	map.SetValueAt(index,5,COleVariant("����"));
	map.SetValueAt(index,6,COleVariant("���"));
	map.SetValueAt(index,7,COleVariant("��"));
	map.SetValueAt(index,8,COleVariant("��"));
	map.SetValueAt(index,9,COleVariant("�������"));
	index++;
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"A1","J1");
	for(pNode=bom.listWeldParts.EnumFirst();pNode;pNode=bom.listWeldParts.EnumNext())
	{
		map.SetValueAt(index,0,COleVariant((long)pNode->data.iSeg));		//�κ�
		map.SetValueAt(index,1,COleVariant(pNode->data.sPartNo));
		map.SetValueAt(index,2,COleVariant((long)pNode->data.nWeldPartNum));//�ӹ���
		int startIndex = index;
		for(pPartRefNode=pNode->data.listPartRef.EnumFirst();pPartRefNode;pPartRefNode=pNode->data.listPartRef.EnumNext(),index++)
		{
			BOMPART **ppPart=hashBomPartPtrByLabel.GetValue(pPartRefNode->data.sPartNo);
			if(ppPart==NULL)
				continue;
			BOMPART *pPart=*ppPart;
			map.SetValueAt(index,3,COleVariant(pPart->sPartNo));
			map.SetValueAt(index,4,COleVariant(pPart->GetPartTypeName()));
			map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pPart->cMaterial)));
			map.SetValueAt(index,6,COleVariant(pPart->sSpec));
			map.SetValueAt(index,7,COleVariant(CXhChar16("%.f",pPart->length)));
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.f",pPart->wide)));
			map.SetValueAt(index,9,COleVariant((long)pPartRefNode->data.nPartNum));
		}
		for(int i=0;i<3;i++)
			CExcelOper::MergeRowRange(excel_sheet,CExcelOper::GetCellPos(i,startIndex+2),CExcelOper::GetCellPos(i,index+1));
	}
	CXhChar16 cell_end("J%d",nRowNum);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2",CXhChar16("C%d",nRowNum),COleVariant((long)-4160));
	CExcelOper::SetRangeBorders(excel_sheet,"A3",cell_end,COleVariant(10.5));	//���ñ߿�������С
	excel_sheet.ReleaseDispatch();	
}

static CXhChar16 GetBoltTypeStr(BOMBOLT& bolt)
{
	CXhChar16 sBoltType="";
	if(!bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //��ͨ��˨
		sBoltType="��˨";	//"��ͨ��˨";
	else if(!bolt.bFootNail&&bolt.bTwoCap&&!bolt.bAntiTheft) //˫ñ��˨
		sBoltType="��˨";//"˫ñ��˨";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&!bolt.bAntiTheft) //��ͨ�Ŷ�
		sBoltType="�Ŷ�";	//��ͨ�Ŷ�
	else if(!bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //������˨
		sBoltType="��˨";	//"������˨";
	else if(bolt.bFootNail&&!bolt.bTwoCap&&bolt.bAntiTheft) //�����Ŷ�
		sBoltType="�Ŷ�";	//"�����Ŷ�";
	else if (bolt.IsDianQuan())
		sBoltType="��Ƭ";
	return sBoltType;
}

void AddBomBoltSumMatTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{	
	//1.��ȡBOM
	if(bom.m_nBoltsN<=0)
		return;
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
		return;	//��ʱ��֧��A~AZ
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("��˨��ϸ��");
	//2.2 ������ϱ�
	//2.2.1 ��ӱ�����
	CStringArray str_arr;
	str_arr.SetSize(4);
	str_arr[0]="����";str_arr[1]="���";str_arr[2]="�޿۳�",str_arr[3]="�ȼ�";
	double col_arr[4]={10.0,17.0,7.5,7.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//��Ӷκ�
	int iCol=4;
	for (SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
	{
		CXhChar50 sCel=CExcelOper::GetCellPos(iCol,1);
		CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant(pSegI->ToString()));
		CExcelOper::SetFontBold(excel_sheet,iCol,1,1);
		iCol++;
	}
	CXhChar50 sCel=CExcelOper::GetCellPos(iCol,1);
	CExcelOper::SetRangeValue(excel_sheet,sCel, sCel,COleVariant("�ܼ�"));
	CExcelOper::SetFontBold(excel_sheet,iCol,1,1);
	//2.2.2 ������ϱ�
	int i=0,nCount=0;
	int index=0;
	CString str;
	CVariant2dArray map(nBoltCount,nColCount+4);
	for(int i=0;i<nBoltCount;i++)
	{
		BOMBOLT bomBolt;
		CopyBomBoltProp(boltArr[i].bomBolt,bomBolt);
		bomBolt.AddPart(boltArr[i].bomBolt.GetPartNum());

		CXhChar16 sBoltType=GetBoltTypeStr(bomBolt);
		if(!bomBolt.IsDianQuan())
		{
			map.SetValueAt(index,0,COleVariant(sBoltType));
			CXhChar200	sGuiGe("M%d*%d",bomBolt.d,bomBolt.L);
			if(bomBolt.bFootNail||bomBolt.bTwoCap)
				sGuiGe.Append("˫ñһ��");
			else
				sGuiGe.Append("һñһ��");
			map.SetValueAt(index,1,COleVariant(sGuiGe));
			map.SetValueAt(index,2,COleVariant((long)bomBolt.nNoThreadlen));//�޿۳���lg��
			CXhChar16 sGrade("%.1f",bomBolt.grade);
			map.SetValueAt(index,3,COleVariant(sGrade));	//��˨�ȼ�
		}
		else	
		{
			CExcelOper::SetFontBold(excel_sheet,0,index+7,4);
			map.SetValueAt(index,0,COleVariant("��Ƭ"));
			CXhChar16 str("-%d(%.1f)",bomBolt.L,bomBolt.d+1.5);
			map.SetValueAt(index,1,COleVariant(str));
			map.SetValueAt(index,3,COleVariant("Q235"));
		}
		iCol=4;
		for(SEGI *pSegI=segIArr.GetFirst();pSegI;pSegI=segIArr.GetNext())
		{
			long *pCount=boltArr[i].m_hashNumBySegI.GetValue(pSegI->iSeg);
			if(pCount)
				map.SetValueAt(index,iCol,COleVariant(*pCount));
			iCol++;
		}
		CXhChar50 sPartNum("=SUM(E%d:%s)",index+2,(char*)CExcelOper::GetCellPos(iCol-1,index+2));
		map.SetValueAt(index,iCol,COleVariant(sPartNum));
		index++;
	}
	//
	CXhChar16 sTempCell("%s%d",(char*)sMaxColCell,index+1);
	CExcelOper::SetRangeColWidth(excel_sheet,10,"A1");
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",sTempCell,map.var);
	//���ö���
	sTempCell.Printf("%s%d",(char*)sMaxColCell,index+1);  //
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",sTempCell,COleVariant((long)3));
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",sTempCell,COleVariant(10.5));
	//������ɫ
	index=2;
	for (int i=0;i<nBoltCount;i++)
	{
		CExcelOper::SetFontBold(excel_sheet,0,index,4);
		if(!boltArr[i].bomBolt.IsDianQuan())
		{
			if(16==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,42,index,0,3);
			else if(20==boltArr[i].bomBolt.d)
				CExcelOper::SetRangeBackColor(excel_sheet,43,index,0,3);
		}
		index++;
	}
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
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(3);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	//1 ������ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartSpecTblSheet(bom,pWorksheet);
	//2 ��˨��ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddBomBoltSumMatTbl(bom,pWorksheet);
	//3 �麸����ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 3));
	AddPartGroupSumMatTbl(bom,pWorksheet);

	excel_sheets.DetachDispatch();
	return 0;
}
