#pragma once

#ifdef __COMPLEX_PART_INC_
#include ".\StdComTempl\StdTemplPropertyList.h"
#include ".\StdComTempl\TubeWalkWayTempl.h"
class CWalkWayPropertyList :public CStdComTemplPropertyList
{
public://protected:
	long m_hBaseTube,m_hBaseNode;	//��ֹundoʱȡ���ڵ㵼��ָ��ʧЧ wjh-2018.10.8
	long m_hOuterBeamDatumNode,m_hInnerBeamDatumNode;
	long m_hOuterAngle,m_hInnerAngle;
public:
	CTubeWalkWayStdComTempl m_xWalkWayTempl;
	struct WALKWAY_PARAMS{
		BYTE m_ciWalkWayType;		//0.�ְ�֧���ߵ�|1.����֧���ߵ�|2.̨��֧���ߵ�
		BOOL m_bCreateWalkWay;		//�Ƿ������ߵ��Ǹ�
		double m_fYA;				//��׼�ֹܵ��ߵ����Ǹֵĺ���ͶӰ����
		double m_fZH;				//��׼�ֹܵ��ߵ��Ǹ�ƽ�������ͶӰ����
		double m_fWidth;			//�ߵ����
		double m_fLength;			//�ߵ�����
		double m_fZh;				//б���²�����ƫ��
		double m_fD1;				//б���²����ƫ��
		double m_fD2;				//б���ϲ�˵���ˮƽ�����ڵĺ���ƫ��
		double m_fYB;				//���ź���ƫ�ƾ�
		double m_fBoltSpace;		//��˨���
		int m_nBoltMd;				//��˨ֱ��
		int m_nBoltNum;				//��˨����
		CXhChar16 m_sWalkWayJgSpec;	//�ߵ��Ǹֹ��
		CXhChar16 m_sHolderJgSpec;	//֧�ܽǸֹ��
		char m_cWalkWayJgMat;		//֧�ܽǸֲ���
		char m_cHolderJgMat;		//֧�ܽǸֲ���
		char m_ciHolderAngleLayoutStyle;	//��ŽǸֳ���0.����;1.����
		char m_ciOtherAngleLayoutStyle;		//б�ŽǸֳ���0.����;1.����
		char m_cPlateMat;			//�ְ����
		int m_nThick;				//�ְ���
		struct PLATE_PARA{
			double A;	//�ߵ����Ӱ����μн�
			double B;	//��˨���Ӱ����μн�
			double H;	//̨�ڸ߶�
			double W;	//
		}para;
		//
		CLDSLineTube* m_pBaseTube;
		CLDSNode* m_pBaseNode;
		CLDSLineAngle* m_pOuterJg;	//�ߵ����Ǹ�
		CLDSLineAngle* m_pInnerJg;	//�ߵ��ڲ�Ǹ�
		double m_fZjOffDist;		//֧��λ��ƫ�ƾ��룬��Ӧ����<X0>����
		double m_fZdOffDist;		//�ߵ�ʼ��ƫ�ƾ��룬��Ӧ����<X1>����
	}params;
protected:
	virtual CStdComTempl* _InternalGetStdComTempl(){return &m_xWalkWayTempl;}
	void UpdateTemplParams();
	bool UnifyParaAngleLayoutByAntiAxisX(CStdTemplParaRod* pParaRod);
	GEPOINT CalNearVecX();
public:
	CWalkWayPropertyList(void);
	virtual ~CWalkWayPropertyList(void);
	void ResetParams();
	void CopyFrom(CTubeWalkWayStdComTempl* pWalkWayTempl);
	void CopyTo(CTubeWalkWayStdComTempl* pWalkWayTempl);
	bool VerifyRelaRodAndNodes();
	virtual long InitPropHashtable(long initId=1,long initHashSize=50,long initStatusHashSize=50);	//������һ����Id
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
	void UpdateAssemblyProp(CPropTreeItem* pParentItem);
	void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	void UpdateHolderProp(CPropTreeItem* pParentItem);
};
#endif
