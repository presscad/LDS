#pragma once

#include "resource.h"

// CNewMapFrameDlg �Ի���
#include "SysPara.h"

class CNewMapFrameDlg : public CDialog
{
	CComboBox *m_pMapFrmNameCmbBox;
public:
	CNewMapFrameDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewMapFrameDlg();
	enum { IDD = IDD_NEW_MAP_FRAME_DLG };
// �Ի�������

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void RefreshMapFrmData();

	DECLARE_MESSAGE_MAP()
public:
	CString m_sMapFrmName;			//ͼ�����ƣ��½�ͼ��
	CString m_sCmbMapFrmName;		//ͼ�����ƣ�����ͼ��
	int m_nLength;					//ͼ�򳤶�
	int m_nWidth;					//ͼ����
	int m_nLeftMargin;				//���ҳ�߼�϶
	int m_nOthersMargin;			//������ҳ�߼�϶
	int m_nBottomRightFixLen;		//ͼ�����²�̶�����
	int m_nBottomRightFixWidth;		//ͼ�����²�̶����

	MAP_FRAME_SIZE m_xMapFrm;
	BOOL m_bNewMapFrm;				//TRUE:	�½�ͼ��FALSE:����ͼ��
	afx_msg void OnCbnSelchangeCmbMapFrameName();
};
