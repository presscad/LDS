// FuncLifeObject.h: interface for the CFuncLifeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCLIFEOBJECT_H__44C71A81_9546_4273_AF24_82A3ED48A71F__INCLUDED_)
#define AFX_FUNCLIFEOBJECT_H__44C71A81_9546_4273_AF24_82A3ED48A71F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFuncLifeObject  
{
	void (*_dump_func)(void* lpDumpInfo);
public:
	void* m_lpDumpInfo;
	CFuncLifeObject(void (*dump_func)(void* lpDumpInfo),void* lpDumpInfo=NULL);
	virtual ~CFuncLifeObject();
};

#endif // !defined(AFX_FUNCLIFEOBJECT_H__44C71A81_9546_4273_AF24_82A3ED48A71F__INCLUDED_)
