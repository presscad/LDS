// PostProcessor.cpp: implementation of the PostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostProcessor.h"
#include "env_def.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if defined(__LDS_)||defined(__TSA_)
static void LoadElemId(CElement* pElem,DWORD id)
{
	pElem->id=id;
}
XHUINT64 BASENODE_REACTION::GetCompositeKey() const
{
	XHUINT64 idid;
	DWORD *pObjId=(DWORD*)&idid;
	DWORD *pObjSubId=pObjId+1;
	*pObjId=pNode->point_i;
	*pObjSubId =pModule->iNo<<27;	//呼高号最多为24,故占高位的5 bits足够 0xf8000000
	*pObjSubId|=(pStatus->iNo<<16)&0x07ff0000;	//工况号占中间的11 bits最多可表示2047种工况足够 0x07ff0000
	if(pModule->m_arrActiveQuadLegNo[0]==pModule->m_arrActiveQuadLegNo[1]&&pModule->m_arrActiveQuadLegNo[0]==pModule->m_arrActiveQuadLegNo[2]&&pModule->m_arrActiveQuadLegNo[0]==pModule->m_arrActiveQuadLegNo[3])
	{
		//*pObjSubId&=0xffff00ff;	//等高腿轮换(第3byte为0),最低位8bit表示一个接腿序号
		*pObjSubId|=pModule->m_arrActiveQuadLegNo[0];
	}
	else
	{	//高低腿轮换,最多可轮换至第15个接腿
		*pObjSubId|=(pModule->m_arrActiveQuadLegNo[0]&0x0f)<<12;
		*pObjSubId|=(pModule->m_arrActiveQuadLegNo[1]&0x0f)<<8;
		*pObjSubId|=(pModule->m_arrActiveQuadLegNo[2]&0x0f)<<4;
		*pObjSubId|=(pModule->m_arrActiveQuadLegNo[3]&0x0f);
	}
	return idid;
}
CXhChar50 BASENODE_REACTION::GetKeyString() const
{
	XHUINT64 idid=GetCompositeKey();
	DWORD *pId=(DWORD*)&idid;
	char keystr[17]={0};
	sprintf(keystr,"%8X%8X",*pId,*(pId+1));
	return keystr;
	/*CXhChar50 keyStr;
	keyStr.Printf("%d-%d-%s-%d-%d-%d-%d",pNode->point_i,pModule->iNo,(char*)pStatus->ToCaseNoString(),
		(pModule->m_arrActiveQuadLegNo[0]-1)%8+'A',(pModule->m_arrActiveQuadLegNo[1]-1)%8+'A',
		(pModule->m_arrActiveQuadLegNo[2]-1)%8+'A',(pModule->m_arrActiveQuadLegNo[3]-1)%8+'A');
	return keyStr;*/
}
bool BASENODE_REACTION::ReadObject(BUFFER_IO* pBuff,DWORD version)
{
	long h;
	pBuff->ReadInteger(&h);pNode=Ta.Node.FromHandle(h);
	pBuff->ReadInteger(&h);pModule=Ta.Module.FromHandle(h);
	pBuff->ReadInteger(&h);pStatus=Ta.WorkStatus.FromHandle(h);
	pBuff->Read(leg_no,4*sizeof(WORD));
	pBuff->ReadInteger(&status);
	pBuff->ReadPoint(reaction_force_design);
	pBuff->ReadPoint(reaction_force_std);
	return true;
}
bool BASENODE_REACTION::WriteObject(BUFFER_IO* pBuff,DWORD version)
{
	pBuff->WriteInteger(pNode->handle);
	pBuff->WriteInteger(pModule->handle);
	pBuff->WriteInteger(pStatus->handle);
	pBuff->Write(leg_no,4*sizeof(WORD));
	pBuff->WriteInteger(status);
	pBuff->WritePoint(reaction_force_design);
	pBuff->WritePoint(reaction_force_std);
	return true;
}
CPostProcessor postprocessor;
CPostProcessor::CPostProcessor()
{
	Reaction.SetHashTableGrowSize(0);
	hashElems.SetHashTableGrowSize(500);
	hashRodRelaElem.SetHashTableGrowSize(500);
	hashElems.LoadDefaultObjectInfo=LoadElemId;
}

CPostProcessor::~CPostProcessor()
{

}

void CPostProcessor::Empty()
{
	hashElems.Empty();	//清空以前所有的杆件单元记录
	Reaction.Empty();//清空基础作用力记录
	hashRodRelaElem.Empty();
	TowerWeight.Empty();//清空塔重记录
}
//计算辅材对主斜材支撑力及斜材对主材支撑力
void CPostProcessor::ApplyBraceForceToElem()
{
}
#endif
