#ifndef __GROUPJG_FUCAI_CHECK_H
#define __GROUPJG_FUCAI_CHECK_H

#include "lds_part.h"
#include "ProcessPart.h"
#include "MemberProperty.h"
//
class CTransFuCai
{
public:
	CLDSLineAngle* m_pCurrAngle;
	CXhChar16 sPartNo;
	GEPOINT vRodVec,vStdRodVec,vPrjRodVec,vStdPrjRodVec;
	GEPOINT m_vRotateAxisVec;
	double m_fDeitaByRodNormal;	//沿当前杆件始末端综合垂向偏移量
	double m_fSina,m_fCosa;		//杆件与展开工作平面间的夹角正弦与余弦
	double m_fNoneOddRodLength,m_fSpreadRodLength;
	double m_fLenZoomCoef;
	GEPOINT vRodNormal;	//与杆件垂直的偏移方向，末端+vRodNormal*m_fDeitaByRodNormal使杆件展开至工作平面
	ATOM_LIST<BOLT_INFO> m_xBoltInfoList;	//螺栓信息
private:
	bool InitTransInfo();	//初始化展开面信息
	void InitBoltInfo();	//初始化螺栓信息
	void TransPointToSpreadPlane(f3dPoint& pt);
public:
	CTransFuCai(CLDSLineAngle* pJg=NULL);
	~CTransFuCai();
	CLDSLineAngle* getCurrAngle(){return m_pCurrAngle;}
	READONLY_PROPERTY(CLDSLineAngle*,pAngle);
	GET(pAngle){return m_pCurrAngle;}
	bool IsInited(){return (m_fDeitaByRodNormal!=0);}
	double SpreadRodLength();
	//0.不同;1.相同;其余:比较失败
	int IsEqual(CTransFuCai *pFuCai,double epsilon=1.0,char *error_info=NULL,
		int *pErrorType=NULL,DWORD *pChkStyleFlag=NULL,bool bSimpleCompareMode=false);
};
#endif