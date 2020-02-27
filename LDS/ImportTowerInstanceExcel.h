#include "stdafx.h"
#include "excel9.h"
#include "Variant.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "exceloper.h"

const int TOWER_POS_COL_COUNT				= 2;
const int TOWER_LEG_EXCEL_COL_COUNT			= 4;
const int TOWER_LEG_EXCEL_SUB_COL_COUNT		= 3;
const int TOWER_INSTANCE_EXCEL_COL_COUNT	= 20;
const char* TITLE_INSTANCE_NO				= "����";
const char* TITLE_TOWERTYPE					= "����";
const char* TITLE_MODULE					= "��������";
const char* TITLE_GAOCHA					= "�������׮�߲�";
const char* TITLE_ZUOBIAO					= "����";
const char* TITLE_DONGZB					= "������";
const char* TITLE_BEIZB						= "������";
const char* TITLE_GAOCHENG					= "�߳�";
const char* TITLE_LEG_ARR[4]				={"����I","����II","����III","����IV"};
const char* TITLE_FOUNDATION_TYPE			= "�������";
const char* TITLE_FOUNDATION_NAME			= "�����ͺ�";
const char* TITLE_LEG_TYPE					= "�����ͺ�";
const char* TITLE_NOTE						= "��ע";
//
CXhChar100 VariantToString(VARIANT value)
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
//////////////////////////////////////////////////////////////////////////
//
struct TOWER_INSTANCE_INFO{
	CXhChar100 sTowerInstanceNo;		//����
	CXhChar100 sTowerType;				//����
	CXhChar100 sModule;					//��������
	CXhChar100 sGaoCha;					//�������׮�߲�
	CXhChar100 sPosY;					//������
	CXhChar100 sPosX;					//������
	CXhChar100 sGaoCheng;				//�߳�
	CXhChar100 sNote;					//��ע
	CXhChar100 sFoundationTypeArr[4];	//�������
	CXhChar100 sFoundationNameArr[4];	//�����ͺ�
	CXhChar100 sLegArr[4];				//�����ͺ�
	//
	CXhChar50  subLegHeightArr[4];
	BYTE       quaArr[4];
	TOWER_INSTANCE_INFO(){quaArr[0]=quaArr[1]=quaArr[2]=quaArr[3]=0;}
	void CloneTowerInsInfo(TOWER_INSTANCE_INFO* pNewInfo);
	BOOL IsVailable();
};
class CTowerInstance
{
	struct TOWER_INSTANCE_SHEET{
		CXhChar100 sName;
		ATOM_LIST<TOWER_INSTANCE_INFO> towerInstanceList;
	};	
	struct MODULE_INFO
	{
		CXhChar100 sModuleName;
		CHashStrList<CXhChar50> legStrList;
	};
	struct CLoftingInfo 
	{
		CXhChar100 sTowerTypeName;
		CHashStrList<MODULE_INFO> moduleInfoList;
	};
public:
	ATOM_LIST<TOWER_INSTANCE_SHEET> towerInstSheetList;
	CHashStrList<CLoftingInfo> hashLoftingInfoList;
protected:
	void GetSubLegHeightAndQuaFromStr(const char* infoStr,TOWER_INSTANCE_INFO* pTowerInst,int iLeg);
	void AddWorkPartsSheet(LPDISPATCH pSheet,const char* sSheetName);
	void AddTowerInstSheet(LPDISPATCH pSheet,TOWER_INSTANCE_SHEET* pTowerInstSheet);
	BOOL ParseTowerInstanceContent(CVariant2dArray &sheetContentMap,ATOM_LIST<TOWER_INSTANCE_INFO>& towerInstanceList);
	void InitTowerLoftingTask(TOWER_INSTANCE_SHEET* pTowerInstSheet);
public:
	BOOL ImportTowerInstanceExcelFile(const char* m_sFileName);
	void ExportTowerInstanceExcelFile();
	void ExportLoftingTaseExcelFile();
};
void TOWER_INSTANCE_INFO::CloneTowerInsInfo(TOWER_INSTANCE_INFO* pNewInfo)
{
	sTowerInstanceNo.Copy(pNewInfo->sTowerInstanceNo);
	sTowerType.Copy(pNewInfo->sTowerType);
	sModule.Copy(pNewInfo->sModule);
	sGaoCha.Copy(pNewInfo->sGaoCha);
	sPosY.Copy(pNewInfo->sPosY);
	sPosX.Copy(pNewInfo->sPosX);
	sGaoCheng.Copy(pNewInfo->sGaoCheng);
	sNote.Copy(pNewInfo->sNote);
	for(int i=0;i<4;i++)
	{
		sLegArr[i].Copy(pNewInfo->sLegArr[i]);
		sFoundationTypeArr[i].Copy(pNewInfo->sFoundationTypeArr[i]);
		sFoundationNameArr[i].Copy(pNewInfo->sFoundationNameArr[i]);
		subLegHeightArr[i].Copy(pNewInfo->subLegHeightArr[i]);
		quaArr[i]=pNewInfo->quaArr[i];
	}
}
BOOL TOWER_INSTANCE_INFO::IsVailable()
{
	for(int i=0;i<4;i++)
	{
		double fLenH=atof(subLegHeightArr[i]);
		if(fLenH==0)
			return TRUE;
	}
	return FALSE;
}
//��ȡ������Ϣ
void CTowerInstance::GetSubLegHeightAndQuaFromStr(const char* infoStr,TOWER_INSTANCE_INFO* pTowerInst,int i)
{
	if(strlen(infoStr)<=0)
		return;
	CXhChar100 strValue(infoStr);
	strValue.Replace(pTowerInst->sTowerType,"A");
	int x=0;
	double fModule=0.0,fLegH=0.0;
	int iQua=0;
	for(char* token=strtok(strValue,"-");token;token=strtok(NULL,"-"))
	{
		if(x==1)
			fModule=atof(token);
		if(x==2)
			fLegH=atof(token);
		else if(x==3)
			iQua=atoi(token);
		x++;
	}
	double fDatumModule=atof(pTowerInst->sModule);
	if(fDatumModule!=fModule)
		fLegH+=fDatumModule-fModule;	
	pTowerInst->sLegArr[i].Printf("%s-%0.2f-%0.2f-%d",(char*)pTowerInst->sTowerType,atof(pTowerInst->sModule),fLegH,iQua);
	pTowerInst->subLegHeightArr[i].Printf("%0.2f",fLegH);
	pTowerInst->quaArr[i]=iQua;
}
//����������λ�ɹ�Excel����ȡ��λ��Ϣ
BOOL CTowerInstance::ParseTowerInstanceContent(CVariant2dArray &sheetContentMap,ATOM_LIST<TOWER_INSTANCE_INFO>& towerInstanceList)
{
	if(sheetContentMap.RowsCount()<1)
		return FALSE;
	towerInstanceList.Empty();
	//1.�����б�����������ӳ���hashColIndexByColTitle
	CHashStrList<DWORD> hashColIndexByColTitle;
	for(int i=0;i<TOWER_INSTANCE_EXCEL_COL_COUNT;i++)
	{
		VARIANT value;
		sheetContentMap.GetValueAt(0,i,value);
		if(value.vt==VT_EMPTY)
			continue;
		if(CString(value.bstrVal).CompareNoCase(TITLE_INSTANCE_NO)==0) 
			hashColIndexByColTitle.SetValue(TITLE_INSTANCE_NO,i);
		else if(CString(value.bstrVal).CompareNoCase(TITLE_TOWERTYPE)==0)
			hashColIndexByColTitle.SetValue(TITLE_TOWERTYPE,i);
		else if(CString(value.bstrVal).CompareNoCase(TITLE_MODULE)==0)
			hashColIndexByColTitle.SetValue(TITLE_MODULE,i);
		else if(CString(value.bstrVal).CompareNoCase(TITLE_GAOCHA)==0)
			hashColIndexByColTitle.SetValue(TITLE_GAOCHA,i);
		else if(CString(value.bstrVal).Find(TITLE_GAOCHENG)==0)
			hashColIndexByColTitle.SetValue(TITLE_GAOCHENG,i);
		else if(CString(value.bstrVal).CompareNoCase(TITLE_NOTE)==0)
			hashColIndexByColTitle.SetValue(TITLE_NOTE,i);
		else if(CString(value.bstrVal).Find(TITLE_ZUOBIAO)==0)
		{
			for(int m=0;m<2;m++)
			{
				sheetContentMap.GetValueAt(1,i+m,value);
				if(CString(value.bstrVal).Find(TITLE_DONGZB)==0)
					hashColIndexByColTitle.SetValue(TITLE_DONGZB,i+m);
				else if(CString(value.bstrVal).Find(TITLE_BEIZB)==0)
					hashColIndexByColTitle.SetValue(TITLE_BEIZB,i+m);
			}
		}
		else
		{
			for(int j=0;j<TOWER_LEG_EXCEL_COL_COUNT;j++)
			{
				if(CString(value.bstrVal).CompareNoCase(TITLE_LEG_ARR[j])!=0)
					continue;
				for (int k=0;k<TOWER_LEG_EXCEL_SUB_COL_COUNT;k++)
				{
					sheetContentMap.GetValueAt(1,i+k,value);
					if(CString(value.bstrVal).CompareNoCase(TITLE_LEG_TYPE)==0)
						hashColIndexByColTitle.SetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_LEG_TYPE),i+k);
					else if(CString(value.bstrVal).CompareNoCase(TITLE_FOUNDATION_TYPE)==0)
						hashColIndexByColTitle.SetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_FOUNDATION_TYPE),i+k);
					else if(CString(value.bstrVal).CompareNoCase(TITLE_FOUNDATION_NAME)==0)
						hashColIndexByColTitle.SetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_FOUNDATION_NAME),i+k);
				}
			}
		}
	}
	if(hashColIndexByColTitle.GetNodeNum()!=TOWER_INSTANCE_EXCEL_COL_COUNT)
		return FALSE;
	//2.��ȡ������Ϣ���洢�������б�
	for(int i=2;i<sheetContentMap.RowsCount();i++)
	{
		VARIANT value;
		DWORD *pColIndex=hashColIndexByColTitle.GetValue(TITLE_TOWERTYPE);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		if(value.vt==VT_EMPTY)
			continue;
		TOWER_INSTANCE_INFO insInfo;
		//����
		insInfo.sTowerType=VariantToString(value);		
		//����
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_INSTANCE_NO);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sTowerInstanceNo=VariantToString(value);
		//����
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_MODULE);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sModule=VariantToString(value);
		//����׮�߲�
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_GAOCHA);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sGaoCha=VariantToString(value);
		//Y����
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_DONGZB);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sPosY=VariantToString(value);
		//X����
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BEIZB);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sPosX=VariantToString(value);
		//�߳�
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_GAOCHENG);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sGaoCheng=VariantToString(value);
		//��ע
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_NOTE);
		sheetContentMap.GetValueAt(i,*pColIndex,value);
		insInfo.sNote=VariantToString(value);
		//
		for(int j=0;j<TOWER_LEG_EXCEL_COL_COUNT;j++)
		{	//����
			pColIndex=hashColIndexByColTitle.GetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_LEG_TYPE));
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			GetSubLegHeightAndQuaFromStr(VariantToString(value),&insInfo,j);
			//�������
			pColIndex=hashColIndexByColTitle.GetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_FOUNDATION_TYPE));
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			insInfo.sFoundationTypeArr[j]=VariantToString(value);
			//�����ͺ�
			pColIndex=hashColIndexByColTitle.GetValue(CXhChar50("%s#%s",TITLE_LEG_ARR[j],TITLE_FOUNDATION_NAME));
			sheetContentMap.GetValueAt(i,*pColIndex,value);
			insInfo.sFoundationNameArr[j]=VariantToString(value);
		}
		towerInstanceList.append(insInfo);
	}
	if(towerInstanceList.GetNodeNum()<1)
		return FALSE;
	return TRUE;
}
//������λ�ɹ���
BOOL CTowerInstance::ImportTowerInstanceExcelFile(const char* sFileName)
{
	LPDISPATCH pRange=NULL,pWorksheet=NULL,pWorksheets=NULL;
	pWorksheets=CExcelOper::OpenExcelWorksheets(sFileName);
	ASSERT(pWorksheets != NULL);
	Sheets       excel_sheets;	//��������
	excel_sheets.AttachDispatch(pWorksheets);
	int nSheetNum=excel_sheets.GetCount();
	if(nSheetNum<1)
	{
		excel_sheets.ReleaseDispatch();
		return FALSE;
	}
	int index=1;
	for(int iSheet=1;iSheet<=nSheetNum;iSheet++)
	{
		pWorksheet=excel_sheets.GetItem(COleVariant((short) iSheet));
		_Worksheet  excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet);
		excel_sheet.Select();
		//1.��ȡExcelָ��Sheet���ݴ洢��sheetContentMap
		Range excel_usedRange,excel_range;
		excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
		excel_range.AttachDispatch(excel_usedRange.GetRows());
		long nRowNum = excel_range.GetCount();
		excel_range.AttachDispatch(excel_usedRange.GetColumns());
		long nColNum = excel_range.GetCount();
		if(nColNum<TOWER_INSTANCE_EXCEL_COL_COUNT)
			continue;
		char cell[20]="";
		sprintf(cell,"%C%d",'A'+TOWER_INSTANCE_EXCEL_COL_COUNT-1,nRowNum);
		pRange = excel_sheet.GetRange(COleVariant("A1"),COleVariant(cell));
		excel_range.AttachDispatch(pRange,FALSE);
		CVariant2dArray sheetContentMap(1,1);
		sheetContentMap.var=excel_range.GetValue();
		excel_range.ReleaseDispatch();
		
		//2������
		ATOM_LIST<TOWER_INSTANCE_INFO> towerInstanceList;
		if(!ParseTowerInstanceContent(sheetContentMap,towerInstanceList))
		{
			logerr.Log("%s��ʽ����",excel_sheet.GetName());
			excel_sheet.ReleaseDispatch();
			continue;
		}
		TOWER_INSTANCE_SHEET* pSheet=towerInstSheetList.append();
		pSheet->sName.Copy(excel_sheet.GetName());
		for(TOWER_INSTANCE_INFO* pInfo=towerInstanceList.GetFirst();pInfo;pInfo=towerInstanceList.GetNext())
		{
			TOWER_INSTANCE_INFO* pNewInfo=pSheet->towerInstanceList.append();
			pNewInfo->CloneTowerInsInfo(pInfo);
		}
		excel_sheet.ReleaseDispatch();
	}
	excel_sheets.ReleaseDispatch();
	return TRUE;
}
//��ʼ����λ���߽�����Ϣ
void CTowerInstance::InitTowerLoftingTask(TOWER_INSTANCE_SHEET* pTowerInstSheet)
{
	hashLoftingInfoList.Empty();
	for(TOWER_INSTANCE_INFO* pInfo=pTowerInstSheet->towerInstanceList.GetFirst();pInfo;pInfo=pTowerInstSheet->towerInstanceList.GetNext())
	{
		CLoftingInfo* pLoftingInfo=NULL;
		if(hashLoftingInfoList.GetValue(pInfo->sTowerType)==NULL)
			pLoftingInfo=hashLoftingInfoList.Add(pInfo->sTowerType);
		else
			pLoftingInfo=hashLoftingInfoList.GetValue(pInfo->sTowerType);
		pLoftingInfo->sTowerTypeName=pInfo->sTowerType;
		//
		MODULE_INFO* pModuleInfo=NULL;
		if(pLoftingInfo->moduleInfoList.GetValue(pInfo->sModule)==NULL)
			pModuleInfo=pLoftingInfo->moduleInfoList.Add(pInfo->sModule);
		else
			pModuleInfo=pLoftingInfo->moduleInfoList.GetValue(pInfo->sModule);
		pModuleInfo->sModuleName=pInfo->sModule;
		//
		for(int i=0;i<4;i++)
		{
			if(pInfo->subLegHeightArr[i].Length()<=0)
				continue;
			CXhChar50* pLegStr=NULL;
			if(pModuleInfo->legStrList.GetValue(pInfo->subLegHeightArr[i]))
				continue;
			pLegStr=pModuleInfo->legStrList.Add(pInfo->subLegHeightArr[i]);
			pLegStr->Copy(pInfo->subLegHeightArr[i]);
		}
	}
}

