#if !defined(AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_)
#define AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignTubeEndLjDlg.h : header file
//

#include "SuperGridCtrl.h"
#include "PropertyList.h"
#include "PropListItem.h"
#include "CallBackDialog.h"
#include "XhListCtrl.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg dialog

class CDesignTubeEndLjDlg : public CCallBackDialog
{
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	//װ��λ�ò���
	typedef struct tagSetUpPosPara  
	{
		CLDSVector workNorm;			//��幤���淨��
		CLDSVector workNormUpVert;		//ʮ�ֲ����ĸ���ϲ�������幤������
		CLDSVector workNormDownVert;	//ʮ�ֲ����ĸ���²�������幤������
		double  H1;						//ƽ�������ֹܶ��뷨������ľ���
		//
		double  m_fWeldHeight;			//����߶� wht 11-05-09 
		double	m_fOddment;				//�ֹ�����ͷ
		double	m_fNormOffset;			//��巨��ƫ����
		double	m_fTubeNormOffset;		//�ֹܷ���ƫ����
		double	m_fTubeEccentricDist;	//�ֹ�ƫ�ľ�
		tagSetUpPosPara(){memset(this,0,sizeof(tagSetUpPosPara));}
	}SETUP_POS_PARA;
	//������˨����
	typedef struct tagLayoutBoltPara 
	{
		int m_iDirection;	//����
		int m_nBoltD;		//ֱ��
		int m_nBoltNum;		//���� 
		//��������˨����
		int m_iStartEdge;	//��������ʼ��
		double m_fOffsetAngle;//����˨�������ʼ��֮���ƫ�ƽǶ�
		double m_fBoltLayoutD;//��˨����ֱ��
		//��������˨����
		int m_nRowNum;		//����
		int m_nColNum;		//����
		double m_fRowSpace;	//�о�
		double m_fColSpace;	//�о�
		f3dPoint m_posFirstBolt;//����˨λ��
		//
		double m_fSpecialSpace;		//������
		BOOL m_bStaggerLayout;		//ˮƽ������ֱ�潻����
		BOOL m_bMirLayout;			//�ԳƲ���������������˨
		tagLayoutBoltPara(){memset(this,0,sizeof(tagLayoutBoltPara)); m_iStartEdge=0;}
	}LAYOUT_BOLT_PARA;
	//���Ӱ����
	typedef struct tagAttachPlatePara  
	{
		char cMaterial;		//���Ӱ����
		char sPartNo[16];	//���
		double A,B,C,D,E,F,G,H;//���β��� 
		int m_iCutMode;					//�߰嵹��ģʽ��0.�н�|1.�л�
		int m_nThick;		//���
		double m_fAngle;	//�׿鷨���߰�����ڻ�׼�ߵ�ƫ�ƽǶȻ�ʮ�ֲ���߰��нǽǶȡ�
		int m_nPlateNum;	//�����߰����
		BOOL m_bUseAngle;	//���ýǶȦ�
		BOOL m_bMirCreate;	//�Գ����ɲ������һ�μӾ���
		CLDSPlate *m_pWeldParentPlate;	//ʮ�ֲ�帽���߰�ĺ��Ӹ����� wht 10-08-02
		tagAttachPlatePara();
	}ATTACHPLATE_PARA;
	//�ڵ��
	typedef struct tagNodePlatePara
	{
		char cMaterial;		//����
		char sPartNo[16];	//���
		int m_nThick;		//���
		int m_nRollHeight;	//��߸߶�
		tagNodePlatePara();
	}NODEPLATE_PARA;
	//������
	typedef struct tagFillPlatePara{
		int nNodePlateThick;	//��ʮ�ֲ�������Ľڵ��ĺ�ȣ����ݸú���ж��Ƿ���Ҫ������壬�ⲿ�趨
		int nThick;				//�����
		char cMat;				//������
		char sPartNo[16];		//�����
		tagFillPlatePara(){memset(this,0,sizeof(tagFillPlatePara)); cMat='S';}
	}FILLPLATE_PARA;
private:
	//���ɱ�׼����
	BOOL NewStandardPart(CLDSStdPartModel *pStdPartModel,CLDSStdPartModel *pCoupleStdPartModel=NULL);
	//������˨
	void CreateBolt(CLDSParamPlate *pParamPlate);
	//���ɸ��Ӹְ�
	void CreateAttachPlate(CLDSParamPlate *pParamPlate,CLDSLineTube *pDatumTube,BOOL bFlCoupleDesign=FALSE);
	//��ʼ����׼��������
	void InitStdPartDesignParam();
	CString GetTubeEndLjName();
public:
	CDesignTubeEndLjDlg(CWnd* pParent = NULL);   // standard constructor
	//���Թ���
	DECLARE_PROP_FUNC(CDesignTubeEndLjDlg);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//��������ID�õ�����ֵ
	void DisplayPropertyList(int iCurSel=0);
	void DisplayChildPropertyList(CPropTreeItem *pItem,BOOL bUpdate=FALSE);
	void FinishSelectObjOper();		//���ѡ�����ĺ�������
	void UpdatePartTypeProperty(CPropTreeItem* pParentItem);		//
	void UpdateBaseInfoProperty(CPropTreeItem* pParentItem);		//���»�����Ϣ
	void UpdateTubeEndProperty(CPropTreeItem* pParentItem);			//���¸ֹܶ�����
	void UpdateSetUpProperty(CPropTreeItem* pParentItem);			//����װ�����
	void UpdateProfileProperty(CPropTreeItem* pParentItem);			//�������β���
	void UpdateBoltProperty(CPropTreeItem* pParentItem);			//������˨����
	void UpdateAttachPlateProperty(CPropTreeItem* pParentItem);	//���¸��Ӱ����
	void UpdateFillPlateProperty(CPropTreeItem* pParentItem);		//���������� wht 11-05-11
	//���¿ؼ���ʾ״̬������
	void Redraw();
	void RefreshTabCtrl(int iCurSel);
	void RefreshCtrlState(BOOL bShowBolt=TRUE);
	void RefreshBoltListColumn();
	void RefreshBoltParaListColumn();
	void RefreshBoltParamList();
	void RefreshBoltList();	
	//������˨���ò���
	void UpdateLayoutBoltPara();
	void LayoutBolt(ATOM_LIST<f3dPoint>& bolt_pt_arr);
	//���µõ���׼��������
	void LoadStdPlateParaByLevel();	
	//���¸ֹ�����ͷ
	void UpdateTubeOddment();
	//��ʼ����Ʋ���
	void UpdateDesignParam(BOOL bInitPartType=TRUE);
	//���ݴ��ڵİ���������Ϣ
	void UpdateDesignParamBySrcPlate(CLDSParamPlate* pSrcParamPlate);
	//����ʾ��ͼ
	void UpdateSketchMap(int iCurSel=0);
	//�ж��Ƿ��Ѿ����ڶ�����
	BOOL IsHasExistEndLj();
	//�õ���������
	int GetPartType() const;
	//���ù�������
	void SetPartType(int iPartType);
	//����ʮ�ֲ�帽�Ӱ�,bIsCloneBolt��ʾ����ϵ���˨ͨ���Ժ����¡���� wht 11-05-27
	void LayoutXEndSubPlate(CLDSParamPlate *pParamPlate,BOOL bIsCloneBolt=FALSE);
	//���ʮ�ֲ�����,ʹ�ð��ִ������ʱ�ݲ�֧��������(�����һ�㲻�����) wht 11-05-11
	void DesignXEndFillPlate(CLDSParamPlate *pParamPlate);
	//��ƾ�߲���ϵļӾ���
	void DesignREndAttachPlate(CLDSParamPlate *pParamPlate);
	//��ƽڵ��
	void InitPlateBasePara(CLDSPlate* pNodePlate,CLDSLineTube *pDatumLineTube);
	void DesignLineAngle(CLDSLineAngle* pRayJg,CLDSPlate* pNodePlate);
	CLDSPlate *CreateNodePlate(CLDSLineTube *pDatumLineTube,CLDSLineAngle* pRayLineAngle);
	CLDSPlate *CreateNodePlate(CLDSLineTube *pDatumLineTube,
		CLDSLineTube *pRayLineTube=NULL,CLDSParamPlate *pParamPlate=NULL,
		CLDSLineTube *pHorizontalTube=NULL);	//pHorizontalTube���������η������ڵ��ʱʹ�� wht 11-06-07
	CLDSParamPlate *GetParamPlate() const{return m_pParamPlate;}
	//���ù����淨��
	void SetWorkNorm(f3dPoint norm){ work_norm=norm; }
	//�����淨��
	f3dPoint GetWorkNorm() const { return work_norm; }
	//���ݲ���ѡ���׼���
	INSERT_PLATE_PARAM* QueryStdInsertPlate(int iLevel,char cMat);
	INSERT_PLATE_PARAM* QueryStdInsertPlateByMat(char cMat,int* pLevelArr=NULL);
	FL_PARAM* QueryStdFlDParaByMat(char cMat,int* pLevelArr=NULL);
//
private:
	f3dPoint work_norm;					//�����淨�߷���
	CBitmap m_CurPlateBmp;
	CLDSParamPlate *m_pParamPlate,*m_pSrcParamPlate;//
	BOOL m_bHasExistEndLj;	//��ǰ�ֹܶ��Ѿ����ڶ�����
	int m_iPartType;		//0.��ͨ���� 1.��׼����(ƽ������) 2.�Ժ����� 3.���Է��� 4.���Է���
	//char sCodeName[16];		//��׼�������� ��¼��һ�α�׼��������,���±�׼��������
	//��׼������ز���
	FL_PARAM *m_pFlDParam,*m_pFlPParam;
	FL_PARAM *m_pFlDParam2,*m_pFlPParam2;
	FL_PARAM2 *m_pRigidFlParam,*m_pFlexibleFlParam;
	FL_PARAM2 *m_pRigidFlParam2,*m_pFlexibleFlParam2;
	INSERT_PLATE_PARAM *m_pInsertPlateParam;
	//����
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
public:
	CLDSNode *m_pBaseNode;		//��׼�ڵ�
	CLDSLineTube *m_pCurLineTube;//��ǰ�ֹ�
	CLDSLineTube *m_pRelatedTube;//�ԽӸֹ�
	bool m_bCoupleFLCreate;
	BOOL m_bStartEnd;
	BOOL m_bSpecEndLjStyle;		//ָ������������
	BOOL m_bDesignNodePlate;	//��ƽڵ�� (������Ű�ʱ��Ҫ)

