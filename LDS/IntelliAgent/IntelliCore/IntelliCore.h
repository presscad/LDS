// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 INTELLICORE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// INTELLICORE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define INTELLICORE_API
#else
#ifdef INTELLICORE_EXPORTS
#define INTELLICORE_API __declspec(dllexport)
#else
#define INTELLICORE_API __declspec(dllimport)
#endif
#endif

#include "List.h"
#include "LogFile.h"
#include "../IntelliModel.h"

#ifdef __RAPID_ROUGH_3DMODEL_
INTELLICORE_API int RecognizeAllStdTempl(ITowerModel* pModel,const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListLiveArms,ILog2File* pLogErrFile=NULL);
#endif