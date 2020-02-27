// DimPlankLs.h: interface for the CDimPlankLs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIMPLANKLS_H__8FCF510C_28DA_471B_B21F_0ACAED41CF87__INCLUDED_)
#define AFX_DIMPLANKLS_H__8FCF510C_28DA_471B_B21F_0ACAED41CF87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PlankDrawing.h"

#ifndef __TSA_FILE_
typedef struct
{
	int iRayNo;		//射线号
	int d;			//螺栓直径
	int L;			//螺栓有效长
	int N;			//螺栓个数
	double grade;	//螺栓级别
}HOLERAYINFO_STRU;
class CDimPlankLs  
{
	BOOL m_bDimStdBoltSpace;
	CArray<TXTDIM_STRU,TXTDIM_STRU&> text_dim_arr;
	CArray<ALIGNDIM_STRU,ALIGNDIM_STRU&>align_dim_arr;
	CArray<LSRAY_STRU,LSRAY_STRU>ls_ray_arr;
	CArray<HOLERAYINFO_STRU,HOLERAYINFO_STRU>ls_ray_info_arr;
public:
	CDimPlankLs();
	virtual ~CDimPlankLs();
private:
	//加标螺栓端距(主要就是将一个与射线垂直的垂足)
	void DimLsEndSpace(CLDSPlate *pPlank);
	//标注螺栓射线及其上的螺栓间距
	void DimLsRay(CLDSPart *pPlate=NULL);
	void AnalizeLsRay(f3dPoint ls_centre,int iRayNo,char *guige);
public:
	BOOL Create(CLDSPlate *pPlate,BOOL bDimStdBoltSpace=FALSE);
	BOOL Create(CLDSParamPlate *pPlate,BOOL bDimStdBoltSpace=FALSE);
	int GetAlignDimCount();
	int GetLsRayCount();
	int GetTextDimCount();
	BOOL GetAlignDimAt(int index, ALIGNDIM_STRU &dim);
	BOOL GetTextDimAt(int index, TXTDIM_STRU&dim);
};
#endif
#endif // !defined(AFX_DIMPLANKLS_H__8FCF510C_28DA_471B_B21F_0ACAED41CF87__INCLUDED_)
