#pragma once
#include ".\StdComTempl\StdTemplPropertyList.h"
//#include ".\StdComTempl\TubeWalkWayTempl.h"

class CTubeSafeHolderPropertyList :public CStdComTemplPropertyList
{
public:
	CStdComTempl m_xSafeHolderTeml;
	CLDSLineTube* m_pBaseTube;
	struct SAFEHOLDER_PARAMS{
		long m_hBaseTube;		//�����ֹ�
		long m_hBaseNode;		//��λ�ڵ�
		CXhChar16 m_sSpec;		//֧�ܲ۸ֹ��
		CXhChar16 m_sPartNo;	//֧�ܲ۸ּ���
		double m_fLen;			//�۸ֳ���
		char m_cMat;			//֧�ܲ۸ֲ���
		int m_nNum;				//֧�ܲ۸�����
		double m_fSectAngle;	//���������߸˼�����ʱ�ڽ�����������μнǣ���
		//
		char m_ciSectNormalStyle;	//0.ˮƽ��;1.�ֹܴ�ֱ����
		char m_ciRotFromEdgeStyle;	//���߷���������
		GEPOINT m_xSectNorm;	//���淨��
		double m_fLenOff;		//Բ���������ƫ����
		double m_fRayAngle;		//�������ڽǶ�
		GEPOINT m_xRayVec;		//���߳�ʼ����
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
	//���Բ���
	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//������һ����Id
	virtual long GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);
	virtual bool ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt);
	virtual bool ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt);
	virtual BOOL HelpPromptProperty(CPropTreeItem* pItem);
	virtual bool CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt);
	virtual BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
	virtual void DisplayPropertyList(int overwrite0_append1=0,DWORD dwPropGroup=1);
	virtual bool DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo=0);
};