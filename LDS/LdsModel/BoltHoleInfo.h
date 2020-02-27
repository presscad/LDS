// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////
#pragma once
#include "LinkNode.h"
#include "Hashtable.h"

class CLDSObject;
class CLsRef;
struct BOLTHOLE{
	CLsRef* pLsRef;
	long hResidePart;
public:
	BOLTHOLE(long _hResidePart=0){hResidePart=_hResidePart;pLsRef=0;}
	bool operator==(BOLTHOLE& hole){return pLsRef==hole.pLsRef&&hResidePart==hole.hResidePart;}
public:
	static BOOL IsPropEqual(long idProp);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	bool ModifyPropValueStr(long id,const char* valueStr);
};
typedef LIST_QUEUENODE<BOLTHOLE> QUEUENODE_HOLE;
extern CHashList<QUEUENODE_HOLE> hashBoltHoles;
