// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SUBSTATIONTEMPL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SUBSTATIONTEMPL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
//data_bufǰ4 Byte��ʾͳ����Ϣdata buffer�ĸ�ʽ�汾��
extern "C" TOWER_BOM_EXPORTS_API int CreateExcelBomFile(char* data_buf,int buf_len,const char* file_name,DWORD dwFlag);
extern "C" TOWER_BOM_EXPORTS_API void ImportExcelBomSegFile(const char* file_path);
