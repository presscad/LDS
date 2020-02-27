/* soapStub.h
   Generated by gSOAP 2.8.19 from d:\CimsForTa\TmsServer\TmsServer.h

Copyright(C) 2000-2014, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapStub_H
#define soapStub_H
#define SOAP_NAMESPACE_OF_ns1	"http://xerofox.com/TMSService/"
#include "stdsoap2.h"
#if GSOAP_VERSION != 20819
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_xsd__base64Binary
#define SOAP_TYPE_xsd__base64Binary (7)
/* Base64 schema type: */
class SOAP_CMAC xsd__base64Binary
{
public:
	unsigned char *__ptr;
	int __size;
	char *id;	/* optional element of type xsd:string */
	char *type;	/* optional element of type xsd:string */
	char *options;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 7; } /* = unique type id SOAP_TYPE_xsd__base64Binary */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__base64Binary() { xsd__base64Binary::soap_default(NULL); }
	virtual ~xsd__base64Binary() { }
};
#endif

#ifndef SOAP_TYPE_ns1__ArrayOfInt
#define SOAP_TYPE_ns1__ArrayOfInt (12)
/* ns1:ArrayOfInt */
class SOAP_CMAC ns1__ArrayOfInt
{
public:
	int __sizeint_;	/* sequence of elements <int> */
	int *int_;	/* optional element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 12; } /* = unique type id SOAP_TYPE_ns1__ArrayOfInt */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns1__ArrayOfInt() { ns1__ArrayOfInt::soap_default(NULL); }
	virtual ~ns1__ArrayOfInt() { }
};
#endif

#ifndef SOAP_TYPE__ns1__loginUser
#define SOAP_TYPE__ns1__loginUser (13)
/* ns1:loginUser */
class SOAP_CMAC _ns1__loginUser
{
public:
	char *userName;	/* optional element of type xsd:string */
	char *password;	/* optional element of type xsd:string */
	char *fingerprint;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 13; } /* = unique type id SOAP_TYPE__ns1__loginUser */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__loginUser() { _ns1__loginUser::soap_default(NULL); }
	virtual ~_ns1__loginUser() { }
};
#endif

#ifndef SOAP_TYPE__ns1__loginUserResponse
#define SOAP_TYPE__ns1__loginUserResponse (14)
/* ns1:loginUserResponse */
class SOAP_CMAC _ns1__loginUserResponse
{
public:
	int loginUserResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 14; } /* = unique type id SOAP_TYPE__ns1__loginUserResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__loginUserResponse() { _ns1__loginUserResponse::soap_default(NULL); }
	virtual ~_ns1__loginUserResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__logoutUser
#define SOAP_TYPE__ns1__logoutUser (15)
/* ns1:logoutUser */
class SOAP_CMAC _ns1__logoutUser
{
public:
	int sessionId;	/* required element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 15; } /* = unique type id SOAP_TYPE__ns1__logoutUser */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__logoutUser() { _ns1__logoutUser::soap_default(NULL); }
	virtual ~_ns1__logoutUser() { }
};
#endif

#ifndef SOAP_TYPE__ns1__logoutUserResponse
#define SOAP_TYPE__ns1__logoutUserResponse (16)
/* ns1:logoutUserResponse */
class SOAP_CMAC _ns1__logoutUserResponse
{
public:
	int logoutUserResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 16; } /* = unique type id SOAP_TYPE__ns1__logoutUserResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__logoutUserResponse() { _ns1__logoutUserResponse::soap_default(NULL); }
	virtual ~_ns1__logoutUserResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__QueryObjects
#define SOAP_TYPE__ns1__QueryObjects (17)
/* ns1:QueryObjects */
class SOAP_CMAC _ns1__QueryObjects
{
public:
	int sessionId;	/* required element of type xsd:int */
	char *clsName;	/* optional element of type xsd:string */
	char *xmlScope;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 17; } /* = unique type id SOAP_TYPE__ns1__QueryObjects */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__QueryObjects() { _ns1__QueryObjects::soap_default(NULL); }
	virtual ~_ns1__QueryObjects() { }
};
#endif

