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
	return 9;
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
		sprintf(sSpec,"%sX%sX%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%sX%s",sWide,sThick);
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
		str.Printf("L%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
}
struct WELD_RELATION{
	CXhChar16 sKeyPartNo;
	struct WELD_SON_PART{
		int index;
		CXhChar16 sPartNo;
		WELD_SON_PART(){index=0;}
	};
	ATOM_LIST<WELD_SON_PART> m_xWeldSonPartArr;
public:
	int GetCount(){return m_xWeldSonPartArr.GetNodeNum();}
	void Append(const char* sPartNo,int iNO)
	{
		WELD_SON_PART* pWeldSonPart=NULL;
		for(pWeldSonPart=m_xWeldSonPartArr.GetFirst();pWeldSonPart;pWeldSonPart=m_xWeldSonPartArr.GetNext())
		{
			if(pWeldSonPart->sPartNo.Equal(sPartNo))
				break;
		}
		if(pWeldSonPart==NULL)
		{
			pWeldSonPart=m_xWeldSonPartArr.append();
			pWeldSonPart->sPartNo.Copy(sPartNo);
			pWeldSonPart->index=iNO;
		}
	}
	int GetWeldPartIndex(const char* sPartNo)
	{
		for(WELD_SON_PART* pWeldPart=m_xWeldSonPartArr.GetFirst();pWeldPart;pWeldPart=m_xWeldSonPartArr.GetNext())
		{
			if(pWeldPart->sPartNo.Equal(sPartNo))
				return pWeldPart->index;
		}
		return 0;
	}
};
void InitWeldPartRelation(ARRAY_LIST<SUPERLIST_NODE<BOMPART>> &BomArr,CHashStrList<WELD_RELATION>& xWeldRelHash)
{
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->cPartType!=BOMPART::PLATE)
			continue;
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(!pVertex->data.m_bWeldEdge || pVertex->data.sWeldMotherPart.GetLength()<=0)
				continue;
			WELD_RELATION* pWeldRel=xWeldRelHash.GetValue(pVertex->data.sWeldMotherPart);
			if(pWeldRel==NULL)
			{
				pWeldRel=xWeldRelHash.Add(pVertex->data.sWeldMotherPart);
				pWeldRel->sKeyPartNo=pVertex->data.sWeldMotherPart;
				pWeldRel->Append(pVertex->data.sWeldMotherPart,1);
			}
			pWeldRel->Append(pPlate->sPartNo,pWeldRel->GetCount()+1);
		}
	}
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart,CHashStrList<WELD_RELATION>& xWeldRelHash)
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
				sDianBan.Copy("��17.5");
			else if(pPlate->m_arrBoltRecs[0].d==20)
				sDianBan.Copy("��21.5");
			else if(pPlate->m_arrBoltRecs[0].d==24)
				sDianBan.Copy("��25.5");
			sProcessParam.Append(CXhChar16("���%s",(char*)sDianBan));
			//�п׵�岻��Ҫ�ٽ��к���
			if(pPlate->m_arrBoltRecs.GetSize()>0)
				pBomPart->bWeldPart=FALSE;
		}
		if(pBomPart->bWeldPart)
			sProcessParam.Append("�纸",' ');
		if(pPlate->bNeedFillet)
			sProcessParam.Append("�¿�",' ');
		//����
		if(pPlate->m_cFaceN>1)
			sProcessParam.Append("����",' ');
		//���
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge&&pPlate->m_cFaceN==1)
			{
				sProcessParam.Append("����",' ');
				break;
			}
		}
		//�����ӹ���
		CXhChar100 sWeldRel;
		WELD_RELATION* pWeldRel=xWeldRelHash.GetValue(pPlate->sPartNo);
		if(pWeldRel)
			sWeldRel.Printf("%s-%d-1",(char*)pPlate->sPartNo,pWeldRel->GetCount());
		//
		CHashStrList<BOOL> hashVertexState;
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(!pVertex->data.m_bWeldEdge||pVertex->data.sWeldMotherPart.GetLength()<=0)
				continue;
			pWeldRel=xWeldRelHash.GetValue(pVertex->data.sWeldMotherPart);
			if(pWeldRel && hashVertexState.GetValue(pVertex->data.sWeldMotherPart)==NULL)
			{
				sWeldRel.Append(CXhChar50("%s-%d-%d",(char*)pVertex->data.sWeldMotherPart,pWeldRel->GetCount(),pWeldRel->GetWeldPartIndex(pPlate->sPartNo)),' ');
				hashVertexState.SetValue(pVertex->data.sWeldMotherPart,TRUE);
			}
		}
		if(sWeldRel.GetLength()>0)
			sProcessParam.Append(sWeldRel,' ');
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		//����
		if(pBomPart->bWeldPart)
			sProcessParam.Append("�纸",' ');
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
			sProcessParam.Append("���",' ');
		if(pBomAngle->bKaiJiao)
			sProcessParam.Append("����",' ');
		if(pBomAngle->bHeJiao) 
			sProcessParam.Append("�Ͻ�",' ');
		if(pBomAngle->bCutAngle)
			sProcessParam.Append("�н�",' ');
		if(pBomAngle->bCutBer)
			sProcessParam.Append("����",' ');
		if(pBomAngle->bCutRoot)
			sProcessParam.Append("���",' ');
		if(pBomAngle->bHasFootNail)
			sProcessParam.Append("����",' ');
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
	CHashStrList<WELD_RELATION> xWeldRelHash;
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
	//��ʼ���ְ�֮��ĺ��ӹ�ϵ
	InitWeldPartRelation(BomArr,xWeldRelHash);
	//��ȡϵͳ��ǰʱ��
	CXhChar100 timeStr;
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	timeStr.Printf("%d.%02d.%02d",st.wYear,st.wMonth,st.wDay);
	//
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet,FALSE);
	excel_sheet.SetName("������ϸ��");	
	//���ʵʱ���ݱ���
	CVariant2dArray map(bom.m_nPartsN+1,19);
	int index=0;
	map.SetValueAt(index,0,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,9,"A1");
	map.SetValueAt(index,1,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("�������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.5,"C1");
	map.SetValueAt(index,3,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"D1");
	map.SetValueAt(index,4,COleVariant("��ƹ��"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"E1");
	map.SetValueAt(index,5,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"F1");
	map.SetValueAt(index,6,COleVariant("B"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"G1");
	map.SetValueAt(index,7,COleVariant("D"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"H1");
	map.SetValueAt(index,8,COleVariant("L"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"I1");
	map.SetValueAt(index,9,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"J1");
	map.SetValueAt(index,10,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"K1");
	map.SetValueAt(index,11,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"L1");
	map.SetValueAt(index,12,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"M1");
	map.SetValueAt(index,13,COleVariant("С������"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"N1");
	map.SetValueAt(index,14,COleVariant("����Ҫ��"));
	CExcelOper::SetRangeColWidth(excel_sheet,16,"O1");
	map.SetValueAt(index,15,COleVariant("��������"));
	CExcelOper::SetRangeColWidth(excel_sheet,11,"P1");
	map.SetValueAt(index,16,COleVariant("��ѹ�ȼ�"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"Q1");
	//���ʵʱ��������
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(bom.m_sTowerTypeName));			//����
		map.SetValueAt(index,1,COleVariant(SEGI(pBomPart->iSeg).ToString()));//����
		map.SetValueAt(index,2,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//�������
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,3,COleVariant("���"));					//��������
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));						//��ƹ��
		if(pBomPart->cMaterial=='S')
			map.SetValueAt(index,5,COleVariant("A3"));	//����
		else
			map.SetValueAt(index,5,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//����
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->thick))));			//B
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));			//D
			map.SetValueAt(index,8,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));		//L
			map.SetValueAt(index,9,COleVariant(CXhChar16("%dx%d",ftol(pBomPart->wide),ftol(pBomPart->length))));	//����
		}
		else if(pBomPart->cPartType==BOMPART::ANGLE)
		{
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));		//B
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->thick))));		//D
			map.SetValueAt(index,8,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//L
			map.SetValueAt(index,9,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//����
		}
		map.SetValueAt(index,10,COleVariant(pBomPart->GetPartNum()));					//��������
		map.SetValueAt(index,11,COleVariant(pBomPart->nMSumLs));						//��������
		map.SetValueAt(index,12,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//��������
		CXhChar16 sFormula("=K%d*M%d",index+1,index+1);
		map.SetValueAt(index,13,COleVariant(sFormula));									//ͨ����ʽ��������
		map.SetValueAt(index,14,COleVariant(GetProcessParam(pBomPart,xWeldRelHash)));	//����Ҫ��
		map.SetValueAt(index,15,COleVariant(timeStr));									//��������
		map.SetValueAt(index,16,COleVariant(bom.m_sPrjName));							//��ѹ�ȼ�
		//
		index++;
	}
	CXhChar16 cell_end("Q%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�������С
	//
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 0;
}