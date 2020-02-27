#pragma once

// CCheckPartDataDlg �Ի���

class CCheckPartDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCheckPartDataDlg)
	SmartPartPtr m_pSelPart;
	bool m_bEnableWindowMoveListen;
	static int nScrLocationX,nScrLocationY;
public:
	BOOL m_bChkPartNo;
	BOOL m_bChkCount;
	BOOL m_bChkMaterial;
	BOOL m_bChkGuige;
	BOOL m_bChkLength;
	BOOL m_bChkM16Num;
	BOOL m_bChkM20Num;
	BOOL m_bChkM24Num;
	BOOL m_bChkElseNum;
	BOOL m_bChkHole;
	//������Ϣ
	BYTE m_ciPartType;
	int  m_nCount;			//��������
	CString m_sMaterial;	//����
	CString	m_sGuige;		//���
	CString m_sLen;			//����
	CString m_sHandle;		//���
	CString m_sPartNo;		//����
	CString	m_sPartType;	//����
	//��˨��Ϣ
	int m_nM16;			//M16������˨����
	int m_nM20;			//M20������˨����
	int m_nM24;			//M24������˨����
	int m_nElse;		//����ǳ���׾�������˨����
	int m_nHole;		//���⹦�ܿף�����߿ס���װ�׵�������˨������
public:
	CCheckPartDataDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCheckPartDataDlg();
	void SetPart(CLDSPart* pPart);
	void InitSelPartInfo();
	void CreatePartCheckData();
// �Ի�������
	enum { IDD = IDD_CHECK_PART_DATA_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg void OnCbnSelchangeCmbMaterial();
	afx_msg void OnCbnSelchangeCmbGuige();
};
