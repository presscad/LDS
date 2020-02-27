#pragma once


// CSubstationBeamPage �Ի���

class CSubstationBeamPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamPage)

public:
	CSubstationBeamPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationBeamPage();

// �Ի�������
	enum { IDD = IDD_SUBSTATION_PANEL_BEAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_fWidth1;			//��ڿ��
	int m_fWidth2;			//ֱ�ڿ��
	int m_fLeftBeamLength;	//����������
	int m_fRightBeamLength;	//����������
	int m_fBeamHeight;		//�����߶�
	int m_nInternodeNum1;	//�����ڼ���
	int m_nInternodeNum2;	//ֱ���ڼ���
	int m_iPoleType;		//�ڼ�˼�����
	int m_nRightBeamInternodeCount;
	double m_fDlWidth;
	afx_msg void OnEnChangeELeftBeamLen();
};
