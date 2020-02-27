#pragma once


// CSubstationSquareBeamPage �Ի���

class CSubstationSquareBeamPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationSquareBeamPage)

public:
	CSubstationSquareBeamPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationSquareBeamPage();

// �Ի�������
	enum { IDD = IDD_SUBSTATION_PANEL_SQUARE_BEAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_fBeamWidth;			//���������
	int m_fBeamHeight;			//�������߶�
	int m_nInternodeNum;		//�������ڼ���
	int m_iPoleType;			//�ڼ�˼�����
};
