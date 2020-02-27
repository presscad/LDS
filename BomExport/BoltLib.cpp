#include "StdAfx.h"
#include "BoltLib.h"
#include "excel9.h"
#include "ExcelOper.h"




double CBoltLib::FindWeight(int d,int L,BOOL bTwoCap)
{
	BOLT_LIB_REC *pBolt=NULL;
	BOLT_LIB_REC *pBoltFromD=NULL;
	int minD = 0;
	for (pBolt=m_xMapBoltRec.GetFirst();pBolt;pBolt=m_xMapBoltRec.GetNext())
	{
		if (minD == 0)
			minD = abs(pBolt->L - L);
		if (pBolt->d == d && abs(pBolt->L - L) < minD)
		{
			pBoltFromD = pBolt;
			minD = abs(pBolt->L - L);
		}
		if (pBolt->d==d&&pBolt->L==L)break;
	}
	//不包含此直径则返回0
	double fWeight=0;
	if (pBolt!=NULL&&bTwoCap)
			return pBolt->m_fWeight2;
	else if(pBolt!=NULL && !bTwoCap)
			return pBolt->m_fWeight;
	else if(pBoltFromD!=NULL)
	{
		double fExcessLen=L-pBoltFromD->L;
		double *pUnitWeight=m_xHashWeightByD.GetValue(d);
		if(pUnitWeight==NULL)
			return fWeight;
		if (bTwoCap)
			fWeight=pBoltFromD->m_fWeight2+(*pUnitWeight)*fExcessLen/5;
		else
			fWeight=pBoltFromD->m_fWeight +(*pUnitWeight)*fExcessLen/5;
	}
	return fWeight;
}
double CBoltLibSet::FindWeight(BOMBOLT &bolt, BYTE cBaseBoltType)
{
	BYTE cType=CBoltLibSet::TYPE_COMMON_L;
	if (bolt.IsDianQuan() && bolt.nSpringGasket > 0)
		cType = CBoltLibSet::TYPE_DIANQUAN_SPRING;
	else if (bolt.IsDianQuan())
		cType = CBoltLibSet::TYPE_DIANQUAN;
	else if (bolt.bFootNail)
	{
		if (bolt.nCommonNut == 3 && bolt.bAntiTheft)
		{

		}
		else if (bolt.nCommonNut == 3 && bolt.bAnitLoose)
		{

		}
		else if (bolt.nCommonNut == 3)
			cType = CBoltLibSet::TYPE_FOOTNAIL_THREE_CAP;
		else if (bolt.bAntiTheft)
			cType = CBoltLibSet::TYPE_FOOTNAIL_TWO_CAP_T;
		else
			cType = CBoltLibSet::TYPE_FOOTNAIL_TWO_CAP_L;
	}
	else if (cBaseBoltType == TYPE_THIN_NUT && bolt.bAntiTheft)
		cType = CBoltLibSet::TYPE_COMMON_T_THIN;
	else if (bolt.bAntiTheft)
		cType=CBoltLibSet::TYPE_COMMON_T;
	/*else if(bolt.nThinNut>0)
		cType = CBoltLibSet::TYPE_COMMON_T_FASTEN;*/
	return FindWeight(bolt.d,bolt.L,cType,bolt.bTwoCap);

}
double CBoltLibSet::FindWeight(int d,int L,BYTE cType,BOOL bTwoCap)
{
	double fWeight=0;
	CBoltLib *pBoltLib = m_xHashBoltsLib.GetValue(cType);
	if(pBoltLib!=NULL)
	{
		fWeight=pBoltLib->FindWeight(d,L,bTwoCap);
		return fWeight;
	}
	else
		return 0;
}
class CSheetTitleRow
{
public:
	static const int COMMON_L			=3;
	static const int COMMON_T			=3;
	static const int COMMON_T_FASTEN	=3;
	static const int COMMON_T_THIN		=3;
	static const int FOOTNAIL_TWO_CAP_T	=3;
	static const int FOOTNAIL_TWO_CAP_L	=3;
	static const int FOOTNAIL_THREE_CAP	=3;
	static const int DIANQUAN			=3;
	static const int DIANQUAN_SPRING	=2;
};

