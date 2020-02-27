#include "stdafx.h"
#include "XhLdsLm.h"
#include "StdTemplPropertyList.h"

///////////////////////////////////////////////////////////////////////////
CNativeSlavePropertyList::CNativeSlavePropertyList()
{
	m_pMasterPropList=NULL;
}
PROPLIST_ITEM* CNativeSlavePropertyList::GetPropItem(long id)
{
	return m_pMasterPropList->GetPropItem(id);
}
long CNativeSlavePropertyList::GetPropID(const char* propStr)			//得到属性ID
{
	return m_pMasterPropList->GetPropID(propStr);
}
long CNativeSlavePropertyList::GetPropValueStrByKeyStr(char* key,char* valueStr)
{
	return m_pMasterPropList->GetPropValueStrByKeyStr(key, valueStr);
}
long CNativeSlavePropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	return m_pMasterPropList->GetPropValueStr(id, valueStr,nMaxStrBufLen);
}
bool CNativeSlavePropertyList::SetPropStatus(long id,BOOL bHide){
	return m_pMasterPropList->SetPropStatus(id, bHide);
}
int  CNativeSlavePropertyList::GetPropStatus(long id){
	return m_pMasterPropList->GetPropStatus(id);
}
bool CNativeSlavePropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt){
	return m_pMasterPropList->ButtonClick(pItem, lpRslt);
}
bool CNativeSlavePropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	return m_pMasterPropList->ModifyValueFunc(pItem, valueStr,lpRslt);
}
bool CNativeSlavePropertyList::ModifyStatusFunc(CPropTreeItem* pItem,BOOL *lpRslt)
{
	return m_pMasterPropList->ModifyStatusFunc(pItem, lpRslt);
}
BOOL CNativeSlavePropertyList::HelpPromptProperty(CPropTreeItem* pItem)
{
	return m_pMasterPropList->HelpPromptProperty(pItem);
}
bool CNativeSlavePropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt){
	return m_pMasterPropList->CallBackCommand(message,wParam,lpRslt);
}
BOOL CNativeSlavePropertyList::FinishSelectObjOper(long hPickObj,long idEventProp){
	return m_pMasterPropList->FinishSelectObjOper(hPickObj,idEventProp);
}
void CNativeSlavePropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	m_pMasterPropList->DisplaySlavePropertyList(NULL,0);
}
///////////////////////////////////////////////////////////////////////////
CStdComTemplPropertyList::CStdComTemplPropertyList()
{
	m_pBelongModel=NULL;
	m_pSketchBitmap=NULL;
	m_pSketchPanel=NULL;
	m_pRelaSubPropList=NULL;
}
IModel* CStdComTemplPropertyList::SetBelongModel(IModel* pModel)
{
	if(this->pStdTempl)
		pStdTempl->SetBelongModel(pModel);
	return m_pBelongModel=pModel;
}
CBitmap* CStdComTemplPropertyList::SetSketchBitmap(CBitmap* pBitmap)
{
	return m_pSketchBitmap=pBitmap;
}

CStatic* CStdComTemplPropertyList::SetSketchImgPanel(CStatic* pSketchPanel)
{
	return m_pSketchPanel=pSketchPanel;
}
CNativeSlavePropertyList* CStdComTemplPropertyList::AttachSubPropList(CNativeSlavePropertyList* pSubPropList)
{
	pSubPropList->SetMasterPropList(this);
	return m_pRelaSubPropList=pSubPropList;
}
void CStdComTemplPropertyList::UpdateSketchMap()
{
	CStdComTempl* pStdComTmpl=_InternalGetStdComTempl();
	if(m_pSketchBitmap==NULL||pStdComTmpl==NULL)
		return;
	m_pSketchBitmap->Detach();
	if(pStdComTmpl->Id==IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH)
	{	//钢板支架式
		if (pStdComTmpl->LoadSketchImage())
		{
			if (m_pSketchBitmap->GetSafeHandle() != NULL)
				m_pSketchBitmap->Detach();
			m_pSketchBitmap->CreateBitmapIndirect(&pStdComTmpl->sketchimg);
		}
	}
	else
		return;
	//
	//CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_MAP);
	HBITMAP hBitmap=(HBITMAP)m_pSketchBitmap->GetSafeHandle();
	m_pSketchPanel->SetBitmap(hBitmap);
}
