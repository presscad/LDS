#pragma once


// CPlateNcSetDlg �Ի���

class CPlateNcSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlateNcSetDlg)

public:
	CPlateNcSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlateNcSetDlg();

// �Ի�������
	enum { IDD = IDD_PLATE_NC_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	int m_iRollOffsetType;
	int m_iDeformType;
};
