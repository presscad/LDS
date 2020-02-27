// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#define NDEBUG
#endif

#define _AFX_SECURE_NO_WARNINGS     // MFC
#define _ATL_SECURE_NO_WARNINGS     // ATL
#define _CRT_SECURE_NO_WARNINGS     // C
#define _CRT_NONSTDC_NO_WARNINGS    // CPOSIX
#define _SCL_SECURE_NO_WARNINGS     // STL
#define _SCL_SECURE_NO_DEPRECATE
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序要求的附加头文件