//�������������
void CTowerInstance::AddWorkPartsSheet(LPDISPATCH pSheet,const char* sSheetName)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName(sSheetName);
	CExcelOper::SetMainTitle(excel_sheet,"A1","E1","�������������");
	//
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int nLineCount=0;
	for(CLoftingInfo* pLoftingInfo=hashLoftingInfoList.GetFirst();pLoftingInfo;pLoftingInfo=hashLoftingInfoList.GetNext())
		nLineCount+=pLoftingInfo->moduleInfoList.GetNodeNum();
	CVariant2dArray map(nLineCount+1,7);
	//��һ�б���
	int index=0;
	map.SetValueAt(index,0,COleVariant("��������"));
	map.SetValueAt(index,1,COleVariant("������"));
	map.SetValueAt(index,2,COleVariant("������"));
	map.SetValueAt(index,3,COleVariant("������"));
	map.SetValueAt(index,4,COleVariant("����ʱ��"));
	CExcelOper::SetRangeColWidth(excel_sheet,15,"A2",NULL);
	CExcelOper::SetRangeColWidth(excel_sheet,30,"C2",NULL);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2","E2",COleVariant((short)3));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2","E2",COleVariant((short)2));
	//�������
	index++;
	int start_i=1;
	for(CLoftingInfo* pInfo=hashLoftingInfoList.GetFirst();pInfo;pInfo=hashLoftingInfoList.GetNext())
	{
		start_i=index+2;
		int nModules=pInfo->moduleInfoList.GetNodeNum();
		_snprintf(cell_start,15,"A%d",start_i);
		_snprintf(cell_end,15,"A%d",start_i+nModules-1);
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//�ϲ���
		map.SetValueAt(index,0,COleVariant(pInfo->sTowerTypeName));
		//
		_snprintf(cell_start,15,"D%d",start_i);
		_snprintf(cell_end,15,"D%d",start_i+nModules-1);
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//�ϲ���
		map.SetValueAt(index,3,COleVariant(""));
		//
		_snprintf(cell_start,15,"E%d",start_i);
		_snprintf(cell_end,15,"E%d",start_i+nModules-1);
		CExcelOper::MergeRowRange(excel_sheet,cell_start,cell_end);	//�ϲ���
		map.SetValueAt(index,4,COleVariant(""));
		//
		for(MODULE_INFO* pModuleInfo=pInfo->moduleInfoList.GetFirst();pModuleInfo;
			pModuleInfo=pInfo->moduleInfoList.GetNext())
		{
			map.SetValueAt(index,1,COleVariant(pModuleInfo->sModuleName));
			CString leg_str;
			for(CXhChar50* pLegStr=pModuleInfo->legStrList.GetFirst();pLegStr;pLegStr=pModuleInfo->legStrList.GetNext())
			{
				if(leg_str.GetLength()>0)
					leg_str+=",";
				CXhChar50 str("-%s",(char*)*pLegStr);
				leg_str+=str;
			}
			map.SetValueAt(index,2,COleVariant((LPCTSTR)leg_str));
			index++;
		}
	}
	//���ö��뷽ʽ
	sprintf(cell_end,"E%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)3));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A2",cell_end,COleVariant((short)2));
	sprintf(cell_end,"C%d",index+1);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"C3",cell_end,COleVariant((short)2));
	//���ñ������
	sprintf(cell_end,"E%d",index+1);
	CExcelOper::SetRangeValue(excel_sheet,"A2",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A2",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
//�������������
void CTowerInstance::ExportLoftingTaseExcelFile()
{
	int nSheet=towerInstSheetList.GetNodeNum();
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(nSheet);
	ASSERT(pWorksheets!= NULL);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//
	int iSheet=1;
	for(TOWER_INSTANCE_SHEET* pTowerInsSheet=towerInstSheetList.GetFirst();pTowerInsSheet;pTowerInsSheet=towerInstSheetList.GetNext())
	{
		InitTowerLoftingTask(pTowerInsSheet);
		LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short)iSheet));
		AddWorkPartsSheet(pWorksheet,pTowerInsSheet->sName);
		iSheet++;
	}
	excel_sheets.ReleaseDispatch();
}
//�����λ�ɹ���sheet
void CTowerInstance::AddTowerInstSheet(LPDISPATCH pSheet,TOWER_INSTANCE_SHEET* pTowerInsSheet)
{
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pSheet,FALSE);
	excel_sheet.Select();
	excel_sheet.SetName(pTowerInsSheet->sName);
	//
	char cell_start[16]="A1";
	char cell_end[16]="A1";
	int nLineCount=pTowerInsSheet->towerInstanceList.GetNodeNum()+2;
	CVariant2dArray map(nLineCount,TOWER_INSTANCE_EXCEL_COL_COUNT);
	//�����б���
	CString sValue;
	int col_i=0;
	CExcelOper::MergeRowRange(excel_sheet,"A1","A2");	//�ϲ���
	map.SetValueAt(0,col_i++,COleVariant(TITLE_INSTANCE_NO));
	CExcelOper::MergeRowRange(excel_sheet,"B1","B2");	//�ϲ���
	map.SetValueAt(0,col_i++,COleVariant(TITLE_TOWERTYPE));
	CExcelOper::MergeRowRange(excel_sheet,"C1","C2");	//�ϲ���
	map.SetValueAt(0,col_i++,COleVariant(TITLE_MODULE));
	CExcelOper::MergeRowRange(excel_sheet,"D1","D2");	//�ϲ���
	map.SetValueAt(0,col_i++,COleVariant(TITLE_GAOCHA));
	CExcelOper::MergeColRange(excel_sheet,"E1","F1");	//�ϲ���
	sValue.Format("%s(m)",TITLE_ZUOBIAO);
	map.SetValueAt(0,col_i,COleVariant(sValue));
	sValue.Format("%s(Y)",TITLE_DONGZB);
	map.SetValueAt(1,col_i++,COleVariant(sValue));
	sValue.Format("%s(X)",TITLE_BEIZB);
	map.SetValueAt(1,col_i++,COleVariant(sValue));
	CExcelOper::MergeRowRange(excel_sheet,"G1","G2");	//�ϲ���
	sValue.Format("%s(m)",TITLE_GAOCHENG);
	map.SetValueAt(0,col_i++,COleVariant(sValue));
	for(int i=0;i<4;i++)
	{
		_snprintf(cell_start,15,"%C1",'A'+col_i);
		_snprintf(cell_end,15,"%C1",'A'+col_i+2);
		CExcelOper::MergeColRange(excel_sheet,cell_start,cell_end);	//�ϲ���
		map.SetValueAt(0,col_i,COleVariant(TITLE_LEG_ARR[i]));
		map.SetValueAt(1,col_i++,COleVariant(TITLE_LEG_TYPE));
		map.SetValueAt(1,col_i++,COleVariant(TITLE_FOUNDATION_TYPE));
		map.SetValueAt(1,col_i++,COleVariant(TITLE_FOUNDATION_NAME));
	}
	CExcelOper::MergeRowRange(excel_sheet,"T1","T2");
	map.SetValueAt(0,col_i,COleVariant(TITLE_NOTE));

	CExcelOper::SetRangeColWidth(excel_sheet,15,"H2","S2",NULL);
	//�������
	int index=2;
	for(TOWER_INSTANCE_INFO* pInfo=pTowerInsSheet->towerInstanceList.GetFirst();pInfo;pInfo=pTowerInsSheet->towerInstanceList.GetNext())
	{
		col_i=0;
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sTowerInstanceNo));
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sTowerType));
		map.SetValueAt(index,col_i++,COleVariant(atof(pInfo->sModule)));
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sGaoCha));
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sPosY));
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sPosX));
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sGaoCheng));
		for(int i=0;i<4;i++)
		{
			map.SetValueAt(index,col_i++,COleVariant(pInfo->sLegArr[i]));
			map.SetValueAt(index,col_i++,COleVariant(pInfo->sFoundationTypeArr[i]));
			map.SetValueAt(index,col_i++,COleVariant(pInfo->sFoundationNameArr[i]));
		}
		map.SetValueAt(index,col_i++,COleVariant(pInfo->sNote));
		if(!pInfo->IsVailable())
			logerr.Log("%s�µĵ�%d�м�����Ϣ����!",(char*)pTowerInsSheet->sName,index+1);
		index++;
	}
	sprintf(cell_end,"C%d",index);
	CExcelOper::SetRangeNumberFormatLocal(excel_sheet,"C3",cell_end,COleVariant("0.00"));
	//���ö��뷽ʽ
	sprintf(cell_end,"T%d",index);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"A1",cell_end,COleVariant((short)3));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"A1",cell_end,COleVariant((short)2));
	sprintf(cell_end,"H%d",index);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"H3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"H3",cell_end,COleVariant((short)2));
	sprintf(cell_end,"K%d",index);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"K3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"K3",cell_end,COleVariant((short)2));
	sprintf(cell_end,"N%d",index);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"N3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"N3",cell_end,COleVariant((short)2));
	sprintf(cell_end,"Q%d",index);
	CExcelOper::SetRangeHorizontalAlignment(excel_sheet,"Q3",cell_end,COleVariant((short)2));
	CExcelOper::SetRangeVerticalAlignment(excel_sheet,"Q3",cell_end,COleVariant((short)2));
	//���ñ������
	sprintf(cell_end,"T%d",index);
	CExcelOper::SetRangeValue(excel_sheet,"A1",cell_end,map.var);
	//���ñ߿�
	CExcelOper::SetRangeBorders(excel_sheet,"A1",cell_end,COleVariant(10.5));
	excel_sheet.DetachDispatch();
}
//������λ�ɹ���
void CTowerInstance::ExportTowerInstanceExcelFile()
{
	int nSheet=towerInstSheetList.GetNodeNum();;
	LPDISPATCH pWorksheets=CExcelOper::CreateExcelWorksheets(nSheet);
	ASSERT(pWorksheets!= NULL);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	//
	int iSheet=1;
	for(TOWER_INSTANCE_SHEET* pTowerInsSheet=towerInstSheetList.GetFirst();pTowerInsSheet;pTowerInsSheet=towerInstSheetList.GetNext())
	{
		LPDISPATCH pWorksheet=excel_sheets.GetItem(COleVariant((short)iSheet));
		AddTowerInstSheet(pWorksheet,pTowerInsSheet);						
		iSheet++;
	}
	excel_sheets.ReleaseDispatch();
}
//////////////////////////////////////////////////////////////////////////
//�����������������
void ExportLoftingTaseExcelFile()
{
	CLogErrorLife logErrLift;
	CFileDialog fieDlg(TRUE,"xls","��λ�ɹ���",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��λ�ɹ���(*.xls)|*.xls|All Files(*.*)|*.*||");
	CString file_name;
	if(fieDlg.DoModal()==IDOK)
		file_name=fieDlg.GetPathName();
	//������λ�ɹ��������������������
	CTowerInstance towerIns;
	if(towerIns.ImportTowerInstanceExcelFile(file_name)==FALSE)
	{
		AfxMessageBox("�������λ�ɹ�������!");
		return;
	}
	towerIns.ExportLoftingTaseExcelFile();	
}
//������λ�ɹ���
void CorrectTowerInsExcelFile()
{
	CLogErrorLife logErrLift;
	CFileDialog fieDlg(TRUE,"xls","��λ�ɹ���",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��λ�ɹ���(*.xls)|*.xls|All Files(*.*)|*.*||");
	CString file_name;
	if(fieDlg.DoModal()==IDOK)
		file_name=fieDlg.GetPathName();
	//������λ�ɹ�����鲢��������Ľ�����Ϣ
	CTowerInstance towerIns;
	if(towerIns.ImportTowerInstanceExcelFile(file_name)==FALSE)
	{
		AfxMessageBox("�������λ�ɹ�������!");
		return;
	}
	towerIns.ExportTowerInstanceExcelFile();
}
