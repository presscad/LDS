//<LOCALE_TRANSLATE/>
#include "stdafx.h"
#include "towerbom.h"
#include "ExcelTblManager.h"
#include "ExcelOper.h"

CTowerBOM bom;

int GetClassTypeOrder(int idClassType)
{
	if(idClassType==CLS_LINEANGLE)
		return 0;
	else if(idClassType==CLS_LINETUBE)
		return 1;
	else if(idClassType==CLS_LINESLOT)
		return 2;
	else if(idClassType==CLS_PLATE||idClassType==CLS_PARAMPLATE)
		return 3;
	else if(idClassType==CLS_BOLT)
		return 4;
	else
		return 5;
}
int GetParttype(char* spec)
{
	CString str(spec);
	int parttype=0;
	if(str.Find("Φ")>=0||str.Find("φ")>=0)	//不能使用spec[0]=='Φ'
	{
		if(strchr(spec,'/'))
			parttype = 3;	//钢板(法兰)
		else
			parttype = 4;	//钢管
	}
	else if(spec[0] == 'L')
		parttype = 1;	//角钢
	else if(spec[0] == '-')
		parttype = 3;	//钢板
	else if(spec[0]=='[')
		parttype = 6;	//槽钢[
	else
		parttype = 0;	//其余构件
	return parttype;
}
CTowerBOM::CTowerBOM()
{
	hashSegI.SetHashTableGrowSize(50);
}
void CTowerBOM::Empty()
{
	hashSegI.Empty();
	bodyList.Empty();
	Parts.Empty();
	Bolts.Empty();
}
void CTowerBOM::CopyFrom(CTowerBOM& srcBom)
{
	Empty();
	for(LEG_DESIGN *pLeg=srcBom.bodyList.GetFirst();pLeg;pLeg=srcBom.bodyList.GetNext())
		bodyList.append(*pLeg);
	for(CManuPart* pPart=srcBom.Parts.GetFirst();pPart;pPart=srcBom.Parts.GetNext())
		Parts.append(*pPart);
	for(CStatBolt* pBolt=srcBom.Bolts.GetFirst();pBolt;pBolt=srcBom.Bolts.GetNext())
		Bolts.append(*pBolt);
	for(int* pSegI=srcBom.hashSegI.GetFirst();pSegI;pSegI=srcBom.hashSegI.GetNext())
		hashSegI.SetValue(srcBom.hashSegI.GetCursorKey(),*pSegI);
}

