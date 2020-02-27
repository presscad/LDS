#pragma once
#include ".\StdComTempl\StdTemplPropertyList.h"
//#include ".\StdComTempl\TubeWalkWayTempl.h"

class CTubeSafeHolderPropertyList :public CStdComTemplPropertyList
{
public:
	CStdComTempl m_xSafeHolderTeml;
	CLDSLineTube* m_pBaseTube;
	struct SAFEHOLDER_PARAMS{
		long m_hBaseTube;		//依赖钢管
		long m_hBaseNode;		//定位节点
		CXhChar16 m_sSpec;		//支架槽钢规格
		CXhChar16 m_sPartNo;	//支架槽钢件号
		double m_fLen;			//槽钢长度
		char m_cMat;			//支架槽钢材质
		int m_nNum;				//支架槽钢数量
		double m_fSectAngle;	//多柱面射线杆件阵列时邻近两根间的扇形夹角，°
		//
		char m_ciSectNormalStyle;	//0.水平面;1.钢管垂直截面
		char m_ciRotFromEdgeStyle;	//射线方向定义类型
		GEPOINT m_xSectNorm;	//截面法向
		double m_fLenOff;		//圆心沿轴向的偏移量
		double m_fRayAngle;		//射线所在角度
		GEPOINT m_xRayVec;		//射线初始方向
	}param;
protected:
	virtual CStdComTempl* _InternalGetStdComTempl(){return &m_xSafeHolderTeml;}
public:
	CTubeSafeHolderPropertyList(void);
	~CTubeSafeHolderPropertyList(void);
	//
	bool VerifyRelaRodAndNodes();
	//
	virtual bool InitializeUI(bool bInternalStart=false,DWORD dwParam=0);
	virtual bool ConfirmOK();
	virtual bool CancelUI();
	virtual bool CreateComTemplInstance();
	//属性操作
	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//返回下一空闲Id
	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	virtual bool CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt);
	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	virtual bool DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo=0);
};