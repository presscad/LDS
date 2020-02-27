#include "stdafx.h"
#include "TMS.h"
#include "ServerTowerType.h"

#ifdef __CONNECT_REMOTE_SERVER_
SOAP_NMAC struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "http://xerofox.com/TMSService/", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
/////////////////////////////////////////////////////////////////////////////
CTmsServer TMS;
CTmsServer::CTmsServer(void)
{
	m_pServer=NULL;
	m_idSession=0;
}


CTmsServer::~CTmsServer(void)
{
	if(m_pServer)
	{
		m_pServer->destroy();
		delete m_pServer;
		m_pServer=NULL;
	}
}
static char g_sServerURL[MAX_PATH]="";
void CTmsServer::SetServerUrl(const char* url)
{
	sprintf(g_sServerURL,"http://%s/TMSService.asmx",url);
}
bool CTmsServer::LoginUser(const char* userName, const char* password, const char* fingerprint/*=NULL*/)
{
	if(g_sServerURL[0]==0)
		strcpy(g_sServerURL,"http://127.0.0.1/MESServer/TMSService.asmx");
	m_pServer=new TMSServiceSoapProxy(g_sServerURL);
	CXhChar50 UserName(userName),Password(password);
	_ns1__loginUser inputPara;
	inputPara.userName=UserName;
	inputPara.fingerprint="";
	inputPara.password=Password;
	_ns1__loginUserResponse response;
	if(m_pServer->loginUser(&inputPara,response)!=SOAP_OK||response.loginUserResult<=0)//Login调用失败表示联网失败!
		return false;
	else 
	{
		m_idSession=response.loginUserResult;
		return true;
	}
}
bool CTmsServer::QueryObjects(const char* clsName,char* xmlscope,BUFFER_IO* pExterLocalBuff)
{
	_ns1__QueryObjects inputPara;
	inputPara.sessionId=m_idSession;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)	//文件组,每个文件组内所有文件表示同一个模型相同分段的不同历史版本
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerTypeFile")==0)	//指定模型分段的某一特定上传历史版本
		inputPara.clsName="TowerTypeFile";
	else if(strcmp(clsName,"TowerAssembly")==0)	//指定塔位的试组塔例模型
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)		//塔型工件(钢板与角钢)的三维工艺数据交换信息
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)	//塔型工件在塔例模型中的装配记录对象
		inputPara.clsName="AssemblePart";
	else
		return false;
	inputPara.xmlScope=xmlscope;
	_ns1__QueryObjectsResponse response;
	if(m_pServer->QueryObjects(&inputPara,response)!=SOAP_OK||response.QueryObjectsResult==NULL
		||response.QueryObjectsResult->__size<=0||response.QueryObjectsResult->__ptr==NULL)
		return false;
	pExterLocalBuff->Write(response.QueryObjectsResult->__ptr,response.QueryObjectsResult->__size);
	return true;
}
UINT CTmsServer::GetObjectsVersion(int sessionId, const char* clsName, int* idarr, int countOfObj/*=1*/)
{
	_ns1__GetObjectsVersion inputPara;
	inputPara.sessionId=m_idSession;
	if(strcmp(clsName,"TowerTypeFile")==0)	//指定模型分段的某一特定上传历史版本
		inputPara.clsName="TowerTypeFile";
	else if(strcmp(clsName,"TowerPart")==0)		//塔型工件(钢板与角钢)的三维工艺数据交换信息
		inputPara.clsName="TowerPart";
	ns1__ArrayOfInt __ns1__idarr;
	__ns1__idarr.__sizeint_=countOfObj;
	__ns1__idarr.int_=idarr;
	inputPara.idarr=&__ns1__idarr;
	_ns1__GetObjectsVersionResponse response;
	if(m_pServer->GetObjectsVersion(&inputPara,response)!=SOAP_OK||response.GetObjectsVersionResult==NULL
		||response.GetObjectsVersionResult->__size<=0||response.GetObjectsVersionResult->__ptr==NULL)
		return false;
	//pExterLocalBuff->Write(response.GetObjectsVersionResult->__ptr,response.GetObjectsVersionResult->__size);
	return false;
}
bool CTmsServer::DownloadFileById(const char* clsName,int fileId,BUFFER_IO* pExterLocalBuff)
{
	_ns1__DownloadFileById inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.fileId=fileId;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerTypeFile")==0)
		inputPara.clsName="TowerTypeFile";
	else if(strcmp(clsName,"TowerAssembly")==0)
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)
		inputPara.clsName="AssemblePart";
	else
		return false;
	_ns1__DownloadFileByIdResponse response;
	if(m_pServer->DownloadFileById(&inputPara,response)!=SOAP_OK||response.DownloadFileByIdResult==NULL
		||response.DownloadFileByIdResult->__size<=0||response.DownloadFileByIdResult->__ptr==NULL)
		return false;
	pExterLocalBuff->Write(response.DownloadFileByIdResult->__ptr,response.DownloadFileByIdResult->__size);
	return false;
}
bool CTmsServer::DownloadTowerTypeFile(void* pTowerTypeFile)
{
	CServerTowerFile* pFile=(CServerTowerFile*)pTowerTypeFile;
	CBuffer buffer;
	if(!DownloadFileById("TowerTypeFile",pFile->m_idFile,&buffer))
		return false;
	pFile->FromBuffer(buffer);
	return true;
}
int  CTmsServer::SaveOrUpdateObjects(const char* clsName,void* ppLocalObjArr,int countOfObj/*=1*/)
{
	int id=-1;
	CBuffer buffer(0x100000);
	_ns1__SaveOrUpdateObjects inputPara;
	inputPara.byteArr=new xsd__base64Binary();
	inputPara.sessionId=m_idSession;
	_ns1__SaveOrUpdateObjectsResponse response;
	if(strcmp(clsName,"TowerTypeFile")==0)
	{
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerFile *pTowerFile=(CServerTowerFile*)((CServerTowerFile**)ppLocalObjArr)[i];
			pTowerFile->ToBuffer(buffer);
		}
		inputPara.clsName="TowerTypeFile";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerTypeFile\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(" </SaveOrUpdate>");
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		delete inputPara.byteArr;
	}
	else if(strcmp(clsName,"TowerTypeFileGroup")==0)
	{
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerFileGroup *pTowerFileGroup=(CServerTowerFileGroup*)((CServerTowerFileGroup**)ppLocalObjArr)[i];
			pTowerFileGroup->ToBuffer(buffer);
		}
		inputPara.clsName="TowerTypeFileGroup";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
	}
	if(strcmp(clsName,"TowerPart")==0)
	{
		for(int i=0;i<countOfObj;i++)
		{
			CServerTowerPart *pTowerPart=(CServerTowerPart*)((CServerTowerPart**)ppLocalObjArr)[i];
			pTowerPart->ToBuffer(buffer);
		}
		inputPara.clsName="TowerPart";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		CXhChar500 xmlformat;
		char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
						" <SaveOrUpdate>";
		xmlformat.Copy(xmlheader);
		xmlformat.Append("     <ClassName value=\"TowerPart\" />");
		xmlformat.Append(CXhChar50("     <ObjectCount value=\"%d\" />",countOfObj));
		xmlformat.Append(" </SaveOrUpdate>");
		inputPara.xmlformat=xmlformat;

		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
			CServerObject::ExtractObjArrFromXmlStr(response.SaveOrUpdateObjectsResult,(CServerObject**)ppLocalObjArr,countOfObj);
		delete inputPara.byteArr;
	}
	/*else if(strcmp(clsName,"TowerAssembly")==0)
	{
		CServerTowerAssembly *pTowerAssembly=(CServerTowerAssembly*)pLocalObj;
		pTowerAssembly->ToBuffer(buffer);
		inputPara.clsName="TowerAssembly";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
		{
			ParseSaveOrUpdateServerTowerAssemblyRetStr(pTowerAssembly,response.SaveOrUpdateObjectsResult);
			pTowerAssembly->m_id=atoi(response.SaveOrUpdateObjectsResult);
		}
	}
	else if(strcmp(clsName,"AssemblePart")==0)
	{
		CServerAssemblePart *pAssemblePart=(CServerAssemblePart*)pLocalObj;
		pAssemblePart->ToBuffer(buffer);
		inputPara.clsName="AssemblePart";
		inputPara.byteArr->__ptr=(BYTE*)buffer.GetBufferPtr();
		inputPara.byteArr->__size=buffer.GetLength();
		if(m_pServer->SaveOrUpdateObjects(&inputPara,response)==SOAP_OK)
		{
			ParseSaveOrUpdateServerAssemblePartRetStr(pAssemblePart,response.SaveOrUpdateObjectsResult);
			pAssemblePart->m_id=atoi(response.SaveOrUpdateObjectsResult);
		}
	}*/
	delete inputPara.byteArr;
	return id;
}
bool CTmsServer::DeleteObject(const char* clsName,int objId)
{
	_ns1__DeleteObject inputPara;
	inputPara.sessionId=m_idSession;
	inputPara.objId=objId;
	_ns1__DeleteObjectResponse response;
	if(strcmp(clsName,"TowerTypeFileGroup")==0)
		inputPara.clsName="TowerTypeFileGroup";
	else if(strcmp(clsName,"TowerTypeFile")==0)
		inputPara.clsName="TowerTypeFile";
	else if(strcmp(clsName,"TowerAssembly")==0)
		inputPara.clsName="TowerAssembly";
	else if(strcmp(clsName,"TowerPart")==0)
		inputPara.clsName="TowerPart";
	else if(strcmp(clsName,"AssemblePart")==0)
		inputPara.clsName="AssemblePart";
	else 
		return false;
	if(m_pServer->DeleteObject(&inputPara,response)!=SOAP_OK||!response.DeleteObjectResult)
		return false;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CTmsServer::QueryStateTowerTypes(BUFFER_IO* pOutLocalBuff,int state,int fromdate/*=0*/,int todate/*=0*/)
{
	CXhChar500 xmlscope;
    char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <query>";
	xmlscope.Copy(xmlheader);
	if(fromdate>0)
		xmlscope.Append(CXhChar50("     <fromdate value=\"%4d-%2d-%2d\" />",fromdate/10000,(fromdate%10000)/100,fromdate%100));
	if(todate>0)
		xmlscope.Append(CXhChar50("     <todate value=\"%4d-%2d-%2d\" />",todate/10000,(todate%10000)/100,todate%100));
	xmlscope.Append(CXhChar50("     <state value=\"%d\" />",state));
    char* xmlfinal="     <serialize value=\"0xffffffff\" />"\
		" </query>";
	xmlscope.Append(xmlfinal);
	return TMS.QueryObjects("TowerTypeFile",xmlscope,pOutLocalBuff);
}
int CTmsServer::QueryProcessParts(BUFFER_IO* pOutLocalBuff,int towerTypeId,CXhChar16* arrLabels,int labelCount/*=1*/)
{
	CXhChar500 xmlscope;
    char* xmlheader="<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
					" <query>";
	xmlscope.Copy(xmlheader);
	xmlscope.Append(CXhChar50("     <TowerTypeId value=\"%d\" />",towerTypeId));
	if(arrLabels==NULL||labelCount<=0)
		return 0;
	xmlscope.Append(CXhChar50("     <QueryLabels count=\"%d\">",labelCount));
	for(int i=0;i<labelCount;i++)
		xmlscope.Append(CXhChar50("     <Label value=\"%s\" />",(char*)arrLabels[i]));
	xmlscope.Append("     </QueryLabels\">");
    char* xmlfinal="     <serialize value=\"0xffffffff\" />"\
		" </query>";
	xmlscope.Append(xmlfinal);
	return TMS.QueryObjects("TowerPart",xmlscope,pOutLocalBuff);
}
#endif
