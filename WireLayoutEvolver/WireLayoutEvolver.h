// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 WIRELAYOUT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// WIRELAYOUT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef WIRELAYOUTEVOLVER_EXPORTS
#define WIRELAYOUTEVOLVER_API __declspec(dllexport)
#else
#define WIRELAYOUTEVOLVER_API __declspec(dllimport)
#endif
#include "Buffer.h"
// 此类是从 WireLayout.dll 导出的
class WIRELAYOUTEVOLVER_API CWireLayoutEvolver {
public:
	CWireLayoutEvolver(void);
	static bool WriteWeather(char* weather_buf,unsigned long size);
	static bool ReadWireLayoutData(CBuffer* pBuff,long version=0,long doc_type=0);
	static bool EvolveWireLayoutToWireLoadBuffer(CBuffer *pBuffer);
};
