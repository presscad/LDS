#pragma once


// CSelNailSeriesDlg �Ի���

class CSelNailSeriesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelNailSeriesDlg)
	long m_idCommonNailSeries,m_idAntiThelftNailSeries;
public:
	CSelNailSeriesDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelNailSeriesDlg();

// �Ի�������
	enum { IDD = IDD_SEL_CLIMB_BOLT_SERIES_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	int m_iCommonNailSeries;
	int m_iAntiThelftNailSeries;
};
