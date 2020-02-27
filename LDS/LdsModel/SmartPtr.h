//<LOCALE_TRANSLATE BY hxr /> 2014-12-8
#ifndef __SMART_PTR_
#define __SMART_PTR_
#include "stdafx.h"

template<class T> class CSmartPtr
{
	T* m_pSrcPtr;
public:
	CSmartPtr(T* pSrcPtr=NULL)
	{
		m_pSrcPtr = pSrcPtr;
	}
	~CSmartPtr()
	{
	};
	BOOL IsHasPtr()
	{
		if(m_pSrcPtr!=NULL)
			return TRUE;
		else 
			return FALSE;
	}
	BOOL IsNULL()
	{
		if(m_pSrcPtr==NULL)
			return TRUE;
		else 
			return FALSE;
	}
	BOOL IsEqualPtr(const void* rPtr)
	{
		if(m_pSrcPtr==rPtr) return TRUE;
		else return FALSE;
	}
	CSmartPtr& operator = (CSmartPtr<T>& rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	CSmartPtr& operator = (T* rPtr)
	{
		m_pSrcPtr = rPtr;
		return *this;
	}
	T* operator->()const
	{
		if(!m_pSrcPtr)
#ifdef  AFX_TARG_ENU_ENGLISH
			throw "Encounter an pointer error";
#else
			throw "÷∏’Î‘‚”ˆ ß∞‹";
#endif
		return m_pSrcPtr;
	}
	operator T*()
	{
		return m_pSrcPtr;
	}
};
#endif