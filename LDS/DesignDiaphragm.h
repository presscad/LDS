#pragma once
#include "Tower.h"
#include "UnifyModelLayer.h"

class CDesignDiaphragm
{
private:
	CIntelliModelDesigner* m_pDesignerContext;
	CLDSPlate *DesignSharedPlate(CLDSNode *pBaseNode, CLDSLinePart *pHorizRod1, CLDSLinePart *pHorizRod2, CLDSLinePart *pXieRod);
public:
	CDesignDiaphragm();
	~CDesignDiaphragm();
	void DesignAllSharedPlate(CIntelliModelDesigner* pIntelliDesigner=NULL);
};

