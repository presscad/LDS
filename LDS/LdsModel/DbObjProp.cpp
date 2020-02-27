#include "StdAfx.h"
#include "lds_part.h"
#include "DbObjPropId.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BYTE CLDSDbObject::PROPVAL::ToByte()
{
	if(ciType==TYPE_BYTE)
		return cbVal;
	else if(ciType==TYPE_WORD)
		return (BYTE)wiVal;
	else if(ciType==TYPE_DWORD)
		return (BYTE)dwVal;
	else if(ciType==TYPE_LONG)
		return (BYTE)liVal;
	else if(ciType==TYPE_DOUBLE)
		return (BYTE)fVal;
	return 0;
}
WORD CLDSDbObject::PROPVAL::ToWord()
{
	if(ciType==TYPE_BYTE)
		return cbVal;
	else if(ciType==TYPE_WORD)
		return wiVal;
	else if(ciType==TYPE_DWORD)
		return (WORD)dwVal;
	else if(ciType==TYPE_LONG)
		return (WORD)liVal;
	else if(ciType==TYPE_DOUBLE)
		return (WORD)fVal;
	return 0;
}
DWORD CLDSDbObject::PROPVAL::ToDword()
{
	if(ciType==TYPE_BYTE)
		return cbVal;
	else if(ciType==TYPE_WORD)
		return wiVal;
	else if(ciType==TYPE_DWORD)
		return dwVal;
	else if(ciType==TYPE_LONG)
		return (DWORD)liVal;
	else if(ciType==TYPE_DOUBLE)
		return (DWORD)fVal;
	return 0;
}
long  CLDSDbObject::PROPVAL::ToLong()
{
	if(ciType==TYPE_BYTE)
		return cbVal;
	else if(ciType==TYPE_WORD)
		return wiVal;
	else if(ciType==TYPE_DWORD)
		return dwVal;
	else if(ciType==TYPE_LONG)
		return (long)liVal;
	else if(ciType==TYPE_DOUBLE)
		return (long)fVal;
	return 0;
}
double CLDSDbObject::PROPVAL::ToDouble()
{
	if(ciType==TYPE_BYTE)
		return cbVal;
	else if(ciType==TYPE_WORD)
		return wiVal;
	else if(ciType==TYPE_DWORD)
		return dwVal;
	else if(ciType==TYPE_LONG)
		return liVal;
	else if(ciType==TYPE_DOUBLE)
		return fVal;
	return 0;
}
const UINT CLDSDbObject::PROP_LOCATION	=1;	//装配位置
const UINT CLDSDbObject::PROP_SIZESPEC	=2;	//构件宽度
const UINT CLDSDbObject::PROP_WIDTH		=3;	//构件宽度
const UINT CLDSDbObject::PROP_THICK		=4;	//构件宽度
const UINT CLDSDbObject::PROP_LENGTH	=5;	//构件长度
const UINT CLDSDbObject::PROP_MATERIAL	=6;	//构件材质
const UINT CLDSDbObject::PROP_NORMAL	=7;	//构件法线
const UINT CLDSDbObject::PROP_WINGX_NORMAL=8;	//角钢X肢法线
const UINT CLDSDbObject::PROP_WINGY_NORMAL=9;	//角钢Y肢法线
const UINT CLDSDbObject::PROP_POS_START	=10;//杆件始端位置
const UINT CLDSDbObject::PROP_POS_END	=11;//杆件终端位置
const UINT CLDSDbObject::PROP_PROFILE	=12;//钢板外形

bool CLDSPart::GetPropParam(long idObjProp,PROPVAL* pVal/*=NULL*/)
{
	pVal->ciType=PROPVAL::TYPE_DOUBLE;
	if(idObjProp==CLDSDbObject::PROP_LOCATION)
	{
		pVal->ciType=PROPVAL::TYPE_COORD3D;
		pVal->vPoint=ucs.origin;
	}
	else if(idObjProp==CLDSDbObject::PROP_WIDTH)
		pVal->fVal=GetWidth();
	else if(idObjProp==CLDSDbObject::PROP_THICK)
		pVal->fVal=GetThick();
	else if(idObjProp==CLDSDbObject::PROP_PROFILE)
		pVal->ciType=0;
	else
		return false;
	return true;
}
bool CLDSPart::SetPropParam(long idObjProp,PROPVAL& propval)
{
	if(idObjProp==PROP_LOCATION)
	{
		ucs.origin=propval.vPoint;
	}
	else
		return false;
	return false;
}

