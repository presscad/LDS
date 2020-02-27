#if !defined(AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_)
#define AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignSpecialFlDlg.h : header file
//

#include "XhListCtrl.h"
#include "PropertyList.h"
#include "PropListItem.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignSpecialFlDlg dialog

class CDesignSpecialFlDlg : public CDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;

	CBitmap m_CurPlateBmp;
	CStringArray m_arrPropGroupLabel;
	//����
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
	//��ʾ����
	void DisplayPropertyList(int iCurSel=0);
	//������˨
	void CreateBolt(CLDSPlate *pPlate);
	//���ɻ����
	CLDSPlate* CreateCircularPlate(CLDSLineTube *pDatumTube);
	//���ɸ��Ӹְ�
	void CreateAttachPlate(CLDSPlate *pPlate,CLDSLineTube *pDatumTube);
	//���ɻ�����
	void CreateCoverPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2,
		CLDSParamPlate *&pCoverPlate1,CLDSParamPlate *&pCoverPlate2);
	//���ɻ����߰�
	CLDSParamPlate* CreateRibPlate(CLDSPlate *pNodePlate1,CLDSPlate *pNodePlate2);
	//ƽ���и�ֹ�
	void CutDatumTubeByPlane(CLDSPlate *pPlate);
	//���η���
	CLDSPlate *CreateSpecialFl(LINEPARTSET *pLinePartSet,double fNormOffset=0);
public:
	int m_iNormOffsetType;
	CLDSPlate *m_pCirPlate;	//0.�ϲ෨�� 1.�²෨�� 2.�м䷨��
	//������˨����
	typedef struct tagLayoutBoltPara 
	{
		int m_iBoltSegI;	//�κ�
		int m_iDirection;	//����
		int m_nBoltD;		//ֱ��
		int m_nBoltNum;		//���� 
		int m_iStartEdge;	//��������ʼ��
		double m_fOffsetAngle;//����˨�������ʼ��֮���ƫ�ƽǶ�
		double m_fBoltLayoutR;//��˨���ڰ뾶
		tagLayoutBoltPara(){memset(this,0,sizeof(tagLayoutBoltPara));}
	}LAYOUT_BOLT_PARA;
	//������
	typedef struct tagElbowPlatePara
	{
		char cMaterial;		//���Ӱ����
		char sPartNo[16];	//���
		int m_nThick;		//���
		double A,B,C,D,E,F,G,H;//���β���
		int m_iCutMode;		//�߰嵹��ģʽ��0.�н�|1.�л�
		double m_fAngle;	//�׿鷨���߰�����ڻ�׼�ߵ�ƫ�ƽǶȻ�ʮ�ֲ���߰��нǽǶ�
		int m_nPlateNum;	//�����߰����
		BOOL m_bObverse;	//�����沼�øְ�
		BOOL m_bVertical;	//��ֱ���Զ���λ�����
		tagElbowPlatePara(){memset(this,0,sizeof(tagElbowPlatePara));}
	}ELBOW_PLATE_PARA;
	//������
	typedef struct tagCoverPlatePara
	{	
		char cMaterial;		//����
		char sPartNo[16];	//���
		int m_nThick;		//���
		double W,D;			//���β��� 
		double m_fAlfa,m_fBeta;	//������
		BOOL m_bUpPlate;	//�Ϸ��
		BOOL m_bDownPlate;	//�·��
		BYTE biHoleType;	//0.Բ��|1.��Բ��|2.�޿�
		double BH;			//��ˮ��ֱ����<0.�޿�;0.��Բ��;>0.Բ�ף�
		UINT uiAttribute1;	//�ڵ��1��ˮ����Ϣ
		UINT uiAttribute2;	//�ڵ��2��ˮ����Ϣ
		tagCoverPlatePara(){memset(this,0,sizeof(tagCoverPlatePara));biHoleType=0;BH=13.5;}
	}COVER_PLATE_PARA;
	//�߰����
	typedef struct tagRibPlatePara
	{
		char cMaterial;		//����
		char sPartNo[16];	//���
		int m_nThick;		//���
		double S,W,D,H,N;//���β��� 
		BOOL m_bUseH;	 //���ò���H
		BOOL m_bTopCir;	 //�߰嶥��Բ������
		BOOL m_bOutterRibPlate;	//����߰�
		BOOL m_bMirCreate;//�Գ�����
		f3dPoint norm;	 //�߰巨��
		tagRibPlatePara(){memset(this,0,sizeof(tagRibPlatePara));}
	}RIB_PLATE_PARA;
public:
	CDesignSpecialFlDlg(CWnd* pParent = NULL);   // standard constructor
	//���Թ���
	DECLARE_PROP_FUNC(CDesignSpecialFlDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	bool UpdateItemPropValue(char* propStr);
	//
	void RefreshTabCtrl(int iCurSel);
	void ReDraw();	//ˢ��
	int LayoutBolt();//������˨,������˨����
	int LayoutAttachPlate();//���ø��Ӱ�
	void UpdateTubeOddment();//���¸ֹ�����ͷ��Ϣ
	void UpdateRibPlateWN();//�����߰�W��N����
	void InitWaterleakHolePara();
	UINT _InitWaterHoleRadiusByThick(double fPlateThick);
	UINT _InitCutRadiusByTubeThick();
	void UpdateWaterHolePropItem(CPropTreeItem* pParentItem);
public:
	CLDSNode *m_pBaseNode;			//��׼�ڵ�
	CLDSLineTube *m_pCurLineTube;	//��ǰ�ֹ�
	CLDSLineTube *m_pCoupleTube;	//�ԽӸֹ� �˸ֹ����������η���
	CLDSLinePart *m_pRayLinePart[4];//���߸˼�	0,1�ϲ����߸˼� 2,3�²����߸˼�
	CLDSPlate *m_pNodePlate[4];		//�ڵ��	0,1�ϲ�ڵ�� 2,3�²�ڵ��
	CLDSParamPlate *m_pCoverPlate[4];//������
	LINEPARTSET *m_pLinePartSet;	//���߸˼�����
	//�װ����
	GEPOINT axis_z;	//�װ幤������
	CXhChar16 m_sPartNo;
	SEGI iSeg;
	char m_cMaterial;
	int	 m_nThick;
	double m_fNormOffset;
	double m_fExternalRadius;	//�������뾶
	double m_fInnerRadius;		//������ڰ뾶
	double H;	//����������ֹܶ�ͷ֮��ľ���
	double m_fOddment;	//�ֹ�����ͷ
	//��˨����
	LAYOUT_BOLT_PARA LayoutBoltPara;
	//���
	ELBOW_PLATE_PARA ElbowPlatePara;
	//���
	COVER_PLATE_PARA CoverPlatePara;
	//�߰�
	RIB_PLATE_PARA RibPlatePara;
	BOOL m_bCreateRibPlate;		//���������Ƿ������߰�	wht 11-05-26
	BOOL m_bCutDatumTube;		//��׼�ֹ��Ƿ����ƽ���и�
// Dialog Data
	//{{AFX_DATA(CDesignSpecialFlDlg)
	enum { IDD = IDD_DESIGN_SPECIAL_FL_DLG };
	CXhListCtrl	m_listPlate;
	CXhListCtrl	m_listBolt;
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignSpecialFlDlg)
protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignSpecialFlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	afx_msg void OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownPlateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickPlateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeLjStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSPECIALFLDLG_H__35EF0998_6C88_43B0_B791_70B92D749DE3__INCLUDED_)
