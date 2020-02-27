// StdAfx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__77B36E85_043C_408B_8389_92C16574AF21__INCLUDED_)
#define AFX_STDAFX_H__77B36E85_043C_408B_8389_92C16574AF21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef STRICT
#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600	//vs7.1 ������ vs10�ı仯
#endif
#endif

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)
#pragma warning(disable: 4819)

// 'DEBUG workaround' below prevents the MFC or ATL #include-s 
// from pulling in "afx.h" that would force the debug CRT through
// #pragma-s.
/* ����֤����2010֮�󣬼�ʹ����Debug�汾arx�ļ���Ҳ������ʹ��_DEBUG����������������Release��MFC��
   ������±��������趨���������壬���һ���������� wjh-2019.8.9
#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#define NDEBUG
#endif*/

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>          // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>         // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>			// MFC support for CEvent class
#include <afxtempl.h>		// MFC support for template class

#include <afxcview.h>		//���а�����CTreeView
#include <comutil.h>		//_bstr_t����
/*
#ifdef _DEBUG_WAS_DEFINED
#undef NDEBUG
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif
*/
#include "macro.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
// RX Includes
#endif // !defined(AFX_STDAFX_H__77B36E85_043C_408B_8389_92C16574AF21__INCLUDED)
