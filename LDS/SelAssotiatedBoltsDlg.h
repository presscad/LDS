#pragma once


// CSelAssotiatedBoltsDlg �Ի���

class CSelAssotiatedBoltsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelAssotiatedBoltsDlg)

public:
	CSelAssotiatedBoltsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelAssotiatedBoltsDlg();

// �Ի�������
	enum { IDD = IDD_SEL_ASSOTIATED_BOLTS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bOrdinaryBolt;
	BOOL m_bFootnailBolt; 
public:
	virtual BOOL OnInitDialog();
};
