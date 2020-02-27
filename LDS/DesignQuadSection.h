#pragma once
#include "Tower.h"
#include "UnifyModelLayer.h"

#ifdef __RAPID_ROUGH_3DMODEL_
class CDesignQuadSection
{
private:
	CIntelliModelDesigner* m_pDesignerContext;
	struct QUADSECT{
		CLDSLinePart* pFrontRod, *pBackRod;
		CLDSLinePart* pRightRod, *pLeftRod;
		CLDSNodePtr xarrCornerNodes[4];	//����ΪF-R,F-L,B-R,B-L�Ľǽڵ�
		CLDSLinePart* xarrDiagRods[4];	//�ı��θ���Ľǽڵ������ӵĽ�б��
		bool IsInsideDiagRod(CLDSLinePart* pDiagRod,int indexOfCheckCorner);
		QUADSECT() { memset(this, 0, sizeof(QUADSECT)); }
	};
	long RetriveQuadSections(CTower *pTower, ARRAY_LIST<QUADSECT>* xarrQuadSects);
public:
	CDesignQuadSection();
	~CDesignQuadSection();
	CLDSPlate *DesignCornerPlate(CLDSNode *pBaseNode, CLDSLinePart *pHorizRod1, CLDSLinePart *pHorizRod2, CLDSLinePart *pXieRod);
	long DesignQuadSects(CTower *pTower,bool blAddCornerPlate=true,CIntelliModelDesigner* pIntelliDesigner=NULL);
};
#endif
