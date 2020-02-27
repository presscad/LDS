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
	return 6;
}
#include "excel9.h"
#include "Bom.h"
#include "ExcelOper.h"
#include "Variant.h"

long ftol(double fVal)
{
	return (long)(fVal+0.5);
}
CXhChar16 QuerySteelMatMark(char cMat)
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
	else //if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}

static bool IsCharInStr(char ch,char* prefix)
{
	if(prefix==NULL)
		return false;
	while(*prefix!='\0'&& ch!=*prefix)
	{
		prefix++;
		continue;
	}
	return (ch==*prefix);
}
bool ParsePartNo(const char* sPartNo,SEGI* pSegI,char* sSerialNo,char* materialPrefix=NULL,char *sSeparator=NULL)
{
	int i;
	int str_len = strlen(sPartNo);
	if(str_len<=0)
		return false;
	int separator = -1;
	SEGI segi;
	if(pSegI==NULL)
		pSegI=&segi;
	pSegI->key.number=0;
	pSegI->key.prefix[0]=pSegI->key.prefix[1]=0;
	int iDigitStart=0;	//����������׸����ֵĳ���λ������
	int iDigitFinal=0;
	if(sSerialNo)
		strcpy(sSerialNo,"");
	for(i=0;i<str_len;i++)
	{
		BOOL bDigit=isdigit((BYTE)sPartNo[i]);	//ʹ��isdigitʱ�����������ת��ΪBYTE��Ϊ����ʱ���׳��쳣 wht 14-09-05
		if(bDigit)
			iDigitFinal=i;
		if(i==0&&!bDigit)
		{
			pSegI->key.prefix[0]=sPartNo[0];
			iDigitStart=1;
		}
		else if(i==1&&!bDigit&&sPartNo[i]!='-'&&sPartNo[i]!='_')
		{
			memcpy(pSegI->key.prefix,sPartNo,2);
			iDigitStart=2;
		}
		else if(separator<0&&(sPartNo[i]=='_' || sPartNo[i]=='-'))
		{
			separator=i;
			if(sSeparator)
				sSeparator[0]=sPartNo[i];
		}
	}
	if(IsCharInStr(pSegI->key.prefix[0],materialPrefix))
		pSegI->key.prefix[0]=pSegI->key.prefix[1];
	if(separator>0)
	{	//�жκ���ˮ�ż�ָ���
		char numStr[6]="";
		int len=min(6,separator-iDigitStart);
		if(len>0)
		{
			memcpy(numStr,&sPartNo[iDigitStart],len);
			for(i=0;i<len;i++)
			{
				if(numStr[i]!='.'&&isdigit((BYTE)numStr[i])==0)	//����ĸ����ʱֱ��ת��Ϊ���ֶκŻ�ʧ�棬��ʱӦ����ĸ֮����Ϊ����ˮ��
				{
					separator=i+iDigitStart-1;
					break;
				}
			}
			pSegI->key.number=atoi(numStr);
		}
		else
			return false;
		if(sSerialNo)
		{
			if(separator+1<str_len)
			{
				memcpy(sSerialNo,&sPartNo[separator+1],str_len-separator);
				sSerialNo[str_len-separator]=0;
			}
			else
				return false;
		}
	}
	else
	{	//�޶κ���ˮ�ż�ָ�����Ĭ��ȡ����λ�������ּ���׺��Ϊ��ˮ��
		char numStr[8]="";
		int len=min(8,iDigitFinal-1-iDigitStart);
		if(len>0)
		{
			memcpy(numStr,&sPartNo[iDigitStart],len);
			pSegI->key.number=atoi(numStr);
		}
		else if(len<0)	
		{	//���FL7A�����ı��(��ˮ������������2)��ȡ
			if(sSerialNo)
			{
				memcpy(sSerialNo,&sPartNo[iDigitStart],str_len-iDigitStart);
				sSerialNo[str_len-iDigitStart]=0;
			}
			return true;
		}
		if(sSerialNo)
		{
			memcpy(sSerialNo,&sPartNo[iDigitFinal-1],str_len-iDigitFinal+1);
			sSerialNo[str_len-iDigitFinal+1]=0;
		}
	}
	return TRUE;
}
short GetKaiHeJiaoN(PART_ANGLE *pAngle)
{
	short siKaiHeJiaoCount=0;
	if(pAngle->bKaiJiao||pAngle->bHeJiao)
	{
		for(LIST_NODE<KAI_HE_JIAO> *pNode=pAngle->listKaihe.EnumFirst();pNode;pNode=pAngle->listKaihe.EnumNext())
			siKaiHeJiaoCount++;
	}
	return siKaiHeJiaoCount;
}

