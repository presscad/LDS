#if !defined(AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
#define AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDianBanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFootnailSpaceDlg dialog
#include "lds_part.h"
#include "CallBackDialog.h"
#include "NailResideAnglePage.h"
#include "NailResideTubePage.h"
#include "MemberProperty.h"
class CFootnailSpaceDlg : public CCallBackDialog
{
	CLDSBolt xNailBolt,xNailBolt2;	//xNailBolt2Ϊ˫�಼��ʱ����һ��۸ֽŶ���Ԥ����ʾ
	//CLDSLineSlot xNailSlot;
	CLDSBolt* m_pReplacerBolt;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
	CNailResideAnglePage pageOnAngle;
	CNailResideTubePage  pageOnTube;
	int m_iOldLayWing,m_iOldNailType;
// Construction
public:
	void InitState(int hCurRod, double CanUseLen,double SumLen,double lenOffset,double fAngle=0,double fAngle2=120);
	CFootnailSpaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFootnailSpaceDlg)
	enum { IDD = IDD_FOOTNAIL_SPACE_DLG };
	int		m_nNailSpace;
	int		m_nDefaultNailSpace;
	double	m_fNailAngle;
	double	m_fNailAngle2;
	double	m_fCurNailAngle,m_fCurNailAngle2;
	CString	m_sCanUseLen;
	CString	m_sCurJg;
	CString	m_sSumLen;
	CString	m_sNailGuiGe;
	BOOL	m_bShiftWing;
	//int		m_iLayWing;
	int		m_iDatumLine;	//0--�Ի�׼��Ϊ��׼ 1--�Ժ�����Ϊ��׼
	//int		m_iGType;	// ����׼���ѡ������:0,��׼׼��g,1,2,3,�ֱ��ʾg1,g2,g3,4��ʾ�Զ���
	//long	m_nGDist;	// �û��Զ���ĽŶ�׼��
	BOOL m_bSearchReplaceBolt;
	BOOL m_bHorizonSlotNail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFootnailSpaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CLDSBolt* SearchReplaceBolt(double preferStepSpace=420,bool bUpdateNailSpace=true);
	// Generated message map functions
	//{{AFX_MSG(CFootnailSpaceDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChkShiftWing();
	afx_msg void OnChkDualSide();
	afx_msg void OnChangeNailAngle();
	afx_msg void OnChangeNailAngle2();
	afx_msg void OnChangeCurNailAngle();
	afx_msg void OnRdoDatumLine();
	afx_msg void OnBnPickReplaceNailbolt();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeENailSpace();
	afx_msg void OnEnChangeDefaultNailSpace();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickedChkSearchReplaceBolt();
	afx_msg void OnChkHorizonNail();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void PreviewNailBolt(BOOL bReFocus=TRUE);
public:
	void FireRdoLayWing(int iOldLayWing);
	void FireResideAngleDist();
	void FireRdoNailType(int iOldNailType);
public:
	// �Ǹ�֫��
	long wing_wide;
	JGZJ xWingXZhunJu,xWingYZhunJu;
	BOOL m_bEnableTeG;
	BOOL m_bHasWeldRoad;//�Ƿ���ں�����
	int	 m_iLineType;	//��׼�������� 0--�Ǹ� 1--�ֹ� 
	int  m_bDualSide;
	void RefreshDlgCtrlState();
	void AddNailGuigeRecord();
	void InitLineTubeFootNail(BOOL bNewCreate=FALSE);
	void InitLineAngleFootNail();
public:
	f3dPoint m_xDatumStart,m_xDatumLenVec;
	CSuperSmartPtr<CLDSLinePart> m_pRod;
	CLDSBolt* m_pMergeBolt;
	double m_fPreRodRemainingLen;	//ǰһ�˼���ʣ�೤��,��˼����õ�һ����˨ʱ�Ŷ������Ҫ��ȥǰһ�˼�ʣ�೤�� wht 15-08-13
	long hFirstLineSlot;		//��׼�۸ֵ������
	CLDSLineSlot xNailSlot;
	CLDSLineAngle xNailAngle;
private:
	CLDSLineSlot xNailSlot2;	//˫�಼��ʱ����һ��۸ֽŶ�����Ԥ����ʾ
public: //����
	PROPERTY(int,m_iLayWing);
	GET(m_iLayWing){ return m_iLineType==0?pageOnAngle.m_iLayWing:pageOnTube.m_iLaySide; }
	SET(m_iLayWing){
		pageOnAngle.m_iLayWing=value;
		pageOnTube.m_iLaySide =value;
	}
	PROPERTY(int,m_iNailType);
	GET(m_iNailType){ return pageOnTube.m_iNailType; }
	SET(m_iNailType){ pageOnTube.m_iNailType =value; }
	//�Ǹ��ϽŶ����ľ�
	PROPERTY(int,m_nGDist);
	GET(m_nGDist){return pageOnAngle.m_nGDist;}
	SET(m_nGDist){pageOnAngle.m_nGDist=value;}
private:
	double m_fLenUseDist;	//��ǰ��������˨��m_xDatumStart��m_xDatumLenVec��������ʹ�þ���
	double m_fFocusZoomCoef;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOOTNAILSPACEDLG_H__9FCCC071_8378_4A9F_A126_A64982A77FDB__INCLUDED_)
