#pragma once
#include "ArrayList.h"

struct WELD_PLATE_ITEM
{
	long m_hBaseTube;
	long m_hBaseNode;
	long m_liAmendOffsetY;	//对活点偏移距
	struct PLATE{
		CLDSPlate* pPlate;
		GEPOINT vRegWeldVec;//焊接边正侧走向vRegWeldVec（与基准钢管始->终一致） wjh-2019.11.02
		f3dLine weld_line;
		long liEdgeLength;	//焊接边长度
		long liSecondAmendOffsetY;	//对活点Y向二次偏移距
		PLATE(CLDSPlate* _pPlate=NULL,long _liSecondAmendOffsetY=0) {
			pPlate=_pPlate;
			liEdgeLength=0;
			liSecondAmendOffsetY =_liSecondAmendOffsetY;
		}
	};
	ATOM_LIST<PLATE> xarrPlates;
public:
	WELD_PLATE_ITEM(){m_hBaseTube=0;m_hBaseNode=0; m_liAmendOffsetY =0;}
	BOOL IsWeldPtOutWeldLine(IModel* pModel);
};
