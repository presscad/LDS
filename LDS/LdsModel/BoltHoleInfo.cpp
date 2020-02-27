#include "stdafx.h"
#include "Tower.h"
#include "BoltHoleInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//每次显示螺栓孔属性时，临时建立存储孔与螺栓间映射关系哈希表 wjh-2019.8.5
CHashList<QUEUENODE_HOLE> hashBoltHoles;

BOOL BOLTHOLE::IsPropEqual(long idProp)
{
	if(hashBoltHoles.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	QUEUENODE_HOLE *pHole=NULL;
	QUEUENODE_HOLE *pFirstHole = hashBoltHoles.GetFirst();
	pFirstHole->xQueueNode.GetPropValueStr(idProp,valueStr);
	pHole=pFirstHole;
	while(pHole!=NULL)
	{
		if(pHole!=pFirstHole)
		{
			char valueStr2[500]="";
			pHole->xQueueNode.GetPropValueStr(idProp,valueStr2);
			if(stricmp(valueStr,valueStr2)!=0)
				return FALSE;
		}
		if((pHole=pHole->pNext)==NULL)
			pHole=hashBoltHoles.GetNext();
	}
	return TRUE;
}
int BOLTHOLE::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(CLDSBolt::GetPropID("dfHoleDiameter")==id)
		sText.Printf("%g",(this->pLsRef->pBolt->d+pLsRef->pBolt->hole_d_increment));
	else if(CLDSBolt::GetPropID("siWaistLength")==id)
		sText.Printf("%d",this->pLsRef->waistLen);
	else if(CLDSBolt::GetPropID("hHoleResidePart")==id)
		sText.Printf("0x%X",this->hResidePart);	//螺栓孔所在构件的句柄
	else if(CLDSBolt::GetPropID("ciHoleFuncType")==id)
	{	//螺栓孔的功能作用类型
		CXhChar50 xarrWords[10];
		int count=DELIMITER_STR::ParseWordsFromStr("0.螺栓孔|1.脚钉孔|2.挂线孔|3.接地孔|4.装配孔|5.挂牌孔|6.引流孔|7.代孔脚钉|8.地脚孔",
			"|",xarrWords,10);
		if(pLsRef->pBolt->m_cFuncType<count)
			sText=xarrWords[pLsRef->pBolt->m_cFuncType];
	}
	else if(CLDSBolt::GetPropID("ciManuProcessType")==id)
	{
		if((pLsRef->cFlag&CLsRef::DRILL_HOLE)>0)
			sText.Copy("钻孔");
		else //if((pLsRef->cFlag&CLsRef::PUNCH_HOLE)>0)
			sText.Copy("冲孔");
	}
	else if(CLDSBolt::GetPropID("blEnlargeHole")==id)
	{
		if((pLsRef->cFlag&CLsRef::ENLARGE_HOLE)>0)
			sText.Copy("是");
		else
			sText.Copy("否");
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		logerr.Log("Bolt hole(0x%X@0x%X)'s property##d string value out of length",pLsRef->pBolt->handle,this->hResidePart,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		logerr.Log("螺栓孔0x%X@0x%X属性#%d字符串超出长度限制",pLsRef->pBolt->handle,this->hResidePart,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen-1);
	return strlen(sText);
}
bool BOLTHOLE::ModifyPropValueStr(long id,const char* valueStr)
{
	if(id==CLDSBolt::GetPropID("dfHoleDiameter"))
		pLsRef->pBolt->hole_d_increment=atof(valueStr)-pLsRef->pBolt->d;
	else if(id==CLDSBolt::GetPropID("siWaistLength"))
		pLsRef->waistLen=atoi(valueStr);
	//else if(id==CLDSBolt::GetPropID("hHoleResidePart"))
	//	sText.Printf("%0x%X",this->hResidePart);	//螺栓孔所在构件的句柄
	else if(id==CLDSBolt::GetPropID("ciHoleFuncType"))
		pLsRef->pBolt->m_cFuncType=valueStr[0]-'0';
	else if(id==CLDSBolt::GetPropID("ciManuProcessType"))
	{
		pLsRef->cFlag&=0xfe;
		if(stricmp(valueStr,"钻孔")==0)
			pLsRef->cFlag|=CLsRef::DRILL_HOLE;
		//else if(stricmp(valueStr,"冲孔")==0)
		//	pLsRef->cFlag|=CLsRef::PUNCH_HOLE;
	}
	else if(CLDSBolt::GetPropID("blEnlargeHole")==0)
	{
		pLsRef->cFlag&=0xfd;
		if(stricmp(valueStr,"是")==0)
			pLsRef->cFlag|=CLsRef::ENLARGE_HOLE;
	}
	else
		return false;
	return true;
}
