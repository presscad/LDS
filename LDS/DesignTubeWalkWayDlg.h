#pragma once

#include "CallBackDialog.h"
#include "PropertyList.h"
#include "XhCharString.h"
#include ".\StdComTempl\TubeWalkWayTempl.h"
//////////////////////////////////////////////////////////////////////////
// CDesWireNodeDlg �Ի���
class CDesignTubeWalkWayDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDesignTubeWalkWayDlg)
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CBitmap m_xWalkWayBmp;
	long m_hBaseTube,m_hBaseNode;	//��ֹundoʱȡ���ڵ㵼��ָ��ʧЧ wjh-2018.10.8
	long m_hOuterBeamDatumNode,m_hInnerBeamDatumNode;
	long m_hOuterAngle,m_hInnerAngle;
public:
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
	char m_ciHolderAngleLayoutStyle;	//0.����;1.����
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
	double m_fZjOffDist;			//֧��λ��ƫ�ƾ���
	double m_fZdOffDist;			//�ߵ�ʼ��ƫ�ƾ���
	//
	bool VerifyRelaRodAndNodes();
	CTubeWalkWayStdComTempl m_xWalkWayTempl;
public:
	CDesignTubeWalkWayDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignTubeWalkWayDlg();

	// �Ի�������
	enum { IDD = IDD_DESIGN_WALKWAY_DLG };
	CPropertyList m_propList;
	CPropertyList m_propSubList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	void UpdateSketchMap();
	void InitTemplParams();
	GEPOINT  CalNearVecX();
	void WorkPlaneSketch();
	void DelWorkPlaneSketch();
	BOOL CreateTubeWalkWay();
	//
	DECLARE_PROP_FUNC(CDesignTubeWalkWayDlg);
	int GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ�ַ���
	void DisplayPropList();
	void DisplaySubPropList();
	void UpdateWalkWayProp(CPropTreeItem* pParentItem);
	void UpdateHolderProp(CPropTreeItem* pParentItem);
	void UpdateAssemblyProp(CPropTreeItem* pParentItem);
	BOOL FinishSelectObjOper(long hPickObj,long idEventProp);
};
