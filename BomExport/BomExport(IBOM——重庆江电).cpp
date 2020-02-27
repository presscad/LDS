// BomExport.cpp : ���� DLL �ĳ�ʼ�����̡�
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

const int COL_COUNT=6;
//�����Ŵ�1��ʼ��0��ʾû�ж�Ӧ��
TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex)
{
	if(colIndexArr==NULL)
		return COL_COUNT;
	else
	{
		if(startRowIndex!=NULL)
			*startRowIndex=6;
		colIndexArr[0]=2;	//����
		colIndexArr[1]=4;	//���
		colIndexArr[2]=5;	//����
		colIndexArr[3]=8;	//����
		colIndexArr[4]=18;	//����
		colIndexArr[5]=6;	//����
		return 6;
	}
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
		sprintf(sSpec,"%s*%s*%s",sWide,sWideY,sThick);
	}
	else 
		sprintf(sSpec,"%s*%s",sWide,sThick);
	if(simple_spec)
		strcpy(simple_spec,sSpec);
}
static CXhChar50 GetSpecStr(BOMPART* pBomPart,BOOL bNeedPlateW=FALSE)
{
	CXhChar50 sSpec;
	if(pBomPart->cPartType==BOMPART::ANGLE)	
		GetAngleSimpleSpec(sSpec,pBomPart->wide,pBomPart->wingWideY,pBomPart->thick);
	else if(pBomPart->cPartType==BOMPART::TUBE)
		sSpec.Printf("%s*%s",(char*)CXhChar16(pBomPart->wide),(char*)CXhChar16(pBomPart->thick));
	else if(pBomPart->cPartType==BOMPART::SLOT)
	{
		sSpec.Copy(pBomPart->sSpec);
		sSpec.Remove('[');
		sSpec.Remove('C');
	}
	else if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==0)
		{
			if(bNeedPlateW)
				sSpec.Printf("%.f*%.f",pBomPart->thick,pBomPart->wide);
			else
				sSpec.Printf("%.f",pBomPart->thick);
		}
		else
			sSpec.Copy("");
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
		if(pPlate->siSubType==BOMPART::SUB_TYPE_BOLT_PAD&&pPlate->m_arrBoltRecs.GetSize()==1)
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

static CXhChar16 GetPartTypeName(BOMPART *pPart,BOOL bSymbol/*=FALSE*/)
{
	CXhChar16 sPartType;
	if(pPart->cPartType==BOMPART::PLATE)		//�ְ�
	{
		sPartType=bSymbol?"-":"�ְ�";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
			sPartType=bSymbol?"":"���Ͳ��";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U)
			sPartType=bSymbol?"":"U�Ͳ��";
		if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
			sPartType=bSymbol?"":"ʮ�ֲ��";
		if( pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FL||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLG||
			pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLR)
			sPartType=bSymbol?"":"����";
	}
	else if(pPart->cPartType==BOMPART::ANGLE)	//�Ǹ�
		sPartType=bSymbol?"L":"�Ǹ�";
	else if(pPart->cPartType==BOMPART::TUBE)	//�ֹ�
		sPartType=bSymbol?"��":"�ֹ�";
	else if(pPart->cPartType==BOMPART::SLOT)	//�۸�
		sPartType=bSymbol?"[":"�۸�";
	else if(pPart->cPartType==BOMPART::FLAT)	//���
		sPartType=bSymbol?"-":"���";
	else if(pPart->cPartType==BOMPART::ROUND)	//Բ��
		sPartType=bSymbol?"��":"Բ��";
	else if(pPart->cPartType==BOMPART::ACCESSORY)
	{
		if(pPart->siSubType==BOMPART::SUB_TYPE_LADDER)
			sPartType=bSymbol?" ":"����";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			sPartType=bSymbol?"-":"�ָ�դ";
		else if(pPart->siSubType==BOMPART::SUB_TYPE_STAY_ROPE)
			sPartType=bSymbol?" ":"��������";
		else
			sPartType=bSymbol?" ":"����";
	}
	return sPartType;
}

