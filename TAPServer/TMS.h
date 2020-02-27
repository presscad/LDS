#pragma once

#ifdef __CONNECT_REMOTE_SERVER_
#include "Buffer.h"
#include "soapH.h"
//#include "TMSServiceSoap.nsmap"
#include "soapTMSServiceSoapProxy.h"
#include "XhCharString.h"

class CTmsServer
{
public:
	int m_idSession;
	TMSServiceSoapProxy* m_pServer;
public:
	CTmsServer(void);
	~CTmsServer(void);
public:
	//clsName有效值为:"TowerTypeFileGroup","TowerTypeFile","TowerAssembly","TowerPart","AssemblePart",
	bool QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff);
    UINT GetObjectsVersion(int sessionId, const char* clsName, int* idarr, int countOfObj=1);
	bool DownloadFileById(const char* clsName,int fileId,BUFFER_IO* pExterLocalBuff);
 	bool DeleteObject(const char* clsName,int objId);
	int  SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj=1);
public:
	void SetServerUrl(const char* url);
	bool LoginUser(const char* userName, const char* password, const char* fingerprint=NULL);
	bool DownloadTowerTypeFile(void* pTowerTypeFile);
	bool QueryStateTowerTypes(BUFFER_IO* pOutLocalBuff,int state=1,int fromdate=0,int todate=0);
	int  QueryProcessParts(BUFFER_IO* pOutLocalBuff,int towerTypeId,CXhChar16* arrLabels,int labelCount=1);
};
extern CTmsServer TMS;
extern SOAP_NMAC struct Namespace namespaces[];
#endif