#ifndef SOAP_TYPE__ns1__QueryObjectsResponse
#define SOAP_TYPE__ns1__QueryObjectsResponse (18)
/* ns1:QueryObjectsResponse */
class SOAP_CMAC _ns1__QueryObjectsResponse
{
public:
	xsd__base64Binary *QueryObjectsResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:base64Binary */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 18; } /* = unique type id SOAP_TYPE__ns1__QueryObjectsResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__QueryObjectsResponse() { _ns1__QueryObjectsResponse::soap_default(NULL); }
	virtual ~_ns1__QueryObjectsResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__GetObjectsVersion
#define SOAP_TYPE__ns1__GetObjectsVersion (19)
/* ns1:GetObjectsVersion */
class SOAP_CMAC _ns1__GetObjectsVersion
{
public:
	int sessionId;	/* required element of type xsd:int */
	char *clsName;	/* optional element of type xsd:string */
	ns1__ArrayOfInt *idarr;	/* optional element of type ns1:ArrayOfInt */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 19; } /* = unique type id SOAP_TYPE__ns1__GetObjectsVersion */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__GetObjectsVersion() { _ns1__GetObjectsVersion::soap_default(NULL); }
	virtual ~_ns1__GetObjectsVersion() { }
};
#endif

#ifndef SOAP_TYPE__ns1__GetObjectsVersionResponse
#define SOAP_TYPE__ns1__GetObjectsVersionResponse (20)
/* ns1:GetObjectsVersionResponse */
class SOAP_CMAC _ns1__GetObjectsVersionResponse
{
public:
	xsd__base64Binary *GetObjectsVersionResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:base64Binary */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 20; } /* = unique type id SOAP_TYPE__ns1__GetObjectsVersionResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__GetObjectsVersionResponse() { _ns1__GetObjectsVersionResponse::soap_default(NULL); }
	virtual ~_ns1__GetObjectsVersionResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__DownloadFileById
#define SOAP_TYPE__ns1__DownloadFileById (21)
/* ns1:DownloadFileById */
class SOAP_CMAC _ns1__DownloadFileById
{
public:
	int sessionId;	/* required element of type xsd:int */
	char *clsName;	/* optional element of type xsd:string */
	int fileId;	/* required element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 21; } /* = unique type id SOAP_TYPE__ns1__DownloadFileById */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__DownloadFileById() { _ns1__DownloadFileById::soap_default(NULL); }
	virtual ~_ns1__DownloadFileById() { }
};
#endif

#ifndef SOAP_TYPE__ns1__DownloadFileByIdResponse
#define SOAP_TYPE__ns1__DownloadFileByIdResponse (22)
/* ns1:DownloadFileByIdResponse */
class SOAP_CMAC _ns1__DownloadFileByIdResponse
{
public:
	xsd__base64Binary *DownloadFileByIdResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:base64Binary */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 22; } /* = unique type id SOAP_TYPE__ns1__DownloadFileByIdResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__DownloadFileByIdResponse() { _ns1__DownloadFileByIdResponse::soap_default(NULL); }
	virtual ~_ns1__DownloadFileByIdResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__SaveOrUpdateObjects
#define SOAP_TYPE__ns1__SaveOrUpdateObjects (23)
/* ns1:SaveOrUpdateObjects */
class SOAP_CMAC _ns1__SaveOrUpdateObjects
{
public:
	int sessionId;	/* required element of type xsd:int */
	char *clsName;	/* optional element of type xsd:string */
	char *xmlformat;	/* optional element of type xsd:string */
	xsd__base64Binary *byteArr;	/* optional element of type xsd:base64Binary */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 23; } /* = unique type id SOAP_TYPE__ns1__SaveOrUpdateObjects */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__SaveOrUpdateObjects() { _ns1__SaveOrUpdateObjects::soap_default(NULL); }
	virtual ~_ns1__SaveOrUpdateObjects() { }
};
#endif

