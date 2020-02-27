#pragma once

#ifdef __PART_DESIGN_INC_
// CCalPerfectSectDlg �Ի���

class CCalPerfectSectsDlg : public CDialog
{
	DECLARE_DYNAMIC(CCalPerfectSectsDlg)
public:
	CLDSLinePart* m_pRod;
	STEELMAT *m_pSteel;
	double m_fR;			//��Բ���뾶
	double m_fRx;			//��ת�뾶(���԰뾶)
	double m_fGrossArea;	//ë���
	double m_fNetArea;		//�����
	double m_fLsCoef;		//��˨����ϵ��
	int m_nBoltD;			//��˨ֱ��
	int m_nSectNum;			//�ڼ���
	char m_cSectionType;	//
public:
	void OptimizeSectCounts();
	double VerityCompStableStress();
public:
	CCalPerfectSectsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCalPerfectSectsDlg();

// �Ի�������
	enum { IDD = IDD_CAL_PERFECT_SECTS_DLG };

public:
	CString	m_sLsGuiGe;		//��˨���
	CString m_sStress;		//ѹ��Ӧ��
	CString m_sSectNum;		//�ڼ���
	CString m_sLsCoef;		//��˨����ϵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangeCmbLsGuige();
	afx_msg void OnChangeCmbSectNum();
	afx_msg void OnChangeEHoleOut();
};
#endif