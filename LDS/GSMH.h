//###########################################################################
//
//       Module:  GSMH.H
//
//  Decriptions:  This is header file of RC-UMH's Win32 API module.
//
//
//  Copyright (C) 2001 Rainbow China Co.,Ltd. All Rights Reserved.
//
//###########################################################################

#ifndef _GSMH_H_
#define _GSMH_H_


#ifdef  __cplusplus
extern "C" {
#endif


#ifdef _NET_DOG
extern DWORD DogHandle;
#endif
extern short int DogBytes,DogAddr;
extern unsigned long DogPassword;
extern unsigned long NewPassword;
extern unsigned long DogResult;
extern unsigned char DogCascade;
extern void * DogData;

extern unsigned long DogCheck(void);
extern unsigned long ReadDog(void);
extern unsigned long DogConvert(void);
extern unsigned long WriteDog(void);
extern unsigned long DisableShare(void);
extern unsigned long GetCurrentNo(void);
extern unsigned long SetPassword(void);
extern unsigned long SetDogCascade(void);

#ifdef  __cplusplus
}
#endif


#endif //_GSMH_H_