const char* TITLE_BOLT_SPEC					= "规格";
const char* TITLE_BOLT_WEIGHT1				= "配一母|理重|单个重量|配3螺母|配三母|单件重量";
const char* TITLE_BOLT_WEIGHT2				= "配二母";
const char* TITLE_BOLT_WEIGHT_ADD 			= "每5mm重量|每5mm的质量";
static void SplitSaec(BOLT_LIB_REC *pBolt)
{
	CXhChar16 sSpec;
	sSpec=pBolt->m_sSpec;
	sSpec.Replace("M","");
	sSpec.Replace("*"," ");
	sSpec.Replace("x"," ");
	sSpec.Replace("X"," ");
	sSpec.Replace("×"," ");
	int d=0,L=0;
	sscanf((char*)sSpec,"%d %d",&d,&L);
	pBolt->d=d;
	pBolt->L=L;
};
static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue="";
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
static BOOL CStringFindOneOfChar(CString str,const char* str_split)
{
	BOOL bFind=FALSE;
	const char *pDelimiter="|";
	CXhChar100 sSplit(str_split);
	char *pStr=strtok(sSplit,pDelimiter);
	while (pStr)
	{
		if (str.Find(pStr)>=0)
		{
			bFind=TRUE;
			break;
		}
		pStr=strtok(NULL,pDelimiter);
	}
	return bFind;
}
bool CBoltLibSet::ReadSheet(const char* file_path,BYTE bolt_type)
{
	int nTitleRow=0;
	if (bolt_type==TYPE_COMMON_L)
		nTitleRow=(int)CSheetTitleRow::COMMON_L;
	else if (bolt_type==TYPE_FOOTNAIL_TWO_CAP_T)
		nTitleRow=(int)CSheetTitleRow::FOOTNAIL_TWO_CAP_T;
	else if (bolt_type==TYPE_FOOTNAIL_TWO_CAP_L)
		nTitleRow=(int)CSheetTitleRow::FOOTNAIL_TWO_CAP_L;
	else if (bolt_type==TYPE_FOOTNAIL_THREE_CAP)
		nTitleRow=(int)CSheetTitleRow::FOOTNAIL_THREE_CAP;
	else if (bolt_type==TYPE_COMMON_T)
		nTitleRow=(int)CSheetTitleRow::COMMON_T;
	else if (bolt_type == TYPE_COMMON_T_FASTEN)
		nTitleRow = (int)CSheetTitleRow::COMMON_T_FASTEN;
	else if (bolt_type == TYPE_COMMON_T_THIN)
		nTitleRow = (int)CSheetTitleRow::COMMON_T_THIN;
	else if (bolt_type==TYPE_DIANQUAN)
		nTitleRow=(int)CSheetTitleRow::DIANQUAN;
	else if (bolt_type==TYPE_DIANQUAN_SPRING)
		nTitleRow=(int)CSheetTitleRow::DIANQUAN_SPRING;
	CVariant2dArray sheetContentMap(1,1);
	if(!CExcelOper::GetExcelContentOfSpecifySheet(file_path,sheetContentMap,bolt_type))
		return false;
	CBoltLib *pBoltLib=m_xHashBoltsLib.Add((int)bolt_type);
	pBoltLib->m_cType=bolt_type;
	//获取列名
	VARIANT value;
	value.intVal=0;
	CHashStrList<DWORD> hashColIndexByColTitle;
	for(int i=0;i<=10;i++)
	{
		sheetContentMap.GetValueAt(nTitleRow-1,i,value);
		if (value.bstrVal==NULL)
			continue;
		else if(CStringFindOneOfChar(CString(value.bstrVal),TITLE_BOLT_WEIGHT1))
			hashColIndexByColTitle.SetValue(TITLE_BOLT_WEIGHT1,i);
		else if(CStringFindOneOfChar(CString(value.bstrVal),TITLE_BOLT_SPEC))
			hashColIndexByColTitle.SetValue(TITLE_BOLT_SPEC,i);
		else if(CStringFindOneOfChar(CString(value.bstrVal),TITLE_BOLT_WEIGHT2))
			hashColIndexByColTitle.SetValue(TITLE_BOLT_WEIGHT2,i);
		else if(CStringFindOneOfChar(CString(value.bstrVal),TITLE_BOLT_WEIGHT_ADD))
			hashColIndexByColTitle.SetValue(TITLE_BOLT_WEIGHT_ADD,i);
	}
	// 获取excel 值
	VARIANT valueBoltGuiGe,valueGrade;
	double fLastSpecD=0,fLastWeightByD=0;
	for(int i = nTitleRow; i < sheetContentMap.RowsCount(); i++)
	{
		BOLT_LIB_REC *pBoltItem=pBoltLib->m_xMapBoltRec.Add(i,0);
		DWORD *pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_SPEC);
		if(pColIndex!=NULL)
		{
			const char* CHAR_SPEC = "m|M";
			sheetContentMap.GetValueAt(i,*pColIndex,valueBoltGuiGe);
			if(!CStringFindOneOfChar(CString(valueBoltGuiGe.bstrVal),CHAR_SPEC))
				break;//读取完第一个标准后结束读取
			pBoltItem->m_sSpec =VariantToString(valueBoltGuiGe);
			SplitSaec(pBoltItem);
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_WEIGHT1);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			pBoltItem->m_fWeight=(float)atof(VariantToString(valueGrade));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_WEIGHT2);
		if(pColIndex!=NULL)
		{
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			pBoltItem->m_fWeight2=(float)atof(VariantToString(valueGrade));
		}
		pColIndex=hashColIndexByColTitle.GetValue(TITLE_BOLT_WEIGHT_ADD);
		if(pColIndex!=NULL && fLastSpecD!=pBoltItem->d)
		{	//有增重列并且是新直径规格的第一行（直径）
			sheetContentMap.GetValueAt(i,*pColIndex,valueGrade);
			CXhChar100 sValue=VariantToString(valueGrade);
			if(!sValue.EqualNoCase(""))
			{
				pBoltLib->m_xHashWeightByD.Append((float)atof(VariantToString(valueGrade)),pBoltItem->d);
				fLastWeightByD=(float)atof(VariantToString(valueGrade));
			}
			else
				pBoltLib->m_xHashWeightByD.Append(fLastWeightByD,pBoltItem->d);
			//更新记录
			fLastSpecD=pBoltItem->d;
		}
	}
	return true;
}
bool CBoltLibSet::ImportBoltLibByExcelFile(const char* file_path)
{
	if(file_path==NULL||strlen(file_path)<=0)
		return false;
	int nHits=0;
	if(ReadSheet(file_path,TYPE_COMMON_L))
		nHits++;
	if(ReadSheet(file_path,TYPE_FOOTNAIL_TWO_CAP_T))
		nHits++;
	if(ReadSheet(file_path,TYPE_FOOTNAIL_TWO_CAP_L))
		nHits++;
	if(ReadSheet(file_path,TYPE_FOOTNAIL_THREE_CAP))
		nHits++;
	if(ReadSheet(file_path,TYPE_COMMON_T))
		nHits++;
	if (ReadSheet(file_path, TYPE_COMMON_T_THIN))
		nHits++;
	if(ReadSheet(file_path,TYPE_COMMON_T_FASTEN))
		nHits++;
	if(ReadSheet(file_path,TYPE_DIANQUAN))
		nHits++;
	if(ReadSheet(file_path,TYPE_DIANQUAN_SPRING))
		nHits++;
	return (nHits==6);
}

