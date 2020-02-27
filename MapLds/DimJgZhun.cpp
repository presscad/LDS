// DimJgZhun.cpp: implementation of the CDimJgZhun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DimJgZhun.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
CDimJgZhun::CDimJgZhun(CLDSLineAngle *pJg)
{
	m_pJg=pJg;
	if(pJg)
		pJg->getUCS(ucs);
}

CDimJgZhun::~CDimJgZhun()
{

}
BOOL CDimJgZhun::DimLsZhun(CLsRef *pLsRef)
{
	INT_PTR i,nDimCount;
	CZhunXianTextDim zhun_dim;
	f3dPoint ls_prj_pos;
	BOOL bHasSameDim = FALSE;
	ls_prj_pos = (*pLsRef)->ucs.origin;
	coord_trans(ls_prj_pos,ucs,FALSE);
	if(fabs((*pLsRef)->get_norm()*m_pJg->get_norm_x_wing())>0.707)
	{
		ls_prj_pos.y = 0;
		ls_prj_pos.x = m_pJg->GetLsG(pLsRef);
	}
	else
	{
		ls_prj_pos.x = 0;
		ls_prj_pos.y = m_pJg->GetLsG(pLsRef);
	}
	nDimCount = zhun_dim_arr.GetSize();
	for(i=0;i<nDimCount;i++)
	{
		if(DistOf3dPtLine(ls_prj_pos,zhun_dim_arr[i].lineStart,zhun_dim_arr[i].lineEnd)<1)
		{
			if(ls_prj_pos.z>zhun_dim_arr[i].lineStart.z-200
				&&ls_prj_pos.z<zhun_dim_arr[i].lineEnd.z+200)
			{
				zhun_dim_arr[i].lineStart.z = __min(zhun_dim_arr[i].lineStart.z,ls_prj_pos.z);
				zhun_dim_arr[i].lineEnd.z = __max(zhun_dim_arr[i].lineEnd.z,ls_prj_pos.z);
				bHasSameDim=TRUE;
			}
		}	
	}
	if(!bHasSameDim)
	{
		zhun_dim.lineStart.Set(ls_prj_pos.x,ls_prj_pos.y,ls_prj_pos.z-25);
		zhun_dim.lineEnd.Set(ls_prj_pos.x,ls_prj_pos.y,ls_prj_pos.z+30);
		zhun_dim.dimStartPos.Set(ls_prj_pos.x,ls_prj_pos.y,ls_prj_pos.z+40);
		zhun_dim.dimEndPos.Set(0,0,ls_prj_pos.z+40);
		zhun_dim.dimPos.Set(ls_prj_pos.x/2,ls_prj_pos.y/2.0,ls_prj_pos.z+50);
		zhun_dim.dist = sqrt(pow(ls_prj_pos.y,2)+pow(ls_prj_pos.x,2));	//圆整化
		coord_trans(zhun_dim.dimStartPos,ucs,TRUE);
		coord_trans(zhun_dim.dimEndPos,ucs,TRUE);
		coord_trans(zhun_dim.dimPos,ucs,TRUE);
		zhun_dim.hRelaBolt=(*pLsRef)->handle;	//记录角钢准据关联螺栓句柄
		zhun_dim_arr.Add(zhun_dim);
		return TRUE;
	}
	return FALSE;
}
void CDimJgZhun::EndDim()
{
	INT_PTR i,n;
	n = zhun_dim_arr.GetSize();
	for(i=0;i<n;i++)
	{
		coord_trans(zhun_dim_arr[i].lineStart,ucs,TRUE);
		coord_trans(zhun_dim_arr[i].lineEnd,ucs,TRUE);
	}
}

int CDimJgZhun::GetZhunDimCount()
{
	return (int)zhun_dim_arr.GetSize();
}
BOOL CDimJgZhun::GetZhunDimAt(int index,CZhunXianTextDim *pDim)
{
	if(index<0||index>zhun_dim_arr.GetUpperBound())
		return FALSE;
	memcpy(pDim,&zhun_dim_arr[index],sizeof(CZhunXianTextDim ));
	return TRUE;
}
void CDimJgZhun::Empty()
{
	zhun_dim_arr.RemoveAll();
}
#endif