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
	int index=0;
	CVariant2dArray map(bom.m_nPartsN+2,18);
	char cell_start[8];
	char cell_end[8];
	CXhChar50 sTitle;
	strcpy(sTitle,"XXX��˾����ԭʼ���ϱ�");
	CExcelOper::SetMainTitle(excel_sheet,"A1","Q1",sTitle);	
	//�ڶ���
	//����
	map.SetValueAt(index,0,COleVariant("����"));
	strcpy(cell_start,"B2");
	strcpy(cell_end,"F2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
	map.SetValueAt(index,1,COleVariant("XT")); 
	// ��������
	map.SetValueAt(index,6,COleVariant("��������"));
	// ��д������������
	strcpy(cell_start,"H2");
	strcpy(cell_end,"K2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);
	map.SetValueAt(index,7,COleVariant(bom.m_sPrjName));
	// ��д������
	map.SetValueAt(index,11,COleVariant("����"));
	map.SetValueAt(index,12,COleVariant((long)bom.m_nTaNum));
	// ��д��˨��������
	strcpy(cell_start,"O2");
	strcpy(cell_end,"R2");
	CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end); 
	map.SetValueAt(index,14,COleVariant("��˨������"));
	index++;
	//�ڶ�������������У���Ҫ��ʽ���
	CStringArray str_arr;
	str_arr.SetSize(18);
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"A3","B3");
	map.SetValueAt(index,2,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"C3","C3");
	map.SetValueAt(index,3,COleVariant("������"));
	map.SetValueAt(index,4,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"E3",NULL);
	map.SetValueAt(index,5,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"F3",NULL);
	map.SetValueAt(index,6,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,7.0,"G3",NULL);
	map.SetValueAt(index,7,COleVariant("������"));
	map.SetValueAt(index,8,COleVariant("�ܼ���"));
	map.SetValueAt(index,9,COleVariant("������"));
	map.SetValueAt(index,10,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.5,"H3","K3");
	map.SetValueAt(index,11,COleVariant("���⹤��"));
	CExcelOper::SetRangeColWidth(excel_sheet,13,"L3",NULL);
	map.SetValueAt(index,12,COleVariant("����߳�"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.2,"M3",NULL);
	map.SetValueAt(index,13,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,4.0,"N3",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,5,"O2","R2");
	map.SetValueAt(index,14,COleVariant("M16"));
	map.SetValueAt(index,15,COleVariant("M20"));
	map.SetValueAt(index,16,COleVariant("M24"));
	map.SetValueAt(index,17,COleVariant("M_X"));
	//�����п�
	CExcelOper::SetRangeColWidth(excel_sheet,2.5,"O3","R3");
	index++;
	//�ϲ���Ԫ��
	if(BomArr.GetSize()<=0)
		return FALSE;
	int nTempIndex=index;
	ARRAY_LIST<int> indexList;
	SUPERLIST_NODE<BOMPART> *pNode=BomArr.GetFirst();
	BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
	SEGI preSeg(pBomPart->iSeg);
	indexList.append(index);
	index++;
	for(pNode=BomArr.GetNext();pNode;pNode=BomArr.GetNext())
	{
		pBomPart=(BOMPART*)pNode->pDataObj;
		if (preSeg.iSeg!=pBomPart->iSeg)
		{
			indexList.append(index);
			preSeg.iSeg=pBomPart->iSeg;
		}
		index++;
	}
	if(indexList.GetSize()>0)
		indexList.append(index);
	if(indexList.GetSize()!=1)
	{
		int * pPreIndex=indexList.GetFirst();
		for (int *pIndex=indexList.GetNext();pIndex;pIndex=indexList.GetNext())
		{
			char cell_start[16]="";
			char cell_end[16]="";
			_snprintf(cell_start,15,"A%d",(*pPreIndex)+2);
			_snprintf(cell_end,15,"A%d",(*pIndex)+1);
			(*pPreIndex)=(*pIndex);
			//�ϲ���
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);
		}
	}
	index=nTempIndex;
	for(pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));	//���
		map.SetValueAt(index,2,COleVariant(pBomPart->sSpec));
		//������
		//��������
		double real_weight=0,min_rect_weight=0;
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
			real_weight=ftol(CalSteelPlankWei(pPlate->real_area*pPlate->thick)*100)/100.0;
			min_rect_weight=ftol(CalSteelPlankWei(pPlate->min_area*pPlate->thick)*100)/100.0;
		}
		else
			min_rect_weight=real_weight=ftol(pBomPart->fPieceWeight*100)/100.0;
		map.SetValueAt(index,4,COleVariant(real_weight));

		//����
		map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));
		//����
		map.SetValueAt(index,6,COleVariant(CXhChar16("%.1f",pBomPart->length)));
		//������
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));
		//�ܼ���
		map.SetValueAt(index,8,COleVariant(bom.m_nTaNum*pBomPart->GetPartNum()));
		//������
		map.SetValueAt(index,9,COleVariant(CXhChar16(min_rect_weight)));
		//����
		if(strcmp(pBomPart->sPartNo,"207")==0)
		{
			int i=0;
			i++;
		}
		CXhChar50 str="";
		_snprintf(str,15,"=E%d*I%d",index+2,index+2);
		map.SetValueAt(index,10,COleVariant(str));
		char ss[200]="";
		GetProcessNotes(pBomPart,ss);
		//���⹤��
		map.SetValueAt(index,11,COleVariant(ss));
		//����߳�
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			CString st="";
			st.Format("%.1f",((PART_PLATE*)pBomPart)->fWeldEdgeLen);
			map.SetValueAt(index,12,COleVariant(st));
			//����
			map.SetValueAt(index,13,COleVariant(((PART_PLATE*)pBomPart)->GetEdgeCount()));
		}
		else
		{
			map.SetValueAt(index,12,COleVariant((long)0));
			map.SetValueAt(index,13,COleVariant((long)0));
		}
		//M16
		if(pBomPart->nM16Ls!=0)
			map.SetValueAt(index,14,COleVariant(pBomPart->nM16Ls));
		//M20
		if(pBomPart->nM20Ls!=0)
			map.SetValueAt(index,15,COleVariant(pBomPart->nM20Ls));
		//M14
		if(pBomPart->nM24Ls!=0)
			map.SetValueAt(index,16,COleVariant(pBomPart->nM24Ls));
		//M_X
		if(pBomPart->nMXLs!=0)
			map.SetValueAt(index,17,COleVariant(pBomPart->nMXLs));
		index++;
	}
	//�����Ҷ���
	sprintf(cell_end,"R%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"B2",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	
	return 1;
}