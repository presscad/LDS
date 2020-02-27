#pragma once
#include "..\LdsModel\IModel.h"
#include "..\NativePropertyList.h"
#include ".\IStdComTempl.h"

class CNativeSlavePropertyList :public CNativePropertyList
{
protected:
	CNativePropertyList* m_pMasterPropList;
public:
	CNativeSlavePropertyList();
	~CNativeSlavePropertyList(){;}
	virtual CNativePropertyList* SetMasterPropList(CNativePropertyList* _pMasterPropList){
		return m_pMasterPropList=_pMasterPropList;
	}
	virtual PROPLIST_ITEM* GetPropItem(long id);
	virtual long GetPropID(const char* propStr);			//µ√µΩ Ù–‘ID
	virtual long GetPropValueStrByKeyStr(char* key,char* valueStr);
	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	virtual bool SetPropStatus(long id,BOOL bHide);
	virtual int  GetPropStatus(long id);
	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	virtual bool ModifyStatusFunc(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	virtual bool CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt);
	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
};
class CStdComTemplPropertyList :public CNativePropertyList
{
protected:
	IModel* m_pBelongModel;
	CBitmap* m_pSketchBitmap;
	CStatic* m_pSketchPanel;
	CNativeSlavePropertyList* m_pRelaSubPropList;
	virtual CStdComTempl* _InternalGetStdComTempl()=0;
public:
	CStdComTempl* get_pStdTempl(){return _InternalGetStdComTempl();}
	__declspec(property(get=get_pStdTempl)) CStdComTempl* pStdTempl;
public:
	CStdComTemplPropertyList();
	virtual IModel* SetBelongModel(IModel* pModel);
	virtual CBitmap* SetSketchBitmap(CBitmap* pBitmap);
	virtual CStatic* SetSketchImgPanel(CStatic* pSketchPanel);
	virtual CNativeSlavePropertyList* AttachSubPropList(CNativeSlavePropertyList* pSubPropList);
	virtual bool CreateComTemplInstance(){return false;}
	virtual void WorkPlaneSketch(){;}
	virtual void DelWorkPlaneSketch(){;}
	virtual void UpdateSketchMap();
	virtual bool InitializeUI(bool bInternalStart=false,DWORD dwParam=0){return false;}
	virtual bool ConfirmOK(){return false;}
	virtual bool CancelUI(){return false;}
};
