#include "stdafx.h"
#include "Tower.h"
#include "XhObjHandler.h"


XHOBJ_HANDLER::XHOBJ_HANDLER(OBJECT_HANDLER& handler)
{
	hiberid=handler.hiberid;
	idClassType=handler.idClassType;
}
BYTE XHOBJ_HANDLER::get_ciXhObjType()
{
	if (hiberid.HiberUpId(1)==1)
		return PROP_OBJ::TYPE_HOLEWALL;
	else if (hiberid.HiberUpId(1)==2)
		return PROP_OBJ::TYPE_WIREPOINTARROW;
	else
		return PROP_OBJ::TYPE_LDSOBJ;
}
UINT XHOBJ_HANDLER::get_hMasterId()
{
	return hiberid.masterId;
}
UINT XHOBJ_HANDLER::get_hBoltResidePart()
{
	return hiberid.HiberUpId(2);
}
