#pragma once
#include "propertylist.h"
#include "CallBackDialog.h"

// CDesignColumnRayRodDlg �Ի���

class CDesignColumnRayRodDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	const static int ROD_TYPE_ANGLE = 0;
	const static int ROD_TYPE_SLOT	= 1;
	const static int ROD_TYPE_FLAT	= 2;
	const static int ROD_TYPE_TUBE	= 3;

	const static int ROD_ENDPOSTYPE_AXISVEC	= 0;
	const static int ROD_ENDPOSTYPE_RAYVEC	= 1;
	const static int ROD_ENDPOSTYPE_USERDEF = 2;

	DECLARE_DYNAMIC(CDesignColumnRayRodDlg)
	void DisplayRayRodPropList();
	void DisplaySubObjPropList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();
	void InitRayRodDesginPara();
	CLDSLinePart *CreateRayRod();
	int m_idParentEventProp;
	//���߸˼�������Ϣ
	CXhChar16 m_sPartNo;	//����
	SEGI  m_xSegI;			//�κ�
	char m_cMaterial;		//����
	int m_iRodType;			//�˼����� 0.�Ǹ� 1.�۸� 2.���� 3.�ֹ�
	CXhChar16 m_sSpec;		//���
	CLDSPoint m_xRodStartPos;	//�˼�ʼ��λ��(Ĭ��Ϊ���涨λ��)
	int m_iEndPosType;			//0.��ʼ�������������� 1.��ʼ�������������� 2.�Զ���
	int m_nRotateArrayN;		//���������߸˼����е�����
	double m_fSectorAngle;		//���������߸˼�����ʱ�ڽ�����������μнǣ���
	CLDSPoint m_xRodEndPos;		//�˼��ն�λ��(Ĭ��Ϊ���涨λ��)
	int m_iRodLenDefType;		//�˼����ȶ�������: 0.ָ���˼����� 1.ָ��ĩ�˸���뾶
	double m_fRayRodLen;		//�˼�����/ĩ�˸���뾶
	//ʼ�����涨λ��
	CLDSVector m_xStartAxisVec;		//���涨λ��:����������
	CLDSVector m_xStartRayVec;		//���涨λ��:���߷���
	CLDSPoint m_xStartSectCenter;	//���涨λ��:����Բ��
	CLDSVector m_xStartSectNorm;	//���涨λ��:���淨�߷���
	//�ն����涨λ��
	CLDSVector m_xEndAxisVec;		//���涨λ��:����������
	CLDSVector m_xEndRayVec;		//���涨λ��:���߷���
	CLDSPoint m_xEndSectCenter;		//���涨λ��:����Բ��
	CLDSVector m_xEndSectNorm;		//���涨λ��:���淨�߷���
	//�ֹܡ��۸֡����������淨�߲���
	f3dPoint m_xWorkPlaneNorm;		//�����淨��
	short  m_iNormCalStyle;			//��׼�淨�߼��㷽ʽ 0.ֱ��ָ�� 1.ʹ�ò��ո˼�����
	f3dPoint m_xNearWorkPlaneNorm;	//nearWorkPlaneNorm�����չ����淨�ߵ�ȷ������
	long m_hWorkNormRefPart;		//���㹤���淨��ʱ�Ĳ��ո˼������Ϊ0������ʼ�˽ڵ���ն˽ڵ�ĸ��˼�Ϊ���ո˼�
	//�Ǹֹ����淨�߲���
	CLDSVector m_xDesVectorX,m_xDesVectorY;		//���������б༭�Ǹֲ���ʱʹ��
	f3dPoint m_xNormX,m_xNormY;					//��¼X��Y֫����
	DESIGN_JGWING_NORM m_xDesNormX,m_xDesNormY;
public:
	CDesignColumnRayRodDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDesignColumnRayRodDlg();
	//
	void UpdateParentEventPropItem();
	//���Թ���
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	void InitPropHashtable();
	static long GetPropID(char *sProp,...);			//�õ�����ID
// �Ի�������
	enum { IDD = IDD_DESIGN_COLUMN_RAY_ROD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedBtnConfirm();
public:
	CLDSLineTube *m_pBaseLineTube;
	CPropertyList m_xPropList;
	CPropertyList m_xSubPropList;
	CString m_sSubObjName;
};
