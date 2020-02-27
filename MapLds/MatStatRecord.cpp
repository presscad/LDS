// MatStatRecord.cpp: implementation of the CMatStatRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MatStatRecord.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatStatRecord::CMatStatRecord()
{
	memset((void*)this,0,sizeof(CMatStatRecord));
}
CMatStatRecord::~CMatStatRecord()
{
	if(seg_weight_arr!=NULL)
		delete []seg_weight_arr;
	if(body_weight_arr!=NULL)
		delete []body_weight_arr;
}
BOOL CMatStatRecord::Create(int nSegN,int nBodyN)
{
	if(nSegN>0)
	{
		seg_weight_arr = new double[nSegN];
		//清零内存区
		memset(seg_weight_arr,0,sizeof(double)*nSegN);
		m_nSegN = nSegN;
	}
	else
		logerr.Log("模型数据中缺少段号信息!");
	if(nBodyN>0)
	{	//可能会由于计算数据文件中没有分配段号，导致输出司令图时统计了总图导致死机　wjh-2017.4.7
		body_weight_arr = new double[nBodyN];
		//清零内存区
		memset(body_weight_arr,0,sizeof(double)*nBodyN);
		m_nBodyN=nBodyN;
	}
	else
		logerr.Log("模型数据中缺少呼高组信息!");
	return nSegN>0||nBodyN>0;
}
BOOL CMatStatRecord::SetSegWeight(int iSegIndex,double weight)
{
	if(iSegIndex<0||iSegIndex>=m_nSegN)
		return FALSE;
	seg_weight_arr[iSegIndex] = weight;
	return TRUE;
}
BOOL CMatStatRecord::SetBodyWeight(int iBody,double weight)
{
	if(iBody<=0||iBody>m_nBodyN)
		return FALSE;
	body_weight_arr[iBody-1] = weight;
	return TRUE;
}
BOOL CMatStatRecord::AddSegWeight(int iSegIndex,double weight)
{
	if(iSegIndex<0||iSegIndex>=m_nSegN)
		return FALSE;
	seg_weight_arr[iSegIndex] += weight;
	return TRUE;
}
void CMatStatRecord::AddBodyWeight(char *limit_str,double weight,BOOL bExclusive)
{
	//CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	for(COEF_NO* pNo=FindAdaptCoefNo(limit_str);pNo;pNo=FindAdaptCoefNo(NULL))
	{
		body_weight_arr[pNo->no-1] += weight*pNo->coef;
	}
}
double CMatStatRecord::GetSegWeight(int iSegIndex)
{
	if(iSegIndex<0||iSegIndex>=m_nSegN)
		return 0;
	else
		return seg_weight_arr[iSegIndex];
}
//索引值以0为基数
double CMatStatRecord::GetBodyWeight(int iBody)
{
	if(iBody<=0||iBody>32)
		return 0;
	else
		return body_weight_arr[iBody-1];
}
