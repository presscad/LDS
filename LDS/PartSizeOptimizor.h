// PartSizeOptimizor.h: interface for the CPartSizeOptimizor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTSIZEOPTIMIZOR_H__2BA10E13_F8F5_4F4D_97E1_2C6F12C50350__INCLUDED_)
#define AFX_PARTSIZEOPTIMIZOR_H__2BA10E13_F8F5_4F4D_97E1_2C6F12C50350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#if defined(__TSA_)||defined(__LDS_)
#include "PartLib.h"
#include "Tower.h"
#include "AnalyzeProcessor.h"

void CalElemRelaBoltInfo(CElement *pElem);
typedef struct tagSIZESPEC_SELECT{
	int orderI;
	double wide,thick,height;
	long idClassType;
	char cSubClassType;
	tagSIZESPEC_SELECT(){wide=thick=height=0;orderI=0;idClassType=CLS_LINEANGLE;cSubClassType='L';}
}SIZESPEC_SELECT;
class CPartSizeOptimizor
{
	//TODO: m_pAngleArr应改为m_pAnglePtrArr 否则易导致m_pAngleArr释放内存后pParaType变为无效指针 wjh-2014.7.26
	JG_PARA_TYPE *m_pAngleArr;
	TUBE_PARA_TYPE *m_pTubeArr;
	BIAN_PARA_TYPE *m_pFlatArr;
	SLOT_PARA_TYPE *m_pSlotArr;
	int m_nAngleSize,m_nTubeSize,m_nFlatSize,m_nSlotSize;
public:
	double m_fMinSizeArea;	//cm2,由于规格库中构件截面积大部分用cm2，便于比较此处单位也用cm2
	CPartSizeOptimizor();
	virtual ~CPartSizeOptimizor();
	void InitPartSizeOrder();
	void UpdateElemSize(CElement *pElem,int iOrder);
	void UpdatePoleSize(CLDSLinePart *pPole,int iOrder);
	int  GetPoleSizeOrderIndex(CLDSLinePart *pPole);
	//bExecuteDichotomy 启用二分法快速选材(此时选材迅速,但由于规格离散且非单调导致不够精准);cMat=0时使用构件当前材质选材，否则按指定材质选材(暂未启用),
	BOOL QuickSelectElemSize(SIZESPEC_SELECT *selectSize,CElement *pElem,int iDesignCode,BOOL bExecuteDichotomy=TRUE,char cMat=0);
};
#endif
#endif // !defined(AFX_PARTSIZEOPTIMIZOR_H__2BA10E13_F8F5_4F4D_97E1_2C6F12C50350__INCLUDED_)