	CLDSParamPlate m_paramPlate;//�洢�������ְ�������Ϣ
	char sCoupleFlPlateNo[16];	//(��ͨ����)���Ⱦ��Խ�ʱ�Խӷ����ı�� wht 10-09-29
	char sWeldPlatePartNo[16];	//ʮ�ֲ�崹����
	char sWrapPlatePartNo[16];	//ʮ�ֲ�������
	char sCodeName[16];			//ʮ�ֲ�����
	int m_iWrapPlateType;		//ʮ�ֲ���������  0.�ְ� 1.�Ǹ�
	char m_cFlLevel1;			//���ܶԺ���������
	char m_cFlLevel2;			//�ԽӶԺ���������
	int m_iStdPartLevel;
	char m_cStdPartMat;
	//�ֹ����
	BOOL m_bFaceToFace;	//�Խ����
	long m_hWeldLinkObj;
	long m_hDatumTube1;
	long m_hDatumTube2;
	CStringArray m_arrPropGroupLabel;
	CStringArray m_arrStdPartLevel;	
	CStringArray m_arrStdFlDLevelM;
	CStringArray m_arrStdFlDLevelC;
	//ƽ���и�
	f3dPoint cutPlaneNorm,cutPlaneOrigin;
	//������˨����	��ʱ���֧��ʮ����˨����
	LAYOUT_BOLT_PARA LayoutBoltPara;
	ATOM_LIST<LAYOUT_BOLT_PARA> LayoutBoltParaList;
	//װ�����
	SETUP_POS_PARA setupInfo;
	//���Ӱ�
	BOOL m_bCreateAttachPlate;
	ATTACHPLATE_PARA AttachPlatePara;
	//�������	wht 11-05-11
	BOOL m_bCreateFillPlate;		
	FILLPLATE_PARA FillPlatePara;
	//�ڵ��
	NODEPLATE_PARA NodePlatePara;
	//���η������ӵ����߸˼�
	BOOL m_bSpecialFlRayPole;
public:
// Dialog Data
	//{{AFX_DATA(CDesignTubeEndLjDlg)
	enum { IDD = IDD_DESIGN_TUBE_ENDLJ_DLG };
	CPropertyList	m_xSubPropList;
	CXhListCtrl		m_listBolt;
	CSuperGridCtrl	m_listBoltParam;
	CTabCtrl	m_ctrlPropGroup;
	CPropertyList	m_propList;
	int		m_iTubeEndLjStyle;	//0:ƽ�������;1.Բ���������;2.����;3.U�Ͳ��;4.C�Ͳ��;5.ʮ�ֲ��;6.�߾�����.(��ӦTUBEJOINT::type)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignTubeEndLjDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignTubeEndLjDlg)
	
	afx_msg void OnSelchangeTubeEndljStyle();
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	afx_msg void OnDelCurEndlj();
	afx_msg void OnPartsDb();
	afx_msg void OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNTUBEENDLJDLG_H__EB9EA7AF_EB03_4AC8_80CB_5CD3F9584EBC__INCLUDED_)
