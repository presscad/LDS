#pragma once
#include "supergridctrl.h"


// CSubstationBeamTemplate �Ի���

class CSubstationBeamTemplateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamTemplateDlg)

public:
	CSubstationBeamTemplateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubstationBeamTemplateDlg();

// �Ի�������
	enum { IDD = IDD_SUBSTATION_BEAM_TEMPL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void OnNewItem();
	void OnDeleteItem();

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrl m_listCtrlSegmentLeftEnd;
	virtual BOOL OnInitDialog();
	void DeleteSegmentItem();
};
