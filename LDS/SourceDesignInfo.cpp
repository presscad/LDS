// PreProcessor.cpp: implementation of the CPreProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "PreProcessor.h"
#include "Query.h"
#include "WindLoad.h"
#include "env_def.h"
#include "SortFunc.h"
#include "GlobalFunc.h"
#include "EarthQuake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __PART_DESIGN_INC_
//////////////////////////////////////////////////////////////////////////

bool CPreProcessor::InitSourceDesignInfo(CTower *pTower)
{
	CFGWORD cfgwordBody,cfgwordLeg;
	for(CLDSModule* pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		cfgwordBody.SetBodyLegs(pModule->GetBodyNo());
		cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
	}
	preprocessor.PrepareTower(pTower,cfgwordBody,cfgwordLeg,true,false);
	CElemInfo* pElemInfo;
	ATOM_LIST<CElemInfo>elemInfoList;
	for(CElement *pElem=preprocessor.result.elemList.GetFirst();pElem;pElem=preprocessor.result.elemList.GetNext())
	{
		pElemInfo = pTower->Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		bool copyCalLenInfo=(pElemInfo==NULL);
		if(pElemInfo==NULL)
			pElemInfo = pTower->Elems.Add(pElem->pStart->handle,pElem->pEnd->handle);
		pElem->CopyBasicInfoTo(pElemInfo,copyCalLenInfo);
		pElemInfo->relaElem.bFatherElem=pElem->relaElem.bFatherElem;
		pElemInfo->relaElem.xElemMirX.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirX;
		pElemInfo->relaElem.xElemMirY.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirY;
		pElemInfo->relaElem.xElemMirZ.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirZ;
		if(!pElem->relaElem.bFatherElem)
		{
			long idFather=GetFatherElemId(pElem);
			if(idFather>0&&idFather==pElem->relaElem.idElemMirX)
				pElemInfo->relaElem.xFatherElem.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirX;	//临时记忆,后续处理
			else if(idFather>0&&idFather==pElem->relaElem.idElemMirY)
				pElemInfo->relaElem.xFatherElem.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirY;
			else if(idFather>0&&idFather==pElem->relaElem.idElemMirZ)
				pElemInfo->relaElem.xFatherElem.pElemInfo=(CElemInfo*)pElem->relaElem.idElemMirZ;
		}
		elemInfoList.append(*pElemInfo);
	}
	pTower->Elems.Empty();
	CElemInfo::ClearInitialGlobalId();
	pTower->ElemPtrArr.Empty();
	pTower->ElemPtrArr.SetSize(0,elemInfoList.GetNodeNum());
	for(pElemInfo=elemInfoList.GetFirst();pElemInfo;pElemInfo=elemInfoList.GetNext())
	{
		pElemInfo->Id=CElemInfo::GetNewId();
		pElemInfo->SetBelongModel(&Ta);
		pElemInfo=pTower->Elems.SetValue(pElemInfo->m_hStartNode,pElemInfo->m_hEndNode,*pElemInfo);
		pTower->ElemPtrArr.append(pElemInfo);
	}
	for(pElemInfo=pTower->Elems.GetFirst();pElemInfo;pElemInfo=pTower->Elems.GetNext())
	{
		CElement *pMirElem;
		CLDSNode* pStart=Ta.Node.FromHandle(pElemInfo->m_hStartNode);
		CLDSNode* pEnd=Ta.Node.FromHandle(pElemInfo->m_hEndNode);
		if(pElemInfo->relaElem.xFatherElem.pElemInfo!=NULL)
		{
			long idMirElem=(long)pElemInfo->relaElem.xFatherElem.pElemInfo;
			if(preprocessor.result.hashElem.GetValueAt(idMirElem,pMirElem))
			{
				pElemInfo->relaElem.xFatherElem.pElemInfo=pTower->Elems.GetValue(pMirElem->pStart->handle,pMirElem->pEnd->handle);
				pElemInfo->relaElem.xFatherElem.hStartNode=pMirElem->pStart->handle;
				pElemInfo->relaElem.xFatherElem.hEndNode  =pMirElem->pEnd->handle;
			}
			else
				pElemInfo->relaElem.xFatherElem.Clear();
		}
		long idElemMirX=(long)pElemInfo->relaElem.xElemMirX.pElemInfo;
		if(preprocessor.result.hashElem.GetValueAt(idElemMirX,pMirElem))
		{
			pElemInfo->relaElem.xElemMirX.pElemInfo=pTower->Elems.GetValue(pMirElem->pStart->handle,pMirElem->pEnd->handle);
			pElemInfo->relaElem.xElemMirX.hStartNode=pMirElem->pStart->handle;
			pElemInfo->relaElem.xElemMirX.hEndNode  =pMirElem->pEnd->handle;
		}
		else
			pElemInfo->relaElem.xElemMirX.Clear();
		long idElemMirY=(long)pElemInfo->relaElem.xElemMirY.pElemInfo;
		if(preprocessor.result.hashElem.GetValueAt(idElemMirY,pMirElem))
		{
			pElemInfo->relaElem.xElemMirY.pElemInfo=pTower->Elems.GetValue(pMirElem->pStart->handle,pMirElem->pEnd->handle);
			pElemInfo->relaElem.xElemMirY.hStartNode=pMirElem->pStart->handle;
			pElemInfo->relaElem.xElemMirY.hEndNode  =pMirElem->pEnd->handle;
		}
		else
			pElemInfo->relaElem.xElemMirY.Clear();
		long idElemMirZ=(long)pElemInfo->relaElem.xElemMirZ.pElemInfo;
		if(preprocessor.result.hashElem.GetValueAt(idElemMirZ,pMirElem))
		{
			pElemInfo->relaElem.xElemMirZ.pElemInfo=pTower->Elems.GetValue(pMirElem->pStart->handle,pMirElem->pEnd->handle);
			pElemInfo->relaElem.xElemMirZ.hStartNode=pMirElem->pStart->handle;
			pElemInfo->relaElem.xElemMirZ.hEndNode  =pMirElem->pEnd->handle;
		}
		else
			pElemInfo->relaElem.xElemMirZ.Clear();
	}
	return true;
}
#endif
