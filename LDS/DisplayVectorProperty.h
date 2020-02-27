#pragma once
#include "Tower.h"
#include "Console.h"
#include "PropertyList.h"

//<DEVELOP_PROCESS_MARK nameId="CDisplayVectorProperty.H">
class CDisplayVectorProperty
{
public:
	CDisplayVectorProperty(void);
	~CDisplayVectorProperty(void);
	
	static BOOL VectorButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem);
	static BOOL ModifyDatumVectorPropValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr);
	static void UpdateDatumVectorPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem, CLDSVector *pVector,
		BOOL bUpdate,BOOL bSpecAngleWingNormStyle=FALSE,long hCurAngle=0);
	static void DisplayPropList(CPropertyList *pPropList,CLDSVector *pVector,BOOL bSpecAngleWingNormStyle=FALSE,long hCurAngle=0);
	static BOOL FinishSelectObjOper(CLDSVector *pVector,long hPickObj,long idEventProp);
};
//</DEVELOP_PROCESS_MARK>