#pragma once

#include "supergridctrl.h"
#include "RememberLocationDlg.h"
// CAngleBoltsDlg �Ի���

struct RODSECTION{
	char ciSectType;	//0.ָ���ڵ㴦;1.�˼�ʼ����;2.�˼��ն���
	CLDSNode* pSectNode;
	RODSECTION(CLDSNode* _pSectNode = NULL) {
		if (pSectNode = _pSectNode)
			ciSectType = 0;
		else
			ciSectType = 1;
	}
};

class CAngleBoltsDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(CAngleBoltsDlg)
public:
	CLDSLinePart* m_pBaseRod,*m_pCrossRod;
	RODSECTION section;
	GEPOINT m_xWingOffsetVec,m_xLineVec;
	int m_iOffsetWing;	//0.X֫ 1.Y֫
	int	m_iRayDirection;//���߷��� 0.ʼ->�� 1.��->ʼ
	int	m_iBoltNorm;	//��˨���� 0.���� 1.����
	int	m_iLsLayStyle;	//��˨���з�ʽ
	int m_iDatumPtStyle;	//��λ��ʽ 1.�ڵ�������ͶӰ 2.�ڵ�������ƽ�� 3.�Ǹ����߽���
	BOOL m_bMirLayLs;	//�ԳƲ���
	f3dPoint viewNorm;	//��ͼ����
	f3dPoint face_norm_offset;	//ƽ���淨��
	CDesignThickPara m_tBoltNormOffset;
	ATOM_LIST<CDesignLsPara> *m_pLsList;
	CHashList<CLDSBolt> m_xHashPrevViewBolts;
	//
	void InitDesignLsPara(CDesignLsPara& ls_para);
	void LayoutBolts();
	void PreviewBolt(CDesignLsPara *pLsPara);
	void UpdateContrlStatus();
	void UpdateLsSpaceData();
	void AppendRecord(CString*str_arr,void* pLsPara=NULL);
	void RefreshLsListCtrl();
	void WorkPlaneSketch();
	void DelWorkPlaneSketch();
	BOOL IsEnableSwitchG(int* g1=NULL,int* g2=NULL);
public:
	CAngleBoltsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAngleBoltsDlg();

// �Ի�������
	enum { IDD = IDD_ANGLE_BOLT_DLG };
	CString m_sBaseAngleSize;
	CString m_sRayAngleSize;
	CString m_sDatumPtText;
	CString m_sDatumPtText2;
	int		m_nG;
	int		m_nG1;
	int		m_nG2;
	int		m_nCrossAngleG;
	BOOL	m_bProject;		//�ڵ�������ͶӰ��
	BOOL	m_bPlaneOffset;	//�ڵ�������ƽ����
	CString	m_sBoltSize;	//��˨���
	double  m_fHoleD;		//�׾�
	int		m_nLsSpace;		//��˨���
	int		m_nOffset0;		//����˨ƫ��
	int		m_nLsNum;		//��˨����
	int		m_iBoltRows;	//��˨���� 0.���� 1.����
	int		m_iHoleFuncType;//�׹�������
	BOOL	m_bPunchHole;	//���
	BOOL	m_bDrillHole;	//���
	CSuperGridCtrl m_xListCtrlLsSet;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedChkProject();
	afx_msg void OnBnClickedChkPlaneOffset();
	afx_msg void OnBnTwoEdgeLayBolts();
	afx_msg void OnBnSingleLayBolts();
	afx_msg void OnBnSwitchG();
	afx_msg void OnChangeCmbBoltSize();
	afx_msg void OnSelchangeCmbLsRows();
	afx_msg void OnChangeELsNum();
	afx_msg void OnClickedChkPunchHole();
	afx_msg void OnClickedChkDrillHole();
	afx_msg void OnClickListLsSet(NMHDR* pNMHDR, LRESULT* pResult);
};