static void  GetProcessNotes(BOMPART* pBomPart,char *notes,bool bIncKaiHeAngle=FALSE)
{
	if(pBomPart->cPartType==BOMPART::ANGLE)
	{
		PART_ANGLE *pBomAngle=(PART_ANGLE*)pBomPart;
		if(((PART_ANGLE*)pBomPart)->bHasFootNail)
			sprintf(notes,"���Ŷ�");
	}
}

int ComparePartNoString(char *str1,char *str2)
{	
	//��������������Ϊ�κź���ˮ��
	SEGI seg1,seg2;
	char serial_str1[16],serial_str2[16];
	char postfix1=0,postfix2=0;
	memset(serial_str1,0,16);
	memset(serial_str2,0,16);
	if(!ParsePartNo(str1,&seg1,serial_str1)||!ParsePartNo(str2,&seg2,serial_str2))
	{
		if(strlen(str1)==strlen(str2))
			return stricmp(str1,str2);
		else
			return strlen(str1)-strlen(str2);
	}

	size_t len1=strlen(serial_str1);
	size_t len2=strlen(serial_str2);
	if(len1>0&&isdigit((BYTE)serial_str1[len1-1])==0)
	{
		postfix1=serial_str1[len1-1];
		serial_str1[len1-1]=0;
		len1--;
	}
	if(len2>0&&isdigit((BYTE)serial_str2[len2-1])==0)
	{
		postfix2=serial_str2[len2-1];
		serial_str2[len2-1]=0;
		len2--;
	}
	//�ȱȶκ�
	if(seg1.Prefix().Length()==2||seg2.Prefix().Length()==2)
	{
		if(seg1.Prefix().Length()>seg2.Prefix().Length())
			return 1;
		else if(seg1.Prefix().Length()<seg2.Prefix().Length())
			return -1;
		int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
		if(comp_seg!=0)
			return comp_seg;
	}
	if(seg1.Digital()>seg2.Digital())
		return 1;
	else if(seg1.Digital()<seg2.Digital())
		return -1;
	int comp_seg=stricmp(seg1.Prefix(),seg2.Prefix());
	if(comp_seg!=0)
		return comp_seg;
	//�κ���ͬ����ˮ��
	int comp_serial=0;
	if(strlen(serial_str1)==strlen(serial_str2))
		comp_serial=stricmp(serial_str1,serial_str2);
	else
		comp_serial=atoi(serial_str1)-atoi(serial_str2);
	if(postfix1==0&&postfix2==0)
		return comp_serial;
	else if(comp_serial==0)	//��ˮ����ͬ�Ⱥ�׺
		return postfix1-postfix2;
	else
		return comp_serial;
}

double CalSteelPlankWei(double volume)
{
	return volume*7.85e-6;
}

double Distance(BOMPROFILE_VERTEX::COORD2D  pos1,BOMPROFILE_VERTEX::COORD2D pos2)
{
	return  sqrt(abs((pos2.x-pos1.x)*(pos2.x-pos1.x))+abs((pos2.y-pos1.y)*(pos2.y-pos1.y)));
}

