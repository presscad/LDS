#pragma once


// CNailResideTubePage �Ի���

class CNailResideTubePage : public CDialog
{
	DECLARE_DYNAMIC(CNailResideTubePage)

public:
	CWnd* m_pParentWnd;	//������ָ��
	CNailResideTubePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNailResideTubePage();

// �Ի�������
	enum { IDD = IDD_NAIL_RESIDE_TUBE_PAGE };
	int m_iLaySide;
	int m_iNailType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK(){;}		//��ֹ�س�����ESC���رնԻ���
	virtual void OnCancel(){;}
	void OnRdoLaySide();
	void OnRdoNailType();

	DECLARE_MESSAGE_MAP()
public:
};
