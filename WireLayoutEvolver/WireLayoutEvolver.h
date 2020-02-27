// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� WIRELAYOUT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// WIRELAYOUT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef WIRELAYOUTEVOLVER_EXPORTS
#define WIRELAYOUTEVOLVER_API __declspec(dllexport)
#else
#define WIRELAYOUTEVOLVER_API __declspec(dllimport)
#endif
#include "Buffer.h"
// �����Ǵ� WireLayout.dll ������
class WIRELAYOUTEVOLVER_API CWireLayoutEvolver {
public:
	CWireLayoutEvolver(void);
	static bool WriteWeather(char* weather_buf,unsigned long size);
	static bool ReadWireLayoutData(CBuffer* pBuff,long version=0,long doc_type=0);
	static bool EvolveWireLayoutToWireLoadBuffer(CBuffer *pBuffer);
};
