
/////////////////////////////////////////////////////////////////////////
/* 
	Copyright (C) Rainbow China Co., Ltd.	
	All rights reserved.

  File name:	NetDogAPI.h
  File description:	
	define the command code, data struct and interface

	Date :	2003-5-6
	Author:	Jason Jiang



	Change history:		
	----Version:----Mender Name:----Date:----Change description----		



*/ 
/////////////////////////////////////////////////////////////////////////

#ifndef _NETDOGAPI_H
#define _NETDOGAPI_H

const BYTE CMD_LOGIN			=	1;		// login
const BYTE CMD_LOGOUT			=	2;		// logout
const BYTE CMD_READ				=	3;		// read dog
const BYTE CMD_GETCURRNO		=	4;		// get current no.
const BYTE CMD_CONVERT			=	5;		// convert
const BYTE CMD_WRITE			=	11;		// write dog
const BYTE CMD_READEX			=	12;		// read from dog's extend part 
const BYTE CMD_WRITEEX			=	13;		// write to dog's extend part
const BYTE CMD_VIRTUAL_READ		=	14;		// virtual read dog
const BYTE CMD_VIRTUAL_WRITE	=	15;		// virtaul write dog
const BYTE CMD_READ_TIME		=	16;		// read time from dog
const BYTE CMD_READ_TIMELIMIT	=	17;		// read time limit from dog
const BYTE CMD_WRITE_TIME		=	18;		// write time to dog
const BYTE CMD_WRITE_TIMELIMIT	=	19;		// write time limit to dog

const ULONG NETDOG_SUCCESS		=	0;

#pragma pack(1)
typedef struct
{
	unsigned char timescet[6] ; // time limit
	unsigned char mask;			// 0 bit value identify to use software mask or not.
								// 0 - invalid 1 - valid
	unsigned char reserved[9];	// reserved
}TIME_LIMIT, *PTIME_LIMIT;
#pragma pack()

#ifdef  __cplusplus
extern "C" {
#endif

extern ULONG __cdecl NetDog(WORD wModule, BYTE OPCODE ,DWORD * pdwDogHandle, 
						    PVOID pDogData,DWORD *pdwDogResult, WORD wDogAddr, 
					        WORD wDogBytes, DWORD dwDogPassword,DWORD dwReserved
						   );

#ifdef  __cplusplus
}
#endif

#endif		// _NETDOGAPI_H