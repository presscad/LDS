#if !defined(AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_)
#define AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DesignSimpleLadderDlg.h : header file
//
#include "PropertyList.h"
#include "XhCharString.h"
#include "CallBackDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDesignSimpleLadderDlg dialog
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

class CDesignSimpleLadderDlg : public CCallBackDialog
{
	CBitmap m_CurPlateBmp;
	CStringArray m_arrPropGroupLabel;
public:
	//�Ŷ���Ϣ
	typedef struct tagFootNailPara{
		CXhChar16 sPartNo;		//�̽Ŷ��ı��
		CXhChar16 sLongPartNo;	//���Ŷ��ı��
		double fUpEndSpace;		//�϶˾�(һ���Ƕ̽Ŷ����ͷ�ľ���)
		double fDownEndSpace;	//�¶˾�(һ���ǳ��Ŷ����ͷ�ľ���)
		double fBetweenSpace;	//�Ŷ����
		int    nNum;			//�̽Ŷ���
		int iDirection;			//��һ���̽Ŷ��ĳ���
		double short_grade,long_grade;
		double fShortLen;		//�̽Ŷ��ĳ���
		double fLongLen;		//���Ŷ��ĳ���
		int D;					//�Ŷ���ֱ��
		tagFootNailPara(){memset(this,0,sizeof(tagFootNailPara));}
	}FOOT_NAIL_PARA;
	//���Ӱ���Ϣ
	typedef struct tagPlatePara{
		CXhChar16 sPartNo;		//Բ�׸ְ�ı��
		CXhChar16 sEllipsePartNo;//��Բ�׸ְ�ı��
		double fUpEndSpace;		//�϶˾�(һ����Բ�׸ְ���϶�ͷ�ľ���)
		CXhChar100 sBetweenSpace;//��Բ�׸ְ�ļ��(���ϵ�����������)
		int nPlateThick;		//���Ӱ���
		double fCircinalPlateA;	//Բ�׸ְ��A
		double fCircinalPlateB;	//Բ�׸ְ��B
		double fEllipsePlateC;	//��Բ�ο׸ְ��C
		int iEllipseAperture;//��Բ�׳����γ���
		CXhChar16 sAnglePartNo;//���ӽǸֵı��
		double fLineAngleLen;	//���ӽǸֵĳ���
		double fDownEndSpace;//�Ǹ־��׼�ֹܿ�ʼ�˾���
		int iConnectType;		//���ӷ�ʽ
		double wide;
		double thick;
		char cMaterial;
		int iAngleDirect;//�Ǹַ��߳��� 0����1����
		int	nLSD;		//Բ�ο׵���˨���
		tagPlatePara(){memset(this,0,sizeof(tagPlatePara));}
	}PLATE_PARA;
	//�ֹ���Ϣ
	typedef struct tagLineTubePara{
		CXhChar16 sPartNo;
		double wide;
		double thick;
		CLDSLineTube *pBaseLineTube;
		double fDownSpace;	//��ֹܿ�ʼ�εľ���
		double fAngle;		//�ͻ�׼��֮��ĽǶ�
		SEGI iSeg;
		tagLineTubePara(){memset(this,0,sizeof(tagLineTubePara));}
	}LINE_TUBE_PARA;
public:
	CDesignSimpleLadderDlg(CWnd* pParent = NULL);   // standard constructor
	//���Թ���
	void InitPropHashtable();
	int GetPropValueStr(long id, char *valueStr);	//��������ID�õ�����ֵ
	int GetPropStr(long id, char *propStr);			//�õ������ַ���
	static CStringKeyHashTable<long>propHashtable;	//�����ַ�����ϣ��
	static long GetPropID(char* propStr);			//�õ�����ID
	void SelectLineTube();//ѡ��ֹ�
	void SetLineTube();//����ѡ�иֹ�
	
	void RefreshTabCtrl(int iCurSel);
	void ReDraw();	//ˢ��
	void SetCmdPickPromptStr(CString sPromptStr);
	//��ʾ����
	void DisplayPropertyList(int iCurSel=0);
	//��������
	BOOL CreateSimpleLadder();
	//�����������Ӱ� iPlateType=0ΪԲ�װ�1Ϊ��Բ  
	BOOL CreateConnectPalte(int iPlateType,double fOffsetSpace,CLDSLineTube *pBaseLineTube,CLDSLineAngle *pLineAngle);
	//�������ݽŶ� iType=0����Ķ̽Ŷ�=1���ҵĶ̽Ŷ�2���Ŷ�
	BOOL CreateFootNail(CLDSLineTube *pLadderLineTube,double fOffsetSpace,int iType);
	//�����ֹ�
	BOOL CreateLineTube(double fLineTubeLen,CLDSLineTube *pLineTube,double fVectorLen);
	//�������ӽǸ�
	CLDSLineAngle* CreateConnectLineAngle(double fOffsetSpace);

public:
	//�Ŷ�
	FOOT_NAIL_PARA FootNailPara;
	//���Ӱ�
	PLATE_PARA PlatePara;
	//�ֹ�
	LINE_TUBE_PARA LineTubePara;
	//����
	CPen solid_pen,dash_pen,dash_dot_pen,green_solid_pen,green_dash_pen,
		red_solid_pen,blue_solid_pen,purple_solid_pen;
// Dialog Data
	//{{AFX_DATA(CDesignSimpleLadderDlg)
	enum { IDD = IDD_DESIGN_SIMPLE_LADDER_DLG };
	CPropertyList	m_propList;
	CTabCtrl	m_ctrlPropGroup;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignSimpleLadderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignSimpleLadderDlg)
	afx_msg void OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnConfirmCreate();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSIMPLELADDERDLG_H__139C3267_91F5_46E2_86B0_B1691D58D791__INCLUDED_)
