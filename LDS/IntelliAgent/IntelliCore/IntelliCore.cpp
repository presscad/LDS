// IntelliCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "IntelliCoreObject.h"
#include "IntelliCore.h"

#if !defined(APP_EMBEDDED_MODULE)&&!defined(APP_EMBEDDED_MODULE_ENCRYPT)
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

#ifdef __RAPID_ROUGH_3DMODEL_
INTELLICORE_API int RecognizeAllStdTempl(ITowerModel* pModel,const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListLiveArms,ILog2File* pLogErrFile/*=NULL*/)
{
	CIntelliCore core;
	return core.RecognizeAllStdTempl(pModel,vxSlopeTop3d,vxSlopeBtm3d,pListLiveArms,pLogErrFile);
}
#endif
