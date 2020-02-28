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
//Exce标题
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
	pBuff->ReadInteger(&uiVersion);	//施工图及构件明细表信息缓存区的存储格式版本号
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
			pBuff->ReadString(szLabel,17);		//构件编号
			pBuff->ReadByte(&ciPartType);
			BOMPART* pBomPart=pDrawBom->AddBomPart(szLabel,ciPartType);
			pBomPart->sPartNo=szLabel;
			pBuff->ReadWord(&pBomPart->siSubType);
			pBuff->ReadByte(&pBomPart->cMaterial);	//材质简化字符，如：'S','H','G'
			pBuff->ReadString(pBomPart->sMappingDrwLabel);	//进行数据比对时，表示当前件号与原图纸明细中的匹配对应的构件编号
			pBuff->ReadDouble(&pBomPart->wide);
			pBuff->ReadDouble(&pBomPart->thick);
			pBuff->ReadDouble(&pBomPart->length);		//构件长度参数
			pBuff->ReadDouble(&pBomPart->wingWideY);	//角钢Y肢宽度(不等肢角钢)
			pBuff->ReadDouble(&pBomPart->fPieceWeight);//单件重(kg)，对于钢板板为最小包容矩形重量
			int nPart=0;
			pBuff->ReadInteger(&nPart);//一个塔型中具有的当前编号单段（基）构件数量
			pBomPart->AddPart(nPart);
			pBuff->ReadInteger(&pBomPart->iSeg);	//段号
			pBuff->ReadString(pBomPart->sSpec,17);	//规格
			pBuff->ReadString(pBomPart->sNotes,50);//构件备注说明
			//short siZhiWan;		//>0：制弯次数，<=0不需要制弯
			//BOOL bWeldPart;		//是否需要焊接
			//BOOL bLegPart;		//是否为腿部构件
			//short nM16Ls;	//M16螺栓孔数
			//short nM20Ls;	//M20螺栓孔数
			//short nM24Ls;	//M24螺栓孔数
			//short nMXLs;	//其它直径螺栓孔数
			//short nMSumLs;	//螺栓孔总数
		}
	}
}
void CDataCmpModel::WriteToDrawAndBomBuffer(CBuffer* pBuff)
{
	pBuff->WriteInteger(1);	//保存格式版本号
	pBuff->WriteInteger(m_drawingDataList.GetNodeNum());
	for(CTowerBOM* pDrawBom=m_drawingDataList.GetFirst();pDrawBom;pDrawBom=m_drawingDataList.GetNext())
	{
		pBuff->WriteString(pDrawBom->m_sBomName);
		pBuff->WriteInteger(pDrawBom->GetPartNum());
		for(BOMPART* pBomPart=pDrawBom->EnumFirstPart();pBomPart;pBomPart=pDrawBom->EnumNextPart())
		{
			pBuff->WriteString(pBomPart->sPartNo,17);		//构件编号
			pBuff->WriteByte(pBomPart->cPartType);
			pBuff->WriteWord(pBomPart->siSubType);
			pBuff->WriteByte(pBomPart->cMaterial);	//材质简化字符，如：'S','H','G'
			pBuff->WriteString(pBomPart->sMappingDrwLabel);	//进行数据比对时，表示当前件号与原图纸明细中的匹配对应的构件编号
			pBuff->WriteDouble(pBomPart->wide);
			pBuff->WriteDouble(pBomPart->thick);
			pBuff->WriteDouble(pBomPart->length);		//构件长度参数
			pBuff->WriteDouble(pBomPart->wingWideY);	//角钢Y肢宽度(不等肢角钢)
			pBuff->WriteDouble(pBomPart->fPieceWeight);//单件重(kg)，对于钢板板为最小包容矩形重量
			pBuff->WriteInteger(pBomPart->GetPartNum());//一个塔型中具有的当前编号单段（基）构件数量
			pBuff->WriteInteger(pBomPart->iSeg);	//段号
			pBuff->WriteString(pBomPart->sSpec,17);	//规格
			pBuff->WriteString(pBomPart->sNotes,50);//构件备注说明
			//short siZhiWan;		//>0：制弯次数，<=0不需要制弯
			//BOOL bWeldPart;		//是否需要焊接
			//BOOL bLegPart;		//是否为腿部构件
			//short nM16Ls;	//M16螺栓孔数
			//short nM20Ls;	//M20螺栓孔数
			//short nM24Ls;	//M24螺栓孔数
			//short nMXLs;	//其它直径螺栓孔数
			//short nMSumLs;	//螺栓孔总数
		}
	}
}
BOMPART* CDataCmpModel::GetDrawingBomPart(const char* szPartLabel,BYTE cls_id/*=0*/)	//根据构件编号查找导入或识别的图纸数据中的BOM明细
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
		logerr.Log("%s文件打开失败!",sFileName);
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
		AfxMessageBox("打不开指定的TXT文件!");
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
		line_txt.Replace("%%c","φ");
		line_txt.Replace("%%C","φ");
		line_txt[strlen(line_txt)-1]='\0';
		char szTokens[] = " \n\t" ;

		SEGI segI;
		CXhChar50 sPartLabel,sSpec;
		//件号
		char* sKey=strtok(line_txt,szTokens);
		if(sKey!=NULL&&stricmp(PLACE_HOLDER,sKey)!=0)
			sPartLabel.Copy(sKey);
		//材质+规格
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&stricmp(PLACE_HOLDER,sKey)!=0)
			sSpec.Copy(sKey);
		if(sPartLabel.GetLength()==0)
		{	//使用规格作为件号
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
		//材质+规格
		BOMPART bomPart;
		if(sSpec.GetLength()>0)
		{	
			CXhChar50 sSize(sSpec);
			sSize.Remove(' ');
			//材质
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
			//规格
			/*int width=0,thick=0,cls_id=0;
			ParseSpec(sSize,thick,width,cls_id);
			pBomPart->wPartType=cls_id;
			if(cls_id==1)
				sprintf(pBomPart->sSizeStr,"L%dX%d",width,thick);
			else if(cls_id==2)
				sprintf(pBomPart->sSizeStr,"-%d",thick);*/
			strncpy(bomPart.sSpec,sSize,16);
			//因为CAD提取时槽钢符号用三条直线绘制导致丢失[,此处补齐 wht 18-12-25
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
		//长度
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->length=atof(sKey);
		//单基数
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->AddPart(atoi(sKey));
		//单重
		sKey=strtok(NULL,szTokens);
		if(sKey!=NULL&&strlen(sKey)>0&&stricmp(PLACE_HOLDER,sKey)!=0)
			pBomPart->fPieceWeight=atof(sKey);
		//总重
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
	if( sPartType.EqualNoCase("角钢")||
		sPartType.EqualNoCase("L")||
		sPartType.EqualNoCase("∠"))
		return BOMPART::ANGLE;
	else if(sPartType.EqualNoCase("钢管")||
			sPartType.EqualNoCase("φ")||
			sPartType.EqualNoCase("Φ"))
		return BOMPART::TUBE;
	else if(sPartType.EqualNoCase("钢板")||
			sPartType.EqualNoCase("板")||
			sPartType.EqualNoCase("板材")||
			sPartType.EqualNoCase("-")||
			sPartType.EqualNoCase("_")||
			sPartType.EqualNoCase("—"))
		return BOMPART::PLATE;
	else if(sPartType.EqualNoCase("槽钢")||
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
		sPartType.Copy("φ");
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
		else if( (first_char=='G'&&sec_char=='J'&&third_char=='-')||	//GJ-50 钢绞线
			(first_char=='N'&&sec_char=='U'&&third_char=='T')||	//UT型线夹
			(first_char=='U'&&sec_char=='-')||						//U型挂环
			(first_char=='J'&&sec_char=='K'&&third_char=='-')||	//钢线夹子
			(first_char=='N'&&sec_char=='X'&&third_char=='-')||	//楔形线夹
			first_char=='T')
		{
			inter_cls_id=BOMPART::ACCESSORY;
			if(first_char=='T')
				inter_sub_type=BOMPART::SUB_TYPE_LADDER;	//爬梯
			else
				inter_sub_type=BOMPART::SUB_TYPE_STAY_ROPE;	//拉索构件
		}
	}
	else if(((validstr=strstr(spec_str,"L"))!=NULL)||(validstr=strstr(spec_str,"∠"))!=NULL)
	{
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		memmove((char*)sSpec,validstr,sSpec.Length-materialstr_len+1);
		if(real_spec!=NULL)
			strcpy(real_spec,sSpec);
		inter_cls_id=BOMPART::ANGLE;	//角钢
		sSpec.Replace("L","");
		sSpec.Replace("∠","");
		sSpec.Replace("×"," ");
		sSpec.Replace("X"," ");
		sSpec.Replace("x"," ");
		sSpec.Replace("*"," ");
		sSpec.Replace("?"," ");	//调试状态下识别错误时系统默认待处理字符
		validstr++;
		sscanf(sSpec,"%lf%lf",&inter_wide,&inter_thick);
	}
	else if((validstr=strstr(spec_str,"-"))!=NULL&&(validstr==spec_str||(validstr-spec_str)==4))
	{
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		inter_cls_id=BOMPART::PLATE;	//钢板
		memmove((char*)sSpec,validstr+1,sSpec.Length);
		if(real_spec!=NULL)
			strcpy(real_spec,sSpec);
		if(strstr(sSpec,"×")||strstr(sSpec,"x")||strstr(sSpec,"*")||strstr(sSpec,"X"))
		{
			sSpec.Replace("×"," ");
			sSpec.Replace("X"," ");
			sSpec.Replace("x"," ");
			sSpec.Replace("*"," ");
			sSpec.Replace("?"," ");	//调试状态下识别错误时系统默认待处理字符
			sscanf(sSpec,"%lf%lf",&inter_thick,&inter_wide);
		}
		else
			inter_thick=atof(sSpec);
	}
	else if(((validstr=strstr(spec_str,"φ"))!=NULL)||(validstr=strstr(spec_str,"Φ"))!=NULL)
	{
		if(real_spec!=NULL)
			strcpy(real_spec,validstr);
		int materialstr_len=validstr-spec_str;
		if(materialstr_len>0)
			material.NCopy((char*)sSpec,materialstr_len);
		int hits=sSpec.Replace("φ"," ");
		hits+=sSpec.Replace("Φ"," ");
		sSpec.Replace("/"," ");
		sSpec.Replace("\\"," ");
		inter_cls_id=BOMPART::TUBE;	//钢管
		if(hits==2)
		{
			inter_cls_id=BOMPART::SUB_TYPE_TUBE_WIRE;	//套管
			sscanf(sSpec,"%lf%lf",&inter_thick,&inter_wide);
		}
		else
		{
			sSpec.Copy(validstr);
			sSpec.Replace("φ"," ");
			sSpec.Replace("Φ"," ");
			int nXCount=sSpec.Replace("×"," ");
			nXCount+=sSpec.Replace("X"," ");
			nXCount+=sSpec.Replace("x"," ");
			nXCount+=sSpec.Replace("*"," ");
			sSpec.Replace("?"," ");	//调试状态下识别错误时系统默认待处理字符
			sscanf(sSpec,"%lf%lf",&inter_wide,&inter_thick);
			if(nXCount<=0)	//圆钢
				inter_cls_id=BOMPART::ROUND;
		}
	}
	else if(((validstr=strstr(spec_str,"["))!=NULL)||(strlen(spec_str)==2&&spec_str[0]=='C'))
	{	//槽钢
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
	{	//角钢
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
		//编号
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
		{	//清除编号中的材质符
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
		{	//如"201PC",表示201件号，Q420C钢材
			cPartNoMat=*pch2ndTail;		//材质简化字符
			cQualityLevel=sTailChar;	//质量等级
			sPartNo.NCopy(sPartNo,sPartNo.Length-2);
		}
		//类型
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_PARTTYPE))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			sPartType = VariantToString(value);
			sPartTypeBriefMark=GetPartTypeBriefMark(sPartType);
		}
		//规格
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
					*(pszMaterial-1)=0;	//针对浙江盛达"L250x26_Q420C"类型的材料表规格列
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
		//材质
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_METERIAL))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			sMaterial = VariantToString(value);
			sMaterial.Remove('B');
			sMaterial.Remove(' ');
			if(sMaterial.GetLength()<=0)
				sMaterial.Copy("Q235");
		}
		//长度
		float fLength=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_LEN))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			CXhChar100 sLen=VariantToString(value);
			if(strstr(sLen,"×")||strstr(sLen,"x")||strstr(sLen,"*")||strstr(sLen,"X"))
			{
				sLen.Replace("×"," ");
				sLen.Replace("X"," ");
				sLen.Replace("x"," ");
				sLen.Replace("*"," ");
				sLen.Replace("?"," ");	//调试状态下识别错误时系统默认待处理字符
				sscanf(sLen,"%lf %f",&wide,&fLength);	//float使用%f才可以正常识别 wht 19-01-10
				/*DELIMITER_STR parser(sLen);
				CXhChar50 xarrItems[2];
				UINT count=DELIMITER_STR::ParseWordsFromStr(sLen," ",xarrItems,2);
				wide=atof(xarrItems[0]);
				fLength=(float)atof(xarrItems[1]);*/
				//sscanf(sLen,"%lf%lf",&wide,&fLength);	//没法读取"168 208"中的208 wjh-2019.1.9
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
		//单基数
		int nNumPerTower=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SING_NUM))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			nNumPerTower=atoi(VariantToString(value));
		}
		//加工数
		/*int nProcessNum=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(T_BOMPART_SUM))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			nProcessNum=atoi(VariantToString(value));
		}*/
		//单基重量
		double fWeight=0;
		if((pColIndex=hashColIndexByColTitle.GetValue(CBomTblTitleCfg::T_SING_WEIGHT))!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			fWeight=atof(VariantToString(value));
		}
		if(cls_id<=0&&sPartType.GetLength()>0)
			cls_id=GetClsIdByPartType(sPartType);
		//初始化哈希表
		BOMPART* pBomPart=pDrawBom->AddBomPart(sPartNo,cls_id);
		pBomPart->cPartType=cls_id;
		pBomPart->siSubType=sub_type;
		pBomPart->wide=wide;
		pBomPart->thick=thick;
		strcpy(pBomPart->sPartNo,sPartNo);
		SEGI segI;
		if(ParsePartNo(sPartNo, &segI, NULL))
			pBomPart->iSeg = segI;
		sMaterial.Remove(' ');	//移除空格
		if(sMaterial.Length>1)
			pBomPart->cMaterial=QuerySteelBriefMatMark(sMaterial);
		else if(sMaterial.Length==1)
			pBomPart->cMaterial=sMaterial[0];
		else
			pBomPart->cMaterial=CSteelMatLibrary::Q235BriefMark();
		if (cQualityLevel!=0)
			pBomPart->cQualityLevel=cQualityLevel;
		STEELMAT *pMat=QuerySteelMatByBriefMark(pBomPart->cMaterial);
		if(pMat==NULL)	//根据材质字符查询材质失败时，使用件号中的材质信息 wht 18-11-27
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
	Sheets       excel_sheets;	//工作表集合
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
		if(nColNum<4)//LOFTING_EXCEL_COL_COUNT || strstr(excel_sheet.GetName(),"表头"))
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
	{	//配置格式不匹配，按默认格式解析单元格内容
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
		logerr.Log("文件格式不对");
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
		{	//杆件，对比规格及长度
			if(fabs(len1-len2)>LEN_TOLERANCE)	//根据客户需求，长度比较误差在50以内
				hashBoolByPropName.SetValue("m_fLength",TRUE);
			if(fabs(wide1-wide2)>EPS2||fabs(thick1-thick2)>EPS2)
				hashBoolByPropName.SetValue("spec",TRUE);		//规格不同
			if(cSrcMat!=cDestMat)
				hashBoolByPropName.SetValue("cMaterial",TRUE);	//对比材质
		}
		else if(pSrcPart->cPartType==BOMPART::PLATE)
		{
			//if( pSrcPart->siSubType!=pDestPart->siSubType||		//子类型不同
			//	pSrcPart->siSubType==0&&cSrcMat!=cDestMat)			//不比较子类型
			if(cSrcMat!=cDestMat)
				hashBoolByPropName.SetValue("cMaterial",TRUE);
			if(pSrcPart->siSubType==0&&fabs(thick1-thick2)>EPS2)
				hashBoolByPropName.SetValue("spec",TRUE);		//规格不同
			//if(pSrcPart->siSubType!=0&&stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)
			//	hashBoolByPropName.SetValue("spec",TRUE);		//规格不同
		}
		else 
		{
			if(cSrcMat!=cDestMat)	//材质
				hashBoolByPropName.SetValue("cMaterial",TRUE);
			if(stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)
				hashBoolByPropName.SetValue("spec",TRUE);					//规格不同
		}
	}
	else
	{
		if(cSrcMat!=cDestMat)	//材质
			hashBoolByPropName.SetValue("cMaterial",TRUE);
		if(stricmp(pSrcPart->sSpec,pDestPart->sSpec)!=0)	//规格
			hashBoolByPropName.SetValue("spec",TRUE);
	}
	if(pSrcPart->GetPartNum()!=pDestPart->GetPartNum())		//单基数
		hashBoolByPropName.SetValue("m_nDanJiNum",TRUE);
	//刘伟反馈TMA中不开放重量对比 wht 19-01-10
	//if(fabsl(pSrcPart->fPieceWeight-pDestPart->fPieceWeight)>WEIGTH_TOLERANCE)
	//	hashBoolByPropName.SetValue("m_fWeight",TRUE);
	return hashBoolByPropName.GetNodeNum()==0;
}
//比较操作操作
int CDataCmpModel::ComparePartData(double LEN_TOLERANCE/*=5*/,double WEIGTH_TOLERANCE/*=0.5*/)
{
	m_hashCompareResultByPartNo.Empty();
	//1.统计放样BOMPART
	CHashStrList<BOMPART*> hashLoftPartPtrByPartNo;
	for(BOMPART *pPart=m_loftData.EnumFirstPart();pPart;pPart=m_loftData.EnumNextPart())
		hashLoftPartPtrByPartNo.SetValue(pPart->sPartNo,pPart);
	//2.统计图纸BOMPART
	CXhPtrSet<BOMPART> orgPartSet;
	for(CTowerBOM* pDrawBom=EnumFirstDrawingData();pDrawBom;pDrawBom=EnumNextDrawingData())
	{
		for(BOMPART *pPart=pDrawBom->EnumFirstPart();pPart;pPart=pDrawBom->EnumNextPart())
			orgPartSet.append(pPart);
	}
	//3.对比数据
	CHashStrList<BOOL> hashBoolByPropName;
	CHashStrList<BOMPART*> hashOrgPartByPartNo;
	for(BOMPART *pOrgPart=orgPartSet.GetFirst();pOrgPart;pOrgPart=orgPartSet.GetNext())
	{
		hashOrgPartByPartNo.SetValue(pOrgPart->sPartNo,pOrgPart);
		BOMPART **ppLoftPart = hashLoftPartPtrByPartNo.GetValue(pOrgPart->sPartNo);
		if (ppLoftPart == NULL||*ppLoftPart==NULL)
		{	//1.存在原始构件，不存在放样构件
			COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.Add(pOrgPart->sPartNo);
			pResult->pOrgPart=pOrgPart;
			pResult->pLoftPart=NULL;
		}
		else 
		{	//2. 对比同一件号构件属性
			BOMPART *pLoftPart=*ppLoftPart;
			if(!IsEqualBomPart(pOrgPart,pLoftPart,hashBoolByPropName,LEN_TOLERANCE,WEIGTH_TOLERANCE))//结点数量
			{	
				COMPARE_PART_RESULT *pResult=m_hashCompareResultByPartNo.Add(pLoftPart->sPartNo);
				pResult->pOrgPart=pOrgPart;
				pResult->pLoftPart=pLoftPart;
				for(BOOL *pValue=hashBoolByPropName.GetFirst();pValue;pValue=hashBoolByPropName.GetNext())
					pResult->hashBoolByPropName.SetValue(hashBoolByPropName.GetCursorKey(),*pValue);
			}
		}
	}
	//2.3 遍历导入的三维模型构件表,查找是否有漏件情况
	for(BOMPART **ppLoftPart=hashLoftPartPtrByPartNo.GetFirst();ppLoftPart;ppLoftPart=hashLoftPartPtrByPartNo.GetNext())
	{
		BOMPART *pLoftPart=*ppLoftPart;
		if(hashOrgPartByPartNo.GetValue(pLoftPart->sPartNo)!=NULL)//szMappingDrwLabel)!=NULL)//sPartNo))
		{//TODO:未写完，以后需要处理由于切角等细节工艺原因的加号
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
	//对校审结果进行排序
	ARRAY_LIST<CXhChar16> keyStrArr;
	CDataCmpModel::COMPARE_PART_RESULT *pResult=NULL;
	for (pResult=m_hashCompareResultByPartNo.GetFirst();pResult;pResult=EnumNextResult())//遍历存储的结果表
	{
		if(pResult->pLoftPart)
			keyStrArr.append(pResult->pLoftPart->sPartNo);
		else
			keyStrArr.append(pResult->pOrgPart->sPartNo);
	}
	CQuickSort<CXhChar16>::QuickSort(keyStrArr.m_pData,keyStrArr.GetSize(),ComparePartNo);
	//添加标题行
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	CStringArray str_arr;
	str_arr.SetSize(6);
	str_arr[0]="构件编号";str_arr[1]="设计规格";str_arr[2]="材质";
	str_arr[3]="长度";str_arr[4]="单基数";str_arr[5]="数据来源";
	double col_arr[6]={15,15,15,15,15,15};
	CExcelOper::AddRowToExcelSheet(excel_sheet,1,str_arr,col_arr,TRUE);
	//填充内容
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int nResult=GetResultCount();
	CXhChar16 sMaterial;
	CVariant2dArray map(nResult*2,6);//获取Excel表格的范围
	int index=0;
	if(iSheet==1)
	{	//第一种结果：ERP和TMA表中的数据信息不同
		excel_sheet.SetName("校审结果");
		for(int i=0;i<keyStrArr.GetSize();i++)
		{
			COMPARE_PART_RESULT *pResult=GetResult(keyStrArr[i]);
			if(pResult==NULL || pResult->pLoftPart==NULL || pResult->pOrgPart==NULL)
				continue;
			_snprintf(cell_start,15,"A%d",index+2);
			_snprintf(cell_end,15,"A%d",index+3);
			CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//合并行
			map.SetValueAt(index,0,COleVariant(pResult->pOrgPart->sPartNo));
			map.SetValueAt(index,1,COleVariant(pResult->pOrgPart->GetSpec()));
			QuerySteelMatMark(pResult->pOrgPart->cMaterial,sMaterial);
			map.SetValueAt(index,2,COleVariant(sMaterial));
			map.SetValueAt(index,3,COleVariant(CXhChar50("%.f",pResult->pOrgPart->length)));
			map.SetValueAt(index,4,COleVariant((long)pResult->pOrgPart->GetPartNum()));
			map.SetValueAt(index,5,COleVariant(CXhChar16("图纸原始数据")));
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
	{	//第二种结果：放样表里有数据，导入原始表里没有数据
		excel_sheet.SetName("模型多余构件");
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
	{	//第三种结果：导入原始表里有数据，放样表里没有数据
		excel_sheet.SetName("模型缺少构件");
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
//导出比较结果
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
	else if((pszSizeSpec=strstr(specstr,"φ"))!=NULL||
			(pszSizeSpec=strstr(specstr,"Φ"))!=NULL)
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
