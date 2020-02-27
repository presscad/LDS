#pragma once 
#include "GimDef.h"
#include "UniWirePointModel.h"
#include "SortFunc.h"
#include "PropListItem.h"

struct XHWIREPOINT : public ICompareClass{
	bool blWireAngle;	//挂孔是否依附于挂线角钢上，一般不是依附在角钢上就是依附在板上
	float sfHoleD;		//挂孔大小
	GEPOINT vStdBasicAxisVec;	//hole[0]->hole[1]的金具转轴方向
	GEPOINT xBasicPoint;		//hole[0]的内贴面中心点（钢板中性层中心点）
	double dfSpaceWidth;		//挂孔间距（金具宽度+间隙）
	struct HOLE{long hBolt,hResidePart;};
	union {
		HOLE holes[2];		//一对挂孔
		struct{
			HOLE hlBasic;	//基准孔，一对挂孔中的任一孔均可
			HOLE hlCoupl;	//对接孔，一对挂孔中的另一孔
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
	BYTE m_ciCodingType;	//编码格式:0.UTF-8|1.ANSI
	GIM_HEAD_PROP_ITEM m_xCurrGimFileHeader;
	TOWER_PRPERTY_ITEM m_xTowerGimPropFile;
public:
	CTowerGimInfo();
	//
	BOOL ReadFamFile(const char* sFamFile);
	void WriteFamFile(const char* sFamFile);
	//属性
	DECLARE_PROP_FUNC(CTowerGimInfo);
	int GetPropValueStr(long id, char* valueStr, UINT nMaxStrBufLen = 100);	//根据属性ID得到属性值字符串
};
class CSolidBody;
bool CreateShaftSolid(CXhWirePoint* pWirePoint,CSolidBody* pSolidBody);
extern CExterDataModel gxExterModel;
extern CUniWireModel gxWirePointModel;
extern CTowerGimInfo gxTowerGimInfo;