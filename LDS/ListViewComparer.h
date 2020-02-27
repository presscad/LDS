// ListViewComparer.h: interface for the CListViewComparer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_)
#define AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum COL_SORT_TYPE{
	SORTTYPE_STRING = 0,	//按字符串排序
	SORTTYPE_PARTNO = 1,	//按件号排序
	SORTTYPE_SPEC = 2,		//按规格排序
	SORTTYPE_INTEGER = 3,	//按整数排序
	SORTTYPE_HEX_NUM = 4,	//按十六进制数排序
};


//用于列表框排序
class CListViewComparer{
	int m_nMaxColumn;
public:
	int (CALLBACK* FireCompareItemFunc)(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	CListCtrl *m_pListCtrl;
	int m_iColumn;
	BOOL *sortOrderAscending;
	COL_SORT_TYPE *sortTypeArr;	//记录每一列对应的排序类型，默认按字符串排序
	CListViewComparer();
	int SetMaxColumnCount(int max_column);
	void SetColumnSortType(int iCol,COL_SORT_TYPE sortType);
	COL_SORT_TYPE GetColumnSortType(int iCol);
	virtual ~CListViewComparer();
};
int CALLBACK CompareItemProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#endif // !defined(AFX_LISTVIEWCOMPARER_H__DEA88421_796C_41D4_9436_5FCA43C5C239__INCLUDED_)
