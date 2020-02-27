#include "stdafx.h"
#include "WireLayoutsEvolvement.h"

////////////////////////////////////////////////////////////////////
CWireLayout::CWireLayout()
{
	iNo=1;
	strcpy(name,"常用方案");
	m_sRelaTemplateName.Copy("");	//TODO:将来赋一荷载计算模板名
	m_fSafetyCoef=1.0;		//结构重要性系数
	m_fGamaG=1.2;			//恒荷载分项系数
	m_fGamaQ=1.4;			//可变荷载分项系数
	m_fLiveLoadFai=0.9;		//活荷载组合系数
	m_fCheckLoadFai=0.75;	//验算荷载组合系数
	m_fLiveLoadZ=1000;		//活荷载重(N)
}
void CWireLayout::Empty()
{
	hashWireLoadModel.Empty();
	groupWorkStatus.hashNodeRoom.Empty();
	groupWorkStatus.hashWorkStatus.Empty();
}
//因LDS V1.2.1.0(含)之前版本，客户使用量极少，且一般不用我公司提供的荷载计算模块，故这部分信息暂不进行迁移。wjh-2014.5.2
void CWireLayout::EvolveToNewVersion()
{
	m_fSafetyCoef=groupWorkStatus.m_fSafetyCoef;	//结构重要性系数
	m_fGamaG=groupWorkStatus.m_fGamaG;		//恒荷载分项系数
	m_fGamaQ=groupWorkStatus.m_fGamaQ;		//可变荷载分项系数
	m_fLiveLoadFai=groupWorkStatus.m_fLiveLoadFai;	//活荷载组合系数
	m_fCheckLoadFai=groupWorkStatus.m_fCheckLoadFai;	//验算荷载组合系数
	m_fLiveLoadZ=groupWorkStatus.m_fLiveLoadZ;	//活荷载重(N)
	hashWirePlace.Empty();
	hashWireNode.Empty();
	for(LOADNODEROOM_STRU* pRoom=groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=groupWorkStatus.hashNodeRoom.GetNext())
	{
		WIREPLACE_STRU* pWirePlace=hashWirePlace.Add(groupWorkStatus.hashNodeRoom.GetCursorKey());
		pWirePlace->idPlace=hashWirePlace.GetCursorKey();
		pWirePlace->m_iRawWireLoadNo=pRoom->m_iRawWireLoadNo;		//原始线条荷载标识号
		strcpy(pWirePlace->helpString,pRoom->helpString);
		pWirePlace->hashWorkEnv.Empty();
		for(long* pCaseI=pRoom->hashWorkEnv.GetFirst();pCaseI;pCaseI=pRoom->hashWorkEnv.GetNext())	//键值是是工况标识号，元素值是相应工况下对应的原始电气工况CRawWireLoad中WorkEnv的标识号
			pWirePlace->hashWorkEnv.SetValue(pRoom->hashWorkEnv.GetCursorKey(),*pCaseI);
		for(WIRENODE_STRU* pWireNode=pRoom->hashNode.GetFirst();pWireNode;pWireNode=pRoom->hashNode.GetNext())
		{
			WIRENODE_STRU* pNewWireNode=hashWireNode.Add(pRoom->hashNode.GetCursorKey());
			pNewWireNode->hNode=pWireNode->hNode;;
			pNewWireNode->fLoadAllocCoefX=pWireNode->fLoadAllocCoefX;
			pNewWireNode->fLoadAllocCoefY=pWireNode->fLoadAllocCoefY;
			pNewWireNode->fLoadAllocCoefZ=pWireNode->fLoadAllocCoefZ;
			//TODO: 完成pNewWireNode->hashWorkEnv=pWireNode->hashWorkEnv的赋值
		}
	}
	for(RAWWORKSTATUS_STRU *pRawCase=groupWorkStatus.hashWorkStatus.GetFirst();pRawCase;pRawCase=groupWorkStatus.hashWorkStatus.GetNext())
	{	//SRCCASE_STRU同RAWWORKSTATUS_STRU
		SRCCASE_STRU* pSrcCase=hashSrcCase.Add(groupWorkStatus.hashWorkStatus.GetCursorKey());
		strcpy(pSrcCase->name,pRawCase->name);//[MAX_TEMP_CHAR_100+1];
		//TODO: 未处理pRawCase->lim_str[MAX_TEMP_CHAR_50+1]属性
		pSrcCase->m_cHoriEarthQuakeAxis=pRawCase->m_cHoriEarthQuakeAxis;	//水平地震验算时的振动方向角°，'X'=0或'Y'=90
		strcpy(pSrcCase->vibration_mode_str,pRawCase->vibration_mode_str);	//[MAX_TEMP_CHAR_50+1];	//振型分解反应谱法分析时需要考虑的振型列表
		//TODO: 未处理pRawCase->m_hWorkStatus;		//对应的CWorkStatus的句柄值
		pSrcCase->m_cMainLoadType=pRawCase->m_cMainLoadType;	//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
		pSrcCase->m_bCheckWorkCond=pRawCase->m_bCheckWorkCond;	//验算工况
		pSrcCase->m_fIceThick=pRawCase->ice_thick;	//基本裹冰厚度(mm)
		pSrcCase->m_bMinVertSpace=pRawCase->m_bMinVertSpace;	//最小垂直档距工况
		pSrcCase->WindLoad=pRawCase->WindLoad;	//风荷载
		//TODO: 未处理pRaseCase->id
	}
	hashNodeLoad;	//当前架线方案在指定挂位和原始工况下的荷载值，键值为:挂位号,原始工况号
}
void CWireLayout::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	int i,n;
	if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
	{
		buffer.ReadInteger(&iNo);
		buffer.ReadString(name,49);
	}
	else
	{	//旧文件给一个默认值
		strcpy(name,"常用方案");
		iNo=1;//pNoGroup->EnumIdleNo();
	}
	//原始线条计算荷载
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		int iNo=0;
		buffer.ReadInteger(&iNo);
		CWireLoadModel *pWireModel=hashWireLoadModel.Add(iNo);
		pWireModel->iNo=iNo;
		pWireModel->FromBuffer(buffer,version,doc_type);
	}
	//工况组合信息
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long key=0;
		buffer.ReadInteger(&key);
		RAWWORKSTATUS_STRU* pRawStatus=groupWorkStatus.hashWorkStatus.Add(key);
		buffer.ReadString(pRawStatus->name);
		if((doc_type==3&&version>=1070303)||(doc_type==4&&version>=1000303))
		{
			buffer.ReadByte(&pRawStatus->m_cHoriEarthQuakeAxis);
			if((doc_type==3&&version<=1070307)||(doc_type==4&&version<=1000307))
			{
				if(pRawStatus->m_cHoriEarthQuakeAxis=='X')
					pRawStatus->m_cHoriEarthQuakeAxis=0;
				else //if(pRawStatus->m_cHoriEarthQuakeAxis=='Y')
					pRawStatus->m_cHoriEarthQuakeAxis=90;
			}
			buffer.ReadString(pRawStatus->vibration_mode_str);
		}
		buffer.ReadString(pRawStatus->lim_str);	//新加
		buffer.ReadInteger(&pRawStatus->m_hWorkStatus);			//对应的CWorkStatus的句柄值
		buffer.ReadByte(&pRawStatus->m_cMainLoadType);			//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
		buffer.ReadBooleanThin(&pRawStatus->m_bCheckWorkCond);	//验算工况
		buffer.ReadBooleanThin(&pRawStatus->m_bMinVertSpace);	//最小垂直档距工况
		buffer.ReadDouble(&pRawStatus->ice_thick);				//基本裹冰厚度(mm)
		buffer.ReadInteger(&pRawStatus->WindLoad.azimuth);		//风荷载,风向角(°)
		buffer.ReadDouble(&pRawStatus->WindLoad.velocity);		//风荷载,风速(m/s)
	}
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		long key=0;
		buffer.ReadInteger(&key);
		LOADNODEROOM_STRU* pRoom=groupWorkStatus.hashNodeRoom.Add(key);
		if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
			buffer.ReadInteger(&key);
		pRoom->m_iRawWireLoadNo=key;
		buffer.ReadString(pRoom->helpString);
		//写入点号位对应的默认原始工况标识
		long j,k,n2,n3,key2;
		buffer.ReadInteger(&n2);
		for(j=0;j<n2;j++)
		{
			buffer.ReadInteger(&key);
			buffer.ReadInteger(&key2);
			pRoom->hashWorkEnv.SetValue(key,key2);
		}
		//写入当前点号位对应的点号信息
		buffer.ReadInteger(&n2);
		for(j=0;j<n2;j++)
		{
			buffer.ReadInteger(&key);
			WIRENODE_STRU *pRawNode=pRoom->hashNode.Add(key);
			buffer.ReadDouble(&pRawNode->fLoadAllocCoefX);
			if((doc_type==3&&version>=1070005)||(doc_type==4&&version>=1000016))
			{
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefY);
				buffer.ReadDouble(&pRawNode->fLoadAllocCoefZ);
				buffer.ReadInteger(&pRawNode->hNode);
			}
			else
				pRawNode->fLoadAllocCoefY=pRawNode->fLoadAllocCoefZ=pRawNode->fLoadAllocCoefX;
			buffer.ReadInteger(&n3);
			for(k=0;k<n3;k++)
			{
				buffer.ReadInteger(&key);
				buffer.ReadInteger(&key2);
				pRawNode->hashWorkEnv.SetValue(key,key2);
			}
		}
	}
}
void CWireLayout::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{	//TODO: 需完成代码
	buffer.WriteString(name);	//方案名称
	buffer.WriteString(m_sRelaTemplateName);
	//TODO: 将来应将m_fLiveLoadZ置于CRawWireLoad下
	buffer.WriteDouble(groupWorkStatus.m_fLiveLoadZ);	//活荷载重(N)
	buffer.WriteInteger(hashWireLoadModel.GetNodeNum());		//原始电气线条荷载,hashWireLoadModel同hashRawWireLoad
	for(CWireLoadModel *pRawWire=hashWireLoadModel.GetFirst();pRawWire;pRawWire=hashWireLoadModel.GetNext())
	{	//CWireLoadModel同CRawWireLoad
		buffer.WriteInteger(pRawWire->iNo);
		pRawWire->ToBuffer(buffer);
	}
	buffer.WriteInteger(groupWorkStatus.hashWorkStatus.GetNodeNum());			//架线方案中原始组合工况
	for(SRCCASE_STRU *pSrcCase=hashSrcCase.GetFirst();pSrcCase;pSrcCase=hashSrcCase.GetNext())
	{
		buffer.WriteInteger(pSrcCase->id);
		pSrcCase->ToBuffer(buffer);
	}
	buffer.WriteInteger(hashWirePlace.GetNodeNum());			//架线方案中挂线位
	//hashWirePlace同hashNodeRoom，LOADNODEROOM_STRU同WIREPLACE_STRU
	for(WIREPLACE_STRU *pWirePlace=hashWirePlace.GetFirst();pWirePlace;pWirePlace=hashWirePlace.GetNext())
	{
		buffer.WriteInteger(pWirePlace->idPlace);
		pWirePlace->ToBuffer(buffer);
	}
	buffer.WriteInteger(hashWireNode.GetNodeNum());			//架线方案中挂线节点
	for(WIRENODE_STRU *pWireNode=hashWireNode.GetFirst();pWireNode;pWireNode=hashWireNode.GetNext())
	{
		buffer.WriteInteger(pWireNode->idNodeI);
		buffer.WriteInteger(pWireNode->idPlace);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefX);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefY);
		buffer.WriteDouble(pWireNode->fLoadAllocCoefZ);
		//pWireNode->hashWorkEnv转至hashNodeLoad
	}
	buffer.WriteInteger(hashNodeLoad.GetNodeNum());			//架线方案计算结果：挂位在某工况下的无系数原始荷载
	for(CExternalNodeLoad *pNodeLoad=hashNodeLoad.GetFirst();pNodeLoad;pNodeLoad=hashNodeLoad.GetNext())
	{
		HASH2KEY key=hashNodeLoad.GetCursorKey();
		buffer.WriteInteger(key.key1);	//idPlace
		buffer.WriteInteger(key.key2);	//idCase
		buffer.WritePoint(pNodeLoad->Fxyz);
		buffer.WriteDouble(pNodeLoad->permanentFz);
	}
}
////////////////////////////////////////////////////////////////////
int CWorkStatusGroupInfo::EnumIdleRawStatusNo()
{
	int maxNo=0;
	for(RAWWORKSTATUS_STRU *pRawStatus=hashWorkStatus.GetFirst();pRawStatus;pRawStatus=hashWorkStatus.GetNext())
		maxNo=max(maxNo,(int)hashWorkStatus.GetCursorKey());
	return ++maxNo;
}
CWorkStatusGroupInfo::CWorkStatusGroupInfo()
{
	m_fSafetyCoef=1.0;		//结构重要性系数
	m_fGamaG=1.2;			//恒荷载分项系数
	m_fGamaQ=1.4;			//可变荷载分项系数
	m_fLiveLoadFai=0.9;		//活荷载组合系数
	m_fCheckLoadFai=0.75;	//验算荷载组合系数
	m_fLiveLoadZ=1000;		//活荷载重(N)
}
////////////////////////////////////////////////////////////////////
WIRENODE_STRU::WIRENODE_STRU()
{
	idNodeI=idPlace=hNode=0;	//与该节点相关联的实际模型中节点的句柄值
	fLoadAllocCoefX=fLoadAllocCoefY=fLoadAllocCoefZ=1.0;
}
void WIREPLACE_STRU::FromBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadInteger(&m_iRawWireLoadNo);		//原始线条荷载标识号
	buffer.ReadString(helpString,100);
	long i,n,idCase,idWorkEnv;
	buffer.ReadInteger(&n);
	for(i=0;i<n;i++)
	{
		buffer.ReadInteger(&idCase);
		buffer.ReadInteger(&idWorkEnv);
		hashWorkEnv.SetValue(idCase,idWorkEnv);
	}
}
void WIREPLACE_STRU::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_iRawWireLoadNo);		//原始线条荷载标识号
	buffer.WriteString(helpString);
	//键值是是工况标识号，元素值是相应工况下对应的原始电气工况CRawWireLoad中WorkEnv的标识号
	buffer.WriteInteger(hashWorkEnv.GetNodeNum());
	for(long* pWorkEnvId=hashWorkEnv.GetFirst();pWorkEnvId;pWorkEnvId=hashWorkEnv.GetNext())
	{
		buffer.WriteInteger(hashWorkEnv.GetCursorKey());
		buffer.WriteInteger(*pWorkEnvId);
	}
}
//////////////////////////////////////////////////////////////////
double SRCCASE_STRU::Kesai()
{	//可变荷载组合系数
	//if(sysPara==NULL||sysPara->GetExecuteLoadCode()==3)
	//	return Fai;
	//else
	{
		if(m_cMainLoadType<=2)	//正常工作工况
			return 1.0;
		else if(m_cMainLoadType>=5)	//抗震工况
			return 1.0;
		else if(m_bCheckWorkCond)	//验算工况
			return 0.75;
		else if(m_cMainLoadType==3||m_cMainLoadType==4)	//断线或安装及不均匀覆冰
			return 0.9;
		else
			return 1.0;
	}
}
double SRCCASE_STRU::GamaQ()
{	//综合可变荷载分项系数
	//if(sysPara!=NULL&&sysPara->GetExecuteLoadCode()==3)
	//	return GamaQi;
	//else
	if(m_cMainLoadType<5)
		return 1.4;
	else
		return 0.5;
}
void SRCCASE_STRU::FromBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadString(name,100);		//对于工况的简要描述
	buffer.ReadString(vibration_mode_str,50);	//振型分解反应谱法分析时需要考虑的振型列表
	buffer.ReadByte(&m_cHoriEarthQuakeAxis);//水平地震验算时的振动方向角°，'X'=0或'Y'=90
	buffer.ReadByte(&m_cMainLoadType);		//0:大风1:覆冰2:低温3:断线及不均匀覆冰4:安装5:水平地震6:竖向地震7:仅水平地震8:仅竖向地震
	buffer.ReadBooleanThin(&m_bCheckWorkCond);		//验算工况
	buffer.ReadDouble(&m_fIceThick);	//基本裹冰厚度(mm)
	buffer.ReadBooleanThin(&m_bMinVertSpace);
	buffer.ReadInteger(&WindLoad.azimuth);
	buffer.ReadDouble(&WindLoad.velocity);	//风荷载
}
void SRCCASE_STRU::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(name);		//对于工况的简要描述
	buffer.WriteString(vibration_mode_str);	//振型分解反应谱法分析时需要考虑的振型列表
	buffer.WriteByte(m_cHoriEarthQuakeAxis);//水平地震验算时的振动方向角°，'X'=0或'Y'=90
	buffer.WriteByte(m_cMainLoadType);		//0:大风1:覆冰2:低温3:断线及不均匀覆冰4:安装5:水平地震6:竖向地震7:仅水平地震8:仅竖向地震
	buffer.WriteBooleanThin(m_bCheckWorkCond);		//验算工况
	buffer.WriteDouble(m_fIceThick);	//基本裹冰厚度(mm)
	buffer.WriteBooleanThin(m_bMinVertSpace);
	buffer.WriteInteger(WindLoad.azimuth);
	buffer.WriteDouble(WindLoad.velocity);	//风荷载
}
////////////////////////////////////////////////////////////////////
CWireLayoutsEvolvement evolve;
CWireLayoutsEvolvement::CWireLayoutsEvolvement(void)
{
}


