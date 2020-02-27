// FuncLifeObject.cpp: implementation of the CFuncLifeObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lds.h"
#include "FuncLifeObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFuncLifeObject::CFuncLifeObject(void (*dump_func)(void* lpDumpInfo),void* lpDumpInfo/*=NULL*/)
{
	_dump_func=dump_func;
	m_lpDumpInfo=lpDumpInfo;
}

CFuncLifeObject::~CFuncLifeObject()
{
	if(_dump_func)
		_dump_func(m_lpDumpInfo);
}
