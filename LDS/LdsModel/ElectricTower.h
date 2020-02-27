#pragma once 
#include "GimDef.h"
#include "UniWirePointModel.h"
#include "SortFunc.h"
#include "PropListItem.h"

struct XHWIREPOINT : public ICompareClass{
	bool blWireAngle;	//�ҿ��Ƿ������ڹ��߽Ǹ��ϣ�һ�㲻�������ڽǸ��Ͼ��������ڰ���
	float sfHoleD;		//�ҿ״�С
	GEPOINT vStdBasicAxisVec;	//hole[0]->hole[1]�Ľ��ת�᷽��
	GEPOINT xBasicPoint;		//hole[0]�����������ĵ㣨�ְ����Բ����ĵ㣩
	double dfSpaceWidth;		//�ҿ׼�ࣨ��߿��+��϶��
	struct HOLE{long hBolt,hResidePart;};
	union {
		HOLE holes[2];		//һ�Թҿ�
		struct{
			HOLE hlBasic;	//��׼�ף�һ�Թҿ��е���һ�׾���
			HOLE hlCoupl;	//�Խӿף�һ�Թҿ��е���һ��
		};
	};
	virtual int Compare(const ICompareClass *pCompareObj)const;
	XHWIREPOINT(){
		blWireAngle=true;
		dfSpaceWidth=sfHoleD=0;
		memset(holes,0,16);
	}
};
class CTower;
class CExterDataModel{
protected:
	CTower* m_pTower;
public:
	CExterDataModel(){;}
	void SetBelongTower(CTower* pTower){m_pTower=pTower;}
	long GetMinUsedHandle();
	long GetNewHandle(){return m_pTower!=NULL?m_pTower->GetNewHandle():0;}
	CXhWirePoint* FromWirePoint(long hWirePoint);
};
class CTowerGimInfo {
public:
	BYTE m_ciCodingType;	//�����ʽ:0.UTF-8|1.ANSI
	GIM_HEAD_PROP_ITEM m_xCurrGimFileHeader;
	TOWER_PRPERTY_ITEM m_xTowerGimPropFile;
public:
	CTowerGimInfo();
	//
	BOOL ReadFamFile(const char* sFamFile);
	void WriteFamFile(const char* sFamFile);
	//����
	DECLARE_PROP_FUNC(CTowerGimInfo);
	int GetPropValueStr(long id, char* valueStr, UINT nMaxStrBufLen = 100);	//��������ID�õ�����ֵ�ַ���
};
class CSolidBody;
bool CreateShaftSolid(CXhWirePoint* pWirePoint,CSolidBody* pSolidBody);
extern CExterDataModel gxExterModel;
extern CUniWireModel gxWirePointModel;
extern CTowerGimInfo gxTowerGimInfo;