CWireLayoutsEvolvement::~CWireLayoutsEvolvement(void)
{
}
void CWireLayoutsEvolvement::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if((doc_type==3&&version>=1070004)||(doc_type==4&&version>=1000015))
	{
		int nWireLayout=1;
		if((doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
			buffer.ReadInteger(&nWireLayout);
		evolve.WireLayouts.Empty();
		for(int iLayout=0;iLayout<nWireLayout;iLayout++)
		{
			CWireLayout* pWireLayout=evolve.WireLayouts.append();
			pWireLayout->FromBuffer(buffer,version,doc_type);
			if((doc_type==3&&version<1070101)||(doc_type==4&&version<1000101))
			{
				for(CWireLoadModel* pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
					pWireModel->weather.CopyWeather(&weather);
			}
		}
	}
}
void CWireLayoutsEvolvement::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(version==0||((doc_type==3&&version>=2000101)||(doc_type==4&&version>=1020101)))
		return;	//自LDS V1.2.1.1开始荷载挂线方案信息已移至CAttachBuffer专用缓存中，并由WireLoad.exe进程管理
	buffer.WriteInteger(evolve.WireLayouts.GetNodeNum());
	for(CWireLayout *pWireLayout=evolve.WireLayouts.GetFirst();pWireLayout;pWireLayout=evolve.WireLayouts.GetNext())
	{
		buffer.WriteInteger(pWireLayout->iNo);
		buffer.WriteString(pWireLayout->name);
		//原始线条计算荷载
		buffer.WriteInteger(pWireLayout->hashWireLoadModel.GetNodeNum());
		for(CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
		{
			buffer.WriteInteger(pWireModel->iNo);
			pWireModel->ToBuffer(buffer);
		}
		//工况组合信息
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetNodeNum());
		for(RAWWORKSTATUS_STRU* pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
		{
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashWorkStatus.GetCursorKey());
			buffer.WriteString(pRawStatus->name);
			buffer.WriteByte(pRawStatus->m_cHoriEarthQuakeAxis);
			buffer.WriteString(pRawStatus->vibration_mode_str);
			buffer.WriteString(pRawStatus->lim_str);	//新加
			buffer.WriteInteger(pRawStatus->m_hWorkStatus);			//对应的CWorkStatus的句柄值
			buffer.WriteByte(pRawStatus->m_cMainLoadType);			//0:大风1:覆冰2:低温3:断线4:安装5:不均匀覆冰6:水平地震7:竖向地震8:仅水平地震9:仅竖向地震10:其余
			buffer.WriteBooleanThin(pRawStatus->m_bCheckWorkCond);	//验算工况
			buffer.WriteBooleanThin(pRawStatus->m_bMinVertSpace);	//最小垂直档距工况
			buffer.WriteDouble(pRawStatus->ice_thick);				//基本裹冰厚度(mm)
			buffer.WriteInteger(pRawStatus->WindLoad.azimuth);		//风荷载,风向角(°)
			buffer.WriteDouble(pRawStatus->WindLoad.velocity);		//风荷载,风速(m/s)
		}
		buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetNodeNum());
		for(LOADNODEROOM_STRU* pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetFirst();pRoom;pRoom=pWireLayout->groupWorkStatus.hashNodeRoom.GetNext())
		{
			long *pWorkEnvKey;
			buffer.WriteInteger(pWireLayout->groupWorkStatus.hashNodeRoom.GetCursorKey());
			buffer.WriteInteger(pRoom->m_iRawWireLoadNo);
			buffer.WriteString(pRoom->helpString);
			//写入点号位对应的默认原始工况标识
			buffer.WriteInteger(pRoom->hashWorkEnv.GetNodeNum());
			for(pWorkEnvKey=pRoom->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRoom->hashWorkEnv.GetNext())
			{
				buffer.WriteInteger(pRoom->hashWorkEnv.GetCursorKey());	//原始工况标识号
				buffer.WriteInteger(*pWorkEnvKey);						//对应原始工况下的原始工作环境标识
			}
			//写入当前点号位对应的点号信息
			buffer.WriteInteger(pRoom->hashNode.GetNodeNum());
			for(WIRENODE_STRU *pRawNode=pRoom->hashNode.GetFirst();pRawNode;pRawNode=pRoom->hashNode.GetNext())
			{
				buffer.WriteInteger(pRoom->hashNode.GetCursorKey());
				buffer.WriteDouble(pRawNode->fLoadAllocCoefX);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefY);
				buffer.WriteDouble(pRawNode->fLoadAllocCoefZ);
				buffer.WriteInteger(pRawNode->hNode);
				buffer.WriteInteger(pRawNode->hashWorkEnv.GetNodeNum());
				for(pWorkEnvKey=pRawNode->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRawNode->hashWorkEnv.GetNext())
				{
					buffer.WriteInteger(pRawNode->hashWorkEnv.GetCursorKey());	//原始工况标识号
					buffer.WriteInteger(*pWorkEnvKey);							//对应原始工况下的原始工作环境标识
				}
			}
		}
	}
}

