// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C8025264_08CB_4CCC_A231_8B070AE4A8AA__INCLUDED_)
#define AFX_STDAFX_H__C8025264_08CB_4CCC_A231_8B070AE4A8AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include "targetver.h"

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>			// MFC support for CEvent class
#include <afxtempl.h>		// MFC support for template class
#include <afxole.h>			// added by wjh for COM
#include "comdef.h"
#include "NetDogAPI.h"
#include "GSMH.h"
#include "SmartPtr.h"
#include <afxcview.h>		//其中包括了CTreeView
#include "macro.h"
#include "winsvc.h"

#ifndef __DRAWING_CONTEXT_
#include <afxcontrolbars.h> // 功能区和控件条的 MFC 支持
#endif

#define _AFX_SECURE_NO_WARNINGS     // MFC
#define _ATL_SECURE_NO_WARNINGS     // ATL
#define _CRT_SECURE_NO_WARNINGS     // C
#define _CRT_NONSTDC_NO_WARNINGS    // CPOSIX
#define _SCL_SECURE_NO_WARNINGS     // STL
#define _SCL_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)		// VC++ 2005 deprecated function warnings

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C8025264_08CB_4CCC_A231_8B070AE4A8AA__INCLUDED_)
