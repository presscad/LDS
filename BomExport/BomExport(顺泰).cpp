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

typedef LIST_NODE<BOMPROFILE_VERTEX>* VERTEX_PTR;
//
static const BYTE WELD_LEN	=1;		//���쳤��
static const BYTE EDGE_NUM	=2;		//����
static const BYTE PERIMETER =3;		//�ܳ�
static const BYTE FOOT_PLATE=4;		//���Ű�
static const BYTE RECT_PLATE=5;		//���ΰ�

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
		sprintf(sSpec,"%sx%s*%s",sWide,sWideY,sThick);
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
static CXhChar50 GetProcessNotes(BOMPART* pBomPart)
{
	CXhChar50 sNotes;
	if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bHasFootNail)
			sNotes.Copy("���Ŷ�");
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE* pBomTube=(PART_TUBE*)pBomPart;
		if(pBomTube->bHasFootNail)
			sNotes.Copy("���Ŷ�");
	}
	return sNotes;
}
static CXhChar16 GetPlateData(BOMPART* pBomPart,BYTE biDataType)
{
	CXhChar16 sValue;
	if(biDataType==WELD_LEN&&pBomPart->bWeldPart&&pBomPart->cPartType==BOMPART::SLOT)
	{	//�۸ֽŶ��ĺ��ӳ���
		sValue.Printf("%.1f",pBomPart->length*2);
		return sValue;
	}
	if(pBomPart->cPartType!=BOMPART::PLATE)
		return sValue; 
	PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
	if(pPlate->fWeldEdgeLen>0 && biDataType==WELD_LEN)
		sValue.Printf("%.1f",pPlate->fWeldEdgeLen);
	else if(biDataType==EDGE_NUM)
		sValue.Printf("%d",pPlate->GetEdgeCount());
	else if(pPlate->fPerimeter>0&&biDataType==PERIMETER)
		sValue.Printf("%.1f",pPlate->fPerimeter);
	else if(pPlate->bTowerFootPlate&&biDataType==FOOT_PLATE)
		sValue.Copy("1");
	else if(pPlate->IsRect()&&biDataType==RECT_PLATE)
		sValue.Copy("1");
	return sValue;
}
static CXhChar100 GetLsHoleStr(BOMPART* pBomPart, BOOL bIncUnit=TRUE)
{
	CXhChar100 sHoleStr="";
	if(pBomPart->nMSumLs<=0)
		return "";
	CHashStrList<int> hashLsNumByD;
	for(BOMBOLT_RECORD* pBomboltRecoed=pBomPart->m_arrBoltRecs.GetFirst();pBomboltRecoed;pBomboltRecoed=pBomPart->m_arrBoltRecs.GetNext())
	{
		int* pNum=hashLsNumByD.GetValue(CXhChar50(pBomboltRecoed->d+pBomboltRecoed->hole_d_increment));
		if(pNum==NULL)
			hashLsNumByD.SetValue(CXhChar16(pBomboltRecoed->d+pBomboltRecoed->hole_d_increment),1);
		else
			(*pNum)++;
	}
	CXhChar50 sCurHoleStr="";
	for(int* pNum=hashLsNumByD.GetFirst();pNum;pNum=hashLsNumByD.GetNext())
	{
		if(sCurHoleStr.GetLength()>0)
			sCurHoleStr.Append(",");
		if(bIncUnit)
			sCurHoleStr.Append(CXhChar16("%smm*%d",(char*)hashLsNumByD.GetCursorKey(),*pNum));
		else
			sCurHoleStr.Append(CXhChar16("%s*%d",(char*)hashLsNumByD.GetCursorKey(),*pNum));
	}
	if(sCurHoleStr.GetLength()>0)
		sHoleStr=sCurHoleStr;
	return sHoleStr;
}
#define      DEGTORAD_COEF    57.2957795130823208768
CXhChar100 GetProcessParam(BOMPART* pBomPart)
{
	int nNum=0;
	CXhChar100 sProcessParam="";
	if(pBomPart->bWeldPart)
		sProcessParam.Append("����",',');
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		int nHuoquCount=pPlate->m_cFaceN-1;
		if(nHuoquCount>0)
		{
			double fHuoquAngle1=fabs(pPlate->huoQuAngleArr[0]);
			double fHuoquAngle2=fabs(pPlate->huoQuAngleArr[1]);
			CXhChar50 sAngle("����%d-%0.1f",nHuoquCount,fHuoquAngle1>fHuoquAngle2?fHuoquAngle1:fHuoquAngle2);
			sProcessParam.Append(sAngle,',');
		}
		for(VERTEX_PTR pVertex=pPlate->listVertex.EnumFirst();pVertex;pVertex=pPlate->listVertex.EnumNext())
		{
			if(pVertex->data.m_bRollEdge)
			{
				sProcessParam.Append("���",',');
				break;
			}
		}
	}
	else if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(pBomAngle->bCutAngle)
			sProcessParam.Append(CXhChar16("�н�%d",GetCutAngleSwordNum(pBomAngle)),',');
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
				//NormalizedVector(len_vec);
				double cosa=prev_len_vec.x*len_vec.x+prev_len_vec.y*len_vec.y+prev_len_vec.z*len_vec.z;
				if(cosa<fMinCosa)
					fMinCosa=cosa;
				prev_len_vec.x=len_vec.x;
				prev_len_vec.y=len_vec.y;
				prev_len_vec.z=len_vec.z;
			}
			double fAngle=acos(fMinCosa)*DEGTORAD_COEF;
			sProcessParam.Append(CXhChar16("%s %d-%0.1f",(char*)zhiwanArr[nHuoquType-1],nHuoquLineCount,fAngle),',');
		}
		//ѹ��
		if(pBomAngle->nPushFlat>0)		//�Ƿ�ѹ��
		{
			int nPushFlat=0;
			if(pBomAngle->nPushFlat&0x01)
				nPushFlat++;
			if(pBomAngle->nPushFlat&0x02)
				nPushFlat++;
			if(pBomAngle->nPushFlat&0x04)
				nPushFlat+=2;
			sProcessParam.Append(CXhChar16("ѹ�� %d",nPushFlat),',');
		}
		//���Ͻ�
		if(pBomAngle->bHeJiao||pBomAngle->bKaiJiao)
		{
			int nCount=0;
			for(LIST_NODE<KAI_HE_JIAO> *pNode=pBomAngle->listKaihe.EnumFirst();pNode;pNode=pBomAngle->listKaihe.EnumNext())
			{
				short siLen=pNode->data.endLength+pNode->data.startLength;
				short siCount=siLen/500;
				if(siLen%500>0)
					siCount+=1;
				nCount+=siCount;
			}
			if(pBomAngle->bKaiJiao)
				sProcessParam.Append(CXhChar16("���� %d-%0.1f",nCount,pBomAngle->wing_angle-90),',');
			else 
				sProcessParam.Append(CXhChar16("�Ͻ� %d-%0.1f",nCount,90-pBomAngle->wing_angle),',');
		}
		if(pBomAngle->bCutBer)			//�Ƿ���Ҫ����
			sProcessParam.Append("����",',');
		if(pBomAngle->bCutRoot)		//�Ƿ���Ҫ���
			sProcessParam.Append("���",',');
	}
	else if(pBomPart->cPartType==BOMPART::TUBE)
	{
		PART_TUBE *pTube=(PART_TUBE*)pBomPart;
		BYTE typeS=pTube->startProcess.type,typeE=pTube->endProcess.type;
		if(typeS==typeE)
		{
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				sProcessParam.Append("һ�Ϳ��ڲ�2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				sProcessParam.Append("U�Ϳ��ڲ�2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				sProcessParam.Append("T�Ϳ��ڲ�2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				sProcessParam.Append("X�Ϳ��ڲ�2",',');
			else if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT||
					typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
				sProcessParam.Append("���2",',');
		}
		else
		{
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_CSLOT)
				sProcessParam.Append("һ�Ϳ��ڲ�1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_USLOT)
				sProcessParam.Append("U�Ϳ��ڲ�1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TSLOT)
				sProcessParam.Append("T�Ϳ��ڲ�1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT||typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_XSLOT)
				sProcessParam.Append("X�Ϳ��ڲ�1",',');
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
			{
				if(typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
					sProcessParam.Append("���2",',');
				else
					sProcessParam.Append("���1",',');
			}
			if(typeS==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_PLANECUT)
			{
				if(typeE==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_TRANSECT)
					sProcessParam.Append("���2",',');
				else
					sProcessParam.Append("���1",',');
			}
		}
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
	excel_sheet.SetName("����ԭʼ���ϱ�");
	//��ӱ�������
	CStringArray str_arr;
	str_arr.SetSize(20);
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="����";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="���";str_arr[7]="��������";										
	str_arr[8]="���ղ���";str_arr[9]="��ע";str_arr[10]="�������";str_arr[11]="���׾�����";								
	str_arr[12]="���޹��߿�/ʩ����";str_arr[13]="�Ƿ����ŵװ�";str_arr[14]="��������";str_arr[15]="����Ƿ����";
	str_arr[16]="���ӱ߳�";str_arr[17]="����";str_arr[18]="���ܳ�mm";str_arr[19]="������п���";
	double col_arr[20]={7.0,7.0,7.0,8.0,7.5,7.0,7.0,7.0,20.0,10.0,7.5,17.0,8.0,6.0,4.0,6.0,7.5,4.5,7.5,11.0};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1","T1",COleVariant((long)3));	//�б���ˮƽ����
	//���ʵʱ����
	CVariant2dArray map(bom.m_nPartsN,20);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));	//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//������
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);	//����
		map.SetValueAt(index,2,COleVariant(sMat));
		CExcelOper::SetRangeBackColor(excel_sheet,GetMatColorIndex(sMat),CXhChar16("B%d",index+2),CXhChar16("C%d",index+2));
		//����
		CXhChar16 sSumTypeName=pBomPart->GetPartSubTypeName();
		if(pBomPart->cPartType==BOMPART::PLATE&&sSumTypeName.GetLength()>0)
			map.SetValueAt(index,3,COleVariant(sSumTypeName));
		else
			map.SetValueAt(index,3,COleVariant(pBomPart->GetPartTypeName()));
		map.SetValueAt(index,4,COleVariant(GetSpecStr(pBomPart)));					//���
		map.SetValueAt(index,5,COleVariant(CXhChar16("%d",ftol(pBomPart->length))));	//����
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::TUBE)
			map.SetValueAt(index,6,COleVariant(CXhChar16("%d",ftol(pBomPart->wide))));	//���
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));	//��������
		map.SetValueAt(index,8,COleVariant(GetProcessParam(pBomPart)));	//���ղ���
		map.SetValueAt(index,9,COleVariant(GetProcessNotes(pBomPart)));	//��ע
		map.SetValueAt(index,10,COleVariant(""));	//�������
		map.SetValueAt(index,11,COleVariant(GetLsHoleStr(pBomPart,FALSE)));	//���׾�����
		map.SetValueAt(index,12,pBomPart->IsHasSpecFuncHole()?COleVariant(long(1)):COleVariant(""));//���޹��߿�/ʩ����
		map.SetValueAt(index,13,COleVariant(GetPlateData(pBomPart,FOOT_PLATE)));	//�Ƿ����ŵװ�
		map.SetValueAt(index,14,pBomPart->IsHasLinearHole()?COleVariant(long(1)):COleVariant(""));	//��������
		map.SetValueAt(index,15,COleVariant(GetPlateData(pBomPart,RECT_PLATE)));	//����Ƿ����
		map.SetValueAt(index,16,COleVariant(GetPlateData(pBomPart,WELD_LEN)));	//���ӱ߳�
		map.SetValueAt(index,17,COleVariant(GetPlateData(pBomPart,EDGE_NUM)));	//����
		map.SetValueAt(index,18,COleVariant(GetPlateData(pBomPart,PERIMETER)));	//���ܳ�mm
		map.SetValueAt(index,19,COleVariant(CXhChar16("%0.3f",pBomPart->CalZincCoatedArea())));//��п���
		index++;
	}
	CXhChar16 cell_end("T%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);	//���ñ������
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((long)2));//ˮƽ����
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"M2",CXhChar16("P%d",index+1),COleVariant((long)3));//ˮƽ����
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));	//���ñ߿�������С
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return 1;
}