#ifndef SOAP_TYPE__ns1__SaveOrUpdateObjectsResponse
#define SOAP_TYPE__ns1__SaveOrUpdateObjectsResponse (24)
/* ns1:SaveOrUpdateObjectsResponse */
class SOAP_CMAC _ns1__SaveOrUpdateObjectsResponse
{
public:
	char *SaveOrUpdateObjectsResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 24; } /* = unique type id SOAP_TYPE__ns1__SaveOrUpdateObjectsResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__SaveOrUpdateObjectsResponse() { _ns1__SaveOrUpdateObjectsResponse::soap_default(NULL); }
	virtual ~_ns1__SaveOrUpdateObjectsResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__DeleteObject
#define SOAP_TYPE__ns1__DeleteObject (25)
/* ns1:DeleteObject */
class SOAP_CMAC _ns1__DeleteObject
{
public:
	int sessionId;	/* required element of type xsd:int */
	char *clsName;	/* optional element of type xsd:string */
	int objId;	/* required element of type xsd:int */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 25; } /* = unique type id SOAP_TYPE__ns1__DeleteObject */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__DeleteObject() { _ns1__DeleteObject::soap_default(NULL); }
	virtual ~_ns1__DeleteObject() { }
};
#endif

#ifndef SOAP_TYPE__ns1__DeleteObjectResponse
#define SOAP_TYPE__ns1__DeleteObjectResponse (26)
/* ns1:DeleteObjectResponse */
class SOAP_CMAC _ns1__DeleteObjectResponse
{
public:
	bool DeleteObjectResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 26; } /* = unique type id SOAP_TYPE__ns1__DeleteObjectResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__DeleteObjectResponse() { _ns1__DeleteObjectResponse::soap_default(NULL); }
	virtual ~_ns1__DeleteObjectResponse() { }
};
#endif

#ifndef SOAP_TYPE___ns1__loginUser
#define SOAP_TYPE___ns1__loginUser (34)
/* Operation wrapper: */
struct __ns1__loginUser
{
public:
	_ns1__loginUser *ns1__loginUser;	/* optional element of type ns1:loginUser */
public:
	int soap_type() const { return 34; } /* = unique type id SOAP_TYPE___ns1__loginUser */
};
#endif

#ifndef SOAP_TYPE___ns1__logoutUser
#define SOAP_TYPE___ns1__logoutUser (38)
/* Operation wrapper: */
struct __ns1__logoutUser
{
public:
	_ns1__logoutUser *ns1__logoutUser;	/* optional element of type ns1:logoutUser */
public:
	int soap_type() const { return 38; } /* = unique type id SOAP_TYPE___ns1__logoutUser */
};
#endif

#ifndef SOAP_TYPE___ns1__QueryObjects
#define SOAP_TYPE___ns1__QueryObjects (42)
/* Operation wrapper: */
struct __ns1__QueryObjects
{
public:
	_ns1__QueryObjects *ns1__QueryObjects;	/* optional element of type ns1:QueryObjects */
public:
	int soap_type() const { return 42; } /* = unique type id SOAP_TYPE___ns1__QueryObjects */
};
#endif

#ifndef SOAP_TYPE___ns1__GetObjectsVersion
#define SOAP_TYPE___ns1__GetObjectsVersion (46)
/* Operation wrapper: */
struct __ns1__GetObjectsVersion
{
public:
	_ns1__GetObjectsVersion *ns1__GetObjectsVersion;	/* optional element of type ns1:GetObjectsVersion */
public:
	int soap_type() const { return 46; } /* = unique type id SOAP_TYPE___ns1__GetObjectsVersion */
};
#endif

#ifndef SOAP_TYPE___ns1__DownloadFileById
#define SOAP_TYPE___ns1__DownloadFileById (50)
/* Operation wrapper: */
struct __ns1__DownloadFileById
{
public:
	_ns1__DownloadFileById *ns1__DownloadFileById;	/* optional element of type ns1:DownloadFileById */
public:
	int soap_type() const { return 50; } /* = unique type id SOAP_TYPE___ns1__DownloadFileById */
};
#endif

#ifndef SOAP_TYPE___ns1__SaveOrUpdateObjects
#define SOAP_TYPE___ns1__SaveOrUpdateObjects (54)
/* Operation wrapper: */
struct __ns1__SaveOrUpdateObjects
{
public:
	_ns1__SaveOrUpdateObjects *ns1__SaveOrUpdateObjects;	/* optional element of type ns1:SaveOrUpdateObjects */
public:
	int soap_type() const { return 54; } /* = unique type id SOAP_TYPE___ns1__SaveOrUpdateObjects */
};
#endif

