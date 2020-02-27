// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� INTELLICORE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// INTELLICORE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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