#include "stdafx.h"
#include "Tower.h"
#include "MirOperObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

bool CMirOperObject::m_bGlobalMirrorStart=false;
CHashListEx<CMirOperObject::MIRROR> CMirOperObject::hashMirObjs;
DWORD CMirOperObject::FullStateOf(CLDSDbObject* pObj)
{
	if (pObj->GetClassTypeId()==CLS_BOLT)
		return MIRBOLT_ALL;
	else if (pObj->GetClassTypeId()==CLS_PARAMPLATE)
		return 0;
	return 0;
}
CMirOperObject::MIRROR::MIROBJ* CMirOperObject::MIRROR::GetMirObj(MIRMSG mirmsg)
{
	for (UINT i=0;i<xarrMirObjs.Count;i++)
	{
		MIRROR::MIROBJ* pMirObj=xarrMirObjs.GetAt(i);
		if (mirmsg.axis_flag!=pMirObj->ciMirFlag)
			continue;
		if ((mirmsg.axis_flag&0x08)!=0&&mirmsg.rotate_angle!=pMirObj->siRotDegAng)
			continue;
		if ((mirmsg.axis_flag&0x10)!=0&&(!xMirOrg.IsEqual(mirmsg.origin)||
			!vMirNormal.IsEqual(mirmsg.mir_norm)))
			continue;
		return pMirObj;
	}
	return NULL;
}
bool CMirOperObject::MIRROR::MIROBJ::IsHasState(DWORD dwFlag)
{
	return (dwStateFlag&dwFlag)==dwFlag;
}
DWORD CMirOperObject::MIRROR::MIROBJ::RemoveState(DWORD dwFlag)
{
	DWORD dwRemoveFlag=0xffffffff^dwFlag;
	dwStateFlag&=dwRemoveFlag;
	return dwStateFlag;
}
MIRMSG CMirOperObject::MIRROR::MIROBJ::get_xMirMsg() const
{
	MIRMSG msg;
	msg.axis_flag=ciMirFlag;
	msg.rotate_angle=siRotDegAng;
	return msg;
}
CLDSDbObject* CMirOperObject::GetInnerMirObj(long hSrcObj,MIRMSG mirmsg)
{
	MIRROR* pMirror=hashMirObjs.GetValue(hSrcObj);
	if (pMirror==NULL)
		return NULL;
	for (UINT i=0;i<pMirror->xarrMirObjs.Count;i++)
	{
		MIRROR::MIROBJ mirobj=pMirror->xarrMirObjs.At(i);
		if (mirmsg.axis_flag!=mirobj.ciMirFlag)
			continue;
		if ((mirmsg.axis_flag&0x08)!=0&&mirmsg.rotate_angle!=mirobj.siRotDegAng)
			continue;
		if ((mirmsg.axis_flag&0x10)!=0&&(!pMirror->xMirOrg.IsEqual(mirmsg.origin)||
			!pMirror->vMirNormal.IsEqual(mirmsg.mir_norm)))
			continue;
		return mirobj.pMirObj;
	}
	return NULL;
}

bool CMirOperObject::ComplementMirInfo(CMirOperObject::MIRROR* pMirror,int indexOfMirObj)
{
	if (indexOfMirObj<0||indexOfMirObj>=(int)pMirror->xarrMirObjs.Count)
		return false;
	MIRROR::MIROBJ* pMirObj=&pMirror->xarrMirObjs[indexOfMirObj];
	if (pMirror->pSrcObj->GetClassTypeId()==CLS_BOLT)
	{
		CLDSBolt* pSrcBolt=(CLDSBolt*)pMirror->pSrcObj;
		CLDSBolt* pMirBolt=(CLDSBolt*)pMirObj->pMirObj;
		MIRROR* pBasePartMirror=hashMirObjs.GetValue(pSrcBolt->des_base_pos.hPart);
		if (pBasePartMirror==NULL||!pBasePartMirror->pSrcObj->IsPart())
			return false;
		SmartPartPtr pDatumPart=(CLDSPart*)pBasePartMirror->pSrcObj;
		SmartPartPtr pMirDatumPart=(CLDSPart*)GetInnerMirObj(pSrcBolt->des_base_pos.hPart,pMirObj->xMirMsg);
		if (!pMirDatumPart.blValid)
			return false;
		MIRMSG mirmsg=pMirObj->xMirMsg;
		if (!pMirObj->IsHasState(MIRBOLT_POS_PARAM))
		{
			if (pDatumPart->IsPlate())
			{	//由于对称连接板时可能先对称螺栓，此时插板还未对称可能导致局部定位的螺栓位置参数错误，需要在此时更新 wjh-2020.1.14
				GEPOINT mir_axis_x=GetMirVector(pDatumPart->ucs.axis_x,mirmsg);
				GEPOINT mir_axis_y=GetMirVector(pDatumPart->ucs.axis_y,mirmsg);
				if (mir_axis_x*pMirDatumPart->ucs.axis_x<0)	//对称后的Ｘ轴已进行了反向
					pMirBolt->des_base_pos.len_offset_dist*=-1.0;
				if (mir_axis_y*pMirDatumPart->ucs.axis_y<0)	//对称后的Ｙ轴已进行了反向
					pMirBolt->des_base_pos.wing_offset_dist*=-1.0;
			}
		}
		if (!pMirObj->IsHasState(MIRBOLT_NORM_PARAM))
		{

		}
	}
	return true;
}
CMirOperObject::CMirOperObject()
{
	//m_pTower=pTower;
	if(!m_bGlobalMirrorStart)
	{
		m_bObjectStart=true;	//有效启动该UNDO对像
		m_bGlobalMirrorStart=true;
	}
	else
		m_bObjectStart=false;
}
void CMirOperObject::Finished()
{
	if (!m_bObjectStart)
		return;
	for (MIRROR* pMirror=hashMirObjs.GetFirst();pMirror!=NULL;pMirror=hashMirObjs.GetNext())
	{
		for (UINT i=0;i<pMirror->xarrMirObjs.Count;i++)
		{
			if (pMirror->xarrMirObjs[i].ciMirFlag==0)
				continue;
			if (pMirror->xarrMirObjs[i].dwStateFlag==FullStateOf(pMirror->pSrcObj))
				continue;
			ComplementMirInfo(pMirror,i);
		}
	}
	hashMirObjs.Empty();
	m_bGlobalMirrorStart=false;
}
CMirOperObject::~CMirOperObject()
{
	Finished();
}
