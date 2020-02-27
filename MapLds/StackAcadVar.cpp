//<LOCALE_TRANSLATE/>
#include <stdafx.h>
#include "ads.h"		//����Ƴ�GRX�ӿ�ʱ�������ads.h�������޷��ҵ�acedGetVar���� wjh-2019.12.7
#include "acadstrc.h"	//define Acad::ErrorStatus
#include "acedads.h"	//define acedGetVar&acedSetVar
#include "adscodes.h"	//����resbuf.restype
#include "StackAcadVar.h"

CStackAcadVar::CStackAcadVar(const char* szVarName,long liVarValue)
{
	m_bVarPushed=false;
	m_liAcedVarValue=0;
	resbuf sysvar;
	memset(&sysvar,0,sizeof(resbuf));
#if defined(_ARX_2007)//&&!defined(_MAPTMA_GRX)
	int retcode=acedGetVar(_bstr_t(szVarName),&sysvar);
#else
	int retcode=acedGetVar(szVarName,&sysvar);
#endif
	m_bVarPushed=true;
	memset(m_szVarName,0,32);
	strncpy(m_szVarName,szVarName,31);
	if (stricmp(m_szVarName,"CMDECHO")==0||	//��ͣ�����н�����ʾ:0.�ر�;1.����
		stricmp(m_szVarName,"GRIPTIPS")==0||
		stricmp(m_szVarName,"AUTOSNAP")==0||
		stricmp(m_szVarName,"OSMODE")==0||	//|0x4000��ʾ�رյ�ǰ����׽ģʽ
		stricmp(m_szVarName,"INPUTHISTORYMODE")==0||	//Acad2012
		stricmp(m_szVarName,"INPUTSEARCHOPTIONS")==0||	//>=Acad2013
		stricmp(m_szVarName,"GRIPMULTIFUNCTIONAL")==0)
	{
		m_liAcedVarValue=sysvar.resval.rint;
		sysvar.resval.rint=(short)liVarValue;
	}
	//{
	//	head = acutBuildList(AcDb::kDxfInt32,handle,0);
	//	acutRelRb(head);
	//}
	else
	{
		m_bVarPushed=false;
		return;
	}
#ifdef _ARX_2007
	acedSetVar(_bstr_t(szVarName),&sysvar);
#else
	acedSetVar(szVarName,&sysvar);
#endif
}
bool CStackAcadVar::PopupAcadVar()
{
	if (!m_bVarPushed)
		return false;
	resbuf sysvar;
	memset(&sysvar,0,sizeof(resbuf));
	if (stricmp(m_szVarName,"CMDECHO")==0||	//��ͣ�����н�����ʾ:0.�ر�;1.����
		stricmp(m_szVarName,"GRIPTIPS")==0||
		stricmp(m_szVarName,"AUTOSNAP")==0||
		stricmp(m_szVarName,"OSMODE")==0||	//|0x4000��ʾ�رյ�ǰ����׽ģʽ
		stricmp(m_szVarName,"INPUTHISTORYMODE")==0||	//Acad2012
		stricmp(m_szVarName,"INPUTSEARCHOPTIONS")==0||	//>=Acad2013
		stricmp(m_szVarName,"GRIPMULTIFUNCTIONAL")==0)
	{
		sysvar.restype=RTSHORT;
		sysvar.resval.rint=(short)m_liAcedVarValue;
	}
	else
		return false;
#ifdef _ARX_2007
	int retcode=acedSetVar(_bstr_t(m_szVarName),&sysvar);
#else
	int retcode=acedSetVar(m_szVarName,&sysvar);
#endif
	return true;
}
CStackAcadVar::~CStackAcadVar()
{
	PopupAcadVar();
}
