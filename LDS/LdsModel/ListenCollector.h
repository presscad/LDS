#pragma once
#include "lds_part.h"
#include "HashTable.h"

class CListenCollector
{
public:
	CHashSet<CLDSPart*> hashBoltRelaParts;
};
extern CListenCollector collector;
