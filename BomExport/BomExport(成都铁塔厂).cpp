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
		sprintf(sSpec,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sx%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(bNeedPlateW)
			sSpec.Printf("%.fx%.f",pBomPart->thick,pBomPart->wide);
		else
			sSpec.Printf("%.f",pBomPart->thick);
	}
	else 
		sSpec.Printf("%s",(char*)pBomPart->sSpec);
	return sSpec;
}
static BOOL IsZhiWan(BOMPART* pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
			return TRUE;	//������
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				return TRUE;	//��߰�
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHeJiao||pBomAngle->bKaiJiao)
			return TRUE;	//�п��Ͻ�
		int nHuoquType=pBomAngle->GetHuoquType();
		if(pBomPart->GetHuoquLineCount()>0&&nHuoquType>=1&&nHuoquType<=4)
			return TRUE;	//����Ǹ�
	}
	return FALSE;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam(pBomPart->sNotes);
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		//���
		if(pPlate->siSubType=BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
		{	//������˨���
			CXhChar16 sDianBan("���%.f",pBomPart->thick);
			if(pPlate->m_arrBoltRecs[0].d==16)
				sDianBan.Append("A");
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Append("B");
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Append("C");
			sProcessParam.Append(sDianBan,' ');
			//�п׵�岻��Ҫ�ٽ��к���
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		if(pBomPart->bWeldPart)
			sProcessParam.Append("����",' ');
		if(pPlate->bNeedFillet)
			sProcessParam.Append("�¿�",' ');
		//����
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
		{
			double fHuoquAngle1=fabs(pPlate->huoQuAngleArr[0]);
			double fHuoquAngle2=fabs(pPlate->huoQuAngleArr[1]);
			CXhChar50 sAngle("����%0.1f",fHuoquAngle1>fHuoquAngle2?fHuoquAngle1:fHuoquAngle2);
			sProcessParam.Append(sAngle,' ');
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
		//����
		if(pBomPart->bWeldPart)
			sProcessParam.Append("����",' ');
		//����
		int nHuoquType=pBomAngle->GetHuoquType();
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		CXhChar16 zhiwanArr[4]={"������","������","������","������"};
		if(nHuoquLineCount>0&&nHuoquType>=1&&nHuoquType<=4)
		{
			BOMPROFILE_VERTEX::COORD3D prev_len_vec;
			prev_len_vec.z=1;
			double fMinCosa=1;
			pBomAngle->listPolyAngle.EnumFirst();
			for(LIST_NODE<PART_ANGLE::POLY_ANGLE_INFO> *pNode=pBomAngle->listPolyAngle.EnumNext();pNode;pNode=pBomAngle->listPolyAngle.EnumNext())
			{
				BOMPROFILE_VERTEX::COORD3D len_vec(pNode->data.len_vec);
				double cosa=prev_len_vec.x*len_vec.x+prev_len_vec.y*len_vec.y+prev_len_vec.z*len_vec.z;
				if(cosa<fMinCosa)
					fMinCosa=cosa;
				prev_len_vec.x=len_vec.x;
				prev_len_vec.y=len_vec.y;
				prev_len_vec.z=len_vec.z;
			}
			double fAngle=acos(fMinCosa)*DEGTORAD_COEF;
			sProcessParam.Append(CXhChar16("%s%0.1f",(char*)zhiwanArr[nHuoquType-1],fAngle),' ');
		}
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
			sProcessParam.Append(CXhChar16("ѹ��"),' ');
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append(CXhChar16("����%0.1f",pBomAngle->wing_angle-90),' ');
		if(pBomAngle->bHeJiao) 
			sProcessParam.Append(CXhChar16("�Ͻ�%0.1f",90-pBomAngle->wing_angle),' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("�н�",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("����",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("���",' ');
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("���Ŷ�",' ');
	}
	return sProcessParam;
}

void AddPartSpecTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("����ԭʼ���ϱ�");
	//��ӱ�����
	CExcelOper::SetMainTitle(excel_sheet,"A1","S1","�ɶ�������",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A2","S2","����ԭʼ���ϱ�",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","B3","������:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"C3","K3",bom.m_sPrjName,15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"L3","M3","����:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N3","S3",bom.m_sTowerTypeName,15,12,2,2,false);
	//�ϲ�������
	CExcelOper::MergeRowRange(excel_sheet,"A4","A5");	//�κ�
	CExcelOper::MergeRowRange(excel_sheet,"B4","B5");	//������
	CExcelOper::MergeRowRange(excel_sheet,"C4","C5");	//����
	CExcelOper::MergeColRange(excel_sheet,"D4","E4");	//���Ϲ��
	CExcelOper::MergeRowRange(excel_sheet,"F4","F5");	//����
	CExcelOper::MergeRowRange(excel_sheet,"G4","G5");	//���
	CExcelOper::MergeRowRange(excel_sheet,"H4","H5");	//�ӹ�����
	CExcelOper::MergeColRange(excel_sheet,"I4","P4");	//������
	CExcelOper::MergeRowRange(excel_sheet,"Q4","Q5");	//�ӹ�˵��
	CExcelOper::MergeRowRange(excel_sheet,"R4","R5");	//��������
	CExcelOper::MergeRowRange(excel_sheet,"S4","S5");	//С������
	//���ʵʱ���ݵ��б���
	double col_arr[19]={4.0,5.0,4.0,7.0,4.5,5.0,5.0,4.0,2.5,4.0,2.5,2.5,2.5,2.5,2.5,2.5,17.0,5.0,5.0};
	CStringArray str_arr;
	str_arr.SetSize(19);
	str_arr[4]=str_arr[9]=str_arr[10]=str_arr[11]=str_arr[12]=str_arr[13]=str_arr[14]=str_arr[15]="";
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="���Ϲ��";
	str_arr[5]="����(mm)";str_arr[6]="���(mm)";str_arr[7]="�ӹ�����";str_arr[8]="������(����)";
	str_arr[16]="�ӹ�˵��";str_arr[17]="��������";str_arr[18]="С������";
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,15,"A4","S4");	//�����и�
	str_arr[0]=str_arr[1]=str_arr[2]=str_arr[5]=str_arr[6]=str_arr[7]=str_arr[16]=str_arr[17]=str_arr[18]="";
	str_arr[3]="��ƹ��";str_arr[4]="����";
	str_arr[8]="�躸";str_arr[9]="����";str_arr[10]="����";str_arr[11]="�н�";
	str_arr[12]="���";str_arr[13]="����";str_arr[14]="�ٽ�";str_arr[15]="���";
	CExcelOper::AddRowToExcelSheet(excel_sheet,5,str_arr,NULL,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,25.5,"A5","S5");	//�����и�
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4","S5",COleVariant((long)3));
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+2,19);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartTypeName(TRUE)));			//����
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));						//��ƹ��
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);
		if(pBomPart->cMaterial!='S')
			map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//����
		if(pBomPart->cMaterial=='P')	//Q420������мӴ�
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+6),NULL);	//����Ӵ�
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));		//����
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));		//���
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//�ӹ�����
		map.SetValueAt(index,8,pBomPart->bWeldPart?COleVariant("*"):COleVariant(""));	//�躸
		map.SetValueAt(index,9,COleVariant(pBomPart->nMSumLs));							//����
		map.SetValueAt(index,10,IsZhiWan(pBomPart)?COleVariant("*"):COleVariant(""));	//����
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			map.SetValueAt(index,11,pBomAngle->bCutAngle?COleVariant("*"):COleVariant(""));	 //�н�
			map.SetValueAt(index,12,pBomAngle->nPushFlat>0?COleVariant("*"):COleVariant(""));//���
			map.SetValueAt(index,13,pBomAngle->bCutBer?COleVariant("*"):COleVariant(""));	 //����
			map.SetValueAt(index,14,pBomAngle->bCutRoot?COleVariant("*"):COleVariant(""));	 //�ٽ�
		}
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			map.SetValueAt(index,14,pBomPlate->bNeedFillet?COleVariant("*"):COleVariant(""));	 //�ٽ�
		}
		map.SetValueAt(index,15,pBomPart->IsDrillHole()?COleVariant("*"):COleVariant(""));	//���
		map.SetValueAt(index,16,COleVariant(GetProcessParam(pBomPart)));				//�ӹ�˵��
		if(pBomPart->cPartType==BOMPART::PLATE)	//�ְ忼�Ǿ��������
			map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",((PART_PLATE*)pBomPart)->fPieceNetWeight)));	//��������
		else
			map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//��������
		CXhChar16 sFormula("=R%d*H%d",index+6,index+6);
		map.SetValueAt(index,18,COleVariant(sFormula));	//ͨ����ʽ��������
		index++;
	}
	CXhChar16 cell_end("S%d",index+6);
	CExcelOper::SetRangeValue(excel_sheet,"A6",cell_end,map.var);				//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A6",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	//���ñ߿�������С
	excel_sheet.ReleaseDispatch();
}
void AddMatEstimateTbl(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
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
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("����Ԥ��");
	//��ӱ�����
	CExcelOper::SetMainTitle(excel_sheet,"A1","R1","�ɶ�������",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A2","R2","����Ԥ����",20,14,3,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"A3","B3","��������:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"C3","K3",bom.m_sPrjName,15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"L3","M3","����:",15,12,2,2,false);
	CExcelOper::SetMainTitle(excel_sheet,"N3","R3",bom.m_sTowerTypeName,15,12,2,2,false);
	//���ʵʱ���ݵ��б���
	CStringArray str_arr;
	str_arr.SetSize(18);
	str_arr[0]="��λ";str_arr[1]="������";str_arr[2]="��������";str_arr[3]="���";str_arr[4]="����";
	str_arr[5]="����";str_arr[6]="���";str_arr[7]="��������";str_arr[8]="�纸";str_arr[9]="��������";
	str_arr[10]="����";str_arr[11]="�н�";str_arr[12]="���";str_arr[13]="����";str_arr[14]="�ٽ�";
	str_arr[15]="���";str_arr[16]="�ӹ�˵��";str_arr[17]="��������";
	double col_arr[18]={4.0,5.0,4.5,7.0,4.5,5.0,5.0,4.0,2.5,4.0,2.5,2.5,2.5,2.5,2.5,2.5,17.0,5.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeRowHeight(excel_sheet,30,"A4","R4");	//�����и�
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4","R4",COleVariant((long)3));	//ˮƽ����
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+2,19);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));			//��λ
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartTypeName(TRUE)));			//��������
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart,TRUE)));					//��ƹ��
		map.SetValueAt(index,4,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//����
		if(pBomPart->cMaterial=='P')	//Q420������мӴ�
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+5),NULL);	//����Ӵ�
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//����
		map.SetValueAt(index,6,COleVariant(""));										//���
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));					//��������
		map.SetValueAt(index,8,pBomPart->bWeldPart?COleVariant("*"):COleVariant(""));	//�纸
		map.SetValueAt(index,9,COleVariant("0"));										//����
		map.SetValueAt(index,10,IsZhiWan(pBomPart)?COleVariant("*"):COleVariant(""));	//����
		if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE* pBomAngle=(PART_ANGLE*)pBomPart;
			map.SetValueAt(index,11,pBomAngle->bCutAngle?COleVariant("*"):COleVariant(""));	 //�н�
			map.SetValueAt(index,12,pBomAngle->nPushFlat>0?COleVariant("*"):COleVariant(""));//���
			map.SetValueAt(index,13,pBomAngle->bCutBer?COleVariant("*"):COleVariant(""));	 //����
			map.SetValueAt(index,14,pBomAngle->bCutRoot?COleVariant("*"):COleVariant(""));	 //�ٽ�
		}
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate=(PART_PLATE*)pBomPart;
			map.SetValueAt(index,14,pBomPlate->bNeedFillet?COleVariant("*"):COleVariant(""));//�ٽ�
		}
		map.SetValueAt(index,15,pBomPart->IsDrillHole()?COleVariant("*"):COleVariant(""));	//���
		map.SetValueAt(index,16,COleVariant(GetProcessParam(pBomPart)));				//�ӹ�˵��
		map.SetValueAt(index,17,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//��������(�����������)
		index++;
	}
	CXhChar16 cell_end("R%d",index+5);
	CExcelOper::SetRangeValue(excel_sheet,"A5",cell_end,map.var);	//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A5",cell_end,COleVariant((long)2));//ˮƽ�����
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	//���ñ߿�
	excel_sheet.ReleaseDispatch();
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
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet;
	//1 ������ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartSpecTblSheet(bom,pWorksheet);
	//2 ����Ԥ����
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddMatEstimateTbl(bom,pWorksheet);
	//
	excel_sheets.ReleaseDispatch();
	return 0;
}