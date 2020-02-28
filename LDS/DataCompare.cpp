#include "StdAfx.h"
#include "DataCompare.h"
#include "LDS.h"
#include "ExcelOper.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "BomTblTitleCfg.h"
#include "ConfigTableTitleDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__TMA_)||defined(__LMA_)

CDataCmpModel globalDataCmpModel;
//Exce����
const int LOFTING_EXCEL_COL_COUNT  =17;
//
static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}
static int ComparePartNo(const CXhChar16& str1,const CXhChar16& str2)
{
	return ComparePartNoString(str1,str2);
}
//////////////////////////////////////////////////////////////////////////
//CDataCmpModel
CDataCmpModel::CDataCmpModel()
{	
}
CDataCmpModel::~CDataCmpModel()
{
}
BOOL CDataCmpModel::Empty()
{
	m_drawingDataList.Empty();
	return TRUE;
}
void CDataCmpModel::ReadFromDrawAndBomBuffer(CBuffer* pBuff)
{
	UINT i,uiVersion=0,uiBomCount=0,uiCount=0;
	pBuff->ReadInteger(&uiVersion);	//ʩ��ͼ��������ϸ����Ϣ�������Ĵ洢��ʽ�汾��
	pBuff->ReadInteger(&uiBomCount);//
	m_drawingDataList.Empty();
	for(i=0;i<uiBomCount;i++)
	{
		CTowerBOM* pDrawBom=m_drawingDataList.append();
		pBuff->ReadString(pDrawBom->m_sBomName);
		pBuff->ReadInteger(&uiCount);
		CXhChar16 szLabel;
		BYTE ciPartType=0;
		for(UINT j=0;j<uiCount;j++)
		{
			pBuff->ReadString(szLabel,17);		//�������
			pBuff->ReadByte(&ciPartType);
			BOMPART* pBomPart=pDrawBom->AddBomPart(szLabel,ciPartType);
			pBomPart->sPartNo=szLabel;
			pBuff->ReadWord(&pBomPart->siSubType);
			pBuff->ReadByte(&pBomPart->cMaterial);	//���ʼ��ַ����磺'S','H','G'
			pBuff->ReadString(pBomPart->sMappingDrwLabel);	//�������ݱȶ�ʱ����ʾ��ǰ������ԭͼֽ��ϸ�е�ƥ���Ӧ�Ĺ������
			pBuff->ReadDouble(&pBomPart->wide);
			pBuff->ReadDouble(&pBomPart->thick);
			pBuff->ReadDouble(&pBomPart->length);		//�������Ȳ���
			pBuff->ReadDouble(&pBomPart->wingWideY);	//�Ǹ�Y֫���(����֫�Ǹ�)
			pBuff->ReadDouble(&pBomPart->fPieceWeight);//������(kg)�����ڸְ��Ϊ��С���ݾ�������
			int nPart=0;
			pBuff->ReadInteger(&nPart);//һ�������о��еĵ�ǰ��ŵ��Σ�������������
			pBomPart->AddPart(nPart);
			pBuff->ReadInteger(&pBomPart->iSeg);	//�κ�
			pBuff->ReadString(pBomPart->sSpec,17);	//���
			pBuff->ReadString(pBomPart->sNotes,50);//������ע˵��
			//short siZhiWan;		//>0�����������<=0����Ҫ����
			//BOOL bWeldPart;		//�Ƿ���Ҫ����
			//BOOL bLegPart;		//�Ƿ�Ϊ�Ȳ�����
			//short nM16Ls;	//M16��˨����
			//short nM20Ls;	//M20��˨����
			//short nM24Ls;	//M24��˨����
			//short nMXLs;	//����ֱ����˨����
			//short nMSumLs;	//��˨������
		}
	}
}
void CDataCmpModel::WriteToDrawAndBomBuffer(CBuffer* pBuff)
{
	pBuff->WriteInteger(1);	//�����ʽ�汾��
	pBuff->WriteInteger(m_drawingDataList.GetNodeNum());
	for(CTowerBOM* pDrawBom=m_drawingDataList.GetFirst();pDrawBom;pDrawBom=m_drawingDataList.GetNext())
	{
		pBuff->WriteString(pDrawBom->m_sBomName);
		pBuff->WriteInteger(pDrawBom->GetPartNum());
		for(BOMPART* pBomPart=pDrawBom->EnumFirstPart();pBomPart;pBomPart=pDrawBom->EnumNextPart())
		{
			pBuff->WriteString(pBomPart->sPartNo,17);		//�������
			pBuff->WriteByte(pBomPart->cPartType);
			pBuff->WriteWord(pBomPart->siSubType);
			pBuff->WriteByte(pBomPart->cMaterial);	//���ʼ��ַ����磺'S','H','G'
			pBuff->WriteString(pBomPart->sMappingDrwLabel);	//�������ݱȶ�ʱ����ʾ��ǰ������ԭͼֽ��ϸ�е�ƥ���Ӧ�Ĺ������
			pBuff->WriteDouble(pBomPart->wide);
			pBuff->WriteDouble(pBomPart->thick);
			pBuff->WriteDouble(pBomPart->length);		//�������Ȳ���
			pBuff->WriteDouble(pBomPart->wingWideY);	//�Ǹ�Y֫���(����֫�Ǹ�)
			pBuff->WriteDouble(pBomPart->fPieceWeight);//������(kg)�����ڸְ��Ϊ��С���ݾ�������
			pBuff->WriteInteger(pBomPart->GetPartNum());//һ�������о��еĵ�ǰ��ŵ��Σ�������������
			pBuff->WriteInteger(pBomPart->iSeg);	//�κ�
			pBuff->WriteString(pBomPart->sSpec,17);	//���
			pBuff->WriteString(pBomPart->sNotes,50);//������ע˵��
			//short siZhiWan;		//>0�����������<=0����Ҫ����
			//BOOL bWeldPart;		//�Ƿ���Ҫ����
			//BOOL bLegPart;		//�Ƿ�Ϊ�Ȳ�����
			//short nM16Ls;	//M16��˨����
			//short nM20Ls;	//M20��˨����
			//short nM24Ls;	//M24��˨����
			//short nMXLs;	//����ֱ����˨����
			//short nMSumLs;	//��˨������
		}
	}
}
BOMPART* CDataCmpModel::GetDrawingBomPart(const char* szPartLabel,BYTE cls_id/*=0*/)	//���ݹ�����Ų��ҵ����ʶ���ͼֽ�����е�BOM��ϸ
{
	BOMPART* pBomPart=NULL;
	for(CTowerBOM *pDrawBom=EnumFirstDrawingData();pDrawBom;pDrawBom=EnumNextDrawingData())
	{
		if((pBomPart=pDrawBom->FromPartNo(szPartLabel))!=NULL)
		{
			if(cls_id==0 || (BYTE)pBomPart->cPartType==cls_id)
				return pBomPart;
		}
	}
	return NULL;
}
CTowerBOM* CDataCmpModel::AppendDrawingDataFromTBOM(const char* sFileName)
{
	FILE* fp=fopen(sFileName,"rb");
	if(fp==NULL)
	{
		logerr.Log("%s�ļ���ʧ��!",sFileName);
		return NULL;
	}
	CBuffer buffer;
	int nVersion=0,nBufSize=0;
	fseek(fp,0,SEEK_SET);
	fread(&nVersion,4,1,fp);
	fread(&nBufSize,4,1,fp);
	buffer.Write(NULL,nBufSize);
	fread(buffer.GetBufferPtr(),nBufSize,1,fp);
	fclose(fp);
	//
	if(buffer.GetLength()>0)
	{
		CModelBOM draw_bom;
		draw_bom.FromBuffer(buffer,nVersion);
		CTowerBOM* pDrawBom=m_drawingDataList.append();
		pDrawBom->CloneFromModelBom(&draw_bom);
		return pDrawBom;
	}
	return NULL;
}
BOOL ParseSpec(BOMPART *pBomPart);
CTowerBOM* CDataCmpModel::AppendDrawingDataFromBOMD(const char* sFileName)
{
	FILE *fp=fopen(sFileName,"rt");
	if(fp==NULL)
	{
		AfxMessageBox("�򲻿�ָ����TXT�ļ�!");
		return NULL;
	}
	CTowerBOM* pDrawBom=m_drawingDataList.append();
	const char *PLACE_HOLDER="NULL";
	CXhChar200 line_txt;
	SEGI validSegI;
	while(!feof(fp))
	{
		if(fgets(line_txt,200,fp)==NULL)
			break;
		line_txt.Replace("%%c","��");
		line_txt.Replace("%%C","��");
		line_txt[strlen(line_txt)-1]='\0';
		char szTokens[] = " \n\t" ;

		SEGI segI;
		CXhChar50 sPartLabel,sSpec;
		//����
		char* sKey=strtok(line_txt,szTokens);
		if(sKey!=NULL&&stricmp(PLACE_HOLDER,sKey)!=0)
			sPartLabel.Copy(sKey);
		//����+���
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&stricmp(PLACE_HOLDER,sKey)!=0)
			sSpec.Copy(sKey);
		if(sPartLabel.GetLength()==0)
		{	//ʹ�ù����Ϊ����
			sPartLabel.Copy(sSpec);
			sSpec.Empty();
			if(validSegI.iSeg>0)
				segI=validSegI;
		}
		else
		{
			if(!ParsePartNo(sPartLabel,&segI,NULL))
				segI.iSeg=0;
			else if(validSegI.iSeg==0)
				validSegI=segI;
		}
		if(sPartLabel.GetLength()<=0)
			continue;
		//����+���
		BOMPART bomPart;
		if(sSpec.GetLength()>0)
		{	
			CXhChar50 sSize(sSpec);
			sSize.Remove(' ');
			//����
			char cFirstChar=sSize.At(0),cTailChar=sSize.At(sSize.GetLength()-1);
			bomPart.cMaterial=CSteelMatLibrary::Q235BriefMark();
			if(strstr(sKey,"Q"))
			{
				CXhChar16 materialStr;
				strncpy(materialStr,sKey,4);
				sSize.Replace(materialStr,"");
				bomPart.cMaterial=QuerySteelBriefMatMark(materialStr);
			}
			else if(CSteelMatLibrary::Q235BriefMark()==cFirstChar||CSteelMatLibrary::Q235BriefMark()==cTailChar)
				bomPart.cMaterial=CSteelMatLibrary::Q235BriefMark();
			else if(CSteelMatLibrary::Q345BriefMark()==cFirstChar||CSteelMatLibrary::Q345BriefMark()==cTailChar)
				bomPart.cMaterial=CSteelMatLibrary::Q345BriefMark();
			else if(CSteelMatLibrary::Q390BriefMark()==cFirstChar||CSteelMatLibrary::Q390BriefMark()==cTailChar)
				bomPart.cMaterial=CSteelMatLibrary::Q390BriefMark();
			else if(CSteelMatLibrary::Q420BriefMark()==cFirstChar||CSteelMatLibrary::Q420BriefMark()==cTailChar)
				bomPart.cMaterial=CSteelMatLibrary::Q420BriefMark();
			else if(CSteelMatLibrary::Q460BriefMark()==cFirstChar||CSteelMatLibrary::Q460BriefMark()==cTailChar)
				bomPart.cMaterial=CSteelMatLibrary::Q460BriefMark();
			//���
			/*int width=0,thick=0,cls_id=0;
			ParseSpec(sSize,thick,width,cls_id);
			pBomPart->wPartType=cls_id;
			if(cls_id==1)
				sprintf(pBomPart->sSizeStr,"L%dX%d",width,thick);
			else if(cls_id==2)
				sprintf(pBomPart->sSizeStr,"-%d",thick);*/
			strncpy(bomPart.sSpec,sSize,16);
			//��ΪCAD��ȡʱ�۸ַ���������ֱ�߻��Ƶ��¶�ʧ[,�˴����� wht 18-12-25
			if(stricmp(bomPart.sSpec,"5")==0)
				strcpy(bomPart.sSpec,"[5");
			ParseSpec(&bomPart);
		}
		BOMPART* pBomPart=pDrawBom->AddBomPart(sPartLabel,bomPart.cPartType);
		strcpy(pBomPart->sPartNo,sPartLabel);
		pBomPart->iSeg=segI.iSeg;
		pBomPart->cMaterial=bomPart.cMaterial;
		pBomPart->sSpec.Copy(bomPart.sSpec);
		pBomPart->wide=bomPart.wide;
		pBomPart->thick=bomPart.thick;
		pBomPart->siSubType=bomPart.siSubType;
		//����
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->length=atof(sKey);
		//������
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->AddPart(atoi(sKey));
		//����
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->fPieceWeight=atof(sKey);
		//����
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->fSumWeight=atof(sKey);
	}
	fclose(fp);
	return pDrawBom;
}
CTowerBOM* CDataCmpModel::ImportExcelBomFileByDllFromat(CVariant2dArray &sheetContextMap)
{
	if(CLDSApp::GetBomExcelFormatEx==NULL)
		return NULL;
	CXhChar200 titleStr;
	int colIndexArr[50]={0},iStartRow=0;
	int nCount=CLDSApp::GetBomExcelFormatEx(colIndexArr,&iStartRow,titleStr);
	if(nCount<4)
		return NULL;
	CBomTblTitleCfg cfg(colIndexArr,titleStr,nCount,iStartRow);
	if(!cfg.IsMatch(sheetContextMap))
		return NULL;
	else
	{
		CHashStrList<DWORD> hashColIndexByColTitle;
		cfg.GetHashColIndexByColTitleTbl(hashColIndexByColTitle);
		return ImportExcelBomFileCore(sheetContextMap,hashColIndexByColTitle,iStartRow-1);
	}
}
static int GetClsIdByPartType(const char* part_type)
{
	if(part_type==NULL)
		return 0;
	CXhChar16 sPartType(part_type);
	if( sPartType.EqualNoCase("�Ǹ�")||
		sPartType.EqualNoCase("L")||
		sPartType.EqualNoCase("��"))
		return BOMPART::ANGLE;
	else if(sPartType.EqualNoCase("�ֹ�")||
			sPartType.EqualNoCase("��")||
			sPartType.EqualNoCase("��"))
		return BOMPART::TUBE;
	else if(sPartType.EqualNoCase("�ְ�")||
			sPartType.EqualNoCase("��")||
			sPartType.EqualNoCase("���")||
			sPartType.EqualNoCase("-")||
			sPartType.EqualNoCase("_")||
			sPartType.EqualNoCase("��"))
		return BOMPART::PLATE;
	else if(sPartType.EqualNoCase("�۸�")||
			sPartType.EqualNoCase("["))
		return BOMPART::SLOT;
	else
		return 0;
}
static  CXhChar16 GetPartTypeBriefMark(const char* part_type)
{
	CXhChar16 sPartType;
	int cls_id=GetClsIdByPartType(part_type);
	if(cls_id==BOMPART::ANGLE)
		sPartType.Copy("L");
	else if(cls_id==BOMPART::PLATE)
		sPartType.Copy("-");
	else if(cls_id==BOMPART::TUBE)
		sPartType.Copy("��");
	else if(cls_id==BOMPART::SLOT)
		sPartType.Copy("[");
	return sPartType;
}
static BOOL ParseSpec(char* spec_str,double* thick,double* wide,int* cls_id,int *sub_type=NULL,bool bPartLabelStr=false,char *real_spec=NULL,char* part_material=NULL)
{
	if(spec_str==NULL||strlen(spec_str)<=0)
		return FALSE;
	CXhChar50 sSpec(spec_str);
	char* validstr=NULL;
	CXhChar16 material;
	int inter_cls_id=0,inter_sub_type=0;
	double inter_wide=0,inter_thick=0;
	char first_char=spec_str[0];
	char sec_char=strlen(spec_str)>1?spec_str[1]:' ';
	char third_char=strlen(spec_str)>2?spec_str[2]:' ';
	if(bPartLabelStr)
	{
		if( strstr(spec_str,"-")==NULL&&
			(first_char=='F'||first_char=='X'||first_char=='C'||first_char=='X'||first_char=='U'))
		{
			inter_cls_id=BOMPART::PLATE;
			if(first_char=='F')
			{
				inter_sub_type=BOMPART::SUB_TYPE_PLATE_FL;
				if(sec_char=='D')
					inter_sub_type=BOMPART::SUB_TYPE_PLATE_FLD;
				else if(sec_char=='P')
					inter_sub_type=BOMPART::SUB_TYPE_PLATE_FLP;
				else if(sec_char=='G')
					inter_sub_type=BOMPART::SUB_TYPE_PLATE_FLG;
				else if(sec_char=='R')
					inter_sub_type=BOMPART::SUB_TYPE_PLATE_FLR;
			}
			else if(first_char=='X')
				inter_sub_type=BOMPART::SUB_TYPE_PLATE_X;
			else if(first_char=='C')
				inter_sub_type=BOMPART::SUB_TYPE_PLATE_C;
			else if(first_char=='U')
				inter_sub_type=BOMPART::SUB_TYPE_PLATE_U;
		}
		else if( (first_char=='G'&&sec_char=='J'&&third_char=='-')||	//GJ-50 �ֽ���
			(first_char=='N'&&sec_char=='U'&&third_char=='T')||	//UT���߼�
			(first_char=='U'&&sec_char=='-')||						//U�͹һ�
			(first_char=='J'&&sec_char=='K'&&third_char=='-')||	//���߼���
			(first_char=='N'&&sec_char=='X'&&third_char=='-')||	//Ш���߼�
			first_char=='T')
		{
			inter_cls_id=BOMPART::ACCESSORY;
			if(first_char=='T')
				inter_sub_type=BOMPART::SUB_TYPE_LADDER;	//����
			else
				inter_sub_type=BOMPART::SUB_TYPE_STAY_ROPE;	//��������
		}
	}
	else if(((validstr=strstr(spec_str,"L"))!=NULL)||(validstr=strstr(spec_str,"��"))!=NULL)
	{
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		memmove((char*)sSpec,validstr,sSpec.Length-materialstr_len+1);
		if(real_spec!=NULL)
			strcpy(real_spec,sSpec);
		inter_cls_id=BOMPART::ANGLE;	//�Ǹ�
		sSpec.Replace("L","");
		sSpec.Replace("��","");
		sSpec.Replace("��"," ");
		sSpec.Replace("X"," ");
		sSpec.Replace("x"," ");
		sSpec.Replace("*"," ");
		sSpec.Replace("?"," ");	//����״̬��ʶ�����ʱϵͳĬ�ϴ������ַ�
		validstr++;
		sscanf(sSpec,"%lf%lf",&inter_wide,&inter_thick);
	}
	else if((validstr=strstr(spec_str,"-"))!=NULL&&(validstr==spec_str||(validstr-spec_str)==4))
	{
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		inter_cls_id=BOMPART::PLATE;	//�ְ�
		memmove((char*)sSpec,validstr+1,sSpec.Length);
		if(real_spec!=NULL)
			strcpy(real_spec,sSpec);
		if(strstr(sSpec,"��")||strstr(sSpec,"x")||strstr(sSpec,"*")||strstr(sSpec,"X"))
		{
			sSpec.Replace("��"," ");
			sSpec.Replace("X"," ");
			sSpec.Replace("x"," ");
			sSpec.Replace("*"," ");
			sSpec.Replace("?"," ");	//����״̬��ʶ�����ʱϵͳĬ�ϴ������ַ�
			sscanf(sSpec,"%lf%lf",&inter_thick,&inter_wide);
		}
		else
			inter_thick=atof(sSpec);
	}
	else if(((validstr=strstr(spec_str,"��"))!=NULL)||(validstr=strstr(spec_str,"��"))!=NULL)
	{
		if(real_spec!=NULL)
			strcpy(real_spec,validstr);
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		int hits=sSpec.Replace("��"," ");
		hits+=sSpec.Replace("��"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		inter_cls_id=BOMPART::TUBE;	//�ֹ�
		if(hits==2)
		{
			inter_cls_id=BOMPART::SUB_TYPE_TUBE_WIRE;	//�׹�
			sscanf(sSpec,"%lf%lf",&inter_thick,&inter_wide);
		}
		else
		{
			sSpec.Copy(validstr);
			sSpec.Replace("��"," ");
			sSpec.Replace("��"," ");
			int nXCount=sSpec.Replace("��"," ");
			nXCount+=sSpec.Replace("X"," ");
			nXCount+=sSpec.Replace("x"," ");
			nXCount+=sSpec.Replace("*"," ");
			sSpec.Replace("?"," ");	//����״̬��ʶ�����ʱϵͳĬ�ϴ������ַ�
			sscanf(sSpec,"%lf%lf",&inter_wide,&inter_thick);
			if(nXCount<=0)	//Բ��
				inter_cls_id=BOMPART::ROUND;
		}
	}
	else if(((validstr=strstr(spec_str,"["))!=NULL)||(strlen(spec_str)==2&&spec_str[0]=='C'))
	{	//�۸�
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		if(real_spec!=NULL)
			strcpy(real_spec,validstr);
		inter_cls_id=BOMPART::SLOT;
	}
	else
		return FALSE;
	if(wide!=NULL)
		*wide=inter_wide;
	if(thick!=NULL)
		*thick=inter_thick;
	if(cls_id!=NULL)
		*cls_id=inter_cls_id;
	if(sub_type!=NULL)
		*sub_type=inter_sub_type;
	if(part_material)
		strcpy(part_material,material);
	return TRUE;
}
static BOOL ParseSpec(BOMPART *pBomPart)
{
	if(pBomPart==NULL||pBomPart->sSpec==NULL)
		return FALSE;
	int cls_id=0,sub_type=0;
	double width=0,thick=0;
	if( ParseSpec(pBomPart->sSpec,&thick,&width,&cls_id,&sub_type)||
		(strlen(pBomPart->sSpec)==0&&ParseSpec(pBomPart->sPartNo,&thick,&width,&cls_id,&sub_type,TRUE)))
	{
		pBomPart->wide=width;
		pBomPart->thick=thick;
		pBomPart->cPartType=cls_id;
		pBomPart->siSubType=(short)sub_type;
		return TRUE;
	}
	else
		return FALSE;
}
double CalPartWeight(BOMPART *pPart)
{
	if(pPart==NULL)
		return 0;
	if((pPart->cPartType==0||pPart->wide==0||pPart->thick==0)&&!ParseSpec(pPart))
		return 0;
	if(pPart->cPartType==BOMPART::ANGLE)
	{	//�Ǹ�
		JG_PARA_TYPE *pType=NULL;
		for(int i=0;i<jgguige_N;i++)
		{
			if( fabs(jgguige_table[i].wing_wide-pPart->wide)<EPS&&
				fabs(jgguige_table[i].wing_thick-pPart->thick)<EPS)
			{
				pType=&jgguige_table[i];
				break;
			}
		}
		if(pType)
		{
			pPart->fPieceWeight=pType->theroy_weight*pPart->length*0.001;
			return pPart->fPieceWeight;
		}
		else
			return 0;
	}
	else if(pPart->cPartType==BOMPART::PLATE)
	{
		if(pPart->siSubType==0)
			pPart->fPieceWeight=pPart->length*pPart->wide*pPart->thick*7.85e-6;
		else
		{
			FL_PARAM *pFlParam=NULL;
			INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
			if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_C)
				pInsertPlateParam=FindRollEndParam(pPart->sPartNo);
			else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_X)
				pInsertPlateParam=FindXEndParam(pPart->sPartNo);
			else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_U)
				pInsertPlateParam=FindUEndParam(pPart->sPartNo);
			else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLD)
				pFlParam=FindFlDParam(pPart->sPartNo);
			else if(pPart->siSubType==BOMPART::SUB_TYPE_PLATE_FLP)
				pFlParam=FindFlPParam(pPart->sPartNo);
			if(pInsertPlateParam)
				pPart->fPieceWeight=pInsertPlateParam->theroy_weight;
			else if(pFlParam)
				pPart->fPieceWeight=pFlParam->Weight();
		}
		return pPart->fPieceWeight;
	}
	else if(pPart->cPartType==BOMPART::TUBE)
	{
		TUBE_PARA_TYPE *pType=NULL;
		for(int i=0;i<tubeguige_N;i++)
		{
			if( fabs(tubeguige_table[i].D-pPart->wide)<EPS&&
				fabs(tubeguige_table[i].thick-pPart->thick)<EPS)
			{
				pType=&tubeguige_table[i];
				break;
			}
		}
		if(pType)
		{
			pPart->fPieceWeight=pType->theroy_weight*pPart->length*0.001;
			return pPart->fPieceWeight;
		}
		else
		{
			double area=Pi*(pPart->wide*pPart->thick+pPart->thick*pPart->thick);
			double theroy_weight=area*7.85e-3;
			pPart->fPieceWeight=theroy_weight*pPart->length*0.001;
			return pPart->fPieceWeight;
		}
	}
	else if(pPart->cPartType==BOMPART::SLOT)
	{
		CXhChar16 sSize(pPart->sSpec);
		sSize.Remove('[');
		sSize.Remove('C');
		SLOT_PARA_TYPE *pSlotType=FindSlotType(sSize);
		if(pSlotType)
			pPart->fPieceWeight=pSlotType->theroy_weight*pPart->length*0.001;
		return pPart->fPieceWeight;
	}
	/*else if(pPart->cPartType==BOMPART::ROUND||pPart->cPartType==0)
	{
		AIDED_PART *pAidedPart=CPartLibrary::FindAidedPart(pPart->sLabel);
		if(pAidedPart)
		{
			if(pAidedPart->siUnitType==1)
				pPart->weight=pAidedPart->weight*pPart->length*0.001;
			else
				pPart->weight=pAidedPart->weight;
			return pPart->weight;
		}
		else
			return 0;
	}*/
	else 
		return 0;
}
CTowerBOM* CDataCmpModel::ImportExcelBomFileCore(CVariant2dArray &sheetContentMap,CHashStrList<DWORD> &hashColIndexByColTitle,int startRowIndex)
{
	if(hashColIndexByColTitle.GetNodeNum()<=0)
		return NULL;
	DWORD *pColIndex =NULL;
	CTowerBOM* pDrawBom=m_drawingDataList.append();
	for(int i=startRowIndex;i<=sheetContentMap.RowsCount();i++)
	{ 
		VARIANT value;
		CXhChar16 sPartType,sPartTypeBriefMark;
		CXhChar100 sPartNo,sMaterial,sSpec;
		//���
		pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_PART_NO);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		if(value.vt==VT_EMPTY)
			continue;
		sPartNo=VariantToString(value);
		sPartNo.Remove(' ');
		if(sPartNo.Length<=0)
			continue;
		char cPartNoMat=0,cQualityLevel=0;
		char sFirstChar=sPartNo[0],sTailChar=sPartNo[sPartNo.Length-1];
		char* pch2ndTail=sPartNo.Length>1?&sPartNo[sPartNo.Length-2]:NULL;
		if( sFirstChar==CSteelMatLibrary::Q235BriefMark()||
			sFirstChar==CSteelMatLibrary::Q345BriefMark()||
			sFirstChar==CSteelMatLibrary::Q390BriefMark()||
			sFirstChar==CSteelMatLibrary::Q420BriefMark()||
			sFirstChar==CSteelMatLibrary::Q460BriefMark())
		{	//�������еĲ��ʷ�
			cPartNoMat=sFirstChar;
			memmove((char*)sPartNo,((char*)sPartNo)+1,sPartNo.Length);
		}
		else if(sTailChar==CSteelMatLibrary::Q235BriefMark()||
				sTailChar==CSteelMatLibrary::Q345BriefMark()||
				sTailChar==CSteelMatLibrary::Q390BriefMark()||
				sTailChar==CSteelMatLibrary::Q420BriefMark()||
				sTailChar==CSteelMatLibrary::Q460BriefMark())
		{	
			cPartNoMat=sTailChar;
			sPartNo.NCopy(sPartNo,sPartNo.Length-1);
		}
		else if (pch2ndTail!=NULL&&(sTailChar>='A'&&sTailChar<='D')&&(
			*pch2ndTail==CSteelMatLibrary::Q235BriefMark()||
			*pch2ndTail==CSteelMatLibrary::Q345BriefMark()||
			*pch2ndTail==CSteelMatLibrary::Q390BriefMark()||
			*pch2ndTail==CSteelMatLibrary::Q420BriefMark()||
			*pch2ndTail==CSteelMatLibrary::Q460BriefMark()))
		{	//��"201PC",��ʾ201���ţ�Q420C�ֲ�
			cPartNoMat=*pch2ndTail;		//���ʼ��ַ�
			cQualityLevel=sTailChar;	//�����ȼ�
			sPartNo.NCopy(sPartNo,sPartNo.Length-2);
		}
		//����
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_PARTTYPE))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			sPartType = VariantToString(value);
			sPartTypeBriefMark=GetPartTypeBriefMark(sPartType);
		}
		//���
		int cls_id=0,sub_type=0;
		double thick=0,wide=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SPEC))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			sSpec = VariantToString(value);
			if(sMaterial.Length==0)
			{
				char* pszMaterial=NULL;
				if((pszMaterial=strstr(sSpec,"Q345"))!=NULL)
					sMaterial="Q345";
				else if((pszMaterial=strstr(sSpec,"Q390"))!=NULL)
					sMaterial="Q390";
				else if((pszMaterial=strstr(sSpec,"Q420"))!=NULL)
					sMaterial="Q420";
				else if((pszMaterial=strstr(sSpec,"Q460"))!=NULL)
					sMaterial="Q460";
				else
					sMaterial="Q235";
				if (pszMaterial!=NULL&&pszMaterial!=sSpec&&*(pszMaterial-1)=='_')
					*(pszMaterial-1)=0;	//����㽭ʢ��"L250x26_Q420C"���͵Ĳ��ϱ�����
				if (pszMaterial!=NULL&&(*(pszMaterial+4)>='A'&&*(pszMaterial+4)<='D'))
					cQualityLevel=*(pszMaterial+4);
			}
			BOOL bRetCode=FALSE;
			if(sMaterial.GetLength()<=0)
				bRetCode=ParseSpec(sSpec,&thick,&wide,&cls_id,&sub_type,false,sSpec,sMaterial);
			else
				bRetCode=ParseSpec(sSpec,&thick,&wide,&cls_id,&sub_type,false,sSpec,NULL);
			if(!bRetCode)
			{
				sSpec.InsertBefore(sPartTypeBriefMark,0);
				if(sMaterial.GetLength()<=0)
					bRetCode=ParseSpec(sSpec,&thick,&wide,&cls_id,&sub_type,false,sSpec,sMaterial);
				else
					bRetCode=ParseSpec(sSpec,&thick,&wide,&cls_id,&sub_type,false,sSpec,NULL);
			}
		}
		//����
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_METERIAL))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			sMaterial = VariantToString(value);
			sMaterial.Remove('B');
			sMaterial.Remove(' ');
			if(sMaterial.GetLength()<=0)
				sMaterial.Copy("Q235");
		}
		//����
		float fLength=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_LEN))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			CXhChar100 sLen=VariantToString(value);
			if(strstr(sLen,"��")||strstr(sLen,"x")||strstr(sLen,"*")||strstr(sLen,"X"))
			{
				sLen.Replace("��"," ");
				sLen.Replace("X"," ");
				sLen.Replace("x"," ");
				sLen.Replace("*"," ");
				sLen.Replace("?"," ");	//����״̬��ʶ�����ʱϵͳĬ�ϴ������ַ�
				sscanf(sLen,"%lf %f",&wide,&fLength);	//floatʹ��%f�ſ�������ʶ�� wht 19-01-10
				/*DELIMITER_STR parser(sLen);
				CXhChar50 xarrItems[2];
				UINT count=DELIMITER_STR::ParseWordsFromStr(sLen," ",xarrItems,2);
				wide=atof(xarrItems[0]);
				fLength=(float)atof(xarrItems[1]);*/
				//sscanf(sLen,"%lf%lf",&wide,&fLength);	//û����ȡ"168 208"�е�208 wjh-2019.1.9
			}
			else
				fLength=(float)atof(sLen);
		}
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_WIDE))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			int fwide=atoi(VariantToString(value));
			if(fwide>0)
				wide=fwide;
		}
		//������
		int nNumPerTower=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SING_NUM))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			nNumPerTower=atoi(VariantToString(value));
		}
		//�ӹ���
		/*int nProcessNum=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(T_BOMPART_SUM))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			nProcessNum=atoi(VariantToString(value));
		}*/
		//��������
		double fWeight=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SING_WEIGHT))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			fWeight=atof(VariantToString(value));
		}
		if(cls_id<=0&&sPartType.GetLength()>0)
			cls_id=GetClsIdByPartType(sPartType);
		//��ʼ����ϣ��
		BOMPART* pBomPart=pDrawBom->AddBomPart(sPartNo,cls_id);
		pBomPart->cPartType=cls_id;
		pBomPart->siSubType=sub_type;
		pBomPart->wide=wide;
		pBomPart->thick=thick;
		strcpy(pBomPart->sPartNo,sPartNo);
		SEGI segI;
		if(ParsePartNo(sPartNo, &segI, NULL))
			pBomPart->iSeg = segI;
		sMaterial.Remove(' ');	//�Ƴ��ո�
		if(sMaterial.Length>1)
			pBomPart->cMaterial=QuerySteelBriefMatMark(sMaterial);
		else if(sMaterial.Length==1)
			pBomPart->cMaterial=sMaterial[0];
		else
			pBomPart->cMaterial=CSteelMatLibrary::Q235BriefMark();
		if (cQualityLevel!=0)
			pBomPart->cQualityLevel=cQualityLevel;
		STEELMAT *pMat=QuerySteelMatByBriefMark(pBomPart->cMaterial);
		if(pMat==NULL)	//���ݲ����ַ���ѯ����ʧ��ʱ��ʹ�ü����еĲ�����Ϣ wht 18-11-27
		{
			if(cPartNoMat==0)
				pBomPart->cMaterial=cPartNoMat;
			else
				pBomPart->cMaterial=CSteelMatLibrary::Q235BriefMark();
		}

		if(cls_id==BOMPART::ANGLE)
			sprintf(pBomPart->sSpec,"L%sX%s",(char*)CXhChar16(wide),(char*)CXhChar16(thick));
		else if(cls_id==BOMPART::PLATE)
		{
			if(wide>0)
				sprintf(pBomPart->sSpec,"-%sX%s",(char*)CXhChar16(thick),(char*)CXhChar16(wide));
			else
				sprintf(pBomPart->sSpec,"-%s",(char*)CXhChar16(thick));
		}
		else
			strncpy(pBomPart->sSpec,sSpec,16);
		pBomPart->AddPart(nNumPerTower);
		pBomPart->length=fLength;
		pBomPart->fPieceWeight=fWeight;
		if(pBomPart->fPieceWeight==0)
			pBomPart->fPieceWeight=CalPartWeight(pBomPart);
	}
	return pDrawBom;
}
CTowerBOM* CDataCmpModel::AppendDrawingDataFromExce(const char* sFileName)
{
	CExcelOperObject excelObj;
	excelObj.OpenExcelFile(sFileName);
	LPDISPATCH pWorksheets=excelObj.GetWorksheets();
	if(pWorksheets==NULL)
		return FALSE;
	ASSERT(pWorksheets != NULL);
	Sheets       excel_sheets;	//��������
	excel_sheets.AttachDispatch(pWorksheets);
	int nSheetNum=excel_sheets.GetCount();
	if(nSheetNum<1)
	{
		excel_sheets.ReleaseDispatch();
		return FALSE;
	}
	CVariant2dArray sheetContentMap(1,1);
	for(int iSheet=1;iSheet<=nSheetNum;iSheet++)
	{
		LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short) iSheet));
		_Worksheet  excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet);
		excel_sheet.Select();
		Range excel_usedRange,excel_range;
		excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
		excel_range.AttachDispatch(excel_usedRange.GetRows());
		long nRowNum = excel_range.GetCount();
		excel_range.AttachDispatch(excel_usedRange.GetColumns());
		long nColNum = excel_range.GetCount();
		if(nColNum<4)//LOFTING_EXCEL_COL_COUNT || strstr(excel_sheet.GetName(),"��ͷ"))
			continue;
		//
		char cell[20]="";
		sprintf(cell,"%C%d",'A'+LOFTING_EXCEL_COL_COUNT-1,nRowNum+2);
		LPDISPATCH pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		sheetContentMap.var=excel_range.GetValue();
		excel_usedRange.ReleaseDispatch();
		excel_range.ReleaseDispatch();
		excel_sheet.ReleaseDispatch();
		break;
	}
	excel_sheets.ReleaseDispatch();

	CTowerBOM *pBom=ImportExcelBomFileByDllFromat(sheetContentMap);
	if(pBom)
		return pBom;
	CBomTblTitleCfg cfg,*pCfg=NULL;
	CConfigTableTitleDlg configDlg;
	configDlg.m_pSheetContextMap=&sheetContentMap;
	if(!CBomTblTitleCfgSet::FindMatchCfg(sheetContentMap,APP_PATH,&cfg)||!cfg.IsMatch(sheetContentMap))
	{
		if(configDlg.DoModal()!=IDOK)
			return NULL;
		pCfg=configDlg.GetTblTitleCfg();
		if(pCfg->IsMatch(sheetContentMap))
			CBomTblTitleCfgSet::AppendRecToFile(*pCfg,APP_PATH);
	}
	else
		pCfg=&cfg;
	CHashStrList<DWORD> hashColIndexByColTitle;
	if(!pCfg->IsMatch(sheetContentMap))
	{	//���ø�ʽ��ƥ�䣬��Ĭ�ϸ�ʽ������Ԫ������
		for(int i=0;i<LOFTING_EXCEL_COL_COUNT;i++)
		{
			VARIANT value;
			sheetContentMap.GetValueAt(0,i,value);
			CString itemstr(value.bstrVal);
			if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_PART_NO,itemstr)) 
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_PART_NO,i);
			else if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_METERIAL,itemstr)) 
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_METERIAL,i);
			else if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_LEN,itemstr)) 
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_LEN,i);
			else if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_SPEC,itemstr)) 
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_SPEC,i);
			else if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_SING_NUM,itemstr))
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_SING_NUM,i);
			else if(CBomTblTitleCfg::IsMatchTitle(CBomTblTitleCfg::INDEX_SING_WEIGHT,itemstr)) 
				hashColIndexByColTitle.SetValue(CBomTblTitleCfg::T_SING_WEIGHT,i);
		}
	}
	else
		pCfg->GetHashColIndexByColTitleTbl(hashColIndexByColTitle);
	//
	if (hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_PART_NO)==NULL||
		hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_LEN)==NULL||
		hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SPEC)==NULL)
	{
		logerr.Log("�ļ���ʽ����");
		return NULL;
	}
	else
		return ImportExcelBomFileCore(sheetContentMap,hashColIndexByColTitle,pCfg->m_nStartRow-1);
}
UINT CDataCmpModel::GetExterDrwBomPartsCount()
{
	UINT uiSummCount=0;
	for(CTowerBOM* pDrwBom=EnumFirstDrawingData();pDrwBom;pDrwBom=EnumNextDrawingData())
		uiSummCount+=pDrwBom->GetPartNum();
	return uiSummCount;
}
void CDataCmpModel::DeleteDwgBomFile(CTowerBOM *pBom)
{
	for(CTowerBOM* pBomFile=EnumFirstDrawingData();pBomFile;pBomFile=EnumNextDrawingData())
	{
		if(pBomFile==pBom)
			m_drawingDataList.DeleteCursor();
	}
	m_drawingDataList.Clean();
}
static BOOL IsEqualBomPart(BOMPART *pSrcPart,BOMPART *pDestPart,
						   CHashStrList<BOOL> &hashBoolByPropName,double LEN_TOLERANCE,double WEIGTH_TOLERANCE)
{
	hashBoolByPropName.Empty();
	//ParseSpec(pSrcPart);
	//ParseSpec(pDestPart);
	char cSrcMat=pSrcPart->cMaterial,cDestMat=pDestPart->cMaterial;
	if(cSrcMat==0)
		cSrcMat='S';
	if(cDestMat==0)
		cDestMat='S';
	if( pSrcPart->cPartType==pDestPart->cPartType&&
		(pSrcPart->cPartType==BOMPART::ANGLE||
		pSrcPart->cPartType==BOMPART::TUBE||
		pSrcPart->cPartType==BOMPART::FLAT||
		pSrcPart->cPartType==BOMPART::SLOT||
		pSrcPart->cPartType==BOMPART::PLATE||
		pSrcPart->cPartType==BOMPART::ACCESSORY||
		pSrcPart->cPartType==BOMPART::ROUND))
	{
		double wide1=pSrcPart->wide,wide2=pDestPart->wide;
		double thick1=pSrcPart->thick,thick2=pDestPart->thick;
		double len1=pSrcPart->length,len2=pDestPart->length;
		if(pSrcPart->cPartType==BOMPART::PLATE)
		{
			if(wide1>len1)
			{
				double w=wide1;
				wide1=len1;
				len1=w;
			}
			if(wide2>len2)
			{
				double w=wide2;
				wide2=len2;
				len2=w;
			}
		}
		if( pSrcPart->cPartType==BOMPART::ANGLE||
			pSrcPart->cPartType==BOMPART::TUBE||
			pSrcPart->cPartType==BOMPART::FLAT||
			pSrcPart->cPartType==BOMPART::SLOT)
		{	//�˼����Աȹ�񼰳���
			if(fabs(len1-len2)>LEN_TOLERANCE)	//���ݿͻ����󣬳��ȱȽ������50����
				hashBoolByPropName.SetValue("m_fLength",TRUE);
			if(fabs(wide1-wide2)>EPS2||fabs(thick1-thick2)>EPS2)
				hashBoolByPropName.SetValue("spec",TRUE);		//���ͬ
			if(cSrcMat!=cDestMat)
				hashBoolByPropName.SetValue("cMaterial",TRUE);	//�ԱȲ���
		}
		else if(pSrcPart->cPartType==BOMPART::PLATE)
		{
			//if( pSrcPart->siSubType!=pDestPart->siSubType||		//�����Ͳ�ͬ
			//	pSrcPart->siSubType==0&&cSrcMat!=cDestMat)			//���Ƚ�������
			if(cSrcMat!=cDestMat)
				hashBoolByPropName.SetValue("cMaterial",TRUE);
			if(pSrcPart->siSubType==0&&fabs(thick1-thick2)>EPS2)
				hashBoolByPropName.SetValue("spec",TRUE);		//���ͬ
			//if(pSrcPart->siSubType!=0&&stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)
			//	hashBoolByPropName.SetValue("spec",TRUE);		//���ͬ
		}
		else 
		{
			if(cSrcMat!=cDestMat)	//����
				hashBoolByPropName.SetValue("cMaterial",TRUE);
			if(stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)
				hashBoolByPropName.SetValue("spec",TRUE);					//���ͬ
		}
	}
	else
	{
		if(cSrcMat!=cDestMat)	//����
			hashBoolByPropName.SetValue("cMaterial",TRUE);
		if(stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)	//���
			hashBoolByPropName.SetValue("spec",TRUE);
	}
	if(pSrcPart->GetPartNum()!=pDestPart->GetPartNum())		//������
		hashBoolByPropName.SetValue("m_nDanJiNum",TRUE);
	//��ΰ����TMA�в����������Ա� wht 19-01-10
	//if(fabsl(pSrcPart->fPieceWeight-pDestPart->fPieceWeight)>WEIGTH_TOLERANCE)
	//	hashBoolByPropName.SetValue("m_fWeight",TRUE);
	return hashBoolByPropName.GetNodeNum()==0;
}
//�Ƚϲ�������
int CDataCmpModel::ComparePartData(double LEN_TOLERANCE/*=5*/,double WEIGTH_TOLERANCE/*=0.5*/)
{
	m_hashCompareResultByPartNo.Empty();
	//1.ͳ�Ʒ���BOMPART
	CHashStrList<BOMPART*> hashLoftPartPtrByPartNo;
	for(BOMPART *pPart=m_loftData.EnumFirstPart();pPart;pPart=m_loftData.EnumNextPart())
		hashLoftPartPtrByPartNo.SetValue(pPart->sPartNo,pPart);
	//2.ͳ��ͼֽBOMPART
	CXhPtrSet<BOMPART> orgPartSet;
	for(CTowerBOM* pDrawBom=EnumFirstDrawingData();pDrawBom;pDrawBom=EnumNextDrawingData())
	{
		for(BOMPART *pPart=pDrawBom->EnumFirstPart();pPart;pPart=pDrawBom->EnumNextPart())
			orgPartSet.append(pPart);
	}
	//3.�Ա�����
	CHashStrList<BOOL> hashBoolByPropName;
	CHashStrList<BOMPART*> hashOrgPartByPartNo;
	for(BOMPART *pOrgPart=orgPartSet.GetFirst();pOrgPart;pOrgPart=orgPartSet.GetNext())
	{
		hashOrgPartByPartNo.SetValue(pOrgPart->sPartNo,pOrgPart);
		BOMPART **ppLoftPart = hashLoftPartPtrByPartNo.GetValue(pOrgPart->sPartNo);
		if (ppLoftPart == NULL||*ppLoftPart==NULL)
		{	//1.����ԭʼ�����������ڷ�������
			COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.Add(pOrgPart->sPartNo);
			pResult->pOrgPart=pOrgPart;
			pResult->pLoftPart=NULL;
		}
		else 
		{	//2. �Ա�ͬһ���Ź�������
			BOMPART *pLoftPart=*ppLoftPart;
			if(!IsEqualBomPart(pOrgPart,pLoftPart,hashBoolByPropName,LEN_TOLERANCE,WEIGTH_TOLERANCE))//�������
			{	
				COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.Add(pLoftPart->sPartNo);
				pResult->pOrgPart=pOrgPart;
				pResult->pLoftPart=pLoftPart;
				for(BOOL *pValue=hashBoolByPropName.GetFirst();pValue;pValue=hashBoolByPropName.GetNext())
					pResult->hashBoolByPropName.SetValue(hashBoolByPropName.GetCursorKey(),*pValue);
			}
		}
	}
	//2.3 �����������άģ�͹�����,�����Ƿ���©�����
	for(BOMPART **ppLoftPart=hashLoftPartPtrByPartNo.GetFirst();ppLoftPart;ppLoftPart=hashLoftPartPtrByPartNo.GetNext())
	{
		BOMPART *pLoftPart=*ppLoftPart;
		if(hashOrgPartByPartNo.GetValue(pLoftPart->sPartNo)!=NULL)//szMappingDrwLabel)!=NULL)//sPartNo))
		{//TODO:δд�꣬�Ժ���Ҫ���������нǵ�ϸ�ڹ���ԭ��ļӺ�
			/*if(!pLoftPart->bRevMappingPart)
			{
				COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.GetValue(pLoftPart->sPartNo);
				//if(pResult->hashBoolByPropName.GetValue("m_nDanJiNum")
			}*/
			continue;
		}
		COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.Add(pLoftPart->sPartNo);
		pResult->pOrgPart=NULL;
		pResult->pLoftPart=pLoftPart;
	}
	if(m_hashCompareResultByPartNo.GetNodeNum()==0)
		return 0;
	else
		return 1;
}

