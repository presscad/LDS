#pragma once
#include "Tower.h"
#include "Console.h"
#include "PropertyList.h"

//<DEVELOP_PROCESS_MARK nameId="CDisplayPointProperty.h">
class CDisplayPointProperty
{
public:
	CDisplayPointProperty(void);
	~CDisplayPointProperty(void);

	static BOOL DatumPointButtonClick(CPropertyList* pPropList,CPropTreeItem *pItem);
	static BOOL ModifyDatumPointPropValue(CPropertyList	*pPropList,CPropTreeItem *pItem, CString &valueStr);
	static void UpdateDatumPointPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem, 
		CLDSPoint *pPoint,BOOL bUpdate,BOOL bDisplayPointSonItem=TRUE);
	static void DisplayPropList(CPropertyList *pPropList,CLDSPoint *pPoint);
	static BOOL FinishSelectObjOper(CLDSPoint *pPoint,long hPickObj,long idEventProp);
	static CString GetPlaneHandleStr();
	static CString GetSelectPlaneDesc(CLDSPoint* pPoint);
};
//</DEVELOP_PROCESS_MARK>