#ifndef SOAP_TYPE___ns1__DeleteObject
#define SOAP_TYPE___ns1__DeleteObject (58)
/* Operation wrapper: */
struct __ns1__DeleteObject
{
public:
	_ns1__DeleteObject *ns1__DeleteObject;	/* optional element of type ns1:DeleteObject */
public:
	int soap_type() const { return 58; } /* = unique type id SOAP_TYPE___ns1__DeleteObject */
};
#endif

#ifndef SOAP_TYPE___ns1__loginUser_
#define SOAP_TYPE___ns1__loginUser_ (60)
/* Operation wrapper: */
struct __ns1__loginUser_
{
public:
	_ns1__loginUser *ns1__loginUser;	/* optional element of type ns1:loginUser */
public:
	int soap_type() const { return 60; } /* = unique type id SOAP_TYPE___ns1__loginUser_ */
};
#endif

#ifndef SOAP_TYPE___ns1__logoutUser_
#define SOAP_TYPE___ns1__logoutUser_ (62)
/* Operation wrapper: */
struct __ns1__logoutUser_
{
public:
	_ns1__logoutUser *ns1__logoutUser;	/* optional element of type ns1:logoutUser */
public:
	int soap_type() const { return 62; } /* = unique type id SOAP_TYPE___ns1__logoutUser_ */
};
#endif

#ifndef SOAP_TYPE___ns1__QueryObjects_
#define SOAP_TYPE___ns1__QueryObjects_ (64)
/* Operation wrapper: */
struct __ns1__QueryObjects_
{
public:
	_ns1__QueryObjects *ns1__QueryObjects;	/* optional element of type ns1:QueryObjects */
public:
	int soap_type() const { return 64; } /* = unique type id SOAP_TYPE___ns1__QueryObjects_ */
};
#endif

#ifndef SOAP_TYPE___ns1__GetObjectsVersion_
#define SOAP_TYPE___ns1__GetObjectsVersion_ (66)
/* Operation wrapper: */
struct __ns1__GetObjectsVersion_
{
public:
	_ns1__GetObjectsVersion *ns1__GetObjectsVersion;	/* optional element of type ns1:GetObjectsVersion */
public:
	int soap_type() const { return 66; } /* = unique type id SOAP_TYPE___ns1__GetObjectsVersion_ */
};
#endif

#ifndef SOAP_TYPE___ns1__DownloadFileById_
#define SOAP_TYPE___ns1__DownloadFileById_ (68)
/* Operation wrapper: */
struct __ns1__DownloadFileById_
{
public:
	_ns1__DownloadFileById *ns1__DownloadFileById;	/* optional element of type ns1:DownloadFileById */
public:
	int soap_type() const { return 68; } /* = unique type id SOAP_TYPE___ns1__DownloadFileById_ */
};
#endif

#ifndef SOAP_TYPE___ns1__SaveOrUpdateObjects_
#define SOAP_TYPE___ns1__SaveOrUpdateObjects_ (70)
/* Operation wrapper: */
struct __ns1__SaveOrUpdateObjects_
{
public:
	_ns1__SaveOrUpdateObjects *ns1__SaveOrUpdateObjects;	/* optional element of type ns1:SaveOrUpdateObjects */
public:
	int soap_type() const { return 70; } /* = unique type id SOAP_TYPE___ns1__SaveOrUpdateObjects_ */
};
#endif

#ifndef SOAP_TYPE___ns1__DeleteObject_
#define SOAP_TYPE___ns1__DeleteObject_ (72)
/* Operation wrapper: */
struct __ns1__DeleteObject_
{
public:
	_ns1__DeleteObject *ns1__DeleteObject;	/* optional element of type ns1:DeleteObject */
public:
	int soap_type() const { return 72; } /* = unique type id SOAP_TYPE___ns1__DeleteObject_ */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (73)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	int soap_type() const { return 73; } /* = unique type id SOAP_TYPE_SOAP_ENV__Header */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (74)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
public:
	int soap_type() const { return 74; } /* = unique type id SOAP_TYPE_SOAP_ENV__Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (76)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
public:
	int soap_type() const { return 76; } /* = unique type id SOAP_TYPE_SOAP_ENV__Detail */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (79)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 79; } /* = unique type id SOAP_TYPE_SOAP_ENV__Reason */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (80)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
public:
	int soap_type() const { return 80; } /* = unique type id SOAP_TYPE_SOAP_ENV__Fault */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


#endif

/* End of soapStub.h */