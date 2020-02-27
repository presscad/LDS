// ListViewComparer.cpp: implementation of the CListViewComparer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListViewComparer.h"
#ifndef _COMPARE_PART_NO_DISABLED
#include "ComparePartNoString.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CALLBACK CompareItemProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListViewComparer *pListViewComparer=(CListViewComparer*)lParamSort;
	LVFINDINFO FindInfo; 
	FindInfo.flags = LVFI_PARAM; 
	FindInfo.lParam = lParam1; 
	int iItem1 = pListViewComparer->m_pListCtrl->FindItem(&FindInfo, -1); 
	FindInfo.lParam = lParam2; 
	int iItem2 = pListViewComparer->m_pListCtrl->FindItem(&FindInfo, -1); 
   	CString strItem1 = pListViewComparer->m_pListCtrl->GetItemText(iItem1, pListViewComparer->m_iColumn);
	CString strItem2 = pListViewComparer->m_pListCtrl->GetItemText(iItem2, pListViewComparer->m_iColumn);
	COL_SORT_TYPE sortType=pListViewComparer->GetColumnSortType(pListViewComparer->m_iColumn);
	int ret=0;
	//无论升序或降序空字符串都应排在非空字符串之后 wht 12-05-25
	if(strItem1.GetLength()==0&&strItem2.GetLength()>0)
		return 1;	
	else if(strItem1.GetLength()>0&&strItem2.GetLength()==0)
		return -1;
#ifndef _COMPARE_PART_NO_DISABLED
	if(sortType==SORTTYPE_PARTNO)
	{	//按件号排序或规格排序
		ret=ComparePartNoString(strItem1.GetBuffer(),strItem2.GetBuffer());	
	}
	else if(sortType==SORTTYPE_SPEC)
	{	//按规格排序
		ret=CompareMultiSectionString(strItem1.GetBuffer(),strItem2.GetBuffer());	
	}
#endif
	else if(sortType==SORTTYPE_INTEGER)
	{	//按整数排序
		int nItem1=atoi(strItem1);
		int nItem2=atoi(strItem2);
		if(nItem1>nItem2)
			ret=1;
		else if(nItem1<nItem2)
			ret=-1;
		else
			ret=0;
	}
	else if(sortType==SORTTYPE_HEX_NUM)
	{	//按十六进制数排序
		long nItem1=0,nItem2=0;
		sscanf(strItem1,"%X",&nItem1);
		sscanf(strItem2,"%X",&nItem2);
		if(nItem1>nItem2)
			ret=1;
		else if(nItem1<nItem2)
			ret=-1;
		else
			ret=0;
	}
	else//按字符串排序
		ret=strItem1.CompareNoCase(strItem2);
	if(pListViewComparer->sortOrderAscending[pListViewComparer->m_iColumn])
		return ret;	//升序
	else
		return -ret;//降序
}

CListViewComparer::CListViewComparer()
{
	m_nMaxColumn=0;
	m_pListCtrl=NULL;
	m_iColumn=-1;	//m_iColumn记录当前排序列索引，-1表示未通过点击列进行排序
	sortOrderAscending=NULL;
	sortTypeArr=NULL;
	FireCompareItemFunc=CompareItemProc;
}
int CListViewComparer::SetMaxColumnCount(int max_column)
{
	m_nMaxColumn=max_column;
	if(sortOrderAscending)
		delete []sortOrderAscending;
	sortOrderAscending=new BOOL[m_nMaxColumn];
	memset(sortOrderAscending,0,sizeof(BOOL)*m_nMaxColumn);
	if(sortTypeArr)
		delete []sortTypeArr;
	sortTypeArr=new COL_SORT_TYPE[m_nMaxColumn];
	memset(sortTypeArr,0,sizeof(COL_SORT_TYPE)*m_nMaxColumn);
	return max_column;
}
//设置指定列的排序类型
void CListViewComparer::SetColumnSortType(int iCol,COL_SORT_TYPE sortType)
{
	if(sortTypeArr==NULL)
		return;
	if(iCol<m_nMaxColumn)
		sortTypeArr[iCol]=sortType;
}
//得到指定列排序类型
COL_SORT_TYPE CListViewComparer::GetColumnSortType(int iCol)
{
	if(sortTypeArr!=NULL&&iCol<m_nMaxColumn)
		return sortTypeArr[iCol];
	return SORTTYPE_STRING;
}
CListViewComparer::~CListViewComparer()
{
	if(sortOrderAscending)
		delete []sortOrderAscending;
	if(sortTypeArr)
		delete []sortTypeArr;
}
