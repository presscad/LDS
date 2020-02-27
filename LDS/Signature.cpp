#include "stdafx.h"
#include "Signature.h"
#include "LDS.h"

BOOL GetSignature(BYTE buf[21])
{
	FILE *fp;
	char path[MAX_PATH];
	GetSysPath(path);
#ifdef __LMA_
	strcat(path,"LatticeTower.sig");
#elif defined(__TMA_)
	strcat(path,"TMA.sig");
#elif defined(__TSA_)
	strcat(path,"TSA.sig");
#else
	strcat(path,"LDS.sig");
#endif
	if((fp=fopen(path,"rb"))==NULL)
		return FALSE;
	fread(buf,20,1,fp);
	buf[20]='\0';
	fclose(fp);
	return TRUE;
}
BOOL WriteSignature(BYTE buf[21])
{
	FILE *fp;
	char path[MAX_PATH];
	GetSysPath(path);
#ifdef __LMA_
	strcat(path,"LatticeTower.sig");
#elif defined(__TMA_)
	strcat(path,"TMA.sig");
#elif defined(__TSA_)
	strcat(path,"TMA.sig");
#else
	strcat(path,"LDS.sig");
#endif
	if((fp=fopen(path,"wb"))==NULL)
		return FALSE;
	fread(buf,20,1,fp);
	buf[20]='\0';
	fclose(fp);
	return TRUE;
}