bool CLDSGeneralPlate::GetPropParam(long idObjProp,PROPVAL* pVal/*=NULL*/)
{
	if(CLDSPart::GetPropParam(idObjProp,pVal))
		return true;
	if(idObjProp==PROP_NORMAL)
	{
		pVal->ciType=PROPVAL::TYPE_COORD3D;
		pVal->vPoint=ucs.axis_z;
	}
	else if(idObjProp==PROP_THICK)
	{
		pVal->fVal=Thick;
		pVal->ciType=PROPVAL::TYPE_DOUBLE;
	}
	else if(wiType==TYPE_STATIONCONNPLATE)
	{
		if(idObjProp==PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R)
		{
			if(!GetDesignItemValue('r',&pVal->fVal))
				return false;
			pVal->ciType=PROPVAL::TYPE_DOUBLE;
		}
		else if(idObjProp==PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H)
		{
			if(!GetDesignItemValue('H',&pVal->fVal))
				return false;
			pVal->ciType=PROPVAL::TYPE_DOUBLE;
		}
		else
			return false;
		return true;
	}
	else if(wiType==TYPE_BASEPLATE)
	{
		if(idObjProp==PLATE_PROPID::BASE::R)
		{
			if(!GetDesignItemValue(KEY2C("R"),&pVal->fVal))
				return false;
			pVal->ciType=PROPVAL::TYPE_DOUBLE;
		}
		else
			return false;
		return true;
	}
	else if(wiType==TYPE_STATIONCROSSPLATE)
	{
		if(idObjProp==PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H)
		{
			if(!GetDesignItemValue(KEY2C("H"),&pVal->fVal))
				return false;
			pVal->ciType=PROPVAL::TYPE_DOUBLE;
		}
		else
			return false;
		return true;
	}
	else
		return false;
	return true;
}
bool CLDSGeneralPlate::SetPropParam(long idObjProp,PROPVAL& propval)
{
	if(idObjProp==PROP_NORMAL)
	{
		ucs.axis_z=propval.vPoint;
	}
	//else if(idObjProp==PROP_THICK)
	//{
	//	if(pVal->ciType==PROPVAL::TYPE_WORD)
	//		Thick=pVal->wiVal;
	//	else if(pVal->ciType==PROPVAL::TYPE_LONG)
	//		Thick=pVal->liVal;
	//	else if(pVal->ciType==PROPVAL::TYPE_DWORD)
	//		Thick=pVal->dwVal;
	//	else if(pVal->ciType==PROPVAL::TYPE_DOUBLE)
	//		Thick=pVal->fVal;
	//	else
	//		return false;
	//}
	else if(wiType==TYPE_STATIONCONNPLATE)
	{
		if(idObjProp==PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_R)
		{
			DESIGN_PLATE_ITEM* pItem=params.GetValue('r');
			if(pItem)
			{
				if(pItem->value.fValue!=propval.fVal)
					SetModified();
				pItem->value.fValue=propval.fVal;
				pItem->dataType=0;
			}
		}
		else if(idObjProp==PLATE_PROPID::STATIONCONNPLATE::PROP_HOLE_H)
		{
			DESIGN_PLATE_ITEM* pItem=params.GetValue('H');
			if(pItem)
			{
				if(pItem->value.fValue!=propval.fVal)
					SetModified();
				pItem->value.fValue=propval.fVal;
				pItem->dataType=0;
			}
		}
		else if(idObjProp==PLATE_PROPID::PROP_PROFILE)
			this->SetSolidModified();
		else
			return false;
		return true;
	}
	else if(wiType==TYPE_STATIONCROSSPLATE)
	{
		if(idObjProp==PLATE_PROPID::PROP_PROFILE)
			this->SetSolidModified();
		else if(idObjProp==PLATE_PROPID::STATIONCROSSPLATE::PROP_PARA_H)
		{
			DESIGN_PLATE_ITEM* pItem=params.GetValue('H');
			if(pItem)
			{
				if(pItem->value.fValue!=propval.fVal)
					SetModified();
				pItem->value.fValue=propval.fVal;
				pItem->dataType=0;
			}
		}
		else
			return false;
		return true;
	}
	else if(wiType==TYPE_STATIONSHOEPLATE)
	{
		if(idObjProp==PLATE_PROPID::PROP_PROFILE)
			this->SetSolidModified();
		else if(idObjProp==PLATE_PROPID::STATIONSHOEPLATE::PROP_HOLE_R||idObjProp==PLATE_PROPID::STATIONSHOEPLATE::PROP_HOLE_H)
			this->SetSolidModified();
		else
			return false;
		return true;
	}
	else if(wiType>0)
	{
	}
	else
		return false;
	return true;
}
bool CLDSGeneralPlate::ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint)
{
	WORD wiCurrMasterPropId=pConstraint->wiMasterPropId;
	WORD wiCurrAcceptPropId=pConstraint->wiAcceptPropId;
	if(pConstraint->dual&&pMasterObj!=NULL&&pMasterObj->handle==pConstraint->hAcceptObj)
	{
		wiCurrMasterPropId=pConstraint->wiAcceptPropId;
		wiCurrAcceptPropId=pConstraint->wiMasterPropId;
	}
	if(this->wiType==TYPE_BASEPLATE)
	{
		pConstraint->expression="M+2";
		if(wiCurrAcceptPropId==PLATE_PROPID::BASE::R)
			pConstraint->Execute(BelongModel(),pMasterObj,this);
	}
	else if(this->wiType==TYPE_STATIONCONNPLATE)
	{
		if(wiCurrAcceptPropId==DBOBJ_PROPID::PROP_PROFILE)
			pConstraint->Execute(BelongModel(),pMasterObj,this);
	}
	return false;
}
