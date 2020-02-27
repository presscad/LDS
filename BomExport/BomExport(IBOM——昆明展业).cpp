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
#include "XhCharString.h"

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
	return 13;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

char* SearchChar(char* srcStr,char ch,bool reverseOrder/*=false*/)
{
	if(!reverseOrder)
		return strchr(srcStr,ch);
	else
	{
		int len=strlen(srcStr);
		for(int i=len-1;i>=0;i--)
		{
			if(srcStr[i]==ch)
				return &srcStr[i];
		}
	}
	return NULL;
}

const int COL_COUNT=7;
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=1;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=3;	//���
		colIndexArr[2]=0;	//����
		colIndexArr[3]=5;	//����
		colIndexArr[4]=6;	//����
		colIndexArr[5]=4;	//����
		return 6;
	}
}

//////////////////////////////////////////////////////////////////////////
//��������
CXhChar16 _QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}

CXhChar100 GetBomPartSpec(BOMPART *pBomPart)
{
	CXhChar100 sSpec,sSpecialPartSpec;
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			if(cMaterial=='D')
				cMaterial='P';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING||
				pBomPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE||
				pBomPart->siSubType==BOMPART::SUB_TYPE_LADDER)
		{
			//const static BYTE SUB_TYPE_STEEL_GRATING= 18;	//�ָ�դ
			//const static BYTE SUB_TYPE_STAY_ROPE	= 19;	//��������
			//const static BYTE SUB_TYPE_LADDER		= 20;	//����
			sSpecialPartSpec.Copy(pBomPart->sPartNo);
			cMaterial='0';
		}
	}
	CXhChar16 materialStr=cMaterial!='0'?_QuerySteelMatMark(cMaterial):"";
	if( pBomPart->sPartNo.EqualNoCase("T2105")||
		pBomPart->sPartNo.EqualNoCase("T2605")||
		pBomPart->sPartNo.EqualNoCase("T3605")||
		pBomPart->sPartNo.EqualNoCase("T3980")||
		pBomPart->sPartNo.EqualNoCase("T5855")||
		pBomPart->sPartNo.EqualNoCase("T6355")||
		pBomPart->cPartType==BOMPART::ACCESSORY)
	{
		materialStr="";
		sSpecialPartSpec.Copy(pBomPart->sPartNo);
	}
	if(pBomPart->cPartType==BOMPART::ANGLE)
		sSpec.Printf("��%.f��%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("��%.f��%.f",pBomPart->wide,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
			sSpec.Printf("-%.f",pBomPart->thick);
		else 
			sSpec.Printf("%s",(char*)pBomPart->sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Replace('C','[');
	}
	else
	{
		sSpec.Copy(pBomPart->sSpec);
		int hits=sSpec.Replace("��"," ");
		hits+=sSpec.Replace("��"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		if(hits==2)
		{
			int D=0,d=0;
			sscanf(sSpec,"%d%d",&D,&d);
			sSpec.Printf("��%d/%d",D,d);
		}
		else 
			sSpec.Copy(pBomPart->sSpec);
	}
	if(sSpecialPartSpec.GetLength()>0)
		return sSpecialPartSpec;
	else if(materialStr.EqualNoCase("Q235"))
		return sSpec;
	else
		return CXhChar100("%s%s",(char*)materialStr,(char*)sSpec);
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
CXhChar200 GetPartNotes(BOMPART *pPart)
{
	CXhChar200 sNotes;
	bool bDrillHole=false;
	if( (pPart->cMaterial=='S'&&pPart->thick>16)||			//���
		(pPart->cMaterial=='H'&&pPart->thick>14)||
		(pPart->cMaterial=='P'&&pPart->thick>12)||
		pPart->cMaterial=='T'||pPart->IsDrillHole())
		bDrillHole=true;
	if(pPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pAngle=(PART_ANGLE*)pPart;
		//���Ŷ�
		if(pAngle->bHasFootNail)
			sNotes.Append("���Ŷ� ");
		//���
		if(bDrillHole)
			sNotes.Append("��� ");
		//�н�
		if(pAngle->IsCutAngleOrWing())				
			sNotes.Append("�н� ");
		//ѹ��
		if(pAngle->nPushFlat==0x05)			//0x01:ʼ��ѹ�� 0x02:�м�ѹ�� 0x04:�ն�ѹ��
			sNotes.Append("����ѹ�� ");
		else if((pAngle->nPushFlat&0x01)||(pAngle->nPushFlat&0x04))
			sNotes.Append("һ��ѹ�� ");
		if(pAngle->nPushFlat&0x02)
			sNotes.Append("�м�ѹ�� ");
		if(pAngle->bCutRoot)			//���
			sNotes.Append("��� ");
		if(pAngle->bCutBer)				//����
			sNotes.Append("���� ");
		if(pAngle->bKaiJiao)			//����
			sNotes.Append("���� ");
		if(pAngle->bHeJiao)				//�Ͻ�
			sNotes.Append("�Ͻ� ");
	}
	else if(pPart->cPartType==BOMPART::PLATE)	
	{	
		PART_PLATE *pPlate=(PART_PLATE*)pPart;
		//���
		if(bDrillHole)
			sNotes.Append("��� ");
		//�¿�
		//if(pPlate->bNeedFillet||pPlate->thick>=8)	
		//	sNotes.Append("�¿� ");
		//���
		int nRollEdgeCount=0;
		for(LIST_NODE<BOMPROFILE_VERTEX>* pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
				nRollEdgeCount++;
		}
		if(nRollEdgeCount>0)
			sNotes.Append("��� ");
		//���
		//�޿�
		int nHoleCount=pPlate->m_arrBoltRecs.GetSize();
		//if(nHoleCount<=0)
		//	sNotes.Append("�޿� ");
		//else 
		if(pPart->siSubType==BOMPART::SUB_TYPE_BOLT_PAD)
		{
			CString str;
			CXhChar50 sNote="���";
			if(pPart->nM16Ls>0)
			{
				str.Format("%d",pPart->nM16Ls);
				sNote+="��17.5*"+str;
			}
			if(pPart->nM20Ls>0)
			{
				str.Format("%d",pPart->nM20Ls);
				sNote+="��21.5*"+str;
			}
			if(pPart->nM24Ls>0)
			{
				str.Format("%d",pPart->nM24Ls);
				sNote+="��25.5*"+str;
			}
			sNotes.Append(sNote);
			sNotes.Append(" ");
		}
	}
	long nHuoquLineCount=pPart->GetHuoquLineCount();
	if(nHuoquLineCount>0)		//����
		sNotes.Append("���� ");
	if(pPart->bWeldPart)		//�Ƿ�纸��
		sNotes.Append("�麸 ");
	return sNotes;
}
void AddPartTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
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
	excel_sheet.Select();
	excel_sheet.SetName("ͼֽ����");
	//CExcelOper::SetRangeColWidth(excel_sheet,12.0,"C1");
	//���ԭ������
	Range excel_usedRange,excel_range;
	excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
	excel_usedRange.Select();
	excel_usedRange.ClearContents();
	
	CXhChar16 cell_start,cell_end;
	int index=0;
	CVariant2dArray map(BomArr.GetSize()+2,17);
	//������
	map.SetValueAt(index,0,COleVariant("�κ�"));
	map.SetValueAt(index,1,COleVariant("���"));
	map.SetValueAt(index,2,COleVariant("����"));
	map.SetValueAt(index,3,COleVariant("���"));
	map.SetValueAt(index,4,COleVariant("����"));
	map.SetValueAt(index,5,COleVariant("������"));
	map.SetValueAt(index,6,COleVariant("���⹤��"));
	index++;
	//�����п�
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"A1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.0,"B1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,6.0,"C1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,18.0,"D1",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,10.5,"E1","F1");
	CExcelOper::SetRangeColWidth(excel_sheet,30.0,"G1",NULL);
	//
	long n=BomArr.GetSize();
	CXhChar16 cell;
	BYTE part_type=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		SEGI segI(pBomPart->iSeg);
		map.SetValueAt(index,0,COleVariant(segI.ToString()));
		CXhChar16 sPartNo(pBomPart->sPartNo);	//pRec->GetPartNo(g_sysPara.m_nMatCharPosType,g_sysPara.m_cMatCharSeparator)
		map.SetValueAt(index,1,COleVariant(sPartNo));
		//����
		char cMat=pBomPart->cMaterial;
		//if('P'==cMat)
		//	cMat='D';
		if(cMat!='S')
			map.SetValueAt(index,2,COleVariant(CXhChar16("%C",cMat)));
		map.SetValueAt(index,3,COleVariant(GetBomPartSpec(pBomPart)));	//���
		//����
		char str[30];
		double length=pBomPart->length;
		if(bom.m_iPlankWeightStatType==2&&pBomPart->cPartType==BOMPART::PLATE)
			length=((PART_PLATE*)pBomPart)->real_area/pBomPart->wide;	//��������ʱӦ���ݸְ徻�������Ƹְ峤��
		if(pBomPart->cPartType==BOMPART::PLATE)
			sprintf(str,"%.f*%.f",pBomPart->wide,length);
		else
			sprintf(str,"%.f",length);
		map.SetValueAt(index,4,COleVariant(str));
		map.SetValueAt(index,5,COleVariant((long)pBomPart->GetPartNum()));	//������
		//CXhChar200 ss =GetPartNotes(pBomPart);
		//strcat(ss,pBomPart->sNotes);
		//map.SetValueAt(index,6,COleVariant(ss));
		index++;
	}
	//���ù�����Ÿ�ʽ
	strcpy(cell_start,"B2");
	sprintf(cell_end,"B%d",index+1);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,cell_start,cell_end,COleVariant("@"));
	//���öκ�ˮƽ���뷽ʽ ���ж��� wht 13-03-11
	strcpy(cell_start,"A1");
	sprintf(cell_end,"A%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,cell_start,cell_end,COleVariant((long)3));
	//���ñ������
	strcpy(cell_start,"A1");
	sprintf(cell_end,"G%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,cell_start,cell_end,map.var);
	//���ñ߿�
	//CExcelOper::SetRangeBorders(excel_sheet,cell_start,cell_end,COleVariant(10.5));
	//
	CExcelOper::SetRangeRowHeight(excel_sheet,18.75,cell_start,cell_end);
	CExcelOper::SetFontSize(excel_sheet,cell_start,cell_end,14.0);
	excel_sheet.ReleaseDispatch();
}

//////////////////////////////////////////////////////////////////////////
//
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
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}