static BOOL IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick)
{	//Y֫��ȴ���֫���Ҳ�����X֫���
	if(y_wing_wide>=thick&&fabs(x_wing_wide-y_wing_wide)>0)
		return TRUE;
	return FALSE;
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

CXhChar100 GetLsHoleStr(BOMPART* pBomPart, BOOL bIncUnit=TRUE)
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
		/*if(pPlate->bNeedFillet)		//��Ҫ�¿�
		{
			if(sProcessParam.GetLength()>0)
				sProcessParam.Append(",");
			sProcessParam.Append("�¿�");
		}*/
		for(LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.EnumFirst();pNode;pNode=pPlate->listVertex.EnumNext())
		{
			if(pNode->data.m_bRollEdge)
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
				NormalizedVector(len_vec);
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
		if( pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD&&
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("�����¿�",',');
		else if(pTube->startProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD||
			pTube->endProcess.type==PART_TUBE::TUBE_PROCESS::PROCESSTYPE_FLD)
			sProcessParam.Append("һ���¿�",',');
	}
	return sProcessParam;
}

bool IsRectPlate(BOMPART *pBomPart)
{
	if(pBomPart->cPartType==BOMPART::PLATE)
	{
		PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
		if(pPlate->GetEdgeCount()==4)
		{	
			LIST_NODE<BOMPROFILE_VERTEX> *pNode1=pPlate->listVertex.EnumFirst(),
				*pNode2=pPlate->listVertex.EnumNext(),*pNode3=pPlate->listVertex.EnumNext(),
				*pNode4=pPlate->listVertex.EnumNext();
			if(Distance(pNode1->data.vertex,pNode2->data.vertex)-Distance(pNode3->data.vertex,pNode4->data.vertex)==0&&
				Distance(pNode2->data.vertex,pNode3->data.vertex)-Distance(pNode1->data.vertex,pNode4->data.vertex)==0&&
				Distance(pNode1->data.vertex,pNode3->data.vertex)-Distance(pNode2->data.vertex,pNode4->data.vertex)==0)
				return true;
			else
				return false;
		}
	}
	return false;
}

TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag)
{
	CHashStrList<int> hashColourIdByMat;
	hashColourIdByMat.SetValue("Q345",6);
	hashColourIdByMat.SetValue("Q390",34);
	hashColourIdByMat.SetValue("Q420",33);
	hashColourIdByMat.SetValue("Q460",31);
	hashColourIdByMat.SetValue("Q235",2);
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
	ARRAY_LIST<SUPERLIST_NODE<BOMPART>> BomArr;
	for(SUPERLIST_NODE<BOMPART> *pNode=bom.listParts.EnumFirst();pNode;pNode=bom.listParts.EnumNext())
		BomArr.append(*pNode);
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
	CStringArray str_arr;
	//
	str_arr.SetSize(20);
	str_arr[0]="�κ�";str_arr[1]="������";str_arr[2]="����";str_arr[3]="����";
	str_arr[4]="���";str_arr[5]="����";str_arr[6]="���";str_arr[7]="��������";										
	str_arr[8]="���ղ���";str_arr[9]="��ע";str_arr[10]="�������";str_arr[11]="���׾�����";								
	str_arr[12]="���޹��߿�/ʩ����";str_arr[13]="�Ƿ����ŵװ�";str_arr[14]="��������";str_arr[15]="����Ƿ����";
	//str_arr[16]="�Ƿ���";
	str_arr[16]="���ӱ߳�";str_arr[17]="����";str_arr[18]="���ܳ�mm";
	str_arr[19]="������п���";
	/*str_arr[21]="���Ӽ�����";str_arr[22]="�нǴ���";str_arr[23]="�������";
	str_arr[24]="�������";str_arr[25]="ѹ�����";str_arr[26]="����";str_arr[27]="���";
	str_arr[28]="���Ǵ���";str_arr[29]="���Ƕ���";str_arr[30]="�ϽǴ���";str_arr[31]="�ϽǶ���";
	str_arr[32]="�Ƿ񺸽�";str_arr[33]="�Ƿ��ʿ�";str_arr[34]="�ʿڳ���";str_arr[35]="���";
	str_arr[36]="��׿׾�����";*/
	double col_arr[20]={7.0,6.5,5.5,5.5,7.5,
						5.0,5.0,5.0,18.0,15.0,
						10.0,15.0,10.0,12.0,8.5,
						13.0,/*12.0,*/10.0,5.0,5.0,5.0};
						//5.0,5.0,10.0,7.5,5.0,
						//5.0,5.0,5.0,5.0,5.0,
						//5.0,5.0,5.0,5.5,5.0,
						//5.0,15};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//
	CVariant2dArray map(bom.m_nPartsN,20);
	int index=0;
	for(SUPERLIST_NODE<BOMPART>*pNode=BomArr.GetFirst();pNode;pNode=BomArr.GetNext())
	{
		BOMPART* pBomPart=(BOMPART*)pNode->pDataObj;
		map.SetValueAt(index,0,COleVariant(SEGI(pBomPart->iSeg).ToString()));		//�κ�
		map.SetValueAt(index,1,COleVariant(pBomPart->GetPartNo(bom.m_biMatCharPosType,bom.m_cMatCharSeparator)));//���
		CXhChar16 sMat=QuerySteelMatMark(pBomPart->cMaterial);
		map.SetValueAt(index,2,COleVariant(sMat));//����
		CXhChar16 sStart=CExcelOper::GetCellPos(1,index+2);
		CXhChar16 sEnd=CExcelOper::GetCellPos(2,index+2);
		CExcelOper::SetRangeBackColor(excel_sheet,*hashColourIdByMat.GetValue(sMat),(char*)sStart,(char*)sEnd);
		//����
		CXhChar16 sTypeName=pBomPart->GetPartTypeName();
		CXhChar16 sSumTypeName=pBomPart->GetPartSubTypeName();
		if(pBomPart->cPartType==BOMPART::PLATE&&sSumTypeName.GetLength()>0)
			sTypeName.Copy(sSumTypeName);
		map.SetValueAt(index,3,COleVariant(sTypeName));
		//���
		CXhChar100 str;
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
		map.SetValueAt(index,4,COleVariant(str));

		//����
		map.SetValueAt(index,5,COleVariant(CXhChar16("%.f",pBomPart->length)));
		//���
		map.SetValueAt(index,6,COleVariant(""));
		//��������
		map.SetValueAt(index,7,COleVariant(pBomPart->GetPartNum()));
		//���ղ���
		map.SetValueAt(index,8,COleVariant(GetProcessParam(pBomPart)));
		//��ע
		char ss[200]="";
		GetProcessNotes(pBomPart,ss);
		map.SetValueAt(index,9,COleVariant(ss));
		//�������
		
		//���׾�����
		map.SetValueAt(index,11,COleVariant(GetLsHoleStr(pBomPart,FALSE)));
		//���޹��߿�/ʩ����	
		if(pBomPart->cPartType==BOMPART::PLATE||pBomPart->cPartType==BOMPART::ANGLE)
		{
			BOOL bHaveLinearHole=FALSE,bWirHoleOrInventedHole=FALSE;
			for (BOMBOLT_RECORD* pBomBoltRecord=pBomPart->m_arrBoltRecs.GetFirst();
				pBomBoltRecord;pBomBoltRecord=pBomPart->m_arrBoltRecs.GetNext())
			{
				if(bHaveLinearHole&&bWirHoleOrInventedHole)
					break;
				if(!bWirHoleOrInventedHole&&(pBomBoltRecord->cFuncType==BOMBOLT_RECORD::FUNC_WIREHOLE||
					(pBomBoltRecord->cFuncType!=BOMBOLT_RECORD::FUNC_COMMON&&
					pBomBoltRecord->cFuncType!=BOMBOLT_RECORD::FUNC_FOOTNAIL&&
					pBomBoltRecord->cFuncType!=BOMBOLT_RECORD::FUNC_FOOTNAIL2)))
					bWirHoleOrInventedHole=TRUE;
				else if(pBomBoltRecord->waistLen>0&&!bHaveLinearHole)
					bHaveLinearHole=TRUE;
			}
			if(pBomPart->cPartType==BOMPART::PLATE&&((PART_PLATE*)pBomPart)->bTowerFootPlate)
				map.SetValueAt(index,13,COleVariant(long(1)));	//�Ƿ����ŵװ�
			else	//���ŵװ��ϵ���˨��λ���������
				map.SetValueAt(index,12,bWirHoleOrInventedHole?COleVariant(long(1)):COleVariant(""));

			map.SetValueAt(index,14,bHaveLinearHole?COleVariant(long(1)):COleVariant(""));
		}
		if(pBomPart->cPartType==BOMPART::PLATE)
		{
			PART_PLATE* pPlate=(PART_PLATE*)pBomPart;
			//����Ƿ����	
			map.SetValueAt(index,15,IsRectPlate(pBomPart)?COleVariant(long(1)):COleVariant(""));
			//�Ƿ���
			//���ӱ߳�
			if(pPlate->fWeldEdgeLen>0)
				map.SetValueAt(index,16,COleVariant(CXhChar16("%.f",pPlate->fWeldEdgeLen)));
			//����
			map.SetValueAt(index,17,COleVariant(pPlate->GetEdgeCount()));
			//���ܳ�mm	
			if(pPlate->fPerimeter>0)
				map.SetValueAt(index,18,COleVariant(CXhChar16("%.f",pPlate->fPerimeter)));
		}
		//��п���
		double fArea=GetPartZincCoatedArea(pBomPart);
		if(fArea>0)
			map.SetValueAt(index,19,COleVariant(CXhChar16("%0.3f",fArea)));
		index++;
	}
	//�����Ҷ���
	char cell_end[30]={};
	sprintf(cell_end,"T%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)2));
	//���ñ������
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	return 1;
}