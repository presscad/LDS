// HashObjIdList.cpp: implementation of the CHashObjIdList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "lds.h"
#include "Buffer.h"
#include "HashObjIdList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHashObjIdList::CHashObjIdList()
{// ���캯��
	NodeNum=0;
	cursor = head = tail = NULL;
	m_uHashTableGrowSize=10000;
}

CHashObjIdList::~CHashObjIdList()
{
	Empty();
}
