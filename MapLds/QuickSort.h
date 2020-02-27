#ifndef _QUICK_SORT_
#define _QUICK_SORT_

//øÏÀŸ≈≈–Ú∑®
template<class TYPE>
class CQuickSort
{
public:
	BOOL QuickSort(TYPE *list,int start,int end,int nCount,int (*compare_func)(const TYPE&,const TYPE&))
	{
		int low,high;
		TYPE pivot_key;
		if(start>end)
			return FALSE;
		else if(start==end)
			return TRUE;
		if(start<0||end>=nCount)
			return FALSE;
		low = start;
		pivot_key = list[low];
		high = end;
		while(low<high)
		{
			while(low<high&&compare_func(list[high],pivot_key)>=0)
				high--;
			list[low] = list[high];
			while(low<high&&compare_func(list[low],pivot_key)<=0)
				low++;
			list[high] = list[low];
		}
		list[low] = pivot_key;
		if(low==end)
		{
			low--;
			end--;
		}
		else if(low==start)
		{
			start++;
			low++;
		}
		if(low-start>1)
			QuickSort(list,start,low,nCount,compare_func);
		if(end-low>1)
			QuickSort(list,low,end,nCount,compare_func);
		return TRUE;
	}
};
#endif
