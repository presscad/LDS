#pragma once
#include "afxwin.h"


// CSubstationGeneralPage �Ի���

class CSubstationGeneralPage : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationGeneralPage)

public:
	CSubstationGeneralPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationGeneralPage();

// �Ի�������
	enum { IDD = IDD_SUBSTATION_PANEL_DESCRIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nSpanNum;		// ����
	double m_fBeamHeight;	//������
	double m_fPostHeight;	//������
	double m_fDL;		//�������ľ���
	int m_iPostType;	//0.���ּ�
	int m_iBeamType;	//0.������ 1.�ķ���
	BOOL m_bStartSup;	//ʼ��֧��
	BOOL m_bEndSup;		//�ն�֧��
	BOOL m_bDesignPost;
	BOOL m_bDesignBeam;
	afx_msg void OnBnClickedChkDesignPost();
	afx_msg void OnBnClickedChkDesignBeam();
};
