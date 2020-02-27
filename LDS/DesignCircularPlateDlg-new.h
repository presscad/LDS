#if !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
#define AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignCircularPlateDlg.h : header file
//

#include "PropertyList.h"
#include "PropListItem.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignCircularPlateDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CConnVectorList
{
	ATOM_LIST<CONNECT_DIRECTION> vecList;	//���ӷ���
public:
	CONNECT_DIRECTION *Append();
	CONNECT_DIRECTION *Append(CONNECT_DIRECTION &vec);
	long GetRelaConnVecLinePart(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetRelaConnInfo(int serial,BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetFirst(BOOL bProfilePole=FALSE);
	CONNECT_DIRECTION *GetNext(BOOL bProfilePole=FALSE);
	int GetCount(BOOL bProfilePole=FALSE);
	void Empty(BOOL bProfilePole=FALSE);
	//
	CONNECT_DIRECTION *GetRelaConnVecByFeature(long feature);
	BYTE DeleteRelaConnVecByFeature(long feature);
	//�����Ƿ����serial==1��serial==2�����ӷ����ж�
	BOOL IsNeedBend();
};

class CDesignCircularPlateDlg : public CCallBackDialog
{
	GEPOINT m_xSpreadVec1,m_xSpreadVec2;
	CBitmap m_xCurPlateBmp;
	CLDSParamPlate *CreateWaterShieldedPlate();	//������ˮ�� 
	//������˨ wht 11-05-16
	void LayoutBolt(CLDSParamPlate *pParamPlate);
public:
	CDesignCircularPlateDlg(CWnd* pParent = NULL);
	
	BOOL IsValidParam();
	void UpdateSketchMap();
	void InitCirPlatePara();//��ʼ�����������
	void InitProfilePara();	//��ʼ���ְ����β���
	BOOL AutoCalWN();		//�Զ�����W,N����
	void ModifyPlateParam(WORD wKey,double fValue);
	void ModifyPlateParam(WORD wKey,long nValue);
	void ReDesignPlate();
	//���㻷����ϲ೤�ȣ��²೤�ȣ������������Ʒ��Ļ���� wht 11-05-31
	void CalMaxMinOffsetDist(double &up_len,double &down_len);
	//�ְ����ɺ���
	CLDSParamPlate *CreateCircularPlate();
	//��ӹ����˼�
	void AddRelaPole(CLDSLinePart *pLinePart);
	//
	static const int TYPE_DATUM_VECTOR	=1;	//��λ��׼����
	static const int TYPE_TUBE_SPREAD	=2;	//�ֹ��᷽��
	static const int TYPE_Z_AXIS_VECTOR =3;	//Z�᷽��
	static const int TYPE_X_AXIS_VECTOR =4;	//X�᷽��
	static const int TYPE_Y_AXIS_VECTOR =5;	//Y�᷽��
 	void WorkPlaneSketch(int iType);
	void DelWorkPlaneSketch();
	void ClearCirPlate();
	//���Թ���
	DECLARE_PROP_FUNC(CDesignCircularPlateDlg);
	void InitEngPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	void DisplayPlatePropList();
	void DisplaySubPropList(CPropTreeItem *pItem=NULL);
	void UpdateCirPlateParamProp(CPropTreeItem *pItem=NULL);
	void UpdatePlateProfileParamProp(CPropTreeItem *pItem);
	void UpdateVectorPropList(CPropTreeItem *pParentItem,char* sPrefix,CLDSVector xVector);
public:
	long m_idSubEventProp;			//��¼�����жϵ�������ID,�ָ�����ʱʹ��
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
public:
	CLDSParamPlate *m_pCirPlate;	//��ǰ���ڱ༭״̬���߰�,Ŀǰ�����ڲ鿴�޸����� wht 11-05-12
	BOOL m_bNewCirPlate;			//TRUE:���������µĻ���� FALSE:�鿴�޸�����
	BOOL m_bDisplayDlg;		//��Ʒ��ʱ���Ƿ񵯳����׶Ի���
	//0.��ͨ�����1.������;2.�����߰�3WL(����֧��ֱ����)3.�����߰�3WA(����֧��Բ����)
	//4.�����߰�2WL(����֧��ֱ����)5.�����߰�2WA(��֧֫��Բ����) 6.��ˮ��
	int m_iPlateType;
	BOOL m_bReadOnlyPlateType;		//���øְ����Ͳ��ɱ༭�����ָ�����������η���ʱʹ�� wht 11-06-07
	//��������
	CLDSNode *m_pDatumNode;
	CLDSSphere *m_pDatumSphere;
	CLDSLineTube *m_pDatumTube;				//��׼�ֹ�
	CLDSPart *m_pRelaPart1, *m_pRelaPart2;	//��������
	CLDSVector design_axis_z;
	CLsRefList LsRefList;			//������˨
	CConnVectorList ConnVecList;	//���ӷ���
	CONNECT_DIRECTION *m_pCurSelConnVec;	//���ڼ�¼��ǰѡ�����ӷ���
	CXhChar16 m_sPartNo;
	CString  m_sSeg;
	char m_cMaterial;
	long thick;
	UCS_STRU plate_ucs;	//���������ϵ wht 11-06-03
	CString m_sWndTitle;//���ô��ڱ��� wht 11-06-07
	//
	struct CIRCULAR_PARAM{
		double A;	//��,����λ��(��λ����),��λ��
		double B;	//��,�������ν�(���β���),��λ��
		long   C;	//��λ��׼�ֹ�(����)���(��λ����)
		double D;	//��׼�ֹ���ڵ����ɱ߾��룬������ɱ߾�
		long   E;	//��һ�������׼�ְ���
		long   F;	//�ڶ��������׼�ְ������򺸽ӹ����ֹܾ��(�����ֹ���ָ�뻷����к���ĸֹܣ���������Ĳ����˨���Ӹֹܣ���˲����ܳ�������)
		long   G;	//���ɱ�����(>0ֱ�ߣ�<=0Բ��)
		double H,h;	//W��N��̨�ڸ߶�
		long   I;	//��һ�����ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
		long   J;	//�ڶ������ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
		long   P;	//��λ��׼�ڵ���
		double R;	//�������Բ���뾶
		double r;	//r:�������Բ�뾶(���η�����360�Ļ����ʹ�� r<0���ݸֹ�������Բ r=0����Բ r>0��ָ���뾶������Բ) wht 11-05-27
		double S;	//�Զ�λ��׼�ڵ���λԭ�������ƫ�ƾ���(S<0�Ϸ��;S>0�·��)
		double T;	//�Զ�λ��׼�ڵ���λԭ�������ƫ�ƾ����ڻ���巨���ϵ�ͶӰ����
		double V;	//�ӹ���϶(��׼�ֹ���ڵ�������ڻ���Ԥ����϶)
		double Y;	//��Բ�ͻ�����λ��(>0��׼�����;=0�Զ�;<0��׼���ڲ�)
		double gama;//y:��,2WA�ͻ����߰����ɲ�ĵ��ǽǶ�,��λ��
		double W;	//��һ������ְ�ĺ��쳤
		double N;	//�ڶ�������ְ��ֹܵĺ��쳤
		BOOL bWeldSemicircleMode; //�������Ƿ�����Ӱ�Բ
		double R1,R2; //��׼��1��ˮ�װ뾶����׼����ˮ�װ뾶
		int nOffsetType;	//0.����ƫ�� 1.����ƫ��
		//����D��HΪһ���������,D��H�����������Ϊ��Խ���� 
		//3WL:0.�������� 1.����������D 2.����������H 
		//3WA:0.������DH 1.����DH
		int nDHRelativeType;	
		//BOOL bNeedBend;		//��Ҫ������߰�,m_iPlateType==0ʱ��ʾ�Ƿ�Ϊ���η���
		BOOL bCreateMirPlate;	//���ɶԳƲ�ְ� wht 11-06-07
		//��Ӧ�����б��е�Y���� wht 12-03-20
		long nCirCoverPlateType;//Y==0��ʾ������ͨ���;Y==1��ʾ�����νǦ���Y��������ļ򻯷��;Y==-1��ʾ�����νǦ���Y�Ḻ�����ļ򻯷��
	}para;
	struct PARAM_WATER
	{
		double D;	//�ڻ�ֱ��
		double W;	//�⻷ֱ��
		double S;	//����ƫ�ƾ���
		double K;	//��ˮ�ۿ��
		double H;	//��ˮ�����
		double L;	//��ˮ�۳���
		double A;	//����ˮ��֮��ļн�
		double O;	//����ˮ�۽�ƽ�������׼��֮���ƫ�ƽ�
		double E;	//��λ�ǻ�׼��:0.��׼��1.������
	}water;
	struct LAYOUT_BOLT
	{
		SEGI m_iBoltSegI;	//�κ�
		int m_iDirection;	//����
		int m_nBoltD;		//ֱ��
		int m_nBoltNum;		//���� 
		//��������˨����
		int m_iStartEdge;		//��������ʼ��,0.��׼��,1.������
		double m_fOffsetAngle;	//����˨�������ʼ��֮���ƫ�ƽǶ�
		double m_fBoltLayoutR;	//��˨���ڰ뾶
		LAYOUT_BOLT(){memset(this,0,sizeof(LAYOUT_BOLT));}
	}m_xLayoutBolt;

// Dialog Data
	//{{AFX_DATA(CDesignCircularPlateDlg)
	enum { IDD = IDD_DESIGN_CIRCULAR_PLATE_DLG };
	CPropertyList	m_xSubPropList;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignCircularPlateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignCircularPlateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnConfirm();
	afx_msg void OnPartsDb();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNCIRCULARPLATEDLG_H__3ECCA5E9_8DB1_4E12_93A3_B8C2CFBAA131__INCLUDED_)
