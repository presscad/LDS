// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SUBSTATIONTEMPL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SUBSTATIONTEMPL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TOWER_BOM_EXPORTS
#define TOWER_BOM_EXPORTS_API __declspec(dllexport)
#else
#define TOWER_BOM_EXPORTS_API __declspec(dllimport)
#endif
#pragma once
#include <WinDef.h>

extern "C" TOWER_BOM_EXPORTS_API DWORD GetSupportBOMType();
extern "C" TOWER_BOM_EXPORTS_API DWORD GetSupportDataBufferVersion();
extern "C" TOWER_BOM_EXPORTS_API int GetExcelFormat(int* colIndexArr,int *startRowIndex=NULL,char *titleStr=NULL);
extern "C" TOWER_BOM_EXPORTS_API int GetExcelFormatEx(int* colIndexArr, int *startRowIndex = NULL, char *titleStr = NULL);
//data_buf前4 Byte表示统材信息data buffer的格式版本号
extern "C" TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag);
extern "C" TOWER_BOM_EXPORTS_API void ImportExcelBomSegFile(const char* file_path);
