#pragma once


// CCutAngleCheckTypeDlg �Ի���

class CCutAngleCheckTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CCutAngleCheckTypeDlg)

public:
	CCutAngleCheckTypeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCutAngleCheckTypeDlg();
// �Ի�������
	enum { IDD = IDD_CUT_ANGLE_TYPE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();
	afx_msg void OnBnClickedChkAngleBolt();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCheckRayAndRay;
	BOOL m_bCheckMainAndRay;
	BOOL m_bCheckAngleAndBolt;
	int m_iBoltDetectType;
	double m_fClearance;
};