void CDataCmpModel::AddBomResultSheet(LPDISPATCH pSheet,int iSheet)
{
	//��У������������
	ARRAY_LIST<CXhChar16> keyStrArr;
	CDataCmpModel::COMPARE_PART_RESULT *pResult=NULL;
	for (pResult=m_hashCompareResultByPartNo.GetFirst();pResult;pResult=EnumNextResult())//�����洢�Ľ����
	{
		if(pResult->pLoftPart)
			keyStrArr.append(pResult->pLoftPart->sPartNo);
		else
			keyStrArr.append(pResult->pOrgPart->sPartNo);
	}
	CQuickSort<CXhChar16>::QuickSort(keyStrArr.m_pData,keyStrArr.GetSize(),ComparePartNo);
	//��ӱ�����
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	CStringArray str_arr;
	str_arr.SetSize(6);
	str_arr[0]="�������";str_arr[1]="��ƹ��";str_arr[2]="����";
	str_arr[3]="����";str_arr[4]="������";str_arr[5]="������Դ";
	double col_arr[6]={15,15,15,15,15,15};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//�������
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int nResult=GetResultCount();
	CXhChar16 sMaterial;
	CVariant2dArray map(nResult*2,6);//��ȡExcel���ķ�Χ
	int index=0;
	if(iSheet==1)
	{	//��һ�ֽ����ERP��TMA���е�������Ϣ��ͬ
		excel_sheet.SetName("У����");
		for(int i=0;i<keyStrArr.GetSize();i++)
		{
			COMPARE_PART_RESULT *pResult=GetResult(keyStrArr[i]);
			if(pResult==NULL || pResult->pLoftPart==NULL || pResult->pOrgPart==NULL)
				continue;
			_snprintf(cell_start,15,"A%d",index+2);
			_snprintf(cell_end,15,"A%d",index+3);
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//�ϲ���
			map.SetValueAt(index,0,COleVariant(pResult->pOrgPart->sPartNo));
			map.SetValueAt(index,1,COleVariant(pResult->pOrgPart->GetSpec()));
			QuerySteelMatMark(pResult->pOrgPart->cMaterial,sMaterial);
			map.SetValueAt(index,2,COleVariant(sMaterial));
			map.SetValueAt(index,3,COleVariant(CXhChar50("%.f",pResult->pOrgPart->length)));
			map.SetValueAt(index,4,COleVariant((long)pResult->pOrgPart->GetPartNum()));
			map.SetValueAt(index,5,COleVariant(CXhChar16("ͼֽԭʼ����")));
			//
			if(pResult->hashBoolByPropName.GetValue("spec"))
			{
				map.SetValueAt(index+1,1,COleVariant(pResult->pLoftPart->GetSpec()));
				_snprintf(cell_start,15,"B%d",index+2);
				CExcelOper::SetRangeBackColor(excel_sheet,42,cell_start);
				_snprintf(cell_start,15,"B%d",index+3);
				CExcelOper::SetRangeBackColor(excel_sheet,44,cell_start);
			}
			if(pResult->hashBoolByPropName.GetValue("cMaterial"))
			{	
				QuerySteelMatMark(pResult->pLoftPart->cMaterial,sMaterial);
				map.SetValueAt(index+1,2,COleVariant(sMaterial));
				_snprintf(cell_start,15,"C%d",index+2);
				CExcelOper::SetRangeBackColor(excel_sheet,42,cell_start);
				_snprintf(cell_start,15,"C%d",index+3);
				CExcelOper::SetRangeBackColor(excel_sheet,44,cell_start);
			}
			if(pResult->hashBoolByPropName.GetValue("m_fLength"))
			{	
				map.SetValueAt(index+1,3,COleVariant(CXhChar50("%.f",pResult->pLoftPart->length)));
				_snprintf(cell_start,15,"D%d",index+2);
				CExcelOper::SetRangeBackColor(excel_sheet,42,cell_start);
				_snprintf(cell_start,15,"D%d",index+3);
				CExcelOper::SetRangeBackColor(excel_sheet,44,cell_start);
			}
			if(pResult->hashBoolByPropName.GetValue("m_nDanJiNum"))
			{	
				map.SetValueAt(index+1,4,COleVariant((long)pResult->pLoftPart->GetPartNum()));
				_snprintf(cell_start,15,"E%d",index+2);
				CExcelOper::SetRangeBackColor(excel_sheet,42,cell_start);
				_snprintf(cell_start,15,"E%d",index+3);
				CExcelOper::SetRangeBackColor(excel_sheet,44,cell_start);
			}
			map.SetValueAt(index+1,5,COleVariant(CXhChar16("TMA")));
			index+=2;
		}
	}
	else if(iSheet==2)
	{	//�ڶ��ֽ�����������������ݣ�����ԭʼ����û������
		excel_sheet.SetName("ģ�Ͷ��๹��");
		for(int i=0;i<keyStrArr.GetSize();i++)
		{
			COMPARE_PART_RESULT *pResult=GetResult(keyStrArr[i]);
			if(pResult==NULL || pResult->pLoftPart==NULL || pResult->pOrgPart)
				continue;
			map.SetValueAt(index,0,COleVariant(pResult->pLoftPart->sPartNo));
			map.SetValueAt(index,1,COleVariant(pResult->pLoftPart->GetSpec()));
			QuerySteelMatMark(pResult->pLoftPart->cMaterial,sMaterial);
			map.SetValueAt(index,2,COleVariant(sMaterial));
			map.SetValueAt(index,3,COleVariant(CXhChar50("%.f",pResult->pLoftPart->length)));
			map.SetValueAt(index,4,COleVariant((long)pResult->pLoftPart->GetPartNum()));
			index++;
		}
	}
	else if(iSheet==3)
	{	//�����ֽ��������ԭʼ���������ݣ���������û������
		excel_sheet.SetName("ģ��ȱ�ٹ���");
		for(int i=0;i<keyStrArr.GetSize();i++)
		{
			COMPARE_PART_RESULT *pResult=GetResult(keyStrArr[i]);
			if(pResult==NULL || pResult->pLoftPart || pResult->pOrgPart==NULL)
				continue;
			map.SetValueAt(index,0,COleVariant(pResult->pOrgPart->sPartNo));
			map.SetValueAt(index,1,COleVariant(pResult->pOrgPart->GetSpec()));
			QuerySteelMatMark(pResult->pOrgPart->cMaterial,sMaterial);
			map.SetValueAt(index,2,COleVariant(sMaterial));
			map.SetValueAt(index,3,COleVariant(CXhChar50("%.f",pResult->pOrgPart->length)));
			map.SetValueAt(index,4,COleVariant((long)pResult->pOrgPart->GetPartNum()));
			index++;
		}
	}
	_snprintf(cell_end,15,"F%d",index+1);
	if(index>0)
		CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((long)3));
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
}
//�����ȽϽ��
void CDataCmpModel::ExportCompareResult()
{
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(3);
	ASSERT(pWorksheets!= NULL);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	for(int i=1;i<=3;i++)
	{
		LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short)i));
		AddBomResultSheet(pWorksheet,i);
	}
	excel_sheets.ReleaseDispatch();
}
//
void CDataCmpModel::RestoreSpec(const char* spec,double *pfwide,double *pfThick,char *matStr/*=NULL*/)
{
	char sMat[16]="",cMark1=' ',cMark2=' ';
	CXhChar100 specstr=spec;
	char* pszSizeSpec=NULL;
	if(strstr(spec,"Q")==(char*)spec)
	{
		if(strstr(spec,"L"))
			sscanf(spec,"%[^L]%c%lf%c%lf",sMat,&cMark1,pfwide,&cMark2,pfThick);
		else if(strstr(spec,"-"))
			sscanf(spec,"%[^-]%c%lf",sMat,&cMark1,pfThick);
	}
	else if(strstr(spec,"L"))
	{
		CXhChar16 sSpec(spec);
		sSpec.Replace("L","");
		sSpec.Replace("*"," ");
		sSpec.Replace("X"," ");
		sSpec.Replace("x"," ");
		sscanf(sSpec,"%lf%lf",pfwide,pfThick);
	}
	else if (strstr(spec,"-"))
		sscanf(spec,"%c%lf",sMat,pfThick);
	else if((pszSizeSpec=strstr(specstr,"��"))!=NULL||
			(pszSizeSpec=strstr(specstr,"��"))!=NULL)
	{
		pszSizeSpec+=2;
		CXhChar50 sizestr=pszSizeSpec;
		CXhChar50 xarrItems[3];
		UINT count=DELIMITER_STR::ParseWordsFromStr(sizestr,"Xx*",xarrItems,3);
		if(count>0)
			*pfwide=atof(xarrItems[0]);
		if(count>1)
			*pfThick=atof(xarrItems[1]);
		//sscanf(spec,"%c%d%c%d",sMat,)
	}
	if(matStr)
		strcpy(matStr,sMat);
}
#endif
