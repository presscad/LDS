#include "stdafx.h"
#include "GSMH.h"

//-----------DOG VARIARBLE--------------
short int DogBytes, DogAddr;
#ifdef _NET_DOG
DWORD DogHandle=0;
unsigned long DogPassword=2896942775;
#else
unsigned long DogPassword=387703383;
#endif
unsigned long NewPassword;
unsigned long DogResult;
unsigned char DogCascade;
void * DogData;
//-----------DOG VARIARBLE--------------

BOOL IsHasDesign()
{
	BYTE version_verify[4];
	DogData = version_verify;
	DogBytes = 4;
	DogAddr = 102;
	ReadDog();
	return TRUE;
}
BOOL IsHasPartMap()
{
	return TRUE;
}