bool CWireLayoutsEvolvement::EvolveToWireLoadBuffer()//CTower* pTower)
{
	/*CAttachBuffer *pWireLoadBuffer=pTower->AttachBuffer.GetValue(CTower::WIRELOAD_MODEL);
	int item_n=0;
	if(pWireLoadBuffer==NULL)
		pWireLoadBuffer=pTower->AttachBuffer.Add(CTower::WIRELOAD_MODEL);
	pWireLoadBuffer->ClearContents();
	CBuffer buffer(1000000);	//1Mb
	buffer.WriteInteger(0);		//线条荷载缓存数据长度
	DWORD base_address=buffer.GetCursorPosition();	//自版本号数据开始的位置在当前缓存区中的起始地址
	buffer.WriteInteger(1);	//版本号
	DWORD sect_index_pos=buffer.GetCursorPosition();
	buffer.Write(NULL,20);	//记载各数据分区自版本号起始位置开始的相对偏移地址
	//1.基本信息区
	DWORD sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//存储空间大小
	DWORD current_pos=buffer.GetCursorPosition();
	buffer.WriteAt(current_pos,pTower->m_sTaType,50);		//塔型名称
	CXhChar50 prjCode;
	prjCode.Copy(pTower->m_sPrjCode.GetBuffer());
	buffer.WriteAt(current_pos+ 50,(char*)prjCode,50);	//工程代号
	buffer.WriteAt(current_pos+100,pTower->m_sPrjName,50);	//工程名称
	buffer.WriteAt(current_pos+150,pTower->m_sOperator,50);	//设计人员
	buffer.WriteAt(current_pos+200,pTower->weatherCode,16);	//天气代号
	buffer.WriteWord((WORD)pTower->volt_grade);					//电压等级
	buffer.WriteByte(pTower->m_cLandscape);					//塔位地貌
	buffer.WriteByte(pTower->m_cLineRoadType);				//线路状况
	buffer.WriteDouble(pTower->m_fIceDensity);				//覆冰厚度
	buffer.WriteDouble(pTower->m_fGamaGForDesign);			//持久荷载分项系数
	buffer.WriteWord(pTower->m_siSafetyGrade);				//安全等级
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//迟写的存储空间大小
	buffer.SeekPosition(current_pos);
	//2.天气信息区
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+4);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//存储空间大小
	buffer.WriteDouble(pTower->weather.m_fMaxIceThick);		//最大覆冰厚度
	buffer.WriteDouble(pTower->weather.m_fMaxIceThickCheck);//验算覆冰厚度
	buffer.WriteDouble(pTower->weather.m_fMaxIceWindV);		//最大覆冰风速
	buffer.WriteDouble(pTower->weather.m_fMaxWindV);		//大风速度
	buffer.WriteDouble(pTower->weather.m_fMaxWindVCheck);	//验算风速
	buffer.WriteDouble(pTower->weather.m_fSetupWindV);		//安装风速
	buffer.WriteInteger(pTower->weather.m_nAverageT);		//平均温度
	buffer.WriteInteger(pTower->weather.m_nHighestT);		//最高温度
	buffer.WriteInteger(pTower->weather.m_nLowestT);		//最低温度
	buffer.WriteInteger(pTower->weather.m_nMaxIceT);		//最大覆冰温度
	buffer.WriteInteger(pTower->weather.m_nMaxWindT);		//最大风速温度
	buffer.WriteInteger(pTower->weather.m_nSetupT);			//安装温度
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//迟写的存储空间大小
	buffer.SeekPosition(current_pos);
	//3.工况集合区
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+8);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//写入工况数据块长度
	//写入工况索引区
	buffer.WriteInteger(pTower->WorkStatus.GetNodeNum());	//工作状况，键值为CWorkCase::iNo
	CWorkStatus *pCase;
	for(pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
	{
		buffer.WriteInteger(pCase->handle);
		DWORD case_sec_start_pos=buffer.GetCursorPosition();
		buffer.WriteInteger(0);						//工况缓存大小
		pCase->ToRawBuffer(buffer);
		//
		current_pos=buffer.GetCursorPosition();
		buffer.SeekPosition(case_sec_start_pos);
		buffer.WriteDword(current_pos-case_sec_start_pos-4);//迟写的工况缓存大小
		buffer.SeekPosition(current_pos);
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//迟写的存储空间大小
	buffer.SeekPosition(current_pos);
	//4.架线方案区
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+12);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//写入架线方案数据块长度
	buffer.WriteInteger(evolve.WireLayouts.GetNodeNum());
	for(CWireLayout *pLayout=evolve.WireLayouts.GetFirst();pLayout;pLayout=evolve.WireLayouts.GetNext())
	{
		buffer.WriteInteger(pLayout->iNo);
		pLayout->ToBuffer(buffer);
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//迟写的存储空间大小
	buffer.SeekPosition(current_pos);
	//5.节点荷载区
	sect_start_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(sect_index_pos+16);
	buffer.WriteDword(sect_start_pos-base_address);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(0);							//写入节点荷载数据块长度
	DWORD tempos=buffer.GetCursorPosition();
	buffer.WriteInteger(0);//hashActualNodes.GetNodeNum());
	int load_nodes=0;
	buffer.WriteInteger(pTower->WorkStatus.GetNodeNum());
	//for(WIRENODE_STRU* pWireNode=hashActualNodes.GetFirst();pWireNode;pWireNode=hashActualNodes.GetNext())
	for(CLDSNode* pLoadNode=pTower->Node.GetFirst();pLoadNode;pLoadNode=pTower->Node.GetNext())
	{
		if(pLoadNode->m_idNodePlace<=0)
			continue;//TODO: 筛选出荷载节点
		load_nodes++;
		buffer.WriteInteger(pLoadNode->point_i);
		buffer.WriteInteger(pLoadNode->m_idNodePlace);	//TODO: 未考虑到一点多挂情况
		//TODO: 挂点的荷载分量分配系数将来也要传回到父进程中 wjh-2014.4.12
		for(pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
		{
			buffer.WriteInteger(pCase->handle);
			//NODE_LOAD load=GetActualNodeLoad(pWireNode->idNode,pCase->id,true);
			CExternalNodeLoad* pLoad=pCase->hashNodeLoad.GetValue(pLoadNode->handle);
			//TODO: pLoad->Fxyz及pLoad->permanentFz乘各项系数
			buffer.WritePoint(pLoad->Fxyz);
			buffer.WriteDouble(pLoad->permanentFz);
		}
	}
	current_pos=buffer.GetCursorPosition();
	buffer.SeekPosition(tempos);	//写入实际汇总出的荷载挂点数目
	buffer.WriteInteger(load_nodes);
	buffer.SeekPosition(sect_start_pos);
	buffer.WriteDword(current_pos-sect_start_pos-4);//迟写的存储空间大小
	buffer.SeekPosition(current_pos);*/
	return true;
}
////////////////////////////
