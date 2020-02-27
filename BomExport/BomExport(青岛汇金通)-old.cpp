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
		if(pPlate->bWeldPart&&pPlate->bNeedFillet)		//�Ƿ���Ҫ����
			sprintf(ss,"%s ����		",ss);
		else if(pPlate->bWeldPart)
		if(pPlate->bWeldPart)		//�Ƿ���Ҫ����
			sprintf(ss,"%s ����",ss);
		if(pPlate->bNeedFillet)		//��Ҫ�¿�
			sprintf(ss,"%s �¿�",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s ���%d",ss,abs(pNode->data.manu_space));
				sprintf(ss,"%s ����",ss);
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
		if(pBomAngle->bCutAngle)		//�Ƿ��н�
			sprintf(ss,"%s �н�",ss);
		if(pBomAngle->bCutBer)			//�Ƿ���Ҫ����
			sprintf(ss,"%s ����",ss);
		if(pBomAngle->bCutRoot)		//�Ƿ���Ҫ���
			sprintf(ss,"%s ���",ss);
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
	//if(bOrgMat)
		excel_sheet.SetName("����ԭʼ���ϱ�");
	//else
	//	excel_sheet.SetName("����������ϸ��");
	//��һ������������У���Ҫ��ʽ���
	CStringArray str_arr;
	str_arr.SetSize(29);
	str_arr[0]="�κ�";
	str_arr[1]="������";
	str_arr[2]="����";
	str_arr[3]="����";
	str_arr[4]="���";
	str_arr[5]="����";
	str_arr[6]="���";
	str_arr[7]="���";
	str_arr[8]="��������";
	str_arr[9]="��������";	//���� 10-10-12
	str_arr[10]="��������";
	str_arr[11]="��ע";
	str_arr[12]="�纸";
	str_arr[13]="����";
	str_arr[14]="�н�";
	str_arr[15]="����";
	str_arr[16]="���";
	str_arr[17]="���";
	str_arr[18]="���Ͻ�";
	str_arr[19]="���";
	str_arr[20]="�¿�";
	str_arr[21]="�������";
	str_arr[22]="������ʽ";
	str_arr[23]="����";
	str_arr[24]="����";
	str_arr[25]="�ܳ�";
	str_arr[26]="�����";
	str_arr[27]="���׾�����";
	str_arr[28]="���ӱ�mm";//���� 10-10-12
	double col_arr[29]={3.0,7.5,3.5,4.5,8.5,
		5.5,4.5,3.0,5.0,5.0,
		5.0,7.5,2.5,2.5,2.5,
		2.5,2.5,2.5,4.0,2.5,
		2.5,4.0,4.0,4.5,2.5,
		3.0,4.0,8.5,5.5};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CVariant2dArray map(bom.m_nPartsN+1,29);
	int index =0, i=0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		i++;
		BOMPART* pBomPart=(BOMPART* )pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//���
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));//����
		//����
		if(pBomPart->cPartType==BOMPART::PLATE)	//�ְ�
		{
			if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL)
				map.SetValueAt(index,3,COleVariant("����"));
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
				map.SetValueAt(index,3,COleVariant("��ˮ��"));
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_FOOT_FL)
				map.SetValueAt(index,3,COleVariant("�׷���"));
			else 
				map.SetValueAt(index,3,COleVariant("�ְ�"));
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)//�Ǹ�
			map.SetValueAt(index,3,COleVariant("�Ǹ�"));
		else if(pBomPart->cPartType==BOMPART::TUBE)//�ֹ�
			map.SetValueAt(index,3,COleVariant("�ֹ�"));
		else if(pBomPart->cPartType==BOMPART::SLOT)//�۸�
			map.SetValueAt(index,3,COleVariant("�۸�"));
		else if(pBomPart->cPartType==BOMPART::FLAT)//���
			map.SetValueAt(index,3,COleVariant("���"));
		//���
		CXhChar100 str;
		char sSpec[30]="";
		if(pBomPart->cPartType==BOMPART::ANGLE)	//��ע�Ǹֹ���
		{
			GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
			str.Printf("L%s",sSpec);
		}
		else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
			&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
			str.Printf("-%.f",pBomPart->thick);
		else 
			str.Printf("%s",(char*)pBomPart->sSpec);
		map.SetValueAt(index,4,COleVariant(str));
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
		//TODO:��ע
		CXhChar100 sNotes;
		GetProcessNotes(pBomPart,sNotes,FALSE);
		sNotes.Append(pBomPart->sNotes);
		map.SetValueAt(index,11,COleVariant(sNotes));			//��ע
		if(pBomPart->bWeldPart)		//�Ƿ�纸��
			map.SetValueAt(index,12,COleVariant("1"));
		long nHuoquLineCount=pBomPart->GetHuoquLineCount();
		if(nHuoquLineCount>0)		//����
			map.SetValueAt(index,13,COleVariant("1"));
		
		if (pBomPart->cPartType==BOMPART::ANGLE)
		{
			PART_ANGLE *pBomAnge = (PART_ANGLE *)pBomPart;
			if(pBomAnge->bCutAngle)		//�н�
				map.SetValueAt(index,14,COleVariant("1"));
			if(pBomAnge->bCutBer)		//����
				map.SetValueAt(index,15,COleVariant("1"));
			if(pBomAnge->bCutRoot)		//���
				map.SetValueAt(index,16,COleVariant("1"));
			if(pBomAnge->nPushFlat>0)	//1 ��ʾһ��ѹ�� 2 ��ʾ����ѹ�� 
				map.SetValueAt(index,17,COleVariant((long)pBomAnge->nPushFlat));
			if(pBomAnge->bKaiJiao)			//����
				map.SetValueAt(index,18,COleVariant("1"));
			else if(pBomAnge->bHeJiao)		//�Ͻ�
				map.SetValueAt(index,18,COleVariant("2"));
			if(pBomAnge->thick>=14)			//���
				map.SetValueAt(index,19,COleVariant("1"));
		}
		if (pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			if(pBomPlate->bNeedFillet)	//�¿�
				map.SetValueAt(index,20,COleVariant("1"));
		}
		//����
		char sTypeName[100]="";
		if(pBomPart->cPartType==BOMPART::TUBE)
		{
			PART_TUBE *pBomTube=(PART_TUBE*)pBomPart;
			if(pBomPart->bWeldPart)
				strcpy(sTypeName,"֧��");
			else 
				strcpy(sTypeName,"����");
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
		else if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_WATER)
				strcpy(sTypeName,"��ˮ��");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL)
				strcpy(sTypeName,"����");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C
				||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U
				||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
				strcpy(sTypeName,"���");
			else if(pBomPart->siSubType==BOMPART::SUB_TYPE_FOOT_FL)
				strcpy(sTypeName,"�׷���");
		}
		map.SetValueAt(index,23,COleVariant(sTypeName));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE * pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,24,COleVariant((long)pBomPlate->GetEdgeCount()));			//����
			map.SetValueAt(index,25,COleVariant(CXhChar16(pBomPlate->fPerimeter)));			//�ܳ�
			//���
			if(bom.m_iPlankWeightStatType==2)
				map.SetValueAt(index,26,COleVariant(CXhChar16(pBomPlate->real_area)));
			else
				map.SetValueAt(index,26,COleVariant(CXhChar16(pBomPlate->min_area)));

		}
		//�����׾�
		char sHoleStr[500]="";
		if(pBomPart->GetLsHoleStr(sHoleStr,FALSE)>0)
			map.SetValueAt(index,27,COleVariant(sHoleStr));
		if(pBomPart->cPartType==BOMPART::PLATE)
		{	//���ӱ�mm
			PART_PLATE* pBomPlate = (PART_PLATE *)pBomPart;
			map.SetValueAt(index,28,COleVariant(CXhChar16(pBomPlate->fWeldEdgeLen)));
		}
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
	return 1;
}
