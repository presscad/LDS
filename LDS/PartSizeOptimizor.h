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
	//TODO: m_pAngleArrӦ��Ϊm_pAnglePtrArr �����׵���m_pAngleArr�ͷ��ڴ��pParaType��Ϊ��Чָ�� wjh-2014.7.26
	JG_PARA_TYPE *m_pAngleArr;
	TUBE_PARA_TYPE *m_pTubeArr;
	BIAN_PARA_TYPE *m_pFlatArr;
	SLOT_PARA_TYPE *m_pSlotArr;
	int m_nAngleSize,m_nTubeSize,m_nFlatSize,m_nSlotSize;
public:
	double m_fMinSizeArea;	//cm2,���ڹ����й���������󲿷���cm2�����ڱȽϴ˴���λҲ��cm2
	CPartSizeOptimizor();
	virtual ~CPartSizeOptimizor();
	void InitPartSizeOrder();
	void UpdateElemSize(CElement *pElem,int iOrder);
	void UpdatePoleSize(CLDSLinePart *pPole,int iOrder);
	int  GetPoleSizeOrderIndex(CLDSLinePart *pPole);
	//bExecuteDichotomy ���ö��ַ�����ѡ��(��ʱѡ��Ѹ��,�����ڹ����ɢ�ҷǵ������²�����׼);cMat=0ʱʹ�ù�����ǰ����ѡ�ģ�����ָ������ѡ��(��δ����),
	BOOL QuickSelectElemSize(SIZESPEC_SELECT *selectSize,CElement *pElem,int iDesignCode,BOOL bExecuteDichotomy=TRUE,char cMat=0);
};
#endif
#endif // !defined(AFX_PARTSIZEOPTIMIZOR_H__2BA10E13_F8F5_4F4D_97E1_2C6F12C50350__INCLUDED_)
