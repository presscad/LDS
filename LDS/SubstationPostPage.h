#pragma once


// CSubstationPostPage �Ի���

class CSubstationPostPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationPostPage)

public:
	CSubstationPostPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationPostPage();

// �Ի�������
	enum { IDD = IDD_SUBSTATION_PANEL_POST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_fTopWidth;		//���ּܶ������
	int m_fBtmWidth;		//���ּܵײ����
	int m_fLinkHigh;		//���ּ����Ӹ߶�
	int m_fTopSideWidth;	//���ּܲ�Ŷ������
	int m_fBtmSideWidth;	//���ּܲ�ŵײ����
};
