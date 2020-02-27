#pragma once
#include "supergridctrl.h"


// CSubstationBeamTemplate 对话框

class CSubstationBeamTemplateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSubstationBeamTemplateDlg)

public:
	CSubstationBeamTemplateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubstationBeamTemplateDlg();

// 对话框数据
	enum { IDD = IDD_SUBSTATION_BEAM_TEMPL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void OnNewItem();
	void OnDeleteItem();

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrl m_listCtrlSegmentLeftEnd;
	virtual BOOL OnInitDialog();
	void DeleteSegmentItem();
};
