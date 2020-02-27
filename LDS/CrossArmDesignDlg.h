#pragma once


// CCrossArmDesignDlg �Ի���
#include "PropertyList.h"
#include "block.h"
class CCrossArmDesignDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrossArmDesignDlg)
public:
	CParaBlockModel* m_pModel;
	CString	m_sKeyNodeOrder;
	long	m_nKeyNodeNum;	//�ؼ�������
	int		m_iKeyNodeMir;	//�ؼ���Գ�
	int		m_iStdFaceMir;	//ģ����Գ�
	CRect m_rectInitWndRgn;		//��ǰ�Ի���ĳ�ʼ���Σ���Ļ���꣩
public:
	CCrossArmDesignDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCrossArmDesignDlg();
	void DisplayPropertyList();
	void UpdateFaceDetailGraph();	//��ͼ������ʾ��ͼ
	int GetPropValueStr(int idProp,char* valueStr);
	void ChangeKeyNodeNum();

// �Ի�������
	enum { IDD = IDD_CROSSARM_DESIGN_DLG };
	CPropertyList	m_propList;
	int m_iCrossArmType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCmbCrossArmType();
	DECLARE_MESSAGE_MAP()
};