void CTowerBOM::ReadTextBOMFile(char* file_name)
{
	//Empty();	//应该是海涛认为以增量导入模式可以快捷导入各分段的bom表更方便,故去掉此处的清空操作 wjh-2017.8.27
	//读txt文件取数据
	FILE *fp;
	fp=fopen(file_name,"rt");
	char line_text[200]="";
	char sText[200]="";
	char key_word[100]="";
	char type_word[100]="";
	CString sLine;
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to open the specified configuration file!");
#else
		AfxMessageBox("打不开指定的配置文件!");
#endif
		return;
	}
	bool body=false;
	bool part=false;
	bool bolt=false;
	int n=1;
	LEG_DESIGN Body;
	CManuPart manuPart;
	CStatBolt statBolt;
	while(!feof(fp))
	{
		fgets(line_text,200,fp);
		strcpy(sText,line_text);
		sLine=sText;
		sLine.Replace('\t',' ');
		sprintf(line_text,"%s",sLine);
		char *skey=strtok((char*)sText,"' ',\t");
		if(_stricmp(skey,"SECTION")==0)
		{
			sscanf(line_text,"%s%s",key_word,type_word);
			if(_stricmp(type_word,"BODY")==0)
			{
				body=true;part=false;bolt=false;
				continue;
			}
			else if(_stricmp(type_word,"PART")==0)
			{
				part=true;body=false;bolt=false;
				continue;
			}	
			else if(_stricmp(type_word,"BOLT")==0)
			{
				bolt=true;body=false;part=false;
				continue;
			}
		}
		if(body)
		{
			sscanf(line_text,"%s%s",&Body.sHeight,&Body.bodySegStr);
			bodyList.append(Body);
		}
		else if(part)
		{
			sscanf(line_text,"%d%s%s%s%lf%d%lf%lf%s",&manuPart.nSeg,manuPart.sPartNo,manuPart.sMaterial,manuPart.sSpec,
			  &manuPart.length,&manuPart.nPartNum,&manuPart.singWeight,&manuPart.sumWeight,manuPart.sNote);
			manuPart.cMaterial=QuerySteelBriefMatMark(manuPart.sMaterial);
			manuPart.part_type=GetParttype(manuPart.sSpec);
			Parts.append(manuPart);
			if(manuPart.nSeg>=0)
				hashSegI.SetValue(manuPart.nSeg+1,manuPart.nSeg);
		}
		else if(bolt)
		{
			sscanf(line_text,"%d%s%d%lf%d%s",&statBolt.nSeg,statBolt.sSpec,&statBolt.nCount,&statBolt.weight,
			&statBolt.type,statBolt.sNote);
			statBolt.part_type=2;
			Bolts.append(statBolt);
			if(statBolt.nSeg>=0)
				hashSegI.SetValue(statBolt.nSeg+1,statBolt.nSeg);
		}
	}
	fclose(fp);
}
void CTowerBOM::WriteTextBOMFile(const char* file_name)
{
	FILE* fp=fopen(file_name,"wt");
	if(fp==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Fail to open the specified configuration file!");
#else
		AfxMessageBox("打不开指定的配置文件!");
#endif
		return;
	}
	fprintf(fp,"SECTION BODY\n");
	for(LEG_DESIGN *pModule=bodyList.GetFirst();pModule;pModule=bodyList.GetNext())
		fprintf(fp,"%s\t%s\n",pModule->sHeight,pModule->bodySegStr);
	fprintf(fp,"SECTION PART\n");
	for(CManuPart *pPartRec=Parts.GetFirst();pPartRec;pPartRec=Parts.GetNext())
	{
		fprintf(fp,"%s\t%s\t%s\t%s\t\t%.1f\t%d\t%.2f\t%.1f\t%s\n", 
			(char*)pPartRec->nSeg.ToString(),pPartRec->sPartNo,pPartRec->sMaterial,pPartRec->sSpec,pPartRec->length,
			pPartRec->nPartNum,pPartRec->singWeight,pPartRec->sumWeight,pPartRec->sNote);
	}
	fprintf(fp,"SECTION BOLT\n");
	for(CStatBolt *pLsRec=Bolts.GetFirst();pLsRec;pLsRec=Bolts.GetNext())	
		fprintf(fp,"%s\t%s\t%d\t%.1f\t%d\t%s\n",(char*)pLsRec->nSeg.ToString(),pLsRec->sSpec,pLsRec->nCount,pLsRec->weight,pLsRec->type,pLsRec->sNote);
	fclose(fp);
}
void CTowerBOM::ReadExcelBOMFile(char* file_name)
{
	//Empty();

	_Application excel_app;
	Range    excel_range;
	_Worksheet   excel_sheet;
	Workbooks   excel_workbooks;
	_Workbook excel_workbook;
	Sheets   excel_sheets;
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	CExcelOperObject::ExcelCLSIDFromProgID(clsid);
	if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK)
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch,
			(void**) &pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	}
	else if(!excel_app.CreateDispatch(clsid))
	{
		AfxMessageBox("Excel program not found");
		return;
	}
	LPDISPATCH  pWorkbooks;
	excel_app.SetSheetsInNewWorkbook(1);
	VERIFY(pWorkbooks = excel_app.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	LPDISPATCH pWorkbook = NULL;
	pWorkbook=excel_workbooks.Open(file_name);//打开文件
	excel_workbook.AttachDispatch(pWorkbook);
	LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	
	LPDISPATCH pWorksheet1 = excel_sheets.GetItem(COleVariant((short) 1));
	excel_sheet.AttachDispatch(pWorksheet1);
	excel_sheet.Select();
	ReadPartSheet(excel_range,excel_sheet);
	LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short) 2));
	excel_sheet.AttachDispatch(pWorksheet2);
	excel_sheet.Select();
	ReadBoltSheet(excel_range,excel_sheet);
	LPDISPATCH pWorksheet3 = excel_sheets.GetItem(COleVariant((short) 3));
	excel_sheet.AttachDispatch(pWorksheet3);
	excel_sheet.Select();
	ReadBodySheet(excel_range,excel_sheet);
	excel_workbook.Close(COleVariant((BYTE)0),COleVariant(file_name),COleVariant());
}
void CTowerBOM::ReadPartSheet(Range excel_range,_Worksheet excel_sheet)
{
	CManuPart* pManuPart=NULL;
	LPDISPATCH pRange;
	char cell_one[8]="A1";
	int record_part=0;
	int i=0,j=0;
	for(i=0;i<9;i++)//列数
	{
		cell_one[0]='A'+i;//A,B,C,D,E,F,G,H,L
		cell_one[1]='1';
		cell_one[2]='\0';
		pRange = excel_sheet.GetRange(COleVariant(cell_one));
		excel_range.AttachDispatch(pRange,FALSE);
		COleVariant cell_value=excel_range.GetValue();//第一行表头
		excel_range.ReleaseDispatch();
		
		if(cell_value.vt==VT_BSTR)
		{
			CString ss(cell_value.bstrVal);
#ifdef AFX_TARG_ENU_ENGLISH
			if(ss.CompareNoCase("Seg Num")==0)
#else
			if(ss.CompareNoCase("段号")==0)
#endif
			{
				for(j=0;1;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);//从第二行读取数据
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.ReleaseDispatch();
					if(cell_value.vt==VT_EMPTY)
						break;
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sSegNo(cell_value.bstrVal);
					pManuPart=Parts.append();
					pManuPart->nSeg=atoi(sSegNo);
					record_part++;//统计数据的行数
					if(pManuPart->nSeg>=0)
						hashSegI.SetValue(pManuPart->nSeg+1,pManuPart->nSeg);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("No.")==0)
#else
			else if(ss.CompareNoCase("编号")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sPartNo(cell_value.bstrVal);
					pManuPart=&Parts[j];
					sprintf(pManuPart->sPartNo,sPartNo);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Material")==0)
#else
			else if(ss.CompareNoCase("材质")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sMaterial(cell_value.bstrVal);
					pManuPart=&Parts[j];
					sprintf(pManuPart->sMaterial,sMaterial);
					pManuPart->cMaterial=QuerySteelBriefMatMark(pManuPart->sMaterial);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Spec")==0)
#else
			else if(ss.CompareNoCase("规格")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();

					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sSpecs(cell_value.bstrVal);
					pManuPart=&Parts[j];
					sprintf(pManuPart->sSpec,sSpecs);
					pManuPart->part_type=GetParttype(pManuPart->sSpec);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Length")==0)
#else
			else if(ss.CompareNoCase("长度")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sLen(cell_value.bstrVal);
					pManuPart=&Parts[j];
					pManuPart->length=atof(sLen);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Amount")==0)
#else
			else if(ss.CompareNoCase("数量")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sNum(cell_value.bstrVal);
					pManuPart=&Parts[j];
					pManuPart->nPartNum=atoi(sNum);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Piece Weight")==0)
#else
			else if(ss.CompareNoCase("单重")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sUnitWeight(cell_value.bstrVal);
					pManuPart=&Parts[j];
					pManuPart->singWeight=atof(sUnitWeight);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Total Weight")==0)
#else
			else if(ss.CompareNoCase("总重")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sumWeight(cell_value.bstrVal);
					pManuPart=&Parts[j];
					pManuPart->sumWeight=atof(sumWeight);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Notes")==0)
#else
			else if(ss.CompareNoCase("备注")==0)
#endif
			{
				for(j=0;j<record_part;j++)
				{
					sprintf(&cell_one[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_one));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sNote(cell_value.bstrVal);
					pManuPart=&Parts[j];
					sprintf(pManuPart->sNote,sNote);
				}
			}
		}
	}
}
void CTowerBOM::ReadBoltSheet(Range excel_range,_Worksheet excel_sheet)
{
	CStatBolt* pStatBolt=NULL;
	LPDISPATCH pRange;
	char cell_two[8]="A1";
	int record_bolt=0;
	int i=0,j=0;
	for(i=0;i<6;i++)//列数
	{
		cell_two[0]='A'+i;
		cell_two[1]='1';
		cell_two[2]='\0';
		pRange = excel_sheet.GetRange(COleVariant(cell_two));
		excel_range.AttachDispatch(pRange,FALSE);
		COleVariant cell_value=excel_range.GetValue();//第一行表头
		excel_range.ReleaseDispatch();
		
		if(cell_value.vt==VT_BSTR)
		{
			CString ss(cell_value.bstrVal);
#ifdef AFX_TARG_ENU_ENGLISH
			if(ss.CompareNoCase("Seg Num")==0)
#else
			if(ss.CompareNoCase("段号")==0)
#endif
			{
				for(j=0;1;j++)
				{
					cell_two[0]='A'+i;
					sprintf(&cell_two[1],"%d",j+2);//从第二行读取数据
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.ReleaseDispatch();
					if(cell_value.vt==VT_EMPTY)
						break;
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sSegNo(cell_value.bstrVal);
					pStatBolt=Bolts.append();
					pStatBolt->nSeg=atoi(sSegNo);
					record_bolt++;
					if(pStatBolt->nSeg>=0)
						hashSegI.SetValue(pStatBolt->nSeg+1,pStatBolt->nSeg);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Level Spec")==0)
#else
			else if(ss.CompareNoCase("级别规格")==0)
#endif
			{
				for(j=0;j<record_bolt;j++)
				{
					sprintf(&cell_two[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sSpecs(cell_value.bstrVal);
					pStatBolt=&Bolts[j];
					sprintf(pStatBolt->sSpec,sSpecs);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Amount")==0)
#else
			else if(ss.CompareNoCase("数量")==0)
#endif
			{
				for(j=0;j<record_bolt;j++)
				{
					sprintf(&cell_two[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sNum(cell_value.bstrVal);
					pStatBolt=&Bolts[j];
					pStatBolt->nCount=atoi(sNum);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Weight")==0)
#else
			else if(ss.CompareNoCase("重量")==0)
#endif
			{
				for(j=0;j<record_bolt;j++)
				{
					sprintf(&cell_two[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();

					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sWeight(cell_value.bstrVal);
					pStatBolt=&Bolts[j];
					pStatBolt->weight=atof(sWeight);
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Type")==0)
#else
			else if(ss.CompareNoCase("类型")==0)
#endif
			{
				for(j=0;j<record_bolt;j++)
				{
					sprintf(&cell_two[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sType(cell_value.bstrVal);
					pStatBolt=&Bolts[j];
					pStatBolt->type=atoi(sType);
					pStatBolt->part_type=2;
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(ss.CompareNoCase("Notes")==0)
#else
			else if(ss.CompareNoCase("备注")==0)
#endif
			{
				for(j=0;j<record_bolt;j++)
				{
					sprintf(&cell_two[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_two));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.DetachDispatch();
					pRange->Release();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sNote(cell_value.bstrVal);
					pStatBolt=&Bolts[j];
					sprintf(pStatBolt->sNote,sNote);
				}
			}
		}
	}
}
void CTowerBOM::ReadBodySheet(Range excel_range,_Worksheet excel_sheet)
{
	LEG_DESIGN* pBody=NULL;
	char cell_three[8]="A1";
	int record_body=0;
	LPDISPATCH pRange;	
	for(int i=0;i<2;i++)
	{
		cell_three[0]='A'+i;
		cell_three[1]='1';
		cell_three[2]='\0';
		pRange = excel_sheet.GetRange(COleVariant(cell_three));
		excel_range.AttachDispatch(pRange,FALSE);
		COleVariant cell_value=excel_range.GetValue();
		excel_range.ReleaseDispatch();
		
		if(cell_value.vt==VT_BSTR)
		{
			CString ss(cell_value.bstrVal);
#ifdef AFX_TARG_ENU_ENGLISH
			if(ss.CompareNoCase("Module Name")==0)
#else
			if(ss.CompareNoCase("呼高名称")==0)
#endif
			{
				for(int j=0;1;j++)
				{
					cell_three[0]='A'+i;
					sprintf(&cell_three[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_three));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.ReleaseDispatch();
					if(cell_value.vt==VT_EMPTY)
						break;
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString sBodyName(cell_value.bstrVal);
					pBody=bodyList.append();
					sprintf(pBody->sHeight,sBodyName);
					record_body++;
				}
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(ss.CompareNoCase("Seg Num Range")==0)
#else
			if(ss.CompareNoCase("段号范围")==0)
#endif
			{
				for(int j=0;j<record_body;j++)
				{
					cell_three[0]='A'+i;
					sprintf(&cell_three[1],"%d",j+2);
					pRange = excel_sheet.GetRange(COleVariant(cell_three));
					excel_range.AttachDispatch(pRange,FALSE);
					COleVariant cell_value=excel_range.GetValue();
					excel_range.ReleaseDispatch();
					
					if(cell_value.vt!=VT_BSTR)
						cell_value.ChangeType(VT_BSTR);
					CString bodySegStr(cell_value.bstrVal);
					pBody=&bodyList[j];
					sprintf(pBody->bodySegStr,bodySegStr);
				}
			}
		}
	}
}
//使用段号加1作为key，兼容段号为0的情况 
void CTowerBOM::GetSegCoefHashTblBySegStr(char *segStr,CHashList<long> &segCoefHashSegIAndOne)
{
	char delimiter1[10]=",";
	char delimiter2[10]="-";
	char delimiter3[10]="*";
	char *tempSegStr=NULL;
	if(segStr)
	{
		tempSegStr=new char[strlen(segStr)+1];
		strcpy(tempSegStr,segStr);
	}
	long iNo;
	if(segStr)
	{
		char *sKey;
		if(delimiter1)
			sKey=strtok(tempSegStr,delimiter1);
		else
			sKey=strtok(tempSegStr,",\n");
		while(sKey)
		{
			char *delimiter;
			if(sKey[1]=='-'||sKey[2]=='-')
			{
				if(delimiter2)
					delimiter=strchr(sKey,*delimiter2);
				else
					delimiter=strchr(sKey,'-');
				
				*delimiter=' ';
				int start_i,end_i;
				sscanf(sKey,"%d%d",&start_i,&end_i);
				for(iNo=start_i;iNo<=end_i;iNo++)
				{
					long key=iNo+1;	//段号加1作为key
					if(!segCoefHashSegIAndOne.GetValue(key))
						segCoefHashSegIAndOne.SetValue(key,1);
				}
			}
			else if(sKey[1]=='*'||sKey[2]=='*')
			{
				if(delimiter3)
					delimiter=strchr(sKey,*delimiter3);
				else
					delimiter=strchr(sKey,'*');
				
				*delimiter=' ';
				int nSeg=0,nSegCoef=1;
				sscanf(sKey,"%d%d",&nSeg,&nSegCoef);
				long key=nSeg+1;	//段号加1作为key
				if(!segCoefHashSegIAndOne.GetValue(key))
					segCoefHashSegIAndOne.SetValue(key,nSegCoef);
			}
			else
			{
				iNo=atoi(sKey);
				long key=iNo+1;	//段号加1作为key
				if(!segCoefHashSegIAndOne.GetValue(key))
					segCoefHashSegIAndOne.SetValue(key,1);
			}
			sKey=strtok(NULL,"+,\n");
		}
	}
	if(tempSegStr)
		delete []tempSegStr;
}
void CTowerBOM::StatTower(ATOM_LIST<CMatStatRecord>* pTowerMatStatList)
{
	CManuPart* pManuPart=NULL;
	CStatBolt* pBolt=NULL;
	CMatStatRecord *pRec = NULL;
	//1.根据manuPartArr、statBoltArr统计出当前模型段号数量nSegNum
	int i=0;
	for(int *pSegI=hashSegI.GetFirst();pSegI;pSegI=hashSegI.GetNext(),i++)
		*pSegI=i;
	int nSegNum=hashSegI.GetNodeNum();

	pTowerMatStatList->Empty();
	//统计各个呼高对应的段号系数哈希表
	typedef CHashList<long> HASH_LIST;
	ATOM_LIST<HASH_LIST> bodySegCoefsList;
	for(LEG_DESIGN* pBody=bodyList.GetFirst();pBody;pBody=bodyList.GetNext())
	{
		HASH_LIST *pSegCoefHashSegI=bodySegCoefsList.append();
		GetSegCoefHashTblBySegStr(pBody->bodySegStr,*pSegCoefHashSegI);
	}
	//工件汇总
	for(pManuPart=Parts.GetFirst();pManuPart;pManuPart=Parts.GetNext())
	{
		//根据Part.nSeg及pBodyArr统计出pPart从属的本体范围字符串body_str（其中
		//含统材系数uStatCoef,如：
		//"1,2*2,3*4"表示1号呼高中统计pPart->num件，2号呼高中统计pPart->num*2件, 
		// 3号呼高中统计pPart->num*4件）
		//......获取body_str
		int iBodyIndex=1;
		CXhChar100 body_str;
		for(HASH_LIST *pSegCoefHashSegI=bodySegCoefsList.GetFirst();pSegCoefHashSegI;
			pSegCoefHashSegI=bodySegCoefsList.GetNext(),iBodyIndex++)
		{	
			char ss[10]="";
			long key=pManuPart->nSeg+1;
			long *pSegCoef=pSegCoefHashSegI->GetValue(key);
			if(pSegCoef==NULL)//无此段号
				continue;
			//if(body_str.GetLength()>0)//已有数据
			//	sprintf(ss,",%d*%d",iBodyIndex,*pSegCoef);
			//else 
			if(*pSegCoef>1)
				sprintf(ss,"%d*%d",iBodyIndex,*pSegCoef);		
			else if(*pSegCoef==1)
				sprintf(ss,"%d",iBodyIndex);
			body_str.Append(ss,',');
		}
		if(body_str.GetLength()<=0)
			continue;
		int iType = 0;
		int iSegIndex=0;
		if (pManuPart->part_type==STEELTYPE_ANGLE)// 角钢 1
			iType=CLS_LINEANGLE;
		else if(pManuPart->part_type==STEELTYPE_PLATE)// 钢板 3
			iType=CLS_PLATE;
		else if(pManuPart->part_type==STEELTYPE_TUBE)// 钢管 4
			iType=CLS_LINETUBE;
		else if(pManuPart->part_type==STEELTYPE_FLAT)//扁铁 5
			iType=CLS_PLATE;
		else if(pManuPart->part_type==STEELTYPE_SLOT)// 槽钢 6
			iType=CLS_LINESLOT;
		if(pManuPart->nSeg<0)
			iSegIndex=0;
		else if(hashSegI.GetValue(pManuPart->nSeg+1))
			iSegIndex=hashSegI[pManuPart->nSeg+1];
		for(pRec=pTowerMatStatList->GetFirst();pRec;pRec=pTowerMatStatList->GetNext())
		{	//进行匹配
			if(GetClassTypeOrder(iType)!=pRec->idClassTypeOrder)
				continue;
			if(pRec->cMaterial!=pManuPart->cMaterial)
				continue;
			CXhChar50 spec(pManuPart->sSpec);
			if(iType==CLS_PLATE)
			{
				char* separator=strchr(spec,'X');
				if(separator==NULL)
					separator=strchr(spec,'x');
				if(separator==NULL)
					separator=strchr(spec,'*');
				if(separator)
					*separator=0;	//钢板在材料汇总表中的规格只需要板厚即可
			}

			if(stricmp(spec,pRec->spec)!=0)
				continue;
			pRec->AddSegWeight(iSegIndex,pManuPart->sumWeight);
			pRec->AddBodyWeight(body_str,pManuPart->sumWeight);
			break;
		}
		if(pRec==NULL)
		{
			pRec=pTowerMatStatList->append();
			pRec->idClassTypeOrder=GetClassTypeOrder(iType);
			pRec->cMaterial=QuerySteelBriefMatMark(pManuPart->sMaterial);
			strcpy(pRec->spec,pManuPart->sSpec);
			if(iType==CLS_PLATE)
			{
				char* separator=strchr(pRec->spec,'X');
				if(separator==NULL)
					separator=strchr(pRec->spec,'x');
				if(separator==NULL)
					separator=strchr(pRec->spec,'*');
				if(separator)
					*separator=0;	//钢板在材料汇总表中的规格只需要板厚即可
			}
			pRec->Create(nSegNum,bodyList.GetNodeNum());
			pRec->AddSegWeight(iSegIndex,pManuPart->sumWeight);
			pRec->AddBodyWeight(body_str,pManuPart->sumWeight);
		}
	}
	//螺栓汇总
	for(pBolt=Bolts.GetFirst();pBolt;pBolt=Bolts.GetNext())
	{
		int iBodyIndex=1;
		int iSegIndex=0;
		CXhChar100 body_str;
		for(CHashList<long> *pSegCoefHashSegI=bodySegCoefsList.GetFirst();pSegCoefHashSegI;
			pSegCoefHashSegI=bodySegCoefsList.GetNext())
		{	
			char ss[10];
			long key=pBolt->nSeg+1;	//段号加1作为键值
			long *pSegCoef=pSegCoefHashSegI->GetValue(key);
			if(pSegCoef==NULL)//无此段号
				continue;
			if(strlen(body_str)>0)//已有数据
				sprintf(ss,",%d*%d",iBodyIndex,*pSegCoef);
			else
				sprintf(ss,"%d*%d",iBodyIndex,*pSegCoef);		
			strcat(body_str,ss);
			iBodyIndex++;
		}
		if(body_str.GetLength()<=0)
			continue;
		if(pBolt->nSeg<0)
			iSegIndex=0;
		else if(hashSegI.GetValue(pBolt->nSeg+1))
			iSegIndex=hashSegI[pBolt->nSeg+1];
		for(pRec=pTowerMatStatList->GetFirst();pRec;pRec=pTowerMatStatList->GetNext())
		{
			if(4!=pRec->idClassTypeOrder)
				continue;	//螺栓
			if(stricmp(pBolt->sSpec,pRec->spec)!=0||pBolt->cMaterialSymbol!=pRec->cMaterial)
				continue;
			if(iSegIndex>=0)
				pRec->AddSegWeight(iSegIndex,pBolt->weight);
			pRec->AddBodyWeight(body_str,pBolt->weight);
			break;
		}
		if(pRec==NULL)
		{
			pRec=pTowerMatStatList->append();
			pRec->cMaterial=pBolt->cMaterialSymbol;

			pRec->idClassTypeOrder=4;
			strcat(pRec->spec,pBolt->sSpec);
			pRec->Create(nSegNum,bodyList.GetNodeNum());
			if(iSegIndex>=0)
				pRec->AddSegWeight(iSegIndex,pBolt->weight);
			pRec->AddBodyWeight(body_str,pBolt->weight);
		}
	}
}
