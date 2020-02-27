//
// StdArx.h : include file for ObjectARX/DBX include files
// this file is only included once by your stdafx.h

#if !defined(AFX_STDARX_H__CE7684FC_0017_4C7E_9F40_D9FB0D1BBA5A__INCLUDED_)
#define AFX_STDARX_H__CE7684FC_0017_4C7E_9F40_D9FB0D1BBA5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#define NDEBUG
#endif*/

#if !defined(_MAPTMA_GRX)&&!defined(_ARX_2007)
#include "rxdebug.h"
#endif
#include "docdata.h"          // Your document specific data
// AutoCAD MFC Extensions:
#include "adui.h"
#include "adsdef.h"
#include "acedads.h"
#include "acdb.h"               // acdb definitions
#include "rxregsvc.h"           // ARX linker
#include "dbapserv.h"           // Host application services
#include "aced.h"               // aced stuff
#include "adslib.h"             // RXADS definitions
#include "acdocman.h"           // MDI document manager
#include "rxmfcapi.h"           // ObjectARX MFC support
#include "actrans.h"			// for dinfine actrTransactionManager
#include "dbpl.h"				// AcDbPolyline
#include "dbhatch.h"			// AcDbHatch
#include "dbxrecrd.h"
#include "dbmain.h"
#include "dblayout.h"
#include "acaplmgr.h"
#include "dbplotsetval.h"
#include "dbplotsettings.h"
#include "AcExtensionModule.h"  // Utility class for extension dlls

/*#ifdef _DEBUG_WAS_DEFINED
#undef NDEBUG
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif*/
// Declare it as an extern here so that it
// becomes available in all modules
//extern AsdkDataManager<CDocData> DocVars;

//{{AFX_ARX_FUNC
//}}AFX_ARX_FUNC

// TODO: Here you can add your own includes / declarations
#endif 
