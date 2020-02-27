// DimJgZhun.h: interface for the CDimJgZhun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIMJGZHUN_H__AFA3FABF_165B_44B4_B1D1_A2CEFF890EE6__INCLUDED_)
#define AFX_DIMJGZHUN_H__AFA3FABF_165B_44B4_B1D1_A2CEFF890EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "DrawUnit.h"
#ifndef __TSA_FILE_
class CDimJgZhun  
{
	UCS_STRU ucs;
public:
	CDimJgZhun(CLDSLineAngle *pJg);
	virtual ~CDimJgZhun();
	BOOL DimLsZhun(CLsRef *pLsRef);
	int GetZhunDimCount();
	BOOL GetZhunDimAt(int index,CZhunXianTextDim *pDim);
	void Empty();
	void EndDim();
public:
	CLDSLineAngle *m_pJg;
	CArray<CZhunXianTextDim,CZhunXianTextDim&>zhun_dim_arr;

};
#endif
#endif // !defined(AFX_DIMJGZHUN_H__AFA3FABF_165B_44B4_B1D1_A2CEFF890EE6__INCLUDED_)
