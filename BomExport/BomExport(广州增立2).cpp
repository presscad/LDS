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
//////////////////////////////////////////////////////////////////////////
//��˨����
class CBoltRecord{
public:
	short d,L;
	float grade;
	long nNoThreadlen;	//�޿۳�
	CXhChar50 sName;	//��˨ϵ������
	CXhChar50 sNote;	//װ�䱸ע
	//
	struct SEG_LS_ITEM{
		long iSeg;
		long nCount;
	};
	ATOM_LIST<SEG_LS_ITEM> m_xSegLsList;
public:
	CBoltRecord(){d=L=0;nNoThreadlen=0;}
	~CBoltRecord(){;}
	//
	void Clone(CBoltRecord* pSrcLsRec)
	{
		sName=pSrcLsRec->sName;
		sNote=pSrcLsRec->sNote;
		d=pSrcLsRec->d;
		L=pSrcLsRec->L;
		grade=pSrcLsRec->grade;
		nNoThreadlen=pSrcLsRec->nNoThreadlen;
		//
		m_xSegLsList.Empty();
		for(int i=0;i<pSrcLsRec->m_xSegLsList.GetNodeNum();i++)
			m_xSegLsList.append(pSrcLsRec->m_xSegLsList[i]);
	}
};
typedef CBoltRecord* BOLTRECPTR;
//
static const BYTE TYPE_COMM_BOLT		=1;
static const BYTE TYPE_SAFE_BOLT		=2;
static const BYTE TYPE_TWOCAP_BOLT		=3;
static const BYTE TYPE_COMM_FOOT_NAIL	=4;
static const BYTE TYPE_SAFE_FOOT_NAIL	=5;
void GetBoltDesc(BYTE ciLsType,CXhChar50& sName,CXhChar50& sNote)
{
	if(ciLsType==TYPE_COMM_BOLT)
	{
		sName.Copy("��ͨ��˨");
		sNote.Copy("��ñ����");
	}
	if(ciLsType==TYPE_SAFE_BOLT)
	{
		sName.Copy("������˨");
		sNote.Copy("��ñ����+����ñ");
	}
	if(ciLsType==3)
	{
		sName.Copy("˫ñ��˨");
		sNote.Copy("˫ñ����");
	}
	if(ciLsType==4)
	{
		sName.Copy("��ͨ�Ŷ�");
		sNote.Copy("˫ñ˫��");
	}
	if(ciLsType==5)
	{
		sName.Copy("�����Ŷ�");
		sNote.Copy("˫ñ˫��+����ñ");
	}
}
static int CompareFunc1(const BOLTRECPTR& item1,const BOLTRECPTR& item2)
{
	if(item1->grade>item2->grade)
		return 1;
	else if(item1->grade<item2->grade)
		return -1;
	else
	{
		if(item1->d>item2->d)
			return 1;
		else if(item1->d<item2->d)
			return -1;
		else
		{
			if(item1->L>item2->L)
				return 1;
			else
				return -1;
		}
	}
}
BOOL ExtractBoltInfo(CModelBOM &bom,const BYTE bolt_series,ARRAY_LIST<CBoltRecord> &xBoltRecArr)
{
	CHashStrList<CBoltRecord> xBoltHash;
	for(int i=0;i<bom.m_nBoltsN;i++)
	{
		BOMBOLT bolt=bom.m_pBoltRecsArr[i];
		if(bolt_series==1&&(bolt.bFootNail||bolt.bTwoCap||bolt.bAntiTheft))
			continue;	//��ͨ��˨
		if(bolt_series==2&&(!bolt.bAntiTheft||bolt.bFootNail||bolt.bTwoCap))	
			continue;	//������˨
		if(bolt_series==3&&(!bolt.bTwoCap||bolt.bFootNail||bolt.bAntiTheft))	
			continue;	//˫ñ��˨
		if(bolt_series==4&&(!bolt.bFootNail||bolt.bTwoCap||bolt.bAntiTheft))	
			continue;	//��ͨ�Ŷ�
		if(bolt_series==5&&(!bolt.bFootNail||!bolt.bAntiTheft||bolt.bTwoCap))
			continue;	//�����Ŷ�
		CXhChar50 sName,sNote;
		GetBoltDesc(bolt_series,sName,sNote);
		CXhChar16 sKey("M%dX%d",bolt.d,bolt.L);
		CBoltRecord* pBoltRec=xBoltHash.GetValue(sKey);
		if(pBoltRec==NULL)
		{	
			pBoltRec=xBoltHash.Add(sKey);
			pBoltRec->d=bolt.d;
			pBoltRec->L=bolt.L;
			pBoltRec->sName=sName;
			pBoltRec->sNote=sNote;
			pBoltRec->grade=bolt.grade;
			pBoltRec->nNoThreadlen=bolt.nNoThreadlen;
		}
		CBoltRecord::SEG_LS_ITEM* pSegLsItem=pBoltRec->m_xSegLsList.append();
		pSegLsItem->iSeg=bolt.iSeg;
		pSegLsItem->nCount=bolt.GetPartNum();
	}
	//������˨ֱ���ͳ��Ƚ�������
	ARRAY_LIST<BOLTRECPTR> boltRecPtrList(0,xBoltHash.GetNodeNum());
	for(CBoltRecord* pBoltRec=xBoltHash.GetFirst();pBoltRec;pBoltRec=xBoltHash.GetNext())
		boltRecPtrList.append(pBoltRec);
	CQuickSort<BOLTRECPTR>::QuickSort(boltRecPtrList.m_pData,boltRecPtrList.GetSize(),CompareFunc1);
	if(boltRecPtrList.GetSize()>0)
	{
		xBoltRecArr.Empty();
		xBoltRecArr.SetSize(0,xBoltHash.GetNodeNum());
		for(int i=0;i<boltRecPtrList.GetSize();i++)
		{
			CBoltRecord* pLsRec=xBoltRecArr.append();
			pLsRec->Clone(boltRecPtrList[i]);
		}
		return TRUE;
	}
	return FALSE;
}
void AddBoltTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	CHashList<SEGI> segTable;
	int nSegNum=GetSegNoHashTblBySegStr(bom.m_sSegStr,segTable);
	//
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName("��˨��");
	//��ӱ���
	CHashStrList<int>	hashSegList;
	int nCount=nSegNum>0?nSegNum+5:6;
	CXhChar16 sCell("%C1",'A'+nCount-1);
	CVariant2dArray map(bom.m_nBoltsN+2,nCount);
	map.SetValueAt(0,0,COleVariant("����"));
	map.SetValueAt(0,1,COleVariant(bom.m_sTowerTypeName));
	CExcelOper::MergeColRange(excel_sheet,"B1","D1");
	CExcelOper::SetRangeColWidth(excel_sheet,8,"A1","D1");
	map.SetValueAt(0,4,COleVariant("����"));
	CExcelOper::MergeRowRange(excel_sheet,"E1","E2");
	CExcelOper::SetRangeColWidth(excel_sheet,20,"E1");
	map.SetValueAt(0,5,COleVariant("��μ�����"));
	CExcelOper::MergeRowRange(excel_sheet,"F1",sCell);
	CExcelOper::SetRangeColWidth(excel_sheet,4,"F1",sCell);
	map.SetValueAt(1,0,COleVariant("����"));
	map.SetValueAt(1,1,COleVariant("�����ȼ�"));
	map.SetValueAt(1,2,COleVariant("���"));
	map.SetValueAt(1,3,COleVariant("�޿�"));
	int index=0;
	for(SEGI* pSeg=segTable.GetFirst();pSeg;pSeg=segTable.GetNext())
	{
		int iCol=index+5;
		map.SetValueAt(1,iCol,COleVariant(pSeg->ToString()));
		hashSegList.SetValue(pSeg->ToString(),iCol);
		index++;
	}
	//�������
	int iRow=2;
	ARRAY_LIST<CBoltRecord> xBoltRecArr;
	for(int i=1;i<6;i++)
	{
		if(!ExtractBoltInfo(bom,i,xBoltRecArr))
			continue;
		for(CBoltRecord* pLsRec=xBoltRecArr.GetFirst();pLsRec;pLsRec=xBoltRecArr.GetNext())
		{
			map.SetValueAt(iRow,0,COleVariant(pLsRec->sName));
			map.SetValueAt(iRow,1,COleVariant(CXhChar16("%.1f",pLsRec->grade)));
			map.SetValueAt(iRow,2,COleVariant(CXhChar16("M%dX%d",pLsRec->d,pLsRec->L)));
			map.SetValueAt(iRow,3,COleVariant(pLsRec->nNoThreadlen));
			map.SetValueAt(iRow,4,COleVariant(pLsRec->sNote));
			for(CBoltRecord::SEG_LS_ITEM* pSegLsItem=pLsRec->m_xSegLsList.GetFirst();pSegLsItem;pSegLsItem=pLsRec->m_xSegLsList.GetNext())
			{
				int* pValue=hashSegList.GetValue(SEGI(pSegLsItem->iSeg).ToString());
				if(pValue)
					map.SetValueAt(iRow,*pValue,COleVariant(pSegLsItem->nCount));
			}
			iRow++;
		}
	}
	CXhChar16 cell_end("%C%d",'A'+nCount-1,iRow);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);	//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)3));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�
	//
	excel_sheet.ReleaseDispatch();
}
//////////////////////////////////////////////////////////////////////////
//��������
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
		str.Printf("L%s",sSpec);
	}
	else if(pBomPart->cPartType==BOMPART::PLATE&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_WATER
		&&pBomPart->siSubType!=BOMPART::SUB_TYPE_PLATE_FL&&pBomPart->siSubType!=BOMPART::SUB_TYPE_FOOT_FL)
		str.Printf("-%.f",pBomPart->thick);
	else 
		str.Printf("%s",(char*)pBomPart->sSpec);
	return str;
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
	excel_sheet.SetName("����ԭʼ���ϱ�");	
	//���ʵʱ���ݱ���
	CVariant2dArray map(bom.m_nPartsN+1,19);
	int index=0;
	map.SetValueAt(index,0,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"A1");
	map.SetValueAt(index,1,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,8.0,"B1");
	map.SetValueAt(index,2,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,5.0,"C1");
	map.SetValueAt(index,3,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"D1");
	map.SetValueAt(index,4,COleVariant("���"));
	CExcelOper::SetRangeColWidth(excel_sheet,8,"E1");
	map.SetValueAt(index,5,COleVariant("����"));
	CExcelOper::SetRangeColWidth(excel_sheet,7,"F1");
	map.SetValueAt(index,6,COleVariant("������"));
	CExcelOper::SetRangeColWidth(excel_sheet,6,"G1");
	map.SetValueAt(index,7,COleVariant("��ע"));
	CExcelOper::SetRangeColWidth(excel_sheet,20,"H1");
	//���ʵʱ��������
	index+=1;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//����
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//�������
		map.SetValueAt(index,2,COleVariant(QuerySteelMatMark(pBomPart->cMaterial)));	//����
		map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));						//��ƹ��
		map.SetValueAt(index,4,COleVariant(""));	//���
		if(pBomPart->cPartType==BOMPART::PLATE)
			map.SetValueAt(index,4,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//����
		map.SetValueAt(index,6,COleVariant(pBomPart->GetPartNum()));					//��������
		map.SetValueAt(index,7,COleVariant(GetProcessParam(pBomPart)));					//����Ҫ��
		//
		index++;
	}
	CXhChar16 cell_end("H%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�������С
	//
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
	LPDISPATCH pWorksheets=NULL,pWorksheet=NULL;
	pWorksheets=CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//1 ������ϸ��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 1));
	AddPartTblSheet(bom,pWorksheet);
	//2 ��˨��
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddBoltTblSheet(bom,pWorksheet);
	//	
	excel_sheets.ReleaseDispatch();
	return 0;
}