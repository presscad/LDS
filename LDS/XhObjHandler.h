#pragma once
#include "I_DrawSolid.h"
class XHOBJ_HANDLER : public OBJECT_HANDLER
{
public:
	XHOBJ_HANDLER() { ; }
	XHOBJ_HANDLER(OBJECT_HANDLER& handler);
	BYTE get_ciXhObjType();
	__declspec(property(get=get_ciXhObjType)) BYTE ciXhObjType;
	UINT get_hMasterId();
	__declspec(property(get=get_hMasterId)) UINT hMasterId;
	UINT get_hBoltResidePart();
	__declspec(property(get=get_hBoltResidePart)) UINT hBoltResidePart;
};