static char GetPartMaterialChar(BOMPART *pBomPart)
{
	if(pBomPart==NULL)
		return '0';
	char cMaterial=pBomPart->cMaterial;
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=9)?pBomPart->sPartNo.At(8):'0';
			//if(cMaterial=='D')
			//	cMaterial='P';
		}
		else if(pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
			pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
		{
			cMaterial=(strlen(pBomPart->sPartNo)>=6)?pBomPart->sPartNo.At(5):'0';
		}
		else if(pBomPart->siSubType!=0)
		{
			cMaterial='0';
		}
		char cRealMat=cMaterial;
		if(cRealMat=='D')
			cRealMat='P';
		if(QuerySteelMatMark(cMaterial).GetLength()<=0)
			cMaterial='0';
	}
	return cMaterial;
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
	//���ʵʱ���ݵ��б���
	int COL_COUNT=10;
	double col_arr[10]={8.0,12.0,10.0,18.0,10.0,
						8.0,10.0,10.0,10.5,10.0};
	CStringArray str_arr;
	str_arr.SetSize(COL_COUNT);
	str_arr[0]="����";str_arr[1]="�������";str_arr[2]="��������";str_arr[3]="���";str_arr[4]="����";
	str_arr[5]="��������";str_arr[6]="����";str_arr[7]="���";str_arr[8]="����";str_arr[9]="��ע";
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+2,COL_COUNT);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		CXhChar50 sSpecialSpec; 
		SEGI segI=SEGI(pBomPart->iSeg);
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			if( pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_U||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_X||
				pBomPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
			{
				sSpecialSpec.Copy(pBomPart->sPartNo);
				if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
					pBomPart->iSeg=0;
			}
		}
		else if(pBomPart->cPartType==BOMPART::ACCESSORY||
				pBomPart->cPartType==BOMPART::SUB_TYPE_STAY_ROPE)	//�������ù�������
		{
			sSpecialSpec.Copy(pBomPart->sPartNo);
			if(segI.Prefix().GetLength()>0&&sSpecialSpec.StartWith(segI.key.prefix[0]))
				pBomPart->iSeg=0;
			//if(pBomPart->siSubType==BOMPART::SUB_TYPE_STEEL_GRATING)
			//	continue;	//�ָ�դ������һ�ű�
		}
		if(pBomPart->iSeg!=0)
			map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));//�κ�
		if(sSpecialSpec.GetLength()<=0)
			map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		map.SetValueAt(index,2,COleVariant(GetPartTypeName(pBomPart,TRUE)));	//����
		if(sSpecialSpec.GetLength()>0)
			map.SetValueAt(index,3,COleVariant(sSpecialSpec));					//��ƹ��
		else
			map.SetValueAt(index,3,COleVariant(GetSpecStr(pBomPart)));			//��ƹ��
		char cMaterial=GetPartMaterialChar(pBomPart);
		if(cMaterial!='0')
			map.SetValueAt(index,4,COleVariant(CXhChar16("%c",cMaterial)));		//����
		if(cMaterial=='P'||cMaterial=='D')	//Q420������мӴ�
			CExcelOper::SetBoldFont(excel_sheet,(char*)CXhChar16("E%d",index+2),NULL);	//����Ӵ�
		if(cMaterial=='H')
			CExcelOper::SetRangeBackColor(excel_sheet,CExcelOper::COLOR_ID_YELLOW,CXhChar16("E%d",index+2));
		map.SetValueAt(index,5,COleVariant(pBomPart->GetPartNum()));					//�ӹ�����
		if(pBomPart->length>0)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));//����
		if((pBomPart->cPartType!=BOMPART::ANGLE&&pBomPart->cPartType!=BOMPART::TUBE)&&pBomPart->wide>0)
			map.SetValueAt(index,7,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//���
		if(pBomPart->cPartType==BOMPART::PLATE)	//�ְ忼�Ǿ��������
		{
			double fWeight=((PART_PLATE*)pBomPart)->fPieceNetWeight;
			if(fWeight<=0)
				fWeight=pBomPart->fPieceWeight;
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",fWeight)));				//��������
		}
		else
			map.SetValueAt(index,8,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//��������
		index++;
	}
	CXhChar16 cell_end("J%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);				//���������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(16.0));	//���ñ߿�������С
	CExcelOper::SetRangeRowHeight(excel_sheet,20.25,"A1",cell_end);
	excel_sheet.ReleaseDispatch();
}

void AddSteelGratingTblSheet(CModelBOM &bom,LPDISPATCH pWorkSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorkSheet,FALSE);
	excel_sheet.SetName("�ָ��");
	excel_sheet.ReleaseDispatch();
	return;
	/*
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
	
	//���ʵʱ���ݵ��б���
	int COL_COUNT=6;
	double col_arr[6]={8.0,12.0,10.0,18.0,10.0,8.0};
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN+2,COL_COUNT);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		if(pBomPart->cPartType!=BOMPART::ACCESSORY&&pBomPart->siSubType!=BOMPART::SUB_TYPE_STEEL_GRATING)
			continue;
		map.SetValueAt(index,0,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		map.SetValueAt(index,1,COleVariant(GetSpecStr(pBomPart)));						//��ƹ��
		map.SetValueAt(index,2,COleVariant(CXhChar16("%d*%d",ftol(pBomPart->wide),ftol(pBomPart->length))));
		map.SetValueAt(index,3,COleVariant(CXhChar16("%d",pBomPart->GetPartNum())));	//����
		map.SetValueAt(index,4,COleVariant(CXhChar16("%.2f",pBomPart->fPieceWeight)));	//����
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.1f",pBomPart->fPieceWeight*pBomPart->GetPartNum())));	//����
		index++;
	}
	if(index>0)
	{
		CXhChar16 cell_end("F%d",index);
		CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);				//���������
		CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
		CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(16.0));	//���ñ߿�������С
		CExcelOper::SetRangeRowHeight(excel_sheet,20.25,"A1",cell_end);
	}
	excel_sheet.ReleaseDispatch();*/
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
	//2.�ָ�դ
	pWorksheet=excel_sheets.GetItem(COleVariant((short) 2));
	AddSteelGratingTblSheet(bom,pWorksheet);
	excel_sheets.ReleaseDispatch();
	return 0;
}