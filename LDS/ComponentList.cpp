// ComponentList.cpp: implementation of the CComponentList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "ComponentList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStructure structure;

CComponentList::CComponentList()
{
	base_handle = NULL;
	m_pModified = NULL;
}

CComponentList::~CComponentList()
{

}
