// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PARSETOWERFILE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PARSETOWERFILE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PARSETOWERFILE_EXPORTS
#define PARSETOWERFILE_API __declspec(dllexport)
#else
#define PARSETOWERFILE_API __declspec(dllimport)
#endif

// �����Ǵ� ParseTowerFile.dll ������
PARSETOWERFILE_API int PrepareTidBuffer(const char *sFilePath,
						void (*DisplayProcessFunc)(int percent,const char *sTitle),
						int startProcess=0,double processCoef=1,void *pLogFile=NULL);
PARSETOWERFILE_API int PrepareTidBufferFromTowerBuffer(char *buf,int buf_len,
						void (*DisplayProcessFunc)(int percent,const char *sTitle),
						int startProcess=0,double processCoef=1,void *pLogFile=NULL);
PARSETOWERFILE_API bool ReadTidBuffer(char *tid_buffer);
