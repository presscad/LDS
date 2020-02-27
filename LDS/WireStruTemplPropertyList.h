#pragma once
#include ".\StdComTempl\StdTemplPropertyList.h"
#include ".\StdComTempl\WireStruComTempl.h"

class CWireStruComPropertyList :public CStdComTemplPropertyList
{
public://protected:
	long m_hSetupNode10,m_hSetupNode20;	//防止undo时取消节点导致指针失效 wjh-2018.10.8
public:
	struct WIRESTRU_TEMPL_PARAMS{
		CLDSNode* m_pSetupNode10;
		CLDSNode* m_pSetupNode20;
		CLDSNode* m_pSetupNode30;
		CLDSNode* m_pSetupNode40;
		double X1,X2,Y1,Y2,Z1,Z2;	//一级尺寸设计参数
		double X3,X4,Z3,Z4;			//二级尺寸设计参数
	}params;
protected:
	CStdComTemplWireStructure xWireStruTmpl;
	virtual CStdComTempl* _InternalGetStdComTempl() { return &xWireStruTmpl; }
	void UpdateTemplParams();
public:
	CLDSNode* get_pSetupNode10();
	CLDSNode* set_pSetupNode10(CLDSNode* _pSetupNode10);
	__declspec(property(put=set_pSetupNode10,get=get_pSetupNode10)) CLDSNode* pSetupNode10;
	CLDSNode* get_pSetupNode20();
	CLDSNode* set_pSetupNode20(CLDSNode* _pSetupNode20);
	__declspec(property(put=set_pSetupNode20,get=get_pSetupNode20)) CLDSNode* pSetupNode20;
	CLDSNode* get_pSetupNode30();
	CLDSNode* set_pSetupNode30(CLDSNode* _pSetupNode30);
	__declspec(property(put=set_pSetupNode30,get=get_pSetupNode30)) CLDSNode* pSetupNode30;
	CLDSNode* get_pSetupNode40();
	CLDSNode* set_pSetupNode40(CLDSNode* _pSetupNode40);
	__declspec(property(put=set_pSetupNode40,get=get_pSetupNode40)) CLDSNode* pSetupNode40;
	CStdComTemplWireStructure* get_pWireStruTmpl(){ return &xWireStruTmpl;}
	__declspec(property(get=get_pWireStruTmpl)) CStdComTemplWireStructure* pWireStruTmpl;
	long get_idStdComTmpl() { return xWireStruTmpl.Id; }
	__declspec(property(get=get_idStdComTmpl)) long idStdComTmpl;
public:
	CWireStruComPropertyList(void);
	virtual ~CWireStruComPropertyList(void);
	void CopyFrom(CArmHeadComTempl* pWalkWayTempl);
	void CopyTo(CArmHeadComTempl* pWalkWayTempl);
	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//返回下一空闲Id
	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	virtual bool CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt);
	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	virtual bool DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo=0);
	virtual void WorkPlaneSketch();
	virtual void DelWorkPlaneSketch();
	virtual void UpdateSketchMap();
	virtual bool CreateComTemplInstance();
	virtual bool InitializeUI(bool bInternalStart=false,DWORD dwParam=0);
	virtual bool ConfirmOK();
	virtual bool CancelUI();
};

