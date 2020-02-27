//<LOCALE_TRANSLATE BY hxr /> 2015-04-23
#include "StdAfx.h"
#include "WireLoadModelBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////
////WireLoadModel
///////////////////////////////////////////////////////////////
CBaseInfoSection::CBaseInfoSection(char* buf/* =NULL */,DWORD size/* =0 */)
{
	if(buf)
		m_data=buf;
	m_dwBufSize=size;
}
CWeatherSection::CWeatherSection(char* buf/* =NULL */,DWORD size/* =0 */)
{
	if(buf)
		m_data=buf;
	m_dwBufSize=size;
}
//////////////////////////////////////////////////////////////////////////
// CWorkCaseSetSection
CWorkCaseSetSection::CWorkCaseSetSection(char* buf/* =NULL */,DWORD size/* =0 */)
{
	if(buf)
		m_data=buf;
	m_dwBufSize=size;
}

BOOL CWorkCaseSetSection::RetrieveWorkCaseSet(CTower* pModel)
{
#ifdef __PART_DESIGN_INC_
	long iActiveCaseNo=0;
	CWorkStatus* pStatus=pModel->GetActiveWorkStatus();
	if(pStatus)
		iActiveCaseNo=pStatus->iNo;
	pModel->WorkStatus.Empty();
	CObjNoGroup* pNoGroup=pModel->NoManager.FromGroupID(WORKSTATUS_GROUP);
	pNoGroup->EmptyNo();
	if(m_data==NULL||m_dwBufSize<=0)
		return FALSE;
	int i=0,nCase=0,idCase=0,caseBufferLen=0;
	CBuffer tempBuffer(m_data,m_dwBufSize);
	tempBuffer.ReadInteger(&nCase);		//工况数
	for(i=0;i<nCase;i++)
	{
		tempBuffer.ReadInteger(&idCase);
		tempBuffer.ReadInteger(&caseBufferLen);	//读取工况缓存大小，用于读完基本信息向下一条跳转
		DWORD cur_pos=tempBuffer.GetCursorPosition();
		CWorkStatus* pCase=pModel->WorkStatus.append();
		pCase->iNo=idCase;
		pNoGroup->SetNoState(pCase->iNo);
		pCase->FromRawBuffer(tempBuffer);
		if(iActiveCaseNo==pCase->iNo)
			pModel->m_hActiveStatus=pCase->handle;
		tempBuffer.SeekPosition(cur_pos+caseBufferLen);
	}
#endif
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CWireLayoutSetSection
CWireLayoutSetSection::CWireLayoutSetSection(char* buf/* =NULL */,DWORD size/* =0 */)
{
	if(buf)
		m_data=buf;
	m_dwBufSize=size;
}
/*BOOL CWireLayoutSetSection::RetrieveWireLayoutSet(ATOM_LIST<CWireLayout> &listWireLayout)
{
	listWireLayout.Empty();
	if(m_data==NULL||m_dwBufSize<=0)
		return FALSE;
	CBuffer tempBuffer(m_data,m_dwBufSize);
	int i=0,j=0,nNode=0,id=0;
	tempBuffer.ReadInteger(&nNode);		//挂点数
	if(nNode>0)
	{
		for(i=0;i<nNode;i++)
		{
			tempBuffer.ReadInteger(&id);
			CWireLayout *pWireLayout=listWireLayout.append();
			pWireLayout->id=id;
			//NoGroupLayout.SetNoState(pWireLayout->id);
			pWireLayout->FromBuffer(tempBuffer);
		}
	}
	return TRUE;
}*/

//////////////////////////////////////////////////////////////////////////
// CNodeLoadSetSection
CNodeLoadSetSection::CNodeLoadSetSection(char* buf/* =NULL */,DWORD size/* =0 */)
{
	if(buf)
		m_data=buf;
	m_dwBufSize=size;
}
BOOL CNodeLoadSetSection::RetrieveWorkCaseActualNodeLoad(CTower* pModel)
{
#ifdef __PART_DESIGN_INC_
	CWorkStatus *pWorkCase=NULL;
	CHashSet<CWorkStatus*> hashCasesByNo;
	for(pWorkCase=pModel->WorkStatus.GetFirst();pWorkCase;pWorkCase=pModel->WorkStatus.GetNext())
	{
		pWorkCase->hashNodeLoad.Empty();//hashNodeLoadByH.Empty();
		hashCasesByNo.SetValue(pWorkCase->iNo,pWorkCase);
	}
	if(m_data==NULL||m_dwBufSize<=0)
		return FALSE;
	CLDSNode* pNode;
	CHashSet<CLDSNodePtr>hashNodeByPointI;
	for(pNode=pModel->Node.GetFirst();pNode;pNode=pModel->Node.GetNext())
	{
		if(pNode->point_i>0)
			hashNodeByPointI.SetValue(pNode->point_i,pNode);
	}
	CBuffer tempBuffer(m_data,m_dwBufSize);
	long i=0,j=0,nNode=0,hNode=0,idNode=0,nCase=0,idCase=0,idNodePlace=0,nWirePlace=0;
	//double fLoadAllocCoefX,fLoadAllocCoefY,fLoadAllocCoefZ;
	tempBuffer.ReadInteger(&nNode);		//挂点数
	tempBuffer.ReadInteger(&nCase);
	for(i=0;i<nNode;i++)
	{
		tempBuffer.ReadInteger(&idNode);		//挂点标识编号
		tempBuffer.ReadInteger(&hNode);
		BYTE cBasicWireType=0;
		tempBuffer.ReadByte(&cBasicWireType);
		tempBuffer.ReadInteger(&nWirePlace);
		tempBuffer.Offset(nWirePlace*28);
		/*
		WIREPLACE wireplace;
		for(j=0;j<nWirePlace;j++)
		{
			tempBuffer.ReadInteger((int*)&wireplace.idNodePlace);	//挂点对应的挂位编号
			tempBuffer.ReadDouble(&wireplace.fLoadAllocCoefX);//挂点对应的荷载分配系数
			tempBuffer.ReadDouble(&wireplace.fLoadAllocCoefY);//挂点对应的荷载分配系数
			tempBuffer.ReadDouble(&wireplace.fLoadAllocCoefZ);//挂点对应的荷载分配系数
		}
		*/
		pNode=hashNodeByPointI.GetValue(idNode);
		for(j=0;j<nCase;j++)
		{
			tempBuffer.ReadInteger(&idCase);
			CWorkStatus* pCase=hashCasesByNo.GetValue(idCase);	//TODO: 有问题
			CExternalNodeLoad nodeLoad;
			tempBuffer.ReadPoint(nodeLoad.stdLoad);
			tempBuffer.ReadPoint(nodeLoad.designLoad);
			nodeLoad.permanentFz=0;	//荷载数据已包含在designLoad及stdLoad中
			nodeLoad.Fxyz.Set();	//荷载数据已包含在designLoad及stdLoad中
			if(pNode&&pCase)
				pCase->hashNodeLoad.SetValue(pNode->handle,nodeLoad);
		}
	}
#endif
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CWireLoadModelBuffer
void CWireLoadModelBuffer::InitWireLoadModelBuffer(CBuffer* pBuffer,const WEATHER& weather)
{
	pBuffer->ClearContents();
	pBuffer->SetBlockSize(10000);
	char sValue[50]="";
	//0.头信息
	pBuffer->WriteInteger(1);	//版本信息
	DWORD sect_index_pos,sect_start_pos,current_pos;
	sect_index_pos=pBuffer->GetCursorPosition();
	pBuffer->Write(NULL,20);	//分区其实位置
	//1.基本信息区
	sect_start_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_index_pos);
	pBuffer->WriteDword(sect_start_pos);
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(0);				//存储空间大小
	current_pos=pBuffer->GetCursorPosition();
	pBuffer->WriteAt(current_pos,sValue,50);		//塔型名称
	pBuffer->WriteAt(current_pos+ 50,sValue,50);	//工程代号
	pBuffer->WriteAt(current_pos+100,sValue,50);	//工程名称
	pBuffer->WriteAt(current_pos+150,sValue,50);	//设计人员	
#ifdef AFX_TARG_ENU_ENGLISH
	pBuffer->WriteAt(current_pos+200,"Unknown",16);		//天气代号
#else
	pBuffer->WriteAt(current_pos+200,"未知",16);		//天气代号
#endif
	pBuffer->WriteWord((short)220);		//电压等级
	pBuffer->WriteByte('B');			//塔位地貌
	pBuffer->WriteByte(0);				//线路状况
	pBuffer->WriteDouble(900);			//覆冰厚度
	pBuffer->WriteDouble(1.2);			//持久荷载分项系数
	pBuffer->WriteWord((short)1);		//安全等级
	current_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(current_pos-sect_start_pos-4);	//迟写的存储空间大小
	pBuffer->SeekPosition(current_pos);
	//2.天气信息区
	sect_start_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_index_pos+4);
	pBuffer->WriteDword(sect_start_pos);
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(0);				//存储空间大小
	pBuffer->WriteDouble(weather.m_fMaxIceThick);		//最大覆冰厚度
	pBuffer->WriteDouble(weather.m_fMaxIceThickCheck);	//验算覆冰厚度
	pBuffer->WriteDouble(weather.m_fMaxIceWindV);		//最大覆冰风速
	pBuffer->WriteDouble(weather.m_fMaxWindV);			//大风速度
	pBuffer->WriteDouble(weather.m_fMaxWindVCheck);		//验算风速
	pBuffer->WriteDouble(weather.m_fSetupWindV);		//安装风速
	pBuffer->WriteInteger(weather.m_nAverageT);			//平均温度
	pBuffer->WriteInteger(weather.m_nHighestT);			//最高温度
	pBuffer->WriteInteger(weather.m_nLowestT);			//最低温度
	pBuffer->WriteInteger(weather.m_nMaxIceT);			//最大覆冰温度
	pBuffer->WriteInteger(weather.m_nMaxWindT);			//最大风速温度
	pBuffer->WriteInteger(weather.m_nSetupT);			//安装温度
	current_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(current_pos-sect_start_pos-4);	//迟写的存储空间大小
	pBuffer->SeekPosition(current_pos);
	//3.工况集合区
	sect_start_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_index_pos+8);
	pBuffer->WriteDword(sect_start_pos);
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(4);		//写入工况数据块长度
	pBuffer->WriteInteger(0);	//写入工况数		
	//4.架线方案区
	sect_start_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_index_pos+12);
	pBuffer->WriteDword(sect_start_pos);
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(4);		//写入架线方案数据块长度
	pBuffer->WriteInteger(0);		//写入架线方案数
	//5.节点荷载区
	sect_start_pos=pBuffer->GetCursorPosition();
	pBuffer->SeekPosition(sect_index_pos+16);
	pBuffer->WriteDword(sect_start_pos);
	pBuffer->SeekPosition(sect_start_pos);
	pBuffer->WriteDword(4);		//写入节点荷载数据块长度
	pBuffer->WriteInteger(0);		//写入节点荷载数
}
CBaseInfoSection CWireLoadModelBuffer::GetBaseInfoSection()
{
	if(GetLength()<=0)
		return NULL;
	DWORD startPos=0,sectSize=0;
	ReadAt(4,&startPos,4);
	SeekPosition(startPos);
	ReadDword(&sectSize);
	return CBaseInfoSection(GetCursorBuffer(),sectSize);
}
CWeatherSection CWireLoadModelBuffer::GetWeatherSection()
{
	if (GetLength()<0)
		return NULL;
	DWORD startPos=0,sectSize=0;
	ReadAt(8,&startPos,4);
	SeekPosition(startPos);
	ReadDword(&sectSize);
	return CWeatherSection(GetCursorBuffer(),sectSize);
}
CWorkCaseSetSection CWireLoadModelBuffer::GetWorkCaseSetSection()
{
	if(GetLength()<=0)
		return NULL;
	DWORD startPos,sectSize=0;
	ReadAt(12,&startPos,4);
	SeekPosition(startPos);
	ReadDword(&sectSize);
	return CWorkCaseSetSection(GetCursorBuffer(),sectSize);
}
CWireLayoutSetSection CWireLoadModelBuffer::GetWireLayoutSetSection()
{
	if(GetLength()<=0)
		return NULL;
	DWORD startPos,sectSize=0;
	ReadAt(16,&startPos,4);
	SeekPosition(startPos);
	ReadDword(&sectSize);
	return CWireLayoutSetSection(GetCursorBuffer(),sectSize);
}
CNodeLoadSetSection CWireLoadModelBuffer::GetNodeLoadSetSection()
{
	if(GetLength()<=0)
		return NULL;
	DWORD startPos,sectSize=0;
	ReadAt(20,&startPos,4);
	SeekPosition(startPos);
	ReadDword(&sectSize);
	return CNodeLoadSetSection(GetCursorBuffer(),sectSize);
}

void CWireLoadModelBuffer::RetrieveWireLoadInfo(CTower* pModel)
{	
#ifdef __PART_DESIGN_INC_
	CWorkCaseSetSection workCaseSection = GetWorkCaseSetSection();
	CNodeLoadSetSection nodeLoadSection = GetNodeLoadSetSection();
	workCaseSection.RetrieveWorkCaseSet(pModel);
	nodeLoadSection.RetrieveWorkCaseActualNodeLoad(pModel);
	CObjNoGroup* pNoGroup=pModel->NoManager.FromGroupID(WORKSTATUS_GROUP);
	pNoGroup->EmptyNo();
	for(CWorkStatus* pCase=pModel->WorkStatus.GetFirst();pCase;pCase=pModel->WorkStatus.GetNext())
	{
		pCase->iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(pCase->iNo);
	}
	//pModel->m_bExterImportedLoads=false;
#endif
}