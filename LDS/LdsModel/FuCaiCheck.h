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
	double m_fDeitaByRodNormal;	//�ص�ǰ�˼�ʼĩ���ۺϴ���ƫ����
	double m_fSina,m_fCosa;		//�˼���չ������ƽ���ļн�����������
	double m_fNoneOddRodLength,m_fSpreadRodLength;
	double m_fLenZoomCoef;
	GEPOINT vRodNormal;	//��˼���ֱ��ƫ�Ʒ���ĩ��+vRodNormal*m_fDeitaByRodNormalʹ�˼�չ��������ƽ��
	ATOM_LIST<BOLT_INFO> m_xBoltInfoList;	//��˨��Ϣ
private:
	bool InitTransInfo();	//��ʼ��չ������Ϣ
	void InitBoltInfo();	//��ʼ����˨��Ϣ
	void TransPointToSpreadPlane(f3dPoint& pt);
public:
	CTransFuCai(CLDSLineAngle* pJg=NULL);
	~CTransFuCai();
	CLDSLineAngle* getCurrAngle(){return m_pCurrAngle;}
	READONLY_PROPERTY(CLDSLineAngle*,pAngle);
	GET(pAngle){return m_pCurrAngle;}
	bool IsInited(){return (m_fDeitaByRodNormal!=0);}
	double SpreadRodLength();
	//0.��ͬ;1.��ͬ;����:�Ƚ�ʧ��
	int IsEqual(CTransFuCai *pFuCai,double epsilon=1.0,char *error_info=NULL,
		int *pErrorType=NULL,DWORD *pChkStyleFlag=NULL,bool bSimpleCompareMode=false);
};
#endif