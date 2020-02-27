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

short CutAngleN(PART_ANGLE *pBomAngle)
{
	short nCutCount=0;
	for(int i=0;i<2;i++)
	{	//0.ʼ�� 1.�ն�
		if(pBomAngle->cut_wing[i]>0&&pBomAngle->cut_wing_para[i][0]>0)
		{	//��֫
			if(pBomAngle->cut_wing_para[i][1]==0)
				nCutCount+=2;		//�п�֫(��2��)
			else 
				nCutCount+=3;		//��֫(��3��)
		}
		if(pBomAngle->cut_angle[i*2][0]>0)		//X֫�н�(��1��)
			nCutCount++;
		if(pBomAngle->cut_angle[i*2+1][0]>0)	//Y֫�н�(��1��)
			nCutCount++;
	}
	return nCutCount;
}
static int GetProcessNotes(BOMPART* pBomPart,char *notes,int nTaNum,bool bIncKaiHeAngle=FALSE)
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
		if (pBomPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{	
			int nBoltNum=pBomPart->nM16Ls+pBomPart->nM20Ls+pBomPart->nM24Ls;
			
			if(pBomPart->nM16Ls>0)
				sprintf(ss,"%s %d-%.1f��Ƭ",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
			if(pBomPart->nM20Ls>0)
				sprintf(ss,"%s %d-%.1f��Ƭ",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
			if(pBomPart->nM24Ls>0)
				sprintf(ss,"%s %d-%.1f��Ƭ",ss,nBoltNum*nTaNum,pPlate->m_arrBoltRecs[0].d+pPlate->m_arrBoltRecs[0].hole_d_increment);
		}
		if(pBomPart->bWeldPart)
			sprintf(ss,"%s ����",ss);
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
			{
				sprintf(ss,"%s ���",ss);
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
	if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)//�Ƿ��н�
		{
			if(pBomAngle->cut_wing[0]!=0||pBomAngle->cut_wing[1]!=0)
				sprintf(ss,"%s ��֫��",ss);
			for (int i=0;i<4;i++)
			{
				if (pBomAngle->cut_angle[i][0]!=0||pBomAngle->cut_angle[i][1]!=0)
				{
					sprintf(ss,"%s �н�",ss);
					break;
				}
			}
		}
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
	
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("����ԭʼ���ϱ�");
	char cell_end[8];
	CExcelOper::MergeColRange(excel_sheet,"A1","Q1");
	CExcelOper::SetMainTitle(excel_sheet,"A1","Q1","�ۺ��嵥");
	//�ڶ���
	CXhChar100 sPrjName="��������: ";
	sPrjName.Append(bom.m_sPrjName);
	CExcelOper::MergeColRange(excel_sheet,"A2","Q2");
	CExcelOper::SetRangeValue(excel_sheet,"A2","A2",COleVariant(sPrjName));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2","Q2",COleVariant((long)1));
	//������
	//����
	CExcelOper::MergeColRange(excel_sheet,"A3","Q3");
	CXhChar100 sTowerType="���ͣ�";
	sTowerType.Append(bom.m_sTowerTypeName);
	CExcelOper::SetRangeValue(excel_sheet,"A3","A3",COleVariant(sTowerType));
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A3","Q3",COleVariant((long)1));
	
	CStringArray str_arr;
	str_arr.SetSize(17);
	str_arr[0]="���";str_arr[1]="����";str_arr[2]="���";str_arr[3]="����";
	str_arr[4]="����";str_arr[5]="����";str_arr[6]="����";str_arr[7]="����";
	str_arr[8]="�ӹ�����";str_arr[9]="��";str_arr[10]="M16";str_arr[11]="M20";
	str_arr[12]="M24";str_arr[13]="M_X";str_arr[14]="����";str_arr[15]="�н�";
	str_arr[16]="���⹤��";
	double col_arr[17]={5.0,5.0,5.0,7.5,5.0,
						5.5,5.0,5.0,5.0,3,
						3,3,3.5,3.5,5.0,
						5.0,13};
	CExcelOper::AddRowToExcelSheet(excel_sheet,4,str_arr,col_arr,TRUE);
	CVariant2dArray map(bom.m_nPartsN,22);
	int index=0;
	for(SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART *pPart=pNode->pDataObj;
		if(pPart==NULL)
			continue;
		//���
		map.SetValueAt(index,0,COleVariant(pPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));
		//����
		CXhChar16 sMaterial=QuerySteelMatMark(pPart->cMaterial);
		if(strcmp(sMaterial,"Q235")!=0) 
			map.SetValueAt(index,1,COleVariant(QuerySteelMatMark(pPart->cMaterial)));
		//���
		map.SetValueAt(index,2,COleVariant(pPart->sSpec));
		//����
		map.SetValueAt(index,3,COleVariant(CXhChar16("%.2f",pPart->length)));
		//����
		map.SetValueAt(index,4,COleVariant((long)pPart->GetPartNum()));
		
		double real_weight=0,min_rect_weight=0;
		if(pPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE *pPlate=(PART_PLATE*)pPart;
			real_weight=ftol(CalSteelPlankWei(pPlate->real_area*pPlate->thick)*100)/100.0;
			min_rect_weight=ftol(CalSteelPlankWei(pPlate->min_area*pPlate->thick)*100)/100.0;
		}
		else
			min_rect_weight=real_weight=ftol(pPart->fPieceWeight*100)/100.0;
		//����
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.2f",min_rect_weight)));
		//����
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.2f",pPart->GetPartNum()*min_rect_weight)));
		//����
		map.SetValueAt(index,7,COleVariant((long)bom.m_nTaNum));
		CXhChar50 sStartCell=CExcelOper::GetCellPos(4,index+5);
		CXhChar50 sEndCell=CExcelOper::GetCellPos(7,index+5);
		CXhChar100 str="=";
		str.Append(sStartCell);
		str.Append("*");
		str.Append(sEndCell);
		//�ӹ�����
		map.SetValueAt(index,8,COleVariant(str));
		//��
		if(pPart->bWeldPart)
			map.SetValueAt(index,9,COleVariant("��"));
		//M16
		if(pPart->nM16Ls!=0)
			map.SetValueAt(index,10,COleVariant(pPart->nM16Ls));
		//M20
		if(pPart->nM20Ls!=0)
			map.SetValueAt(index,11,COleVariant(pPart->nM20Ls));
		//M24
		if(pPart->nM24Ls!=0)
			map.SetValueAt(index,12,COleVariant(pPart->nM24Ls));
		//M_X
		if(pPart->nMXLs!=0)
			map.SetValueAt(index,13,COleVariant(pPart->nMXLs));
		if (pPart->cPartType==BOMPART::ANGLE)
		{	//����
			PART_ANGLE *pBomAngle=(PART_ANGLE*)pPart;
			short siKaiHeJiaoCount=GetKaiHeJiaoN(pBomAngle);
			if(siKaiHeJiaoCount>0)
				map.SetValueAt(index,14,COleVariant((long)siKaiHeJiaoCount));
			//�н�
			if(pBomAngle->bCutAngle)
				map.SetValueAt(index,15,COleVariant(CutAngleN(pBomAngle)));
		}
		//���⹩����
		CXhChar100 sNots="";
		GetProcessNotes(pPart,sNots,bom.m_nTaNum);
		map.SetValueAt(index,16,COleVariant(sNots));
		index++;
	}
	//�����Ҷ���
	sprintf(cell_end,"Q%d",index+4);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A4",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A5",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A4",cell_end,COleVariant(10.5));	
	return 1;
}