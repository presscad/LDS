#pragma once


// CNailResideAnglePage �Ի���

class CNailResideAnglePage : public CDialog
{
	DECLARE_DYNAMIC(CNailResideAnglePage)

public:
	CWnd* m_pParentWnd;	//������ָ��
	JGZJ xWingXZhunJu,xWingYZhunJu;
	CNailResideAnglePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNailResideAnglePage();

// �Ի�������
	enum { IDD = IDD_NAIL_RESIDE_ANGLE_PAGE };
	int		m_iLayWing;
	int		m_iGType;	// ����׼���ѡ������:0,��׼׼��g,1,2,3,�ֱ��ʾg1,g2,g3,4��ʾ�Զ���
	long	m_nGDist;	// �û��Զ���ĽŶ�׼��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK(){;}		//��ֹ�س�����ESC���رնԻ���
	virtual void OnCancel(){;}
public:
	void OnRdoLayWing();
	void OnCbnSelchangeCmbGType();
	void OnChangeGDist();

	DECLARE_MESSAGE_MAP